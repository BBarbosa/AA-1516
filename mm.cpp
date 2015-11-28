#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include "papi.h"
#include <omp.h>

using namespace std;

#define SIZE 10
#define REPETITIONS 4
#define NUM_EVENTS 4

//int Events[NUM_EVENTS] = {PAPI_L1_TCM,PAPI_L2_TCM,PAPI_L3_TCM,PAPI_BR_INS};
//int Events[NUM_EVENTS] = {PAPI_L1_TCA,PAPI_L2_TCA,PAPI_L3_TCA,PAPI_LD_INS};
int Events[NUM_EVENTS] = {PAPI_TOT_INS,PAPI_TOT_CYC,PAPI_FP_INS,PAPI_FP_OPS};
long long values[NUM_EVENTS];
int EventSet;

float m1[SIZE][SIZE],m2[SIZE][SIZE],result[SIZE][SIZE];


void matrixMult5(float** mat1, float** mat2, float** res){
    int i,j,k;

    for(k=0;k<SIZE;k++)
    {
        for(i=0;i<SIZE;i++)
        {
            for(j=0;j<SIZE;j++)
            {
                res[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

void matrixMult(float** mat1, float** mat2, float** res){
    int i,j,k;
		//float sum;

    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
        {
						//sum = 0;
            for(k=0;k<SIZE;k++)
            {
                res[i][j] += mat1[i][k] * mat2[k][j];
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

int comp (const void * elem1, const void * elem2) 
{
    double f = *((double*)elem1);
   	double s = *((double*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}

int main(int argc, char *argv[]){
	float **mat1, **mat2, **res;
	double *clearcache = NULL;
	int retval;
	FILE *f;

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


	//faz a medição do tempo
	double time[REPETITIONS];
	for (unsigned i = 0; i < REPETITIONS; i++) 
	{

		if(clearcache != NULL) 
		{
			free(clearcache);
		}
		clearcache = new double [3300000];
		

		time[i] = omp_get_wtime();
		matrixMult(mat1, mat2, res);
		time[i] = omp_get_wtime() - time[i];
		
	}
	qsort (time, sizeof(time)/sizeof(*time), sizeof(*time), comp);
	f = fopen("timeResults.txt","w");
	for (unsigned i = 0; i < REPETITIONS; i++) 
	{
		fprintf(f,"%f\n",time[i]*1000);
	}
	fclose(f);


	//faz medição do papi
	retval = PAPI_library_init(PAPI_VER_CURRENT);
	retval = PAPI_create_eventset(&EventSet);
	retval = PAPI_add_events(EventSet,Events,NUM_EVENTS);
	f = fopen("Results.txt","w");
	for (unsigned i = 0; i < REPETITIONS; i++) {
		
		if(clearcache != NULL) {
			free(clearcache);
		}
		clearcache = new double [3300000];
		

		PAPI_start_counters(Events,NUM_EVENTS);
		matrixMult(mat1, mat2, res);
		int retval = PAPI_stop_counters(values,NUM_EVENTS);

		fprintf(f,"PAPI_TOT_INS: %lld\n",values[0]);
		fprintf(f,"PAPI_TOT_CYC: %lld\n",values[1]);
		fprintf(f,"PAPI_FP_INS: %lld\n",values[2]);
		fprintf(f,"PAPI_FP_OPS: %lld\n\n",values[3]);
	}
	fclose(f);

	//printMatrice(res);

	return 0;
}
