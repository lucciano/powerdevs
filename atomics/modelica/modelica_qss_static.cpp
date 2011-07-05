#include "modelica_qss_static.h"
void modelica_qss_static::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//      %Type% is the parameter type
//We get the parameters here
Inputs = (int)va_arg(parameters,double);
Outputs = (int)va_arg(parameters,double);
Index = (int)va_arg(parameters,double);
Method=int_method();
abs_accuracy=function_abs_acc();
rel_accuracy=function_rel_acc();

//printLog("Setting up static function %i with %i Inputs and %i Outputs\n",Index,Inputs, Outputs);

//Initialization of states and outputs

dt=1e-6; 

for (int i=0;i<10;i++) {
	for (int j=0;j<Inputs;j++) {
		u[j][i]=0;
		uaux[j][i]=0;
	};
	y[i]=0;
	inp[i]=0;
};


if (strcmp(Method,"QSS")==0 || strcmp(Method,"LIQSS")==0) { 
	order=1;
};

if (strcmp(Method,"QSS2")==0 || strcmp(Method,"LIQSS2")==0) {
	order=2;
};
if (strcmp(Method,"QSS3")==0 || strcmp(Method,"LIQSS3")==0 || strcmp(Method,"MIXED_LIQSS3")==0 ){
	order=3;
};
if (strcmp(Method,"QSS4")==0){
	order=4;
};
if ((strcmp(Method,"BQSS")==0)||(strcmp(Method,"CQSS")==0)){
	order=1; 
};

lastport=Outputs;

function_staticBlocks(Index,t,inp,outp); //evaluate in case no input
					  //arrives
  	for (int i=0;i<Outputs;i++) {
		f[i][0]=outp[i];
	} 

//Start with a small step (in case no input arrives)
sigma=1e-8;
}
double modelica_qss_static::ta(double t) {
//This function returns a double.
return sigma;
}
void modelica_qss_static::dint(double t) {
double ddf;
if (sigma>0) { //We did not received any events

	switch(order) { 

	case 1: 
		for (int i=0;i<Inputs-1;i++) {
			inp[i]=u[i][0];
		}
		function_staticBlocks(Index,t,inp,outp); //evaluate the right hand side
		for (int i=0;i<Outputs;i++) {
			f[i][0]=outp[i];
		}

   break;

	case 2: 
		for (int i=0;i<Inputs-1;i++) {
				advance_time(u[i],sigma,1);	
		};
		for (int i=0;i<Inputs-1;i++) {
			inp[i]=u[i][0];
		}
		function_staticBlocks(Index,t,inp,outp); 
 	  //we evaluated the right hand side...
		for (int i=0;i<Inputs-1;i++) {
			inp[i]=evaluate_poly(u[i],dt,1);
		}
		function_staticBlocks(Index,t+dt,inp,outdt); 
		for (int i=0;i<Inputs-1;i++) {
			inp[i]=evaluate_poly(u[i],-dt,1);
		}
		function_staticBlocks(Index,t-dt,inp,out_dt); 
 		for (int i=0;i<Outputs;i++) {
			f[i][0]=outp[i];
			f[i][1]=(outdt[i]-out_dt[i])/(2*dt);
		} 	
	break;

case 3: 
	for (int i=0;i<Inputs-1;i++) {
			advance_time(u[i],sigma,2);
	};
	for (int i=0;i<Inputs-1;i++) {
		inp[i]=u[i][0];
	}
	function_staticBlocks(Index,t,inp,outp); 
   //we evaluated the right hand side...
	for (int i=0;i<Inputs-1;i++) {
		inp[i]=evaluate_poly(u[i],dt,2);
	}
	function_staticBlocks(Index,t+dt,inp,outdt); 
 //we evaluated the right hand side again...
	for (int i=0;i<Inputs-1;i++) {
		inp[i]=evaluate_poly(u[i],-dt,2);
	}
	function_staticBlocks(Index,t-dt,inp,out_dt); 
 	for (int i=0;i<Outputs;i++) {
		f[i][0]=outp[i];
		f[i][1]=(outdt[i]-out_dt[i])/(2*dt);
		f[i][2]=(outdt[i]-2*outp[i]+out_dt[i])/(dt*dt*2);
	} 	
	break;

	}

	lastport=Outputs-1;
	sigma=0;

} else {	// Sigma ==0
	if (lastport==0) {
		sigma=INF;

		switch(order) {

		case 1:
			for (int i=0;i<Inputs-1;i++) {
				inp[i]=u[i][0];
			}
			function_staticBlocks(Index,t+dt,inp,outp); //evaluate the right hand side
			for (int i=0;i<Outputs;i++) {
				//printLog("With Index= %d; We evaluate f[i][0]= %g  and outp[i]=%g \n",Index,f[i][0],outp[i]);
				if (outp[i]!=f[i][0]) {
					tolerr=abs_accuracy+rel_accuracy*fabs(f[i][0]);
					dtmax=tolerr*dt/fabs(outp[i]-f[i][0])+1e-12;
					if (dtmax<sigma){
						sigma=dtmax;
					}
				}	
			}
		if (sigma>1e-12){dt=sigma/100;}

		break;

		case 2:
			for (int i=0;i<Inputs-1;i++) {
				inp[i]=evaluate_poly(u[i],dt,1);
			}
			function_staticBlocks(Index,t+dt,inp,outdt); 
			for (int i=0;i<Inputs-1;i++) {
				inp[i]=evaluate_poly(u[i],-dt,1);
			}
			function_staticBlocks(Index,t-dt,inp,out_dt); 
 
 			for (int i=0;i<Outputs;i++) {
        ddf=(outdt[i]-2*f[i][0]-out_dt[i])/(dt*dt*2);
				if (ddf!=0) {
					tolerr=abs_accuracy+rel_accuracy*fabs(f[i][0]);
					dtmax=dt*sqrt(fabs(tolerr/ddf))+1e-8;
					if (dtmax<sigma){
						sigma=dtmax;
					}
				}					
			}
			if (sigma>1e-8){dt=sigma/100;} 			
		 // sigma=INF;	
		break;

		case 3:
			for (int i=0;i<Inputs-1;i++) {
				inp[i]=evaluate_poly(u[i],dt,2);
			}
			function_staticBlocks(Index,t+dt,inp,outdt); 
			for (int i=0;i<Inputs-1;i++) {
				inp[i]=evaluate_poly(u[i],-dt,2);
			}
			function_staticBlocks(Index,t-dt,inp,out_dt); 
			for (int i=0;i<Inputs-1;i++) {
				inp[i]=evaluate_poly(u[i],2*dt,2);
			}
			function_staticBlocks(Index,t+2*dt,inp,out2dt); 
			for (int i=0;i<Inputs-1;i++) {
				inp[i]=evaluate_poly(u[i],-2*dt,2);
			}
			function_staticBlocks(Index,t-2*dt,inp,out_2dt); 

 			for (int i=0;i<Outputs;i++) {
        double f3 = (out2dt[i]-2*outdt[i]+2*out_dt[i]-out_2dt[i])/(12*dt*dt*dt);
				if (f3!=0) {
					tolerr=abs_accuracy+rel_accuracy*fabs(f[i][0]);
					dtmax=pow(fabs(tolerr/f3),1/3.0)+1e-12;
					if (dtmax<sigma){
					  sigma=dtmax;
					}
				}
			}
		break;


		}
    	lastport=Outputs; //next event goes nowhere
		//printLog("t=%g: Static Func[%i]: We set sigma=%g \n",t,Index,sigma);
  } else {
    sigma=0;
    lastport--;
  };

}

}
void modelica_qss_static::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number


