#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/io.h>
#include <signal.h>

#include <rtai_sched.h>
#include <rtai_usi.h>
#include <rtai_lxrt.h>
#include <rtai_mbx.h>
#include <rtai_msg.h>
#include <rtai_fifos.h>

#include "types.h"
#include "event.h"
#include "pdevslib.h"
#include "root_simulator.h"

#define MIN_WAIT_NS (1e-6)
#define MAX_BUSY_SLEEP (20e-6)
#define NORMAL_WAIT_DIFF (MAX_BUSY_SLEEP/2)
#define FIFOSPACE (0x100000)
#define NIRQS (16)
#define SOCKET_ERROR        -1
#define TCP_BACKDOOR
/*! \brief This structure is used to notify the simulation engine that an IRQ has happend */
struct IRQMessage {
    //! Which IRQ ocurred
    int irq;
    Time time;
};
/*! \brief Linked list to notify whan an IRQ has happend */
struct Node {
    void *atomic;
    struct Node *next;
};

struct Node *isr[NIRQS];

void initIRQMBX();
void clearISR();
void deleteIRQMBX();
double max_latency=0, initial_t;
double avg_latency=0, waits=1;
float overruns = 0;
float events = 0;
static volatile int end = 1;
static volatile int lastirq = -1;


double getRealSimulationTime() {
 	return getTime()-realTi;
}
void alertOfIRQ(IRQMessage msg) {
    struct Node *n =  isr[msg.irq];
    class Simulator *s;
    msg.time = getTime()-realTi;
    for (;n != NULL; n = n->next) {
        s = (Simulator*)n->atomic;
		//rtai_print_to_screen("[t=%g] Notifying atomic %s of an IRQ\n",msg.time,s->name);
        s->externalinput(msg.time,msg.irq);
    };
}



OS getOs() 
{
	return RTAI;
}
double getTime()
{
	return count2nano(rt_get_time())*1e-9;
}


static MBX *mbxirq;
MBX *getIRQMBX() { return mbxirq; }
int waitFor(Time delay, RealTimeMode mode) 
{ 
	IRQMessage msg;
    MBX *mbx = getIRQMBX();
	int ret;
    if (delay<0) {
		overruns++;
		overRun=true;
		return 0;	
	}
	if (delay>MIN_WAIT_NS ) { // Do wait if the delay is less than MINWAIT_NS
		events++;
		Time beforeWait=getTime();
		if ( mode == REALTIME ) {
			RTIME sleep=nano2count(delay*1e9);
			//rt_sleep(sleep);
			ret = rt_mbx_receive_timed( mbx, &msg, sizeof(msg), sleep);
			if (ret == 0  && msg.irq > 0) { // An IRQ has happend. Notify this
/*
            	rtai_print_to_screen("Recibi un mail con la irq %d\n en t=%.12g\n",
				msg.irq,msg.time);
*/
                alertOfIRQ(msg);
                return -1;
            }

		} else {
			if ( delay < MAX_BUSY_SLEEP) { // The latency is lower than MAXBUSYSLEEP, then do a busy_sleep
				rt_busy_sleep(delay*1e9);
			} else {
				RTIME sleep=nano2count(delay*1e9 - NORMAL_WAIT_DIFF*1e9);
				ret = rt_mbx_receive_timed( mbx, &msg, sizeof(msg), sleep);
				if (ret == 0  && msg.irq > 0) { // An IRQ has happend. Notify this
/*
            	rtai_print_to_screen("Recibi un mail con la irq %d\n en t=%.12g\n",
				msg.irq,msg.time);
*/
               				 alertOfIRQ(msg);
			                return -1;
		        }

				Time newdelay = delay - (getTime()-beforeWait);
				//printLog("Waited %g in normal wait and going to wait %d in busy sleep\n",delay*1e9 - NORMAL_WAIT_DIFF,int(newdelay*1e9));
				if (newdelay > MIN_WAIT_NS) { // Compensate
					int n=newdelay*1e9;
					Time i=getTime();
					rt_busy_sleep(n);
					Time f=getTime();
					//printLog(" Lat busy(%d) sleep %d\n",n,int((f-i)*1e9));
				}
			}
		}
		Time afterWait=getTime();
		Time latency=(afterWait-beforeWait)-delay; 	
		if ((double)latency>0) {
			avg_latency+=latency;
			waits++;
		}
		max_latency   = (latency > max_latency ? latency : max_latency);
	}
	return 0;


}

