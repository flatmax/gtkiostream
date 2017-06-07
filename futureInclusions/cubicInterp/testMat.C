#include <fstream>
#include <iostream>
using namespace std;

#include "f2c.h"

extern "C" int pchim_(integer *n, real *x, real *f, real *d__, integer *incfd, integer *ierr);
extern "C" int pchfe_(integer *n, real *x, real *f, real *d__, integer *incfd, logical *skip, integer *ne, real *xe, real *fe, integer *ierr);

int main(int argc, char *argv[]){

ifstream input("cdata.txt");
float temp;

cout<<"reading input data"<<endl;

input>>temp;
int N=(int)temp; // the number of reference data points
float x[N], y[N], x1[1024];
cout<<"x="<<endl;
for (int i=0;i<N;i++){
input>>x[i];
cout<<x[i]<<'\t';
}
cout<<endl;
cout<<"y="<<endl;
for (int i=0;i<N;i++){
input>>y[i];
cout<<y[i]<<'\t';
}
cout<<endl;
int N1=0;
cout<<"x1="<<endl;
while (input>>x1[N1]){
cout<<x1[N1]<<'\t';
	N1++;
}
cout<<endl;
input.close();

 cout<<"N="<<N<<endl;
 cout<<"N1="<<N1<<endl;

// find the point derivatives
float d1[1024]; // the derivatives
int err, step=1;
int test=pchim_(&N, x, y, d1, &step, &err);
cout<<"returns "<<test<<endl;

// call the Hermit interpolation ....
logical skip=1;
float y1[1024]; // the interpolates
test=pchfe_(&N, x, y, d1, &step, &skip, &N1, x1, y1, &err);
cout<<"returns "<<test<<endl;

ofstream output("cdataOut.txt");
for (int i=0;i<N1;i++)
	output<<y1[i]<<'\t';
output.close();
return 0;
}
