/*
 ============================================================================
 Name        : JacobiSeq.c
 Author      : Sergio Guastaferro
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../include/ProjectUtils.h"

#define DEBUG 0

int main(int argc, char* argv[]) {

	int i, j;
	double diffnorm;
	double** mat;

	int num_rows_cols;
	char* input_file;


	if( argc < 2 ){
		printf("USAGE \n%s <DIM_MATRIX> <INPUT_FILE> \n", argv[0]);
		printf("%s <DIM_MATRIX>\n", argv[0]);
		exit(-1);
	}

	if( argc >= 2 ){
		num_rows_cols = atoi(argv[1]);
		fprintf(stderr, "num_rows_cols=%d\n", num_rows_cols);
	}

	if( argc == 3 ){ //caricamento da file

		if( !file_exist(argv[2]) ){
			printf("File %s doesn't exist!\n", argv[2]);
			exit(-1);
		}
		input_file = argv[2];
		fprintf(stderr, "File %s : OK\n", input_file);

		mat = load_matrix(input_file, num_rows_cols, num_rows_cols, 0, GHOST_NONE);

	}

	else if( argc == 2 ){ // genera matrice
		mat = gen_matrix(num_rows_cols);
	}


	clock_t start_inst = clock();

	double** newmat = create_matrix_zero(num_rows_cols, num_rows_cols, GHOST_NONE);

	DD fprint_smatrix(stdout, mat, num_rows_cols);

	int k;
	for( k = 0; k < MAX_ITERATION; k++ ){

		/// MAKE_ITERATION()
		///make_iteration(mat, newmat, NUM_ROWS_COLS);
		for( i = 1; i < num_rows_cols - 1; i++ ){
			for( j = 1; j < num_rows_cols - 1; j++ ){
				newmat[i][j] = (mat[i+1][j] + mat[i-1][j] + mat[i][j+1] + mat[i][j-1])/4;
			}
		}

		//DEBUG
		DD printf("ITERAZIONE %d\n", k);
		/////DD fprint_smatrix(stdout, mat, NUM_ROWS_COLS);
		DD fprint_smatrix(stdout, mat, num_rows_cols);

		/// TEST_CONVERGENZA()
		///if( test_convergenza(mat, newmat, NUM_ROWS_COLS) == 1 ) break;
		diffnorm = 0.0;
		for( i = 1; i < num_rows_cols - 1; i++ ){
			for( j = 1; j < num_rows_cols - 1; j++ ){
				diffnorm += ( (newmat[i][j] - mat[i][j]) * (newmat[i][j] - mat[i][j]) );
			}
		}

		DD printf("#####TestConvergenza --> diffnorm=%.3f\n", diffnorm);

		DD {
			if( (diffnorm - sqrt( diffnorm )) < DELTA_APPROX  ) break;
		}


		/// NEW_IN_OLD()
		////new_in_old(mat, newmat, NUM_ROWS_COLS);
		for( i = 1; i < num_rows_cols - 1; i++ ){
			for( j = 1; j < num_rows_cols - 1; j++ ){
				mat[i][j] = newmat[i][j];
			}
		}


	}

	clock_t stop_inst = clock();

	double timer = (double)(stop_inst - start_inst) / CLOCKS_PER_SEC;

	printf("RESULT SEQUENTIAL BENCHMARK :: DimMatrix:%d   NumDoneIter:%d   NumMaxIter:%d   Tempo:%lf s\n", num_rows_cols, k, MAX_ITERATION, timer);
	DD fprint_smatrix(stdout, mat, num_rows_cols);


}
