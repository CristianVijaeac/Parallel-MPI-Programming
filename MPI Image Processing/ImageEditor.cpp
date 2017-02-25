#include <iostream>
#include <mpi.h>
#include <vector>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;


//filtrul smooth
void smooth(int** block,int** result,int height,int width){
  for (int i = 0 ; i < height ; i++){
    for (int j = 0 ; j < width ; j++){
      
      result[i][j] = (block[i][j] * 1 + block[i][j+1] * 1 + block[i][j+2] * 1 + 
      				block[i+1][j] * 1 + block[i+1][j+1] * 1 + block[i+1][j+2] * 1 +
      				block[i+2][j] * 1 + block[i+2][j+1] * 1 + block[i+2][j+2] * 1) / 9 ;
    	//daca se depaseste intervalul se reinitializeaza valoarea la una cunoscuta
    	if (result[i][j] < 0) result[i][j] = 0;
    	if (result[i][j] > 255) result[i][j] = 255;
    }
  }
}

//filtrul blur
void blur(int** block,int** result,int height,int width){
  for (int i = 0 ; i < height ; i++){
    for (int j = 0 ; j < width ; j++){
      
      result[i][j] = (block[i][j] * 1 + block[i][j+1] * 2 + block[i][j+2] * 1 + 
      				block[i+1][j] * 2 + block[i+1][j+1] * 4 + block[i+1][j+2] * 2 +
      				block[i+2][j] * 1 + block[i+2][j+1] * 2 + block[i+2][j+2] * 1) / 16 ;
    	//daca se depaseste intervalul se reinitializeaza valoarea la una cunoscuta
    	if (result[i][j] < 0) result[i][j] = 0;
    	if (result[i][j] > 255) result[i][j] = 255;
    }
  }
}

//filtrul sharpen
void sharpen(int** block,int** result,int height,int width){
  for (int i = 0 ; i < height ; i++){
    for (int j = 0 ; j < width ; j++){
     
      result[i][j] = (block[i][j] * 0 + block[i][j+1] * -2 + block[i][j+2] * 0 + 
      				block[i+1][j] * -2 + block[i+1][j+1] * 11 + block[i+1][j+2] * -2 +
      				block[i+2][j] * 0 + block[i+2][j+1] * -2 + block[i+2][j+2] * 0) / 3 ;
    	//daca se depaseste intervalul se reinitializeaza valoarea la una cunoscuta
    	if (result[i][j] < 0) result[i][j] = 0;
    	if (result[i][j] > 255) result[i][j] = 255;
    }
  }
}

//filtrul mean_removal
void mean_removal(int** block,int** result,int height,int width){
  for (int i = 0 ; i < height ; i++){
    for (int j = 0 ; j < width ; j++){
      
      result[i][j] = (block[i][j] * -1 + block[i][j+1] * -1 + 
      				block[i][j+2] * -1 + block[i+1][j] * -1 + block[i+1][j+1] * 9 + block[i+1][j+2] * -1 +
      				block[i+2][j] * -1 + block[i+2][j+1] * -1 + block[i+2][j+2] * -1) / 1 ;
    	//daca se depaseste intervalul se reinitializeaza valoarea la una cunoscuta
    	if (result[i][j] < 0) result[i][j] = 0;
    	if (result[i][j] > 255) result[i][j] = 255;
    }
  }
}


