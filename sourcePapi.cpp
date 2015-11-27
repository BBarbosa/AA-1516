#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include "papi.h"

using namespace std;

#define SIZE 10
#define REPETITIONS 2
#define NUM_EVENTS 4

float m1[SIZE][SIZE],m2[SIZE][SIZE],result[SIZE][SIZE];

long long values[NUM_EVENTS];
int Events[NUM_EVENTS] = {PAPI_FP_INS,PAPI_SR_INS,PAPI_LD_INS,PAPI_BR_INS};
//			 {PAPI_FP_INS,PAPI_SR_INS,PAPI_LD_INS,PAPI_BR_INS,PAPI_VEC_SP,PAPI_VEC_DP,
//			  PAPI_FP_OPS,PAPI_SP_OPS,PAPI_DP_OPS,PAPI_TOT_INS,
//			  PAPI_L1_TCM,PAPI_L2_TCM,PAPI_L1_TCA,PAPI_L2_TCA,PAPI_TOT_CYC};
int EventSet;
int retval;

void printEvents(int rep) {
	printf("---- REP %d ----\n",rep);
	printf("PAPI_FP_INS: %lld\n",values[0]);
	printf("PAPI_SR_INS: %lld\n",values[1]);
	printf("PAPI_LD_INS: %lld\n",values[2]);
	printf("PAPI_BR_INS: %lld\n",values[3]);
}

void matrixMult(float** mat1, float** mat2, float** res){
    int i,j,k;
		float sum;

    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
        {
						sum = 0;
            for(k=0;k<SIZE;k++)
            {
                sum += mat1[i][k] * mat2[k][j];
            }
						res[i][j] = sum;
        }
    }
}

void fillMatrices(void) {
    for (unsigned i = 0; i < SIZE; ++i) {
    	  for (unsigned j = 0; j < SIZE; ++j) {
			       m1[i][j] = ((float) rand()) / ((float) RAND_MAX);
			       m2[i][j] = ((float) rand()) / ((float) RAND_MAX);
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
			//mat2[i][j] = m2[i][j];
			mat2[j][i] = m2[i][j]; //trasnpose
      result[i][j] = res[i][j];
		}
	}

	double *clearcache = NULL;
	for (unsigned i = 0; i < REPETITIONS; i++) {
		if(clearcache != NULL) {
			free(clearcache);
		}
		clearcache = new double [3300000];

		PAPI_start_counters(Events,NUM_EVENTS);
		matrixMult(mat1, mat2, res);
		int retval = PAPI_stop_counters(values,NUM_EVENTS);

		printEvents(i);
	}
  	//printMatrice(res);

	return 1;
}
