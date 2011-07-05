/* ==================================================================== */
/* This file is released into the public domain */
/* ==================================================================== */
#include "backdoor.h"
#include <stdio.h>
/* ==================================================================== */

#define TCP_PORT 27020
#define UDP_PORT 27020
#define BIND_IP "127.0.0.1"
#define BUFF_SIZE 8092
#define RESULT_VAR "bd_result"
#define DONE_STR "done\n"

#ifdef  __linux__

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

#else

#include <windows.h>
#include <winsock.h>

#endif

#ifdef  __linux__
pthread_t threadTCP, threadUDP;
int listenTCP,listenUDP;

void *waitTCPRequests();
void *waitUDPRequests();

#else

DWORD dwThreadId,dwThreadUDPId;

DWORD waitTCPRequests (LPVOID lpdwThreadParam );
DWORD waitUDPRequests (LPVOID lpdwThreadParam );
SOCKET listenTCP;


#endif

void openBackDoor()
{
#ifdef  __linux__
	pthread_create( &threadTCP, NULL, waitTCPRequests, (void*) NULL);
	//pthread_create( &threadUDP, NULL, waitUDPRequests, (void*) NULL);
#else
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&waitTCPRequests, (LPVOID) NULL, 0, &dwThreadId);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&waitUDPRequests, (LPVOID) NULL, 0, &dwThreadId);
#endif
}

void closeBackDoor()
{
#ifdef  __linux__
	pthread_kill(threadTCP,SIGKILL);
#else
  TerminateThread(dwThreadId,0);
#endif
}

#ifdef  __linux__
void takeRequests(void* arg)
#else
 DWORD takeRequests(LPVOID arg)
#endif
{
  char buff[BUFF_SIZE];
  char result[BUFF_SIZE];
  char cmd[BUFF_SIZE];
  char *eol,*c;
  struct sockaddr_in addr,client;
  int socksize;
  int m=1,n=1;
  int commandComplete;
  int  received=0;
#ifdef  __linux__
  int socket = (int)arg;
#else
  SOCKET socket = (SOCKET)arg;
#endif
	double d=1234567;
  
  /*createNamedMatrixOfDouble(pvApiCtx,"fede",1,1,&d);
  SendScilabJob("a=3243;");
  return;
  */
  sprintf(cmd,"mprintf(\"\tBackDoor: new connection accepted\n\");");
	SendScilabJob(cmd);
	commandComplete=0;
	strcpy(cmd,""); // Clear the command
	do  { // Accept commands through the backdoor
	  received=recv(socket,buff,BUFF_SIZE,0);		
		buff[received]='\0'; // Terminate string with NULL char
		c=buff;
		while ((eol = strchr(c,'\n'))!=NULL) { // A end-of-line char has been received
		  eol[0]='\0'; // Terminate the string
			if (eol[-1]=='\r') 
			  eol[-1]='\0'; // Remove \r too
      strcat(cmd,c);
			if (cmd[0]=='@') { // If the first char is the '@' we need to send the result (ans var)
			  ReadMatrix(RESULT_VAR,&m,&n,(double*)result);
				send(socket,result,sizeof(double),0);
      } else if (cmd[0]=='!') { // The '!' denotes that the client needs a confirmation that the job is ready
			  if (cmd[1]=='\\')
          sprintf(result,"%s",cmd+2);
        else
				  sprintf(result,"%s=%s",RESULT_VAR,cmd+1);
        SendScilabJob(result);
				send(socket,DONE_STR,strlen(DONE_STR),0);
      } else if (cmd[0]=='\\') { // The '!' denotes that the client needs a confirmation that the job is ready
			  sprintf(result,"%s",cmd+1);
				SendScilabJob(result);
      } else {
			  sprintf(result,"%s=%s",RESULT_VAR,cmd);// run the job
				//printf("Run command '%s'\n",result);
				SendScilabJob(result);
			}
			strcpy(cmd,""); // Blank for new command
			c=eol+1;
		}
		strcat(cmd,c); // Copy the last part for the new command
	} while (received);
}

