//CPP:qss/qsstools.cpp

//CPP:qss/res_qss_integrator.cpp
#if !defined res_qss_integrator_h
#define res_qss_integrator_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "math.h"
#include "string.h"
#include "qsstools.h"


class res_qss_integrator: public Simulator { 
// Declare here the state, output
// variables and parameters
double X[10],q[10];
double sigma;

double qs,qi;

//parameters
double dQ;
char* Method;
double dQmin,dQrel;
int met,order;

double eps,ep;
int band;

double y[10]; //output















public:
	res_qss_integrator(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