int main(int argc,char* argv[]) {
	int rank;
	int nProcesses;
	vector<int> neighbours;
	vector<int> children;
	int parent = 0;
	char state;
	char send;
	int discovered = 0;
	int filter_no = 0;
	char finish;
	int* statistics;
	int statistics_no = 0;
	int leaf_or_not;
 
 	//initializare procese
	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Request request;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

	//initializare vector statistici
	statistics = (int*)malloc(nProcesses * sizeof(int));
	for (int i = 0 ; i < nProcesses ; i++)
        	statistics[i] = -1;

    //citire topologie si adaugarea fieacarui vecin intr-un vector
	string line;
	ifstream topologie(argv[1]);

	while (getline(topologie, line))
	{
    	char* tmp = strtok (&*line.begin(),":");
    	if (stoi(tmp,0,10) == rank){
        	while (tmp != NULL){
        		tmp = strtok(NULL," ");
        		if (tmp == NULL) break;
        		neighbours.push_back(stoi(tmp,0,10));
        	}
        	
        	break;
        }
    }
    topologie.close();

    /*ARBORE DE ACOPERIRE*/
  
    //daca suntem root
	if (rank == 0) {

		//inseamna ca am fost descoperiti si trimitem un mesaj de descoperire copiilor
		discovered = 1;
		for (int i = 0 ; i < neighbours.size() ; i++){
    		send = 'S';
    		MPI_Send(&send,1,MPI_CHAR,neighbours[i],0,MPI_COMM_WORLD);
    	}
   		
   		//asteptam mesaj de confirmare de la vecini ca sa vedem care sunt copii
    	for (int i = 0 ; i < neighbours.size() ; i++){
    		MPI_Recv(&state,1,MPI_CHAR,neighbours[i],0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      		if (state == 'Y') children.push_back(neighbours[i]);
    	}

	}else{

		//daca sunt noduri/frunze asteptam mesaj de la parinte
		for (int i = 0 ; i < neighbours.size() ; i++){
    		MPI_Recv(&state,1,MPI_CHAR,neighbours[i],0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    		if (state == 'S') {
    			//daca e prima oara cand primeste mesaj inseamna ca initiatorul este parintele lui
        		if (discovered == 0){
            		discovered = 1;
            		parent = neighbours[i];
            		send = 'S';
            		for (int i = 0 ; i < neighbours.size();i++){
            			//trimitem mai departe mesaje pt descoperirea copiilor
                		if (neighbours[i] != parent) {
              		  		MPI_Send(&send,1,MPI_CHAR,neighbours[i],0,MPI_COMM_WORLD);
              			}
            		}
            		break;
        		}
      		}
    	}
    	
    	//daca avem un singur vecin,inseamna ca este o frunza si ne intoarcem
    	if (neighbours.size() == 1){
    		send = 'Y';
        	MPI_Send(&send,1,MPI_CHAR,parent,0,MPI_COMM_WORLD);
    	
    	//daca nu suntem frunza,asteptam mesaj de la vecini(mai putin parinte) ca sa vedem care sunt copii nodului curent(initiatorii ecoului)
    	}else{
    		for (int i = 0 ; i < neighbours.size();i++){
     			if (neighbours[i] != parent) MPI_Recv(&state,1,MPI_CHAR,neighbours[i],0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      			if (state == 'Y') children.push_back(neighbours[i]);
    		}
      	send = 'Y';
      	MPI_Send(&send,1,MPI_CHAR,parent,0,MPI_COMM_WORLD);
    	}
    }

    /*PRELUCRARE IMAGINE + STATISTICI*/

	if (rank == 0 ) {
    	int no_img = 0;
    	string line2;
    	int line_no = 0;

    	ifstream imagini(argv[2]);
    	
        getline(imagini, line2);
        no_img = stoi(line2,0,10);
    	    
        for (int i = 0 ; i < no_img ; i++){
        	//cat timp mai avem imagini de prelucrat inseamna ca procesele nu se pot oprii(trimitem "N");  	
          	for (int j = 0 ; j < children.size();j++){
            	finish = 'N';
          	  	MPI_Send(&finish,1,MPI_CHAR,children[j],0,MPI_COMM_WORLD);
          	}
            
            //citim datele din fisier
            getline(imagini, line2);
            char* tmp2 = strtok (&*line2.begin()," ");
            int order = 1;
            string filter,name,new_name;
            
            while (tmp2 != NULL){
            	if (order == 1)
                	filter = tmp2;
              	if (order == 2)
                	name = tmp2;
              	if (order == 3)
                	new_name = tmp2;
              	order++;
              	tmp2 = strtok(NULL," ");
            }
            
            //alegem filtrul care se doreste a fi aplicat
            if (filter == "smooth") filter_no = 1;
            if (filter == "blur") filter_no = 2;
            if (filter == "sharpen") filter_no = 3;
            if (filter == "mean_removal") filter_no = 4;
            
            //citim datele din imagine + matricea
            ifstream imagine(name);
            int skip_lines = 2;
            string file_line,first,second,third;
             
            getline(imagine, first);
            getline(imagine, second);
            
            int width,height;
            getline(imagine, file_line,' ');
            width = stoi(file_line,0,10);
            getline(imagine, file_line);
            height = stoi(file_line,0,10);
            
            getline(imagine, third);   
            
            int** matrix = (int**) malloc ((height+2) * sizeof(int*));
      		for (int i = 0 ; i < height+2;i++){
        		matrix[i] = (int*)malloc ((width+2) * sizeof(int));
      		}

      		//bordam matricea cu 0-uri
            for (int i = 0 ; i < width + 2 ; i++){
              matrix[0][i] = 0;
            }
            for (int i = 0 ;i < width + 2 ; i++){
              matrix[height + 1][i] = 0;
            }
            for (int i = 0 ; i < height + 2 ; i++){
              matrix[i][0] = 0;
            }

            for (int i = 0 ; i < height + 2 ; i++){
              matrix[i][width + 1] = 0;
            }

            //introducem valorile din fisier in matricea noastra
            for (int i = 1 ; i <= height ; i++){
              for (int j = 1 ; j <= width ; j++){
                  getline(imagine, file_line);
                  matrix[i][j] = stoi(file_line,0,10);
               }
            }

            //realizam impartirea in blocuri in functie de copii(in cazul in care nu se poate impartii egal,surplus se va trimite ultimului copil)
            int new_height = height/children.size();
            for (int i = 0 ; i < children.size()-1;i++){
            	//trimitem filtrul,noua inaltime si latimea
              	MPI_Send(&filter_no,1,MPI_INT,children[i],0,MPI_COMM_WORLD);
              	MPI_Send(&new_height,1,MPI_INT,children[i],0,MPI_COMM_WORLD);
              	MPI_Send(&width,1,MPI_INT,children[i],0,MPI_COMM_WORLD);
              	
              	//trimitem fiecare linie corespunzatoare fiecarui copil inclusiv cele 2 linii in plus de sus si jos
              	for (int j = new_height * i ; j < new_height*(i+1) + 2; j++){
               		vector<int> aux;
                	for (int k = 0 ; k < width + 2; k++){
                  		aux.push_back(matrix[j][k]);
                	}
                	
                	MPI_Send(&aux[0],width+2,MPI_INT,children[i],0,MPI_COMM_WORLD);
              	}

              	//daca nu se realizeaza impartirea exacta in blocuri,surplusul este trimis ultimului copil
              	if (height % 2 != 0 ){
                	vector<int> aux;
                	int for_last = height - children.size() * new_height;
                	int new_height2 = new_height+for_last;
                
                	MPI_Send(&filter_no,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
                	MPI_Send(&new_height2,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
                	MPI_Send(&width,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
                	
                	for(int i = (children.size() - 1) * new_height ; i < (children.size()) * new_height + for_last + 2; i++){
                  		for (int k = 0 ; k < width + 2; k++){
                    		aux.push_back(matrix[i][k]);
                  		}	
                		MPI_Send(&aux[0],width+2,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
                	}
              	
              	}else{
                	MPI_Send(&filter_no,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
                	MPI_Send(&new_height,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
                	MPI_Send(&width,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
                
                	for (int j = new_height * (children.size()-1) ; j < new_height*(children.size()) +2 ; j++){
                  		vector<int> aux;
                  		for (int k = 0 ; k < width +2; k++){
                    		aux.push_back(matrix[j][k]);
                  		}
                   		MPI_Send(&aux[0],width+2,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
                	}
              	}
            }

          //asteptam primirea blocurilor prelucrate de la copii
           int** block_aux = (int**) malloc ((height) * sizeof(int*));
      	   for (int i = 0 ; i < height;i++){
          		block_aux[i] = (int*)malloc ((width) * sizeof(int));
      	   }

      	   //unim blocurile primite 
           int aux = 0;
           for (int i = 0 ; i < children.size() ; i ++){
           		int tmp_height;		
           		
           		MPI_Recv(&tmp_height,1,MPI_INT,children[i],0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
           		
           		for (int j = aux ; j < tmp_height + aux ; j++){
           			MPI_Recv(&block_aux[j][0],width,MPI_INT,children[i],0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
           		}
           		aux = tmp_height + aux;
        	}
           
           //reconstruim imaginea
           ofstream image_out(new_name);	
           image_out<<first<<"\n";
           image_out<<second<<"\n";
           image_out<<width<<" "<<height<<"\n";
           image_out<<third<<"\n";
           
           for (int i = 0 ; i < height ; i++)
           	for (int j = 0 ; j < width ; j++)
           		image_out<<block_aux[i][j]<<"\n";
           image_out.close();

        } 

        //nu mai sunt imagini de prelucrat(primim statisticile si oprim programul)-->"F" inseamna "trimite statisticile"
        finish = 'F';
        for (int i = 0 ; i < children.size();i++){
           	MPI_Send(&finish,1,MPI_CHAR,children[i],0,MPI_COMM_WORLD);
        }

        //se asteapta primirea acestora de la copii
        for (int i = 0 ; i < children.size();i++){
          int* statistics_aux;
          MPI_Recv(&statistics_aux[0],nProcesses,MPI_INT,children[i],0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
          for(int i = 0 ; i < nProcesses ; i++){
            if (statistics_aux[i] != -1) statistics[i] = statistics_aux[i];
	         }
	       }
        
        //se actualizeaza si valoare nodului curent
        statistics[0] = 0;

        //se scriu datele din acest vector in fisierul de statistici
        ofstream statistics_out(argv[3]);
        for (int i = 0 ; i < nProcesses;i++)
        	statistics_out<<i<<": "<<statistics[i]<<"\n";
        statistics_out.close();
	        
    //cazul in care ne aflam intr-un nod/frunza
    }else {
    	//programul se executa pana se primeste tagul de sfarsit "F" dupa care se trimit statisticile si iesim
   		while (true){
    		MPI_Recv(&finish,1,MPI_CHAR,parent,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    		//daca nu mai avem de prelucrat imagini
    		if (finish == 'F') {
          //anuntam toti copii sa trimita statisticila si sa se inchida
    			for (int i = 0 ; i < children.size();i++){
	           finish = 'F';
	          	MPI_Send(&finish,1,MPI_CHAR,children[i],0,MPI_COMM_WORLD);
	         }
          //primim vectorul de statistici de la copii
	        for (int i = 0 ; i < children.size();i++){
	           int* statistics_aux;
	           MPI_Recv(&statistics_aux[0],nProcesses,MPI_INT,children[i],0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	           for(int i = 0 ; i < nProcesses ; i++){
	               if (statistics_aux[i] != -1) statistics[i] = statistics_aux[i];
	            }
	        }
	        //daca nu este frunza actualizam nodul intermediar cu valoarea 0
          if (children.size() != 0 ){
            statistics[rank] = 0;
	          //trimitem mai departe vectorul
	          MPI_Send(&statistics[0],nProcesses,MPI_INT,parent,0,MPI_COMM_WORLD);
	          break;
	        //daca este frunza actualizam valoarea ei cu nr de linii prelucrate
          }else{
            statistics[rank] = statistics_no;
            MPI_Send(&statistics[0],nProcesses,MPI_INT,parent,0,MPI_COMM_WORLD);
            break;
          }
        }	

            //cazul in care mai avem de prelucrat(anuntam toti vecinii)   	
        for (int i = 0 ; i < children.size();i++){
	        finish = 'N';
	        MPI_Send(&finish,1,MPI_CHAR,children[i],0,MPI_COMM_WORLD);
	      }	

	        //primim de la parinte tipul filtrului,latimea si inaltimea
	    	int tmp_width,tmp_height;
	      	MPI_Recv(&filter_no,1,MPI_INT,parent,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	      	MPI_Recv(&tmp_height,1,MPI_INT,parent,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	      	MPI_Recv(&tmp_width,1,MPI_INT,parent,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	      
	     	int** block = (int**) malloc ((tmp_height+2) * sizeof(int*));
	      	for (int i = 0 ; i < tmp_height+2;i++){
	        	block[i] = (int*)malloc ((tmp_width+2) * sizeof(int));
	      	}

	      	int** result = (int**) malloc ((tmp_height) * sizeof(int*));
	      	for (int i = 0 ; i < tmp_height;i++){
	        	result[i] = (int*)malloc ((tmp_width) * sizeof(int));
	      	}
	      
	      	//primim blocul de date
	      	for (int i = 0 ; i < tmp_height + 2; i++){
	          MPI_Recv(&block[i][0],tmp_width+2,MPI_INT,parent,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	      	}

	      	//daca nu este frunza
	      	if (children.size() != 0 ){

	      		//reimpartim blocul de date in functie de numarul de copii si trimite cate unul fiecaruia
	        	int new_height = tmp_height/children.size();
	          	for (int i = 0 ; i < children.size()-1;i++){
	           		MPI_Send(&filter_no,1,MPI_INT,children[i],0,MPI_COMM_WORLD);
	           		MPI_Send(&new_height,1,MPI_INT,children[i],0,MPI_COMM_WORLD);
	           		MPI_Send(&tmp_width,1,MPI_INT,children[i],0,MPI_COMM_WORLD);
	            
	           		for (int j = new_height * i ; j < new_height*(i+1)+2 ; j++){
	           			vector<int> aux;
	           			for (int k = 0 ; k < tmp_width +2; k++){
	               			aux.push_back(block[j][k]);
	           			}
             			MPI_Send(&aux[0],tmp_width+2,MPI_INT,children[i],0,MPI_COMM_WORLD);  
	          		}
	      		}

	      		//acelasi principiu ca la radacina->daca nu se imparte exact,surplusul se duce la ultimul copil
	        	if (tmp_height % 2 != 0 ){    
	            	int for_last = tmp_height - children.size() * new_height;
	                int new_height2 = new_height+for_last;
	                
	                MPI_Send(&filter_no,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
	                MPI_Send(&new_height2,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
	                MPI_Send(&tmp_width,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
	                
	                for(int i = (children.size() - 1) * new_height ; i < (children.size()) * new_height + for_last +2; i++){
	                	vector<int> aux;
	                	for (int k = 0 ; k < tmp_width +2; k++){
	                    	aux.push_back(block[i][k]);
	                  	}
	                	MPI_Send(&aux[0],tmp_width+2,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
	                }
	          	
	          	}else{
	            	MPI_Send(&filter_no,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
	            	MPI_Send(&new_height,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
	            	MPI_Send(&tmp_width,1,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
	            
	            	for (int j = new_height * (children.size()-1) ; j < new_height*(children.size()) +2; j++){
	              		vector<int> aux;
	              		for (int k = 0 ; k < tmp_width +2 ; k++){
	                		aux.push_back(block[j][k]);
	              		}
	              		MPI_Send(&aux[0],tmp_width+2,MPI_INT,children[children.size()-1],0,MPI_COMM_WORLD);
	            	}
	          	}
        	

        		//aici se astepta primirea blocurilor prelucrate de la copii
        		int aux = 0;
          
        		int** block_aux = (int**) malloc ((tmp_height) * sizeof(int*));
      	  		for (int i = 0 ; i < tmp_height;i++){
          			block_aux[i] = (int*)malloc ((tmp_width) * sizeof(int));
      	  		}
          		
          		//unim blocurile primite 
           		for (int i = 0 ; i < children.size() ; i ++){
           			int height;
           			MPI_Recv(&height,1,MPI_INT,children[i],0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
           			for (int j = aux ; j < height + aux; j++ ){
           				MPI_Recv(&block_aux[j][0],tmp_width,MPI_INT,children[i],0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
           			}

           			aux = height + aux;
           		}
           		
           		//trimitem blocul nou format parintelui
           		MPI_Send(&tmp_height,1,MPI_INT,parent,0,MPI_COMM_WORLD);
           		for (int j = 0 ; j < tmp_height  ; j++){
           			MPI_Send(&block_aux[j][0],tmp_width,MPI_INT,parent,0,MPI_COMM_WORLD);
           		}

           	//daca este frunza
        	}else{
         		
         		//modificam blocul de date care a ajuns in aceasta in functie de filtrul aplicat
          		if (filter_no == 1) smooth(block,result,tmp_height,tmp_width);
          		if (filter_no == 2) blur(block,result,tmp_height,tmp_width);
          		if (filter_no == 3) sharpen(block,result,tmp_height,tmp_width);
          		if (filter_no == 4) mean_removal(block,result,tmp_height,tmp_width);
  				
  				//trimitem parintelui blocul modificat si actualizam nr de linii prelucrate
 		        MPI_Send(&tmp_height,1,MPI_INT,parent,0,MPI_COMM_WORLD);  
          		statistics_no += tmp_height;
          		for (int j = 0 ; j < tmp_height  ; j++){
          			MPI_Send(&result[j][0],tmp_width ,MPI_INT,parent,0,MPI_COMM_WORLD);
          		}
      		}
  		}
  	}

	MPI_Finalize();

	return 0;
}
