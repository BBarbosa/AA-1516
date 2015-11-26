#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include "papi.h"

using namespace std;

#define SIZE 10
#define REPETITIONS 1
#define NUM_EVENTS 2

float m1[SIZE][SIZE],m2[SIZE][SIZE],result[SIZE][SIZE];

long long values[NUM_EVENTS];
int Events[NUM_EVENTS] = {PAPI_TOT_INS,PAPI_TOT_CYC};
int EventSet;
int retval;

void matrixMult(float** mat1, float** mat2, float** res){
    int i,j,k;

    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
        {
            for(k=0;k<SIZE;k++)
            {
                res[i][j] += mat1[i][k] * mat2[k][j];
            }
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
			mat2[i][j] = m2[i][j];
      result[i][j] = res[i][j];
		}
	}

	double *clearcache;
	for (unsigned i = 0; i < REPETITIONS; i++) {
		free(clearcache);
		clearcache = new double [3300000];

		PAPI_start_counters((int*)Events,NUM_EVENTS);
		matrixMult(mat1, mat2, res);
		int retval = PAPI_stop_counters(values,NUM_EVENTS);
	}
  printMatrice(res);

	return 1;
}
