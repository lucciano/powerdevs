#include "xpow_n.h"
void xpow_n::init(double t,...) {
va_list parameters;
va_start(parameters, t);
n=va_arg(parameters,double);
S[0]=0;
S[1]=0;
S[2]=0;
Sigma=4e10;
for(int i=0;i<10;i++){y[i]=0;};
nm=0;
}
double xpow_n::ta(double t) {
return Sigma;
}
void xpow_n::dint(double t) {
Sigma=4e10;
}
void xpow_n::dext(Event x, double t) {
double *Aux;
Aux=(double*)(x.value);
S[0]=Aux[0];
S[1]=Aux[1];
S[2]=Aux[2];
Sigma=0;
if (Aux[2]!=0) nm=1;
}
Event xpow_n::lambda(double t) {
y[0]=pow(S[0],n);
y[1]=n*pow(S[0],n-1)*S[1];
if (nm==1) y[2]=n*(n-1)*pow(S[0],n-2)*S[1]*S[1]/2+n*pow(S[0],n-1)*S[2];
return Event(&y[0],0);
}
void xpow_n::exit() {

}
