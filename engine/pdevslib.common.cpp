#define SOCKET_ERROR        -1
#define TCP_BACKDOOR

int SendSocket;
//int SendSocket,SendSocketUDP;
struct sockaddr_in service, serviceUDP;
bool activeScilab;


double executeVoidScilabJob(char *cmd,bool blocking) {
  char buff[1024];
  sprintf(buff,"\\%s",cmd); // If the command starts with a slash the command is not written to the out var
  executeScilabJob(buff,blocking);
  return 0.0;
}

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


void getScilabMatrix(char* varname, int *rows, int *cols, double **data) {
  char *s;
  double d=strtod(varname,&s);
  if (varname+strlen(varname)==s)
  {
    for (int i=0;i<*rows;i++)
      for (int j=0;j<*cols;j++)
        data[i][j]=d;
    return;
  }
  char buf[1024];
  sprintf(buf,"tempvar=%s",varname);
  executeVoidScilabJob(buf,true);
  sprintf(buf,"save('%s/../output/temp.dat',tempvar)",getenv("PWD"));
  executeVoidScilabJob(buf,true);
  FILE *FOpen;
  FOpen=fopen("temp.dat","rb");
  if (FOpen==NULL) {
    printLog("No connection to Scilab to get %s\n",varname);
    exit(-1);
  }
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
  unlink("temp.dat");
}

void getScilabVector(char* varname, int *length, double *data) {
  char *s;
  double d=strtod(varname,&s);
  if (varname+strlen(varname)==s)
  {
    *length=1;
    data[0]=d;
    return;
  }
 
  int rows,cols;
  char buf[1024];
  sprintf(buf,"tempvar=%s",varname);
  executeVoidScilabJob(buf,true);
  sprintf(buf,"save('%s/../output/temp.dat',tempvar)",getenv("PWD"));
  executeVoidScilabJob(buf,true);
  FILE *FOpen;
  FOpen=fopen("temp.dat","rb");
  if (FOpen==NULL) {
    printLog("No connection to Scilab to get %s\n",varname);
    exit(-1);
  }
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
  unlink("temp.dat");
}

extern double tf;
double getFinalTime()
{
	return tf;
}
