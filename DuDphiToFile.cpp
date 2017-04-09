/*
to compile and run..

g++ DuDphi.cpp -lgsl -lgslcblas -lm -g -Wall

*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#include <math.h>

using namespace std;

enum endState { BH , Orbit, Inf };

int func (double t, const double y[], double f[], void *params) {
	(void)(t); /* avoid unused parameter warning */

	/*Define ODE system for d2u/dv2 == 3u^2 - u*/
	f[0] = y[1];	/*du/dphi = v*/
	f[1] = 3.0*(y[0]*y[0])-y[0]; /*dv/dphi = 3u^2 - u*/

	return GSL_SUCCESS;
}

int jac (double t, const double y[], double *dfdy, double dfdt[], void *params) {
  (void)(t); /* avoid unused parameter warning */
  (void)(params);
  dfdt[1] = 0.0;
  dfdt[0] = 0.0;

  /*Create Matrix*/
  gsl_matrix_view dfdy_mat 
    = gsl_matrix_view_array (dfdy, 2, 2);
  gsl_matrix * m = &dfdy_mat.matrix; 

  /*Define partial derivatives*/
  gsl_matrix_set (m, 0, 0, 0.0); // df0/du
  gsl_matrix_set (m, 0, 1, 1.0); // df0/dv
  gsl_matrix_set (m, 1, 0, 6*y[0] - 1.0); //df1/du
  gsl_matrix_set (m, 1, 1, 0);   // df1/dv

  return GSL_SUCCESS;
}

endState makePrediction(double r0, double s0d) {
	double Br0 =  r0/sqrt(1.0-2.0/r0);
	double threerootthreeoverBr0 = 3.0*sqrt(3.0)/Br0;
	double sinSig0 = sin(s0d*M_PI/180.0);

	endState p = BH;

	if (r0 < 2.0){
		p=BH;
	} else if (r0<3.0) {
		if (s0d<90.0 && sinSig0<threerootthreeoverBr0){
			p=Inf;
		} else 
			p=BH;
	} else if ( r0==3.0 ){
		if (s0d>90.0){
			p=BH;
		} else if (s0d==90.0){
			p=Orbit;
		} else
			p=Inf;
	} else {
		if (s0d<=90.0 || (s0d>90.0 && sinSig0>threerootthreeoverBr0) ){
			p=Inf;
		} else 
			p=BH;
	}

  // cout << "sinSig0" << sinSig0 << "\tthreerootthreeoverBr0" << threerootthreeoverBr0 << endl;

	// switch (p){
	// 	case BH: cout << "Predict=BH\n"; break;
	// 	case Orbit: cout << "Predict=Orbit\n"; break;
	// 	default: cout << "Predict=Inf\n";
	// }

	return p;
}

int main (int argc, char *argv[]) {

	FILE *file;
	double mu = 10;
	gsl_odeiv2_system sys = { func, jac, 2, &mu };

	gsl_odeiv2_driver *d =
	gsl_odeiv2_driver_alloc_y_new (/*defined system*/&sys,
				/*stepper function*/gsl_odeiv2_step_rk8pd,
				/*initial step (radians)*/1e-20,
				/*max abs err*/1e-10,
				/*max rel err*/1e-10);

	double radius0 = atof(argv[1]);
	double phi0 = 0.0L;
	double toRadians =(M_PI/180.0);

	double sigma0d = atof(argv[2]);
	double sigma0r = sigma0d*toRadians;
	double u0 = 1./radius0; /*u = 1/r*/

	double t = phi0; /*the variable that is incremented*/
	double y[2] = {u0,-u0/tan(sigma0r)}; /*the dependent variables*/
	int s; /*error trap*/
	double xval,yval;

	/* Make Qualitative Prediction...*/
	endState predict=makePrediction(radius0,sigma0d);

	/* Open file */
	// char buf[50];
	// sprintf(buf,"plots/r%.1fs%.2f.tsv",radius0,sigma0d);
	// printf("%s\n",buf); /*print filename*/ 
	char buf[50]="orbit.txt";
	file = fopen(buf,"w");

	/* Print first two lines of file */
	// fprintf(file,"x\ty\n");	/*print just x,y pairs*/
	fprintf(file,"[[%.10f,%.10f,0.0],",cos(t)/y[0],sin(t)/y[0]);


	/*loop variables*/
	double stepSize=1e-6;
	double numSteps=1000;
	int i=0;
	double fourLoops = 4.0*2.0*M_PI;

	while (y[0] > 0.001 && y[0] < 0.5 && t< fourLoops && i<100000) {
		i++;
		s = gsl_odeiv2_driver_apply_fixed_step (d, &t, stepSize, numSteps, y);

		if (s != GSL_SUCCESS) {
			fprintf (stderr, "failed, gsl_errno=%d\n", s);
			printf ("error: %s\n", gsl_strerror (s));
			exit (-1);
		}

		xval=cos(t)/y[0];
		yval=sin(t)/y[0];

		if (i%100==0)
			fprintf(file,"[%.10e,%.10e,0.0],",xval,yval);
	}

	endState final;
	int err=0;
	if (y[0] < 0.001) {
		// printf("Stopping because large r: %.5f\nPrediction? => %d\n",1.0/y[0],predict==Inf);
		final=Inf;
	} else if (y[0] > 0.5) {
		// printf("Stopping because fell in BH r=%.5f\nPrediction? => %d\n",1.0/y[0], predict==BH);
		final=BH;
	} else if (i>100000){
		// printf("Exceeded maximum iterations\nPrediction? => 0\n");
		err=-1;
	} else {
		// printf("Stopping becsause stable orbit\nPrediction? => %d\n", predict==Orbit);
		final=Orbit;
	}

	if (err==-1) fprintf(file,"[%d,%d]]",(int)predict,err);
	else fprintf(file,"[%d,%d]]",(int)predict,(int)final);

	fclose(file);
	gsl_odeiv2_driver_free (d);
	return 0;
}