#define STACKSIZE 0x1000
RT_TASK *task;
int fd_stdout;
void enterRealTime (){
	int period;
	initIRQMBX();
 	if (! (task = 
	rt_task_init(nam2num("PDEVS"), 0,STACKSIZE,0x0))) {
		printf("Error: Can't init the scheduler\n");
		exit(-1);
	}
	
	rt_set_oneshot_mode();
	period = start_rt_timer(nano2count(100000));
	mlockall(MCL_CURRENT | MCL_FUTURE);
	printLog("Going realtime...(PID = %d)\n",getpid());
	rt_make_hard_real_time();
	rt_task_make_periodic(task,rt_get_time()+10*period,period);
}
void leaveRealTime() {
	rt_make_soft_real_time();
	clearISR();
	printLog("Leaving realtime...\n");
	deleteIRQMBX();
	rt_task_delete(task);
	printLog("***********************************************\n");
	printLog("Max latency=%g\n",max_latency);
	printLog("Avg latency=%g\n", avg_latency/waits);
	printLog("Overruns %g over %g\n", overruns,events);
	printLog("***********************************************\n");
}


void spawnProcess(const char *path, char *arg) {
	char *argv[10],buff[1024];
	if (fork()==0) { // Child process
			int i=0;
			argv[i]=(char*)malloc(strlen(path));
			strcpy(argv[i++],path);
			while (sscanf(arg,"%s",buff)>0) {
				if (strlen(buff)==0) break;
				argv[i]=(char*)malloc(strlen(buff));
				strcpy(argv[i],buff);
				arg+=strlen(argv[i++]);
			}
			argv[i]=NULL;
			execv(path,argv);
			printLog("ERROR: %s not found\n",path);
	}
}

void writeToPort(short v,int port) {
	outb_p(v,port);
}

short readFromPort(int port) {
	return inb_p(port);
}


int SendSocket;
//int SendSocket,SendSocketUDP;
struct sockaddr_in service, serviceUDP;
bool activeScilab;


double executeScilabJob(char *cmd,bool blocking) {
#ifdef TCP_BACKDOOR
  double ans=0;
  char buff[1024];
  int result = -1;

  initScilab();
  strcpy(buff,"");
  if (blocking)
    strcpy(buff,"!");
  strcat(buff,cmd);
  strcat(buff,"\n");
  if (!activeScilab) {
    exitStatus = -1;
    printLog("There's not an instance of Scilab running. Returing zero\n");
    return 0.0;
  }
  if (write(SendSocket,buff,strlen(buff))<(signed)strlen(buff))
    printLog("Incomplete TCP message\n");
  if (blocking) {
	  result = read(SendSocket, buff , 1024);
  }
  return ans;
#else
  // UDP implementation
  double ans=0;
  char buff[1024];
  socklen_t  SenderAddrSize = sizeof(sockaddr_in);
  int result = -1;

  initScilab();
  strcpy(buff,"");
  if (blocking)
    strcpy(buff,"!");
  strcat(buff,cmd);
  if (!activeScilab) {
    exitStatus = -1;
    printLog("There's not an instance of Scilab running. Returing zero\n");
    return 0.0;
  }
  int ret=sendto(SendSocketUDP,buff,strlen(buff),0,(struct sockaddr*)&serviceUDP,sizeof(struct sockaddr_in));
  if (blocking) {
    recvfrom(SendSocketUDP,buff,sizeof(double),0,(struct sockaddr*)&serviceUDP,&SenderAddrSize);
  }
  return ans;
#endif
}

void initScilab() {
  static int init=0;
  if (init++ == 0) {
  	SendSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(27020);
    if (connect(SendSocket,(struct sockaddr*)&service,sizeof(service)) == SOCKET_ERROR)
    {
	    activeScilab=false;
      exitStatus = -1;
      return;
    }
    /*
    serviceUDP.sin_family = AF_INET;
    serviceUDP.sin_port = htons(27020);
    serviceUDP.sin_addr.s_addr = inet_addr("127.0.0.1");
  	SendSocketUDP=socket(AF_INET,SOCK_DGRAM, 0);
    */
    activeScilab=true;
  } 
}
void cleanScilab() {
 static int clean=0;
 if (clean++ == 0) {
#ifdef TCP_BACKDOOR
    close(SendSocket);
#else
    close(SendSocketUDP);
#endif
 }
}


void putScilabVar(char *varname, double var) {
  initScilab();
   char buf[1024];
   sprintf(buf,"%s=%g",varname,var);
   executeVoidScilabJob(buf,false);
}

