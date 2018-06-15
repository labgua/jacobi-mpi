/*
 ============================================================================
 Name        : JacobiGen.c
 Author      : Sergio Guastaferro
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ProjectUtils.h"

#define OUTPUT_PATH_FILE "data/"

int main(int argc, char **argv) {

	char label[30];
	char path[200];

	for( int i = 1; i <= NUM_MAX_PROC_FOR_WEAK_SCALABILITY; i++ ){

		int num_rows = MIN_ROW_PER_PROC * ( i * VCPU_PER_INSTANCE ) + 2;

		strcpy(path, OUTPUT_PATH_FILE);
		sprintf(label, "jacobi_%dx%d.dat", num_rows, num_rows);
		strcat(path, label);

		printf("%d  %s\n", num_rows, path);

		double** mat = gen_matrix(num_rows);

		save_matrix(path, mat, num_rows);

	}

	printf("END!\n");

}

