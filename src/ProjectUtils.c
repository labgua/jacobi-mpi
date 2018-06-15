/*
 * ProjectUtils.c
 *
 *  Created on: 09 giu 2018
 *      Author: sergio
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/ProjectUtils.h"

double** create_matrix(int rows_cols){

	double** M = malloc( sizeof(double*) * rows_cols);

	for( int i = 0; i < rows_cols; i++ ){
		M[i] = malloc( sizeof(double) * rows_cols );
	}

	return M;

}


double** create_matrix_zero(int num_rows, int num_cols, enum GHOST_OPTION gopt){

	int rows_for_ghost = abs(gopt);

	///fprintf(stderr, "!!!!!!!!!!!!! num_rows_in_new_mat = %d\n", rows_cols + rows_for_ghost);

	num_rows += rows_for_ghost;

	double** M = malloc( sizeof(double*) * num_rows );

	for( int i = 1; i < num_rows - 1; i++ ){   /// la prima e l'ultima sono allocate e gestite nel main
		M[i] = calloc( num_cols, sizeof(double));
	}

	return M;

}


double** gen_matrix(int rows_cols){

	int i, j;

	///double** M = create_matrix(rows_cols);
	double** M = malloc( sizeof(double*) * rows_cols);

	for( int i = 0; i < rows_cols; i++ ){
		M[i] = malloc( sizeof(double) * rows_cols );
	}



	srand(SEED);

	for( i = 0; i < rows_cols; i++ ){
		for( j = 0; j < rows_cols; j++ ){
			M[i][j] = (double)(rand() % MAX_NUM);
		}
	}

	return M;

}

void fprint_smatrix(FILE* f, double** M, int rows_cols){

	for( int i = 0; i < rows_cols; i++ ){
		for( int j = 0; j < rows_cols; j++ ){
			fprintf(f, "%3.3f ", M[i][j]);
		}
		fprintf(f, "\n");
	}

}

void fprint_matrix(FILE* f, double** M, int num_rows, int num_cols){

	for( int i = 0; i < num_rows; i++ ){
		for( int j = 0; j < num_cols; j++ ){
			fprintf(f, "%3.3f\t", M[i][j]);
		}
		fprintf(f, "\n");
	}

}

void fprint_matrix_buffered(FILE* f, char* label, double** M, int num_rows, int num_cols){

	char* buf = calloc(num_rows * num_cols * 10, sizeof(char));
	char* line = calloc(num_cols*15, sizeof(char));

	sprintf(line, "%s num_rows:%d -- \n", label, num_rows);
	strcat(buf, line);

	for( int i = 0; i < num_rows; i++ ){
		for( int j = 0; j < num_cols; j++ ){
			sprintf(line, "%3.3f\t", M[i][j]);
			strcat(buf, line);
		}
		strcat(buf, "\n");
	}

	fflush(f);
	//fprintf(f, "%s\n", buf);
	fputs(buf, f);
	fflush(f);

}

void fprint_row(FILE* f, char* label, double* A, int n){

	fprintf(f,"%s -- [ ", label);
	for(int i = 0; i < n; i++){
		fprintf(f,"%lf, ", A[i]);
	}
	fprintf(f," ]\n");
}

int file_exist (char *filename){
  struct stat buffer;
  return ( stat(filename, &buffer) == 0);
}


double** load_matrix( char* name_file, int num_row, int num_cols, int offset_row, enum GHOST_OPTION gopt ){


	int rows_for_ghost =  abs(gopt);
	////fprintf(stderr, "GHOST, adding %d rows\n", rows_for_ghost);


	FILE* fp_mat = fopen(name_file, "r");
	if( fp_mat == NULL ){
		fprintf(stderr, "FILE %s NOT FOUND!!\n", name_file);
		return NULL;
	}

	int i,j;


	/// alloca lo spazio per la matrice
	double** matrix = malloc( (num_row + rows_for_ghost) * sizeof(double*) );

	for( i = 0; i < num_row + rows_for_ghost; i++ ){
		matrix[i] = calloc(num_cols, sizeof(double));
	}


	/// genera l'offset nel file, per righe [hp partzionamento orizzontale]..
	//char* npt = NULL;
	//size_t len = 0;
	for( i = 0; i < offset_row; i++ ){
		fscanf(fp_mat, "%*[^\n]\n");
		///getline(&npt, &len, fp_mat); NON C STANDARD
	}


	/// quindi leggi la sottomatrice (hp partizionata orizzontalemnte)
	/// e gestisci i ghost [UP, DOWN, BOTH, NONE]
	double buf_num;
	i = 0;
	if( gopt == GHOST_BOTH || gopt == GHOST_UP ){
		i = 1;
		num_row++;
	}
	for( ; i < num_row; i++ ){
		for(j = 0; j < num_cols; j++){
			if( fscanf(fp_mat, "%lf", &buf_num) == 1 ){
				matrix[i][j] = buf_num;
			}
			else{
				fprintf(stderr, "ERRORE LETTURA FILE!\n");
				return matrix;
			}
		}
	}

	fclose(fp_mat);

	return matrix;

}


void save_matrix(char* path_file, double** M, int dim_matrix){

	FILE* fp;

	fp = fopen(path_file, "w");
	if( fp == NULL ){
		fprintf(stderr, "ERRORE APERTURA FILE!\n");
		return;
	}

	fprint_matrix(fp, M, dim_matrix, dim_matrix);

	fclose(fp);
	return;

}