#ifdef  __linux__
void *waitTCPRequests()
#else
DWORD waitTCPRequests(LPVOID lpdwThreadParam)
#endif
{
	char buff[BUFF_SIZE];
	char result[BUFF_SIZE];
	char cmd[BUFF_SIZE];
	char *eol,*c;
	struct sockaddr_in addr,client;
	int socksize=sizeof(struct sockaddr);
	int m=1,n=1;
  double d=12345;  
	int received=0;
#ifndef  __linux__
  DWORD dwThreadIdTask;
#endif
  listenTCP=socket(AF_INET,SOCK_STREAM,0);
  setsockopt(listenTCP, SOL_SOCKET, SO_REUSEADDR, &m, sizeof(m));

//  WSADATA wsaData;
#ifdef  __linux__
	sleep(1); // Wait a little bit to scilab to be ready 
#else
  // In windows set as blocking
  createNamedMatrixOfUnsignedInteger32(pvApiCtx,"sock",1,1,&listenTCP);
  ioctlsocket (listenTCP,FIONBIO,&received);
#endif

  addr.sin_family = AF_INET;
	addr.sin_port = htons(TCP_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (listenTCP, (struct sockaddr *) &addr, sizeof (addr))<0) {
#ifdef  __linux__
    pthread_exit(NULL);
#else
		ExitThread(NULL);
#endif
	}
  sprintf(cmd,"mprintf(\"\tBackDoor: listening to connections %d\n\");",TCP_PORT);
  SendScilabJob(cmd);
	if (listen(listenTCP,5)==0) {
    do {
#ifdef  __linux__
		  int socket=accept(listenTCP,(struct sockaddr *)&client, &socksize);
      pthread_t t;
	    pthread_create( &t, NULL, takeRequests, (void*) socket);
    } while (socket>0 );
#else
		  SOCKET socket=accept(listenTCP,(struct sockaddr *)&client, &socksize);
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&takeRequests, (LPVOID) socket, 0, &dwThreadIdTask);
    } while (socket!=INVALID_SOCKET);
#endif
  }
	
#ifdef  __linux__
  close(listenTCP);
  pthread_exit(NULL);
#else
  closesocket(listenTCP);
  ExitThread(NULL);
#endif
}

#ifdef  __linux__
void *waitUDPRequests()
#else
DWORD waitUDPRequests(LPVOID lpdwThreadParam)
#endif
{
	char buff[BUFF_SIZE];
	char result[BUFF_SIZE];
	char cmd[BUFF_SIZE];
	char *eol,*c;
	struct sockaddr_in addr,client;
	int socksize=sizeof(struct sockaddr);
	int m=1,n=1;
  double d=12345;  
	int received=0;
#ifndef  __linux__
  DWORD dwThreadIdTask;
#endif
  listenUDP=socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP);

//  WSADATA wsaData;
#ifdef  __linux__
	sleep(1); // Wait a little bit to scilab to be ready 
#else
  // In windows set as blocking
  createNamedMatrixOfUnsignedInteger32(pvApiCtx,"sock",1,1,&listenTCP);
  ioctlsocket (listenTCP,FIONBIO,&received);
#endif

  addr.sin_family = AF_INET;
	addr.sin_port = htons(UDP_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(listenUDP, &addr, sizeof(addr))==-1) {
#ifdef  __linux__
    pthread_exit(NULL);
#else
		ExitThread(NULL);
#endif
  }
  printf("Receiving from UDP '%s'\n",buff);
  do {
#ifdef  __linux__
    if ((received=recvfrom(listenUDP,buff,BUFF_SIZE,0,&client,&socksize))==-1)
      break;
		buff[received]='\0'; // Terminate string with NULL char
		if (buff[0]=='@') { // If the first char is the '@' we need to send the result (ans var)
		  ReadMatrix(RESULT_VAR,&m,&n,(double*)result);
      sendto(listenUDP, result, sizeof(double), 0, &client, socksize);
    } else if (buff[0]=='!') { // The '!' denotes that the client needs a confirmation that the job is ready
		  if (buff[1]=='\\')
        sprintf(result,"%s",cmd+2);
      else
			  sprintf(result,"%s=%s",RESULT_VAR,cmd+1);
      SendScilabJob(result);
      sendto(listenUDP, result, sizeof(double), 0, &client, socksize);
    } else if (buff[0]=='\\') { // The '!' denotes that the client needs a confirmation that the job is ready
		  sprintf(result,"%s",buff+1);
			SendScilabJob(result);
    } else {
		  sprintf(result,"%s=%s",RESULT_VAR,buff);// run the job
			printf("Run UDP command '%s'\n",result);
			SendScilabJob(result);
		}
#endif
  } while (received);
	
#ifdef  __linux__
  close(listenTCP);
  pthread_exit(NULL);
#else
  closesocket(listenTCP);
  ExitThread(NULL);
#endif
}



/* ==================================================================== */