if (x.port!=Inputs-1) { //not a dummy event 
	for (int j=0;j<order;j++) {
		 u[x.port][j]=x.getDouble(j);
	}
	dummy=false;
	if (e>1e-5){dt=e/100;}
} else {
  dummy=true;
//  printLog("Dummy event\n");
}

sigma=0; 


switch(order) { 
case 1: 
	for (int i=0;i<Inputs-1;i++) {
		inp[i]=u[i][0];
	}
	function_staticBlocks(Index,t,inp,outp); //evaluate the right hand side
  for (int i=0;i<Outputs;i++) {
		f[i][0]=outp[i];
	} 
	
 
break;

case 2: 
	for (int i=0;i<Inputs-1;i++) {
		if (i!=x.port) {
			advance_time(u[i],e,1);
		};
	};
	for (int i=0;i<Inputs-1;i++) {
		inp[i]=u[i][0];
	}
	function_staticBlocks(Index,t,inp,outp); 
   //we evaluated the right hand side...
	for (int i=0;i<Inputs-1;i++) {
		inp[i]=evaluate_poly(u[i],dt,1);
	}
	function_staticBlocks(Index,t+dt,inp,outdt); 
	for (int i=0;i<Inputs-1;i++) {
		inp[i]=evaluate_poly(u[i],-dt,1);
	}
	function_staticBlocks(Index,t-dt,inp,outdt); 
 
 	for (int i=0;i<Outputs;i++) {
		f[i][0]=outp[i];
		f[i][1]=(outdt[i]-out_dt[i])/(2*dt);
	} 	

break;

case 3: 
	for (int i=0;i<Inputs-1;i++) {
		if (i!=x.port) {
			advance_time(u[i],e,2);
		};
	};
	for (int i=0;i<Inputs-1;i++) {
		inp[i]=u[i][0];
	}
	function_staticBlocks(Index,t,inp,outp); 
  //we evaluated the right hand side...
	for (int i=0;i<Inputs-1;i++) {
		inp[i]=evaluate_poly(u[i],dt,2);
	}
	function_staticBlocks(Index,t+dt,inp,outdt);
	for (int i=0;i<Inputs-1;i++) {
		inp[i]=evaluate_poly(u[i],-dt,2);
	}
	function_staticBlocks(Index,t-dt,inp,out_dt); 

 	for (int i=0;i<Outputs;i++) {
		f[i][0]=outp[i];
		f[i][1]=(outdt[i]-out_dt[i])/(2*dt);
		f[i][2]=(outdt[i]-2*outp[i]+out_dt[i])/(dt*dt*2);
	} 	
  //printLog("[t=%g] Function value (%d) is {%g,%g,%g}\n",t,Index,f[0][0],f[0][1],f[0][2]);

break;

}

lastport=Outputs-1;
}
Event modelica_qss_static::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)

if (sigma==0) {
	for (int i=0;i<order;i++) { 
		y[i]=f[lastport][i];
	};
//	printLog("Emitting %g through port %d\n",y[0],lastport);
} else {
	//printLog("Emitting %g %g through port %d\n",y[0],y[1],lastport);
  //printLog("sigma>0\n");
}; 
return Event(y,lastport);
}
void modelica_qss_static::exit() {

}
