#include "res_qss_integrator.h"
void res_qss_integrator::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//      %Type% is the parameter type
for (int i=0;i<10;i++) {
	y[i]=0;
 	X[i]=0;
	q[i]=0;
};
Method=va_arg(parameters,char*);
char *fvar= va_arg(parameters,char*);
dQmin=getScilabVar(fvar );
fvar= va_arg(parameters,char*);
dQrel=getScilabVar(fvar );
fvar= va_arg(parameters,char*);
X[0]=getScilabVar(fvar);
dQ=fabs(X[0])*dQrel;
if (dQ<dQmin){dQ=dQmin;};

if (strcmp(Method,"QSS")==0) {
          met=1;
          q[0]=X[0]; 
			order=met;

};

if (strcmp(Method,"QSS2")==0) {
      met=2;       
      q[0]=X[0];
order=met;

};
if (strcmp(Method,"QSS3")==0){
        q[0]=X[0];
        met=3;  
		order=met;

};
if (strcmp(Method,"QSS4")==0){
        q[0]=X[0];
        met=4;
		order=met;
  
};
if ((strcmp(Method,"BQSS")==0)||(strcmp(Method,"CQSS")==0)){
			order=1;
        met=5;  
        if(strcmp(Method,"CQSS")==0){met=6;};
	ep=dQmin/100; 
   qs=X[0]+dQ; 
   qi=X[0]-dQ; 
	q[0]=qi;
	eps=1e-20;
	band=1;
};
sigma=0;
}
double res_qss_integrator::ta(double t) {
//This function return a double.
return sigma;
}
void res_qss_integrator::dint(double t) {
switch(met) {
case 1: // QSS
	advance_time(X,sigma,1);
 	q[0]=X[0];
	dQ=dQrel*fabs(X[0]);
	if (dQ<dQmin)dQ=dQmin;
	if (X[1]==0){
		sigma=INF;
	} else {
		sigma=fabs(dQ/X[1]);
	};
break;
case 2: // QSS2
	advance_time(X,sigma,2);
 	q[0]=X[0];
 	q[1]=X[1];
	dQ=dQrel*fabs(X[0]);
	if (dQ<dQmin)dQ=dQmin;
 	if (X[2]==0){
		sigma=INF;
 	} else {
 		sigma=sqrt(fabs(dQ/X[2]));
	};  		
break;
case 3:  // QSS3
  	advance_time(X,sigma,3);
 	q[0]=X[0];
 	q[1]=X[1];
 	q[2]=X[2];
	dQ=dQrel*fabs(X[0]);
	if (dQ<dQmin)dQ=dQmin;
   if (X[3]==0) {
		sigma=INF;
	} else {
		sigma=pow(fabs(dQ/X[3]),1.0/3);
	};  		
break;
case 4:  // QSS4
  	advance_time(X,sigma,4);
 	q[0]=X[0];
 	q[1]=X[1];
 	q[2]=X[2];
 	q[3]=X[3];
	dQ=dQrel*fabs(X[0]);
	if (dQ<dQmin)dQ=dQmin;
   if (X[4]==0) {
		sigma=INF;
	} else {
		sigma=pow(fabs(dQ/X[4]),1.0/4);
	};  		
break;
case 5: //BQSS and CQSS 
case 6: 
	advance_time(X,sigma,1);
	if (sigma>eps) { // X arrives to a new level (q)
		dQ=fabs(X[0])*dQrel; 
		if (dQ<dQmin){dQ=dQmin;};
		ep=dQ/100; 

		if (X[1]>0) {
			qs=qs+dQ;
			q[0]=qs;
			qi=q[0]-2*dQ;
		}else{
			qi=qi-dQ;
			q[0]=qi;
			qs=q[0]+2*dQ;
		}; 
	} else { // slope change
		if(X[1]>0) {
			q[0]=qs;
		}else{
			q[0]=qi;
		};
	};
	if (X[1]!=0) {
		sigma=(q[0]-X[0])/X[1]+2*eps;
	} else {
		sigma=INF;
	};
	band=0;
	break;
}
}
void res_qss_integrator::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value
//     'x.port' is the port number
double *derx;
double diffxq[10];
double dt1;

