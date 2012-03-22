#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "types.h"
#include <sys/time.h>
#include <math.h>

#include <root_simulator.h>

OS getOs() 
{
	return LINUX;
}

double getTime()
{
	return 1.0*clock()/CLOCKS_PER_SEC;
}


double getRealSimulationTime()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return (tv.tv_sec + tv.tv_usec*1.0e-6)  - realTiSimulation;
}

int waitFor(Time t, RealTimeMode m) 
{
	double ti=getRealSimulationTime();
	if (t<=0) 
    return 0;
	while ((getRealSimulationTime()-ti)<t) ;
  return 0;
}

void initLib()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  realTiSimulation = tv.tv_sec + tv.tv_usec*1.0e-6;
}

void cleanLib()
{

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
      exitStatus = -1;
			printLog("ERROR: %s not found\n",path);
	}
}

void writeToPort(short v,int port) {
}

short readFromPort(int port) {
  return 0;
}

long int RTFileOpen(char* name,char mode) {
	char strMode[2];
	strMode[0]=mode;
	strMode[1]='\0';
	return (long int)fopen(name,strMode);
};
long int RTFileWrite(long int file ,char* buf,int size) {
	fwrite(buf,size,1,(FILE*)file);	
  fflush((FILE*)file);
};
long int RTFileRead(long int file ,char* buf ,int size){
  int r;
	if ((r=fread(buf,size,1,(FILE*)file))<size)
    printLog("Incomplete read from file\n");
  return r;
}
void RTFileClose(long int file){
	fclose((FILE*)file);
};

void printLog(const char *fmt,...) {
  static int init=0;
  if (!init) {
    init=1;
    fclose(fopen("pdevs.log","w"));
  }
	va_list va;
	va_start(va,fmt);
	FILE *fd=fopen("pdevs.log","a");
	vfprintf(fd,fmt,va);
	fclose(fd);
	va_end(va);
}

void RequestIRQ(unsigned irq, void  *a){

}

#include "pdevslib.common.cpp"
