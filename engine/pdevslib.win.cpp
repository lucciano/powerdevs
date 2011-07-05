
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "winsock2.h"
#include <unistd.h>
#include <sys/types.h> 

OS getOs() 
{
	return WINDOWS;
}
double getTime()
{
	return 1.0*clock()/CLOCKS_PER_SEC;
}

int waitFor(Time t, RealTimeMode m) 
{
	if (t<=0) return -1;
	double ti=getTime();
	while ((getTime()-ti)<t) ;

}

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

void spawnProcess(const char *path, char *arg) {
	spawnl(P_NOWAIT , path, " ", arg,NULL);;
}

void writeToPort(short v,int port) {
}

short readFromPort(int port) {
}

/* Scilab Interface */

SOCKET SendSocket;
struct sockaddr_in service;
bool scilabActive;


double getScilabVar(char *varname) {
 initScilab();
  char buf[1024];
  double f;
  sprintf(buf,"anss=%s",varname);
  executeScilabJob(buf,false);
  executeScilabJob("exists anss",false);
  getAns(&f,1,1);
  if (!scilabActive) {
     printLog("There is not a Scilab instance running. Returning atof(%s)\n",varname); 
     return atof(varname); 
  }	  
  if (f==0) {
     printLog("Variable %s does not exists!\n",varname); 
     return 0.0; 
  }
  executeScilabJob("ans=anss",false);
  getAns(&f,1,1);
  executeScilabJob("clear anss",false);
  return f;
}

void putScilabVar(char *varname, double var) { 
   char buf[1024];
   sprintf(buf,"%s=%g",varname,var);
   executeScilabJob(buf,false);
}

double executeScilabJob(char *cmd,bool blocking) {
   initScilab();
   double ans=0;
   int timeout = 2000;
   char buff[1024];
   int SenderAddrSize = sizeof(sockaddr_in);
   int result = -1;
   strcpy(buff,"");
   if (blocking) 
	   strcpy(buff,"!");
   strcat(buff,cmd);
   sendto(SendSocket,buff,strlen(buff),0,(SOCKADDR *) & service,sizeof(struct sockaddr_in));
   if (blocking) {
	   result = recvfrom(SendSocket, buff , 1024, 0,(SOCKADDR *) & service ,&SenderAddrSize);
	   if (result<1) {
		printLog("Socket Error %d\n",WSAGetLastError());
	   }
   }
   return ans;

}
void initScilab() {
  static int init=0;
  char cmd[1024];
  scilabActive=true;
  if (init++ == 0) {
	  WSADATA wsaData;
	  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	  SendSocket = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);
	  service.sin_family = AF_INET;
	  service.sin_addr.s_addr = inet_addr("127.0.0.1");
	  service.sin_port = htons(27015);
  }
}
void cleanScilab() {
 static int clean=0;
 if (clean++ == 0) {
    closesocket(SendSocket);
    WSACleanup();
 }
}

void getAns(double *ans, int rows, int cols) {
   char cmd[124] = "@";
   int timeout = 2000;
   int iResult;
   int SenderAddrSize = sizeof(sockaddr_in);
   sendto(SendSocket,cmd,strlen(cmd),0,(SOCKADDR *) & service,sizeof(struct sockaddr_in));
   ans[0]=0;
   iResult = recvfrom(SendSocket, (char*)ans, sizeof(double)*rows*cols, 0,(SOCKADDR *) & service ,& SenderAddrSize);
   if (iResult < 0)
	   scilabActive = false;
}

extern double tf;
double getFinalTime()
{
	return tf;
}

long int RTFileOpen(char* name,char mode) {
	char strMode[2];
	strMode[0]=mode;
	strMode[1]='\0';
	return (long int)fopen(name,strMode);
};
void RTFileWrite(long int file ,char* buf,int size) {
	fwrite(buf,size,1,(FILE*)file);	
  fflush((FILE*)file);
};
void RTFileRead(long int file ,char* buf ,int size){
	fread(buf,size,1,(FILE*)file);
}
void RTFileClose(long int file){
	fclose((FILE*)file);
}
