#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include "papi.h"
#include <omp.h>

using namespace std;

#define SIZE 500
#define REPETITIONS 8
#define NUM_EVENTS 1

float m1[SIZE][SIZE],m2[SIZE][SIZE],result[SIZE][SIZE];

long long values[NUM_EVENTS];
//int Events[NUM_EVENTS] = {PAPI_L2_TCM};
//int Events[NUM_EVENTS] = {PAPI_L2_TCA};
//int Events[NUM_EVENTS] = {PAPI_BR_INS};
//int Events[NUM_EVENTS] = {PAPI_L1_TCA};
//int Events[NUM_EVENTS] = {PAPI_LD_INS};
//int Events[NUM_EVENTS] = {PAPI_SR_INS};
//int Events[NUM_EVENTS] = {PAPI_TOT_INS};
//int Events[NUM_EVENTS] = {PAPI_TOT_CYC};
int Events[NUM_EVENTS] = {PAPI_VEC_SP};
//int Events[NUM_EVENTS] = {PAPI_FP_OPS};

int EventSet;
int retval;

void printEvents(int rep) {
	//printf("---- REP %d ----\n",rep);
	printf("%lld\n",values[0]);
	//printf("PAPI_2: %lld\n",values[1]);
}

void matrixMult5(float** mat1, float** mat2, float** res){
    int i,j,k;
		float aux;

    for(k=0;k<SIZE;k++)
    {
        for(i=0;i<SIZE;i++)
        {
						aux = mat1[k][i];
            for(j=0;j<SIZE;j++)
            {
								//res[i][j] += mat1[i][k] * mat2[k][j];
								/* transpose */
								res[i][j] += mat1[k][i] * mat2[k][j];
            }
        }
    }
}

void matrixMult(float** mat1, float** mat2, float** res){
    int i,j,k;
		//float sum;

		#pragma vector always
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
        {
						//sum = 0;
            for(k=0;k<SIZE;k++)
            {
                res[i][j] += mat1[i][k] * mat2[k][j];
								/* transpose */
								//res[i][j] += mat1[i][k] * mat2[j][k];
            }
						//res[i][j] = sum;
        }
    }
}

void fillMatrices(void) {
    for (unsigned i = 0; i < SIZE; ++i) {
    	  for (unsigned j = 0; j < SIZE; ++j) {
			       m1[i][j] = ((float) rand()) / ((float) RAND_MAX);
			       m2[i][j] = ((float) rand()) / ((float) RAND_MAX);
						 //m2[i][j] = 1;
			       result[i][j]  = 0;
		}
	}
}

void printMatrice(float** res){
  for(int i=0; i<SIZE;i++) {
    for(int j=0; j<SIZE;j++) {
      printf("%f ",res[i][j]);
    }
    printf("\n");
  }
}

int main(int argc, char *argv[]){
  float **mat1, **mat2, **res;
	long long unsigned time;

  retval = PAPI_library_init(PAPI_VER_CURRENT);
	retval = PAPI_create_eventset(&EventSet);
	retval = PAPI_add_events(EventSet,Events,NUM_EVENTS);

	mat1 = new float*[SIZE];
	mat2 = new float*[SIZE];
	res = new float*[SIZE];

	for (unsigned i = 0; i < SIZE; ++i) {
		mat1[i] = new float[SIZE];
		mat2[i] = new float[SIZE];
		res[i] = new float[SIZE];
	}

	fillMatrices();

	for (unsigned i = 0; i < SIZE; ++i){
		for (unsigned j = 0; j < SIZE; ++j) {
			mat1[i][j] = m1[i][j];
			/* normal */
			mat2[i][j] = m2[i][j];
			/* transpose */
			//mat2[j][i] = m2[i][j];
      result[i][j] = res[i][j];
		}
	}

	double *clearcache = NULL;
	for (unsigned i = 0; i < REPETITIONS; i++) {
		if(clearcache != NULL) {
			free(clearcache);
		}
		clearcache = new double [3300000];

		double time = omp_get_wtime();

		PAPI_start_counters(Events,NUM_EVENTS);
		matrixMult5(mat1, mat2, res);
		int retval = PAPI_stop_counters(values,NUM_EVENTS);

		time = omp_get_wtime() - time;

		printEvents(i);
		//printf("%f\n",time*1000); //*1000 in miliseconds
	}
  //printMatrice(res);

	return 0;
}