derx=(double*)x.value;
 
if (x.port==0) {
switch(met) {
case 1: //QSS
   X[0]=X[0]+X[1]*e;
   X[1]=derx[0];
	if (sigma>0){
   	diffxq[1]=-X[1];
   	diffxq[0]=q[0]-X[0]-dQ;
   	sigma=minposroot(diffxq,1);
   	diffxq[0]=q[0]-X[0]+dQ;
   	dt1=minposroot(diffxq,1);
   	if (dt1<sigma) sigma=dt1;
   	if (fabs(X[0]-q[0])>dQ) sigma=0;
	};
	break;  
break;
case 2: // QSS2
   X[0]=X[0]+X[1]*e+X[2]*e*e;
   X[1]=derx[0];
   X[2]=derx[1]/2;
	if (sigma>0){
	   advance_time(q,e,1);
   	diffxq[1]=q[1]-X[1];
		diffxq[2]=-X[2];
   	diffxq[0]=q[0]-X[0]-dQ;
   	sigma=minposroot(diffxq,2);
   	diffxq[0]=q[0]-X[0]+dQ;
   	dt1=minposroot(diffxq,2);
   	if (dt1<sigma) sigma=dt1;
   	if (fabs(X[0]-q[0])>dQ) sigma=0;
	};
	break;
case 3: // QSS3
   X[0]=X[0]+X[1]*e+X[2]*e*e+X[3]*e*e*e;
   X[1]=derx[0];
   X[2]=derx[1]/2;
   X[3]=derx[2]/3;
	if (sigma>0){
	   advance_time(q,e,2);
   	diffxq[1]=q[1]-X[1];
		diffxq[2]=q[2]-X[2];
		diffxq[3]=-X[3];
   	diffxq[0]=q[0]-X[0]-dQ;
   	sigma=minposroot(diffxq,3);
   	diffxq[0]=q[0]-X[0]+dQ;
   	dt1=minposroot(diffxq,3);
   	if (dt1<sigma) sigma=dt1;
   	if (fabs(X[0]-q[0])>dQ) sigma=0;       
	};
break;
case 4: // QSS4
   X[0]=X[0]+X[1]*e+X[2]*e*e+X[3]*e*e*e+X[4]*e*e*e*e;
   X[1]=derx[0];
   X[2]=derx[1]/2;
   X[3]=derx[2]/3;
   X[4]=derx[3]/4;
	if (sigma>0){
	   advance_time(q,e,3);
   	diffxq[1]=q[1]-X[1];
		diffxq[2]=q[2]-X[2];
		diffxq[3]=q[3]-X[3];
		diffxq[4]=-X[4];
   	diffxq[0]=q[0]-X[0]-dQ;
   	sigma=minposroot(diffxq,4);
   	diffxq[0]=q[0]-X[0]+dQ;
   	dt1=minposroot(diffxq,4);
   	if (dt1<sigma) sigma=dt1;
		if ((sigma>1e19)&&(X[4]!=0)) {
			//We suspect numerical error, and try with third order roots
			   	sigma=minposroot(diffxq,3);
			   	diffxq[0]=q[0]-X[0]-dQ;
			   	dt1=minposroot(diffxq,3);
   				if (dt1<sigma) sigma=dt1;
		};			
   	if (fabs(X[0]-q[0])>dQ) {
			sigma=1e-20;
		};       
	};
break;
case 5: //BQSS and CQSS 
case 6:
	if (e*X[1]!=0) {
		X[0]=X[0]+e*X[1];
		if (X[1]>0) { // we check the correct value of qi;
			if (X[0]-qi>=dQ+ep) qi=qi+dQ;
		} else { //we check the correct value of qs; 
			if (qs-X[0]>=dQ+ep) qs=qs-dQ;
		};
	};
	X[1]=derx[0];
	if (t==0) {// initialization
		if (band==1) { //we need to send the output
			sigma=0;
		} else { //the output was already sent and there was a change in u
			if (X[1]*(q[0]-X[0])>=0) { //q is still ok
				if (X[1]!=0) {
					sigma=(q[0]-X[0])/X[1]+eps;
				} else {
					sigma=INF;
				};
			} else { // q is wrong, but we wait an infinitesimal until changing it 
				sigma=eps;
			};
		};
	} else {
		if ((sigma-e)<eps){
			sigma=2*eps;
		}else{ 
			if (e>0) { //derivative change 
				if (X[1]*(q[0]-X[0])>=0) { // q was ok
					if (X[1]!=0) {
						sigma=(q[0]-X[0])/X[1]+2*eps;
					}else{
						sigma=INF;
					};
				} else { //we need to change q
					if (met==5){
						sigma=0;
					}else{
						if (X[1]>0){
							sigma=(qs-X[0])/X[1]+2*eps;
							q[0]=qs;
						}else{
							sigma=(qi-X[0])/X[1]+2*eps;
							q[0]=qi;
						};
					};
				};
			}else { //we had already sent the value q 
				if (X[1]*(q[0]-X[0])>=0) { // q is still ok 
					if (X[1]!=0) {
						sigma=(q[0]-X[0])/X[1]+eps;
					} else {
						sigma=INF;
					}; 
				} else { // q is wrong, but we are close to u=0, so we set u=0.
					X[1]=0;
					sigma=INF;
				};
			};
		};
	};
break;
};

} else {
	//reset 
	advance_time(X,e,order);
	X[0]=derx[0];
	sigma=0;
	if (met>4){ 
		ep=dQ/100;
		//ep=0;
		qs=floor(X[0]/dQ)*dQ+dQ; 
		qi=floor(X[0]/dQ-ep)*dQ; 
		q[0]=qi;
		eps=1e-20;
		band=1;
	};

};
}
Event res_qss_integrator::lambda(double t) {
//This function return an event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% is a direction to the variable that contain the value.
//     %NroPort% is the port number (from 0 to n-1)
switch(met) {
case 1: // QSS
  y[0]=X[0];
  y[1]=X[1];
  advance_time(y,sigma,1);
  y[1]=0;	 
break;
case 2: // QSS2
  y[0]=X[0];
  y[1]=X[1];
  y[2]=X[2];	 
  advance_time(y,sigma,2);
  y[2]=0;	 
break;
case 3: // QSS3
  y[0]=X[0];
  y[1]=X[1];
  y[2]=X[2];	 
  y[3]=X[3];	 
  advance_time(y,sigma,3);
  y[3]=0;	 
break;
case 4: //QSS4
  y[0]=X[0];
  y[1]=X[1];
  y[2]=X[2];	 
  y[3]=X[3];	 
  y[4]=X[4];	 
  advance_time(y,sigma,4);
  y[4]=0;	 
  break;

case 5: //BQSS 
	if (sigma<=eps) { //derivative change
		if (X[1]>0){
			y[0]=qs;
		}else{
			y[0]=qi;
		};
	} else { // X arrives to a new level (q)
		double dQnew;
		dQnew=fabs(X[0]+sigma*X[1])*dQrel;
		if (dQnew<dQmin){dQnew=dQmin;};
		if (X[1]>0){
			y[0]=q[0]+dQnew;
		}else{
			y[0]=q[0]-dQnew;
		};
	};
	break;
case 6: //CQSS
	if (sigma<=eps) { //derivative change
		if (X[1]>0){
			y[0]=q[0];
		} else {
			y[0]=qi;
		};
	} else { // X arrives to a new level (q)
		if (X[1]>0){
			y[0]=q[0]+dQ;
		}else{
			y[0]=q[0]-dQ;
		};
	};
	y[0]=(y[0]+X[0]+sigma*X[1])/2;
	break;

};
return Event(&y,0);
}
void res_qss_integrator::exit() {

}
