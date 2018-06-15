/*
 * ProjectUtils.h
 *
 *  Created on: 09 giu 2018
 *      Author: sergio
 */

#ifndef PROJECTUTILS_H_
#define PROJECTUTILS_H_

#include <stdio.h>

//#define DEBUG 0
#define DD if(DEBUG)

#define DELTA_APPROX 1.0E-2
#define SEED 1
#define MAX_NUM 100
#define MAX_ITERATION 100

#define MASTER 0
#define FIRST  0
#define LAST   p-1

#define MSG_STOP_COMPUTATION 1
#define MSG_CONTINUE_COMPUTATION 0


enum GHOST_OPTION {
	GHOST_UP = -1,
	GHOST_DOWN = +1,
	GHOST_BOTH = +2,
	GHOST_NONE = 0
};



///// PARAMS DATASET WEAK SCALABILITY /////

#define MIN_ROW_PER_PROC 600
#define NUM_MAX_PROC_FOR_WEAK_SCALABILITY 8
#define VCPU_PER_INSTANCE 2

//////////////////////////////////////////


/**
 * Create a square matrix random with numbers in range [0; MAX_NUM]
 */
double** gen_matrix(int rows_cols);

/**
 * Create a square matrix in head with size (row_cols x row_cols) NOT INITIALIZED
 */
double** create_matrix(int rows_cols);

/**
 * Create a square matrix in head with size (row_cols x num_cols) WITH ZERO
 */
double** create_matrix_zero(int num_rows, int num_cols, enum GHOST_OPTION gopt);

/**
 * Load a generic matrix with size (num_row x num_cols) from file.
 * The offset define the numbers of rows to escape before the reading
 * The GHOST_OPTION define how and where it has to make a ghost-border in matrix
 */
double** load_matrix( char* name_file, int num_row, int num_cols, int offset_row, enum GHOST_OPTION gopt);

/**
 * Print on file a square double matrix
 */
void fprint_smatrix(FILE* f, double** M, int rows_cols);

/**
 * Print on file a double matrix generic
 */
void fprint_matrix(FILE* f, double** M, int num_rows, int num_cols);

/**
 * Print a double row with a label (buffered, with fprintf) [used for MPI debugging]
 */
void fprint_row(FILE* f, char* label, double* A, int n);


/**
 * Print a double matrix (buffered) [used for MPI debugging]
 */
void fprint_matrix_buffered(FILE* f, char* label, double** M, int num_rows, int num_cols);

/**
 * Check if file exists.
 * Returns 1 if file exists 0 otherwise
 */
int file_exist (char *filename);

/**
 * Save matrix on file
 */
void save_matrix(char* path_file, double** M, int dim_matrix);

#endif /* PROJECTUTILS_H_ */
