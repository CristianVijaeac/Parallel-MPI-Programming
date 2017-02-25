#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"



void main (int argc , char* argv[]) {

	int no_lines = 0;
	int no_cols = 0;
	int N = atoi(argv[2]);
	int i,j = 0;
	FILE *f = fopen(argv[1],"rt");
	FILE *g = fopen(argv[3],"wt");
	char** matrix = NULL;
	char** matrix_aux = NULL;
	char tmp ;	
	
	//citire nr linii si coloane

	fscanf(f,"%d",&no_lines);
	fscanf(f,"%d",&no_cols);

	fprintf(g,"%d ",no_lines);
	fprintf(g,"%d\n",no_cols);

	//alocare spatiu matrici
	no_lines += 2 ;
	no_cols += 2;

	matrix = (char**) malloc (no_lines * sizeof(char*) );
	matrix_aux = (char**) malloc (no_lines * sizeof(char*) );
	
	for (i = 0 ; i < no_lines ; i++){
		matrix[i] = (char*) malloc (no_cols * sizeof(char) );
		matrix_aux[i] = (char*) malloc (no_cols * sizeof(char) );
	}

	//construire matrici

	for (i = 1 ; i < no_lines - 1 ; i++){
		for (j = 1 ; j < no_cols - 1; j++){
			fscanf(f,"%c",&tmp);
			if ( strncmp(&tmp," ",1) != 0 && strncmp(&tmp,"\n",1) != 0){			
				matrix[i][j] = tmp;				
			}else{ 
				j--;	
			}
			
		}	
	}

	//construire margini pt matrici

	matrix[0][0] = matrix [no_lines-2][no_cols-2];
	matrix[0][no_cols - 1] = matrix[no_lines - 2][1];
	matrix[no_lines-1][0] = matrix[1][no_cols - 2];
	matrix[no_lines - 1][no_cols - 1] = matrix[1][1];

	for(j = 1 ; j < no_cols - 1;j++){
		matrix[0][j] = matrix[no_lines - 2][j];
		matrix[no_lines - 1][j] = matrix [1][j];
	}
	


	for(i = 1 ; i < no_lines - 1;i++){
		matrix[i][0] = matrix [i][no_cols - 2];
		matrix[i][no_cols-1] = matrix [i][1];
	}

	//copiem matricea originala peste matricea auxiliara

	#pragma omp parallel for private(j) shared(matrix,matrix_aux) schedule(static) collapse(2)

	for (i = 0 ; i < no_lines ; i++){
		for (j = 0 ; j < no_cols ; j++){
			matrix_aux[i][j] = matrix[i][j];	
		}
	}

	//parcurgerea si modificarea matricilor de N ori

	int k = 0;
	int no_alive = 0;
	for (k = 0 ; k < N ; k++){

		#pragma omp parallel for private(j,no_alive) shared(matrix,matrix_aux) schedule(static) collapse(2)
		for (i = 1 ; i < no_lines - 1 ; i++){			
			for (j = 1 ; j < no_cols - 1 ; j++){

				//calculam numarul de vecini alive

				no_alive = 0;				
				if (matrix[i - 1][j - 1] == 'X') {
					no_alive ++ ;
					
				}
				if (matrix[i - 1][j] == 'X') {
					no_alive ++ ;

				}
				if (matrix[i - 1][j + 1] == 'X') {
					no_alive ++ ;

				}
				if (matrix[i][j - 1] == 'X') {
					no_alive ++ ;
				}
				if (matrix[i][j + 1] == 'X') {
					no_alive ++ ;
				}
				if (matrix[i + 1][j - 1] == 'X') {
					no_alive ++ ;
				}
				if (matrix[i + 1][j] == 'X') {
					no_alive ++ ;
				}
				if (matrix[i + 1][j + 1] == 'X') {
					no_alive ++ ;
				}
			
				//in functie de numarul de celule alive,alegem starea noii celule

				if (no_alive < 2 || no_alive > 3 ) {
					matrix_aux[i][j] = '.';
					
				}
				if (matrix[i][j] == 'X' && (no_alive == 2 || no_alive == 3 )) {
					matrix_aux[i][j] = 'X';
				}				
				if (matrix[i][j] == '.' && no_alive == 3 ) {
					matrix_aux[i][j] = 'X';
							
				}
			}
		}

		//construim noile margini ale matricilor in functie de noile valori
		
		int a=0,b=0;		

		matrix_aux[0][0] = matrix_aux[no_lines-2][no_cols-2];
		matrix_aux[0][no_cols - 1] = matrix_aux[no_lines - 2][1];
		matrix_aux[no_lines-1][0] = matrix_aux[1][no_cols - 2];
		matrix_aux[no_lines - 1][no_cols - 1] = matrix_aux[1][1];


		for(a = 1 ; a < no_cols - 1;a++){
			matrix_aux[0][a] = matrix_aux[no_lines - 2][a];
			matrix_aux[no_lines - 1][a] = matrix_aux[1][a];
		}

	
		for(b = 1 ; b < no_lines - 1;b++){
			matrix_aux[b][0] = matrix_aux[b][no_cols - 2];
			matrix_aux[b][no_cols-1] = matrix_aux[b][1];
		}

		//copiem matricea auxiliara peste cea originala

		#pragma omp parallel for private(b) shared(matrix,matrix_aux) schedule(static) collapse(2)
		for (a = 0 ; a < no_lines ; a++){
			for (b = 0 ; b < no_cols ; b++){
				matrix[a][b] = matrix_aux[a][b];	
			}
		}
	}

	//scriem in fisier
				
	for (i = 1 ; i < no_lines -1 ; i++){
		for (j = 1 ; j < no_cols - 1 ; j++)
			fprintf(g,"%c ",matrix[i][j]);
		fprintf(g,"\n");
	}
	
	fclose(f);
	fclose(g);
	

}