double getScilabVar(char *varname) {
  // Try to solve it locally
  char *s;
  double d=strtod(varname,&s);
  if (varname+strlen(varname)==s)
    return d;
  initScilab();
  char buf[1024];
  double f;
  sprintf(buf,"anss=%s",varname);
  executeVoidScilabJob(buf,true);
  executeScilabJob((char*)"exists('anss')",false);
  getAns(&f,1,1);
  if (!activeScilab) {
     exitStatus = -1;
     printLog("There's not an instance of Scilab running. Returning atof(%s)\n",varname); 
     return atof(varname);
  }
  if (f==0) {
     exitStatus = -1;
     printLog("Variable %s does not exists! Returning zero\n",varname); 
     return 0.0;
  }
  executeScilabJob((char*)"anss",true);
  getAns(&f,1,1);
  executeVoidScilabJob((char*)"clear anss",true);
  return f;
}


void getAns(double *ans, int rows, int cols) {
#ifdef TCP_BACKDOOR
  initScilab();
  if (!activeScilab) {
    exitStatus = -1;
    printLog("There's not an instance of Scilab running. Returing zero\n");
	  ans[0]=0.0;
  }
  char cmd[124] = "@";
  int iResult;
  struct pollfd p;
  executeScilabJob(cmd,false);
  ans[0]=0;
  if (!activeScilab) 
    return;
  p.fd=SendSocket;
  p.events = POLLIN;
  if (true /*poll(&p,1,1000)!=0*/) {
	  iResult = read(SendSocket, (char*)ans, sizeof(double)*rows*cols);
    //printLog("Read returned %d\n",iResult);
  } else {
    exitStatus = -1;
	  printLog("There's not an instance of Scilab running. Returing zero\n");
	  activeScilab=false;
  }
#else
  char cmd[124] = "@";
  int iResult;
  struct pollfd p;
  socklen_t  SenderAddrSize = sizeof(sockaddr_in);

  initScilab();
  if (!activeScilab) {
    exitStatus = -1;
    printLog("There's not an instance of Scilab running. Returing zero\n");
	  ans[0]=0.0;
  }

  executeScilabJob(cmd,false);
  ans[0]=0;
  if (!activeScilab) 
    return;
  recvfrom(SendSocketUDP,(char*)ans,sizeof(double),0,(struct sockaddr*)&serviceUDP,&SenderAddrSize);
  // UDP implementation
#endif
}

long int RTFileOpen(char* name,char mode) 
{
	return 0;
};
void RTFileWrite(long int file ,char* buf,int size) {
	//rtf_put(file,buf,size);
};
void RTFileRead(long int file ,char* buf ,int size){
	//rtf_get(file,buf,size);
}
void RTFileClose(long int file){
  /*
	rtf_put(file,"\0",1);
	rtf_destroy(file);
  */
};

void printLog(const char *fmt,...) {
  static int init=0;
  if (!init) {
    init=1;
    FILE *fd= fopen("pdevs.log","w");
	if (fd!=NULL) {
		struct tm *ptr;
		time_t tm;
		tm = time(NULL);
		ptr = localtime(&tm);
		fprintf(fd,"Log of execution: %s\n",asctime(ptr));
		fclose(fd);
	}
	else printf("Error writing in log file\n");
  }
	va_list va;
	va_start(va,fmt);
	FILE *fd=fopen("pdevs.log","a");
	if (fd!=NULL) {
		vfprintf(fd,fmt,va);
		fclose(fd);
	} else {
		printf("Error writing in log file\n");
	}
	va_end(va);
}

int isrInitialized = 0;

void initIRQMBX() {
    mbxirq = (MBX*) rt_mbx_init(nam2num("IRQMBX"),20*sizeof(IRQMessage));
    printLog("Initializing MBX (at %x)\n",mbxirq);
}

static void endme (int dummy) {
    rtai_print_to_screen("Ending IRQS\n");
    unsigned int i;
    for (   i=0;i<NIRQS;i++) {
        if (isr[i]!=NULL) {
                rt_release_irq_task(i);
                rt_unmask_irq((unsigned)i);
        }
    }
    _exit(0);
}

void initISR() {
    int i;
    for (i=0;i<NIRQS;i++){
         isr[i]=NULL;
    }
}


void clearISR() {
    int i;
    for ( i=0;i<NIRQS;i++) {
        if (isr[i]!=NULL) {
            end = 0;
            rt_irq_signal(i);
            rt_sleep(2000000000); // Give time to IRQ handlers to exit
            rtai_print_to_screen("Ending IRQS from Clear\n");
        }
    }
}
    
void deleteIRQMBX() {
    rt_mbx_delete(mbxirq);
}

