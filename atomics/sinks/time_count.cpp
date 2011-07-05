#include "time_count.h"
void time_count::init(double t,...) {
char* FName;
va_list parameters;
va_start(parameters, t);
FName=va_arg(parameters, char*);
tend=va_arg(parameters, double);
FOutput = fopen(FName, "w");
fclose(FOutput);
FOutput = fopen(FName, "a");
Sigma=0;
}
double time_count::ta(double t) {
return Sigma;
}
void time_count::dint(double t) {
if (Sigma==0) {
   intim=clock();
   Sigma=tend;
} else {
  fprintf(FOutput, "%g\n",(double)(clock()-intim)/CLOCKS_PER_SEC);
  fclose(FOutput);
  Sigma=4e10;
};
}
void time_count::dext(Event x, double t) {

}
Event time_count::lambda(double t) {
return Event(0,0);
}
void time_count::exit() {

}
