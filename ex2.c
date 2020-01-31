#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

#define MAXLINE 400

int getFileLines(FILE *Fin) {
  //Find lines of file
  int ch;
  int l=0;

  while(!feof(Fin)) {
    ch = fgetc(Fin);
    if(ch == '\n') {
      l++;
    }
  }

  return l;
}

int main(int argc,char **argv) {

	if (argc!=3) {
		printf("Please provide input and output file...\n");
		return -1;
	}

	MPI_Init(&argc, &argv);
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Open the input file given from CLI for input
	FILE * Fin= fopen(argv[1], "r");

	int lines = getFileLines(Fin);
	fseek(Fin, 0, SEEK_SET);

	if(rank == 0) {
		// Open the output file given from CLI for output
		FILE * Fout = fopen(argv[2], "a");

		char s[400];
		MPI_Status status;
		int m=0;
		for(m=0; m<lines; m=m+4) {
			MPI_Recv(s, 400, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			//output the nucleotide sequence and the GC content fastq to the output file
			fprintf(Fout, "%s", s);
		}
		//close the files opened
		fclose(Fout);

	} else {

		int GC=0;
		int AT=0;
		size_t len = 0;

		// Malloc for a 2-dimensional array of strings with
		// 4 lines and MAXLINE of characters per line
		char ** buffer;
		buffer=(char**)malloc(sizeof(char*)*lines);
		for(int i=0;i<lines;i++)
		buffer[i]=(char*)malloc(sizeof(char)*MAXLINE);

		// read line-by-line the first 4 lines of the file
		// and store each in the array named buffer
		for(int Line=0;Line<lines;Line++)
		getline(&buffer[Line], &len, Fin);

		int step = lines/(size-1);
		// We dont want to split a 4-row
		if (step%4 != 0) {
			step -= step%4;
		}
		int start = step * (rank-1);
		int stop = 0;
		if(rank == size-1) {
			stop = lines;
		} else {
			stop = start + step;
		}

		for(int j=start; j<stop; j=j+4) {

			// The number of nucleotides in the second line
			// or equally in the last line
			int MaxLen=strlen(buffer[j+1])-1;
			printf("Number of Nucelotides %d:\n", MaxLen);

			// length of line[1] and line[3] MUST be equally
			if (strlen(buffer[j+3])!=strlen(buffer[j+1]))
			{
			printf("ERROR Lines 2 and 4 have different length\n");
			exit(-1);
			}

			// Count the number of GC and non-GC nucleotides
			// per read
			for (int k=0;k< strlen(buffer[j+1])-1;k++)
			{
			//DEBUG
			//  printf("%c",buffer[1][k]);
			switch (buffer[j+1][k])
			{
				case 'G':
				case 'C':GC++;
				break;
				case 'A':
				case 'T': AT++;
				break;
			}
			}

			//DEBUG
			//printf("GC=%d AT=%d GC content=%f\n",GC,AT,1.0*GC/(GC+AT));
			char bufferToSend[MAXLINE]; 
			//output the nucleotide sequence and the GC content fastq to the output file
			sprintf(bufferToSend,"%f\t%s",1.0*GC/(GC+AT),buffer[j+1]);
			MPI_Send(&bufferToSend, MAXLINE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			GC=0;
			AT=0;
			len=0;
		}

		//free the allocated memory
		for (int i=0;i<lines;i++)
		free(buffer[i]);
		free(buffer);

		//close the files opened
		fclose(Fin);

	}
	MPI_Finalize();
	exit(0);

}
