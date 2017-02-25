#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(int argc, char** argv){
	if(argc != 4){
		printf("./<exec> <nr_linii> <nr_coloane> <nume_fisier>\n");
		return 0;
	}
	srand(time(NULL));
	int l = atoi(argv[1]);
	int c = atoi(argv[2]);
	int i = 0;
	int j = 0;
	FILE* out = fopen(argv[3],"w");
	fprintf(out, "%d %d\n", l, c);
	for(i=0; i<l; i++){
		for(j=0; j<c; j++){
			char c;
			if(rand() % 2 == 1){
				c = 'X';
			}else{
				c = '.';
			}
			fprintf(out, "%c ", c);
		}
		if(i!=l-1) fprintf(out,"\n");
	}
	fclose(out);
	return 0;
}