static void *irq_handler(void *args) {
        unsigned int irq = (int)args;
        char name[] ="HNDLR  ";
        name[5] = '0' + irq;
        MBX *mbx = NULL;
        struct IRQMessage msg;
        signal(SIGHUP,endme);
        signal(SIGINT,endme);
        signal(SIGKILL,endme);
        signal(SIGTERM,endme);
        char buf[] = "IRQ\n";
        RT_TASK *handler_task=rt_task_init_schmod(nam2num(name),0,0,0,SCHED_FIFO,0xF);
        rt_allow_nonroot_hrt();
        mlockall(MCL_CURRENT | MCL_FUTURE);
        rt_make_hard_real_time();
        while (mbx == NULL) {
            mbx = (MBX*)rt_get_adr(nam2num("IRQMBX"));
            rt_sleep(1000000);
        }
        rtai_print_to_screen("Waiting for IRQ %d\n",irq);
        rt_request_irq_task(irq, handler_task, RT_IRQ_TASK, 1);
        rt_startup_irq(irq);
        rt_enable_irq(irq);
        while (end) {
            int ret = rt_irq_wait(irq);
			if (ret == RT_IRQ_TASK_ERR) break;
            if (!end) break;
            if (!ret) {
                rt_ack_irq(irq);
                msg.irq = irq;
                //rtai_print_to_screen("Detected IRQ %d\n",irq);
                if (mbx!=NULL ) {
                    //rtai_print_to_screen("Sending mail to MBX at %x\n",mbx);
                    rt_mbx_send(mbx,&msg,sizeof(msg));
                }
				rt_pend_linux_irq(irq);

            }
        }
        rtai_print_to_screen("Releasing handler IRQ\n");
		rt_release_irq_task(irq);
        rt_make_soft_real_time();
        rt_task_delete(handler_task);
        return 0;
}

void RequestIRQ(unsigned int irq, void *atomic) {
    if (!isrInitialized) { initISR(); isrInitialized=1;}
    if (irq>NIRQS) return;
    struct Node *newnode = (Node*)malloc(sizeof(Node));
    newnode->atomic = atomic;
    newnode->next=isr[irq];
    void *handler = (void*)irq_handler;
    if (isr[irq]==NULL) { // Instalo la ISR
        rt_thread_create(handler,(void*)irq,0x10000);
    }
    isr[irq]=newnode;
    return;
}


void getScilabMatrix(char* varname, int *rows, int *cols, double **data) {
  char buf[1024];
  sprintf(buf,"tempvar=%s",varname);
  executeVoidScilabJob(buf,true);
  sprintf(buf,"save('%s/../output/temp.dat',tempvar)",getenv("PWD"));
  executeVoidScilabJob(buf,true);
  FILE *FOpen;
  FOpen=fopen("temp.dat","rb");
  char name[24];
  int res=fread(&name,24,1,FOpen);
  int varint;
  res=fread(&varint,sizeof(int),1,FOpen);
  res=fread(rows,sizeof(int),1,FOpen);
  res=fread(cols,sizeof(int),1,FOpen);
  res=fread(&varint,sizeof(int),1,FOpen);
  int nrows=*rows;
  int ncols=*cols;
  double lastdata;
  for (int j=0;j<ncols;j++) {
    for (int i=0;i<nrows;i++) {
       res=fread(&lastdata,sizeof(double),1,FOpen);
       data[i][j]=lastdata;
    }
  }
  if (res<=0)
    printLog("Incomplete read in getScilabMatrix\n");
  fclose(FOpen); 
}

double executeVoidScilabJob(char *cmd,bool blocking) {
  char buff[1024];
  sprintf(buff,"\\%s",cmd); // If the command starts with a slash the command is not written to the out var
  executeScilabJob(buff,blocking);
  return 0.0;
}
void getScilabVector(char* varname, int *length, double *data) {
  int rows,cols;
  char buf[1024];
  sprintf(buf,"tempvar=%s",varname);
  executeVoidScilabJob(buf,true);
  sprintf(buf,"save('%s/../output/temp.dat',tempvar)",getenv("PWD"));
  executeVoidScilabJob(buf,true);
  FILE *FOpen;
  FOpen=fopen("temp.dat","rb");
  char name[24];
  int res=fread(&name,24,1,FOpen);
  int varint;
  res=fread(&varint,sizeof(int),1,FOpen);
  res=fread(&rows,sizeof(int),1,FOpen);
  res=fread(&cols,sizeof(int),1,FOpen);
  res=fread(&varint,sizeof(int),1,FOpen);
  if (rows>cols) *length=rows; else *length=cols;
  double lastdata;
  for (int i=0;i<*length;i++) {
     res=fread(&lastdata,sizeof(double),1,FOpen);
     data[i]=lastdata;
  }
  if (res<=0)
    printLog("Incomplete read in getScilabMatrix\n");
  fclose(FOpen);  
}


extern double tf;
double getFinalTime()
{
	return tf;
}
void cleanLib()
{

}
void initLib()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  realTiSimulation = tv.tv_sec + tv.tv_usec*1.0e-6;
}
