/*
 ============================================================================
 Name        : JacobiPar.c
 Author      : Sergio Guastaferro
 Version     :
 Copyright   : Your copyright notice
 Description : Hello MPI World in C 

 Si utilizza un partizionamento orizzontale

 num_rows_cols ::> numero di righe/colonna dell'intera matrice in input

 Si esclude nel benchmark il caricamento da file, ma non il tempo di
 configurazione del partizionamento

 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

#include "../include/ProjectUtils.h"


#define DEBUG 0


int main(int argc, char* argv[]){

	int  my_rank; /* rank of process */
	int  p;       /* number of processes */
	int tag=0;    /* tag for messages */
	int status_computation; /* status convergence state*/

	int num_rows_cols;
	char* input_file;

	if( argc != 3 ){
		printf("USAGE %s <DIM_MATRIX> <INPUT_FILE> \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	num_rows_cols = atoi(argv[1]);
	fprintf(stderr, "num_rows_cols=%d\n", num_rows_cols);

	if( !file_exist(argv[2]) ){
		printf("File %s doesn't exist!\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	input_file = argv[2];
	fprintf(stderr, "File %s : OK\n", input_file);


	double timer = 0.0;
	double inst_start, inst_stop;

	MPI_Status status ;   /* return status for receive */
	
	/* start up MPI */
	
	MPI_Init(&argc, &argv);
	
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	
	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p); 

	if( p == 1 ){
		fprintf(stderr, "To run sequentially, launch JacobiSeq\n");
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	DD{
		if( my_rank == MASTER ){
			printf("NUM_ROWS_COLS = %d\n", num_rows_cols);
			printf("Numero di processori :: %d\n", p);
		}
	}

	if( my_rank == MASTER )	inst_start = MPI_Wtime();

	////// START:Indicizzazione del PARTIZIONAMENTO  [curr_num_rows, offset, ghost]
	
	int num_row_per_p = (num_rows_cols - 2) / p;      //riga-sup e riga-inf non calcolate per l'approx
	int reminder_rows = (num_rows_cols - 2) % p;


	int curr_num_rows = num_row_per_p;

	if(my_rank == MASTER || my_rank == LAST) curr_num_rows++;
	else if(my_rank == MASTER && my_rank == LAST) curr_num_rows += 2; // caso unico processore!

	if(reminder_rows > 0 && my_rank < reminder_rows) curr_num_rows++;


	/// calcolo offset, in maniera iterativa
	int offset = 0;
	if( my_rank == MASTER ) offset = 0;
	else if( my_rank == LAST ) offset = num_rows_cols - curr_num_rows;
	else{

		int rows_of_pk;
		for( int k = 0; k < my_rank; k++ ){

			rows_of_pk = num_row_per_p;

			if(k == MASTER || k == LAST) rows_of_pk++;
			else if(k == MASTER && k == LAST) rows_of_pk += 2; // caso unico processore!

			if(reminder_rows > 0 && k < reminder_rows) rows_of_pk++;

			// rows_of_pk è noto, quindi lo aggiungo all'offset
			offset += rows_of_pk;
		}

	}

	DD printf("IO sono p:%d e prendo %d righe a partire da offset:%d\n", my_rank, curr_num_rows, offset);

	////// END:Indicizzazione del PARTIZIONAMENTO


	//carica la sottomatrice
	int ghost_opt;
	if( my_rank == MASTER ) ghost_opt = GHOST_DOWN;
	else if( my_rank == LAST ) ghost_opt = GHOST_UP;
	else ghost_opt = GHOST_BOTH;

	if( my_rank == MASTER )	inst_stop = MPI_Wtime();
	timer += inst_stop - inst_start;

	DD fprintf(stderr, "Pause for reading from file...\n");
	DD if( my_rank == MASTER ) fprintf(stderr, "Timer : %lf\n", timer);

	MPI_Barrier(MPI_COMM_WORLD);

	///ESCLUDI DAL TEMPO IL CARICAMENTO DA FILE!
	double** oldMatrix = load_matrix(input_file, curr_num_rows, num_rows_cols, offset, ghost_opt);

	MPI_Barrier(MPI_COMM_WORLD);
	DD fprintf(stderr, "Resume computation...\n");

	if( my_rank == MASTER )	inst_start = MPI_Wtime();

	double** newMatrix = create_matrix_zero(curr_num_rows, num_rows_cols, ghost_opt);

	double* recv_row_sup = malloc( num_rows_cols * sizeof(double) );
	double* recv_row_inf = malloc( num_rows_cols * sizeof(double) );


	///calcolo processore precedente e successore
	int my_next_proc = (my_rank + 1) % p;
	int my_prev_proc = (my_rank + p - 1) % p;

	char label[50];

	DD{
		sprintf(label, "my_rank:%d -- prev:%d,  next:%d", my_rank, my_prev_proc, my_next_proc);
		printf("%s\n", label);
	}



	int i,j,k;
	int num_done_iter = 0;
	double my_diffnorm, recv_diffnorm;

	for( int k = 0; k < MAX_ITERATION; k++ ){
		num_done_iter++;
		if( p == 2 ){
			if( my_rank == FIRST ){
				//send riga bottom a LAST
				MPI_Send(&oldMatrix[curr_num_rows-1][0], num_rows_cols, MPI_DOUBLE, LAST, tag, MPI_COMM_WORLD);

				//receivce riga top da LAST
				MPI_Recv(recv_row_sup, num_rows_cols, MPI_DOUBLE, LAST, tag, MPI_COMM_WORLD, &status);
			}
			else if( my_rank == LAST ){
				//receive riga bottom da FIRST
				MPI_Recv(recv_row_inf, num_rows_cols, MPI_DOUBLE, FIRST, tag, MPI_COMM_WORLD, &status);

				//send riga top a FIRST
				MPI_Send(&oldMatrix[1][0], num_rows_cols, MPI_DOUBLE, FIRST, tag, MPI_COMM_WORLD);
			}
		}
		else if( p > 2 ){

			if( my_rank == FIRST ){ // e FIRST è sempre pari!!!
				//send riga bottom a my_rank+1
				MPI_Send(&oldMatrix[curr_num_rows-1][0], num_rows_cols, MPI_DOUBLE, my_next_proc, tag, MPI_COMM_WORLD);

				//receive riga top da my_rank+1
				MPI_Recv(recv_row_sup, num_rows_cols, MPI_DOUBLE, my_next_proc, tag, MPI_COMM_WORLD, &status);

			}
			else if( my_rank == LAST ){
				if( (LAST+1) % 2  == 0 ){
					//receive riga bottom da my_rank-1
					MPI_Recv(recv_row_inf, num_rows_cols, MPI_DOUBLE, my_prev_proc, tag, MPI_COMM_WORLD, &status);

					//send riga top a my_rank-1
					MPI_Send(&oldMatrix[1][0], num_rows_cols, MPI_DOUBLE, my_prev_proc, tag, MPI_COMM_WORLD);
				}
				else{
					//send riga top a my_rank-1
					MPI_Send(&oldMatrix[1][0], num_rows_cols, MPI_DOUBLE, my_prev_proc, tag, MPI_COMM_WORLD);

					//receive riga bottom da my_rank-1
					MPI_Recv(recv_row_inf, num_rows_cols, MPI_DOUBLE, my_prev_proc, tag, MPI_COMM_WORLD, &status);
				}
			}
			else{

				if( my_rank % 2 == 0 ){ // PARI

					//send riga bottom a my_rank+1
					MPI_Send(&oldMatrix[curr_num_rows][0], num_rows_cols, MPI_DOUBLE, my_next_proc, tag, MPI_COMM_WORLD);

					//receive riga top da my_rank+1
					MPI_Recv(recv_row_sup, num_rows_cols, MPI_DOUBLE, my_next_proc, tag, MPI_COMM_WORLD, &status);

					//send riga top a my_rank-1
					MPI_Send(&oldMatrix[1][0], num_rows_cols, MPI_DOUBLE, my_prev_proc, tag, MPI_COMM_WORLD);

					//receive riga bottom da my_rank-1
					MPI_Recv(recv_row_inf, num_rows_cols, MPI_DOUBLE, my_prev_proc, tag, MPI_COMM_WORLD, &status);
				}
				else{ // DISPARI

					//receive riga bottom da my_rank-1
					MPI_Recv(recv_row_inf, num_rows_cols, MPI_DOUBLE, my_prev_proc, tag, MPI_COMM_WORLD, &status);

					//send riga top a my_rank-1
					MPI_Send(&oldMatrix[1][0], num_rows_cols, MPI_DOUBLE, my_prev_proc, tag, MPI_COMM_WORLD);

					//receive riga top da my_rank+1
					MPI_Recv(recv_row_sup, num_rows_cols, MPI_DOUBLE, my_next_proc, tag, MPI_COMM_WORLD, &status);

					//send riga bottom a my_rank+1
					MPI_Send(&oldMatrix[curr_num_rows][0], num_rows_cols, MPI_DOUBLE, my_next_proc, tag, MPI_COMM_WORLD);
				}

			}

		}


		DD{
			sprintf(label, "INF_my_rank:%d", my_rank);
			fprint_row(stdout, label, recv_row_inf, num_rows_cols);
			sprintf(label, "SUP_my_rank:%d", my_rank);
			fprint_row(stdout, label, recv_row_sup, num_rows_cols);
		}


		//// move recv_lines in oldMatrix
		if( my_rank == FIRST ){
			oldMatrix[curr_num_rows] = recv_row_sup;
		}
		else if( my_rank == LAST ){
			oldMatrix[0] = recv_row_inf;
		}
		else{
			oldMatrix[curr_num_rows+1] = recv_row_sup;
			oldMatrix[0] = recv_row_inf;
		}


		/////////////////////////fine invio righe!



		int last_index_row = curr_num_rows + abs(ghost_opt) - 1;
		//fprintf(stderr, ">>>>>> my_rank:%d -- last_index_row=%d\n", my_rank, last_index_row);



		/// MAKE_ITERATION()
		///make_iteration(oldMatrix, newMatrix, num_rows_cols);
		for( i = 1; i < last_index_row; i++ ){
			for( j = 1; j < num_rows_cols - 1; j++ ){
				DD fprintf(stderr, "<<< my_rank:%d <<<<<Make Iteration>>  %d - %d >>>>  --- [%lf, %lf, %lf, %lf]\n", my_rank, i, j,      oldMatrix[i+1][j], oldMatrix[i-1][j], oldMatrix[i][j+1],oldMatrix[i][j-1]  );
				newMatrix[i][j] = (oldMatrix[i+1][j] + oldMatrix[i-1][j] + oldMatrix[i][j+1] + oldMatrix[i][j-1])/4;
			}
		}
		/// END: MAKE_ITERATION


		/// TEST_CONVERGENZA PARALLELO [calcola i diffnorm parziali e testa sul master]
		///if( test_convergenza(mat, newmat, num_rows_cols) == 1 ) break;
		my_diffnorm = 0.0;
		for( i = 1; i < last_index_row; i++ ){
			for( j = 1; j < num_rows_cols - 1; j++ ){
				my_diffnorm += ( (newMatrix[i][j] - oldMatrix[i][j]) * (newMatrix[i][j] - oldMatrix[i][j]) );
			}
		}

		if( my_rank == MASTER ){

			for( i = 1; i < p; i++ ){
				//receive my_diffnorm da pi processor
				MPI_Recv(&recv_diffnorm, 1, MPI_DOUBLE, i, tag, MPI_COMM_WORLD, &status);

				///fprintf(stderr, ">> ricevuto diffnorm=%lf da proc:%d\n", recv_diffnorm, i);

				my_diffnorm += recv_diffnorm;
			}

			DD fprintf(stderr, "######### tot_diffnorm=%lf \n", my_diffnorm);
			if( (my_diffnorm - sqrt( my_diffnorm )) < DELTA_APPROX  ) {
				//avvisa tutti!!
				DD printf(">>>>>>>>> STOP_COMPUTATION >>>>>>>>>\n");

				status_computation = MSG_STOP_COMPUTATION;
				MPI_Bcast(&status_computation, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

				break;
			}
			else{
				status_computation = MSG_CONTINUE_COMPUTATION;
				MPI_Bcast(&status_computation, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
			}

		}
		else{
			// send my_diffnorm a MASTER
			MPI_Send(&my_diffnorm, 1, MPI_DOUBLE, MASTER, tag, MPI_COMM_WORLD);

			MPI_Bcast(&status_computation, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

			if( status_computation == MSG_STOP_COMPUTATION ) break;
		}
		////// END: TEST_CONVERGENZA PARALLELO




		/// NEW_IN_OLD()
		////new_in_old(mat, newmat, num_rows_cols);
		for( i = 1; i < last_index_row; i++ ){
			for( j = 1; j < num_rows_cols - 1; j++ ){
				oldMatrix[i][j] = newMatrix[i][j];
			}
		}
		/// END: NEW_IN_OLD()


	}

	DD fprintf(stderr, "my_rank:%d è fuori dal for!\n", my_rank);





	///// REDUCE riunire i risultati e mostrare l'output
	double** result_matrix;
	if( my_rank == MASTER ){

		result_matrix = create_matrix(num_rows_cols);
		double* recv_line_matrix = malloc((num_rows_cols-2)* sizeof(double));


		for( i = 1; i < curr_num_rows; i++ ){
			for( j = 1; j < num_rows_cols-1; j++){
				result_matrix[i][j] = oldMatrix[i][j];
			}
		}


		int row_index = curr_num_rows;
		for( k = 1; k < p; k++ ){

			int rows_of_pk;
			rows_of_pk = num_row_per_p;
			if(k == MASTER && k == LAST) rows_of_pk += 2; // caso unico processore!
			if(reminder_rows > 0 && k < reminder_rows) rows_of_pk++;


			DD printf("##################################### MASTER, ricevi %d row dal proc:%d\n", rows_of_pk, k);

			for( i = 0; i < rows_of_pk; i++ ){
				MPI_Recv(recv_line_matrix, num_rows_cols-2, MPI_DOUBLE, k, tag, MPI_COMM_WORLD, &status);

				DD{
					sprintf(label, ">>>>>>>>>>> from P:%d  Row:%d", k, i);
					fprint_row(stdout, label, recv_line_matrix, num_rows_cols-2);
				}

				for( j = 1; j < num_rows_cols-1; j++ ){
					result_matrix[row_index][j] = recv_line_matrix[j-1];
				}

				row_index++;

			}

		}

	}
	else{

		int last_index = curr_num_rows;

		if( my_rank == LAST ) last_index--;

		for( i = 1; i <= last_index; i++ ){
			DD printf("------------------ P:%d send row:%d\n", my_rank, i);
			MPI_Send(&oldMatrix[i][1], num_rows_cols-2, MPI_DOUBLE, MASTER, tag, MPI_COMM_WORLD);
		}

	}
	///// END: REDUCE riunire i risultati e mostrare l'output


	if( my_rank == MASTER )	{
		inst_stop = MPI_Wtime();
		timer += inst_stop - inst_start;
		DD fprint_smatrix(stdout, result_matrix, num_rows_cols);
		printf("RESULT BENCHMARK :: NumProc:%d   DimMatrix:%d   NumDoneIter:%d   NumMaxIter:%d   Tempo:%lf s\n", p, num_rows_cols, num_done_iter, MAX_ITERATION,timer);
	}


	/* shut down MPI */
	MPI_Finalize(); 
	
	
	return 0;
}
