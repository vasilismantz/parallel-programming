#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAXLINE 400

int getFileLines(FILE *Fin) {
  int l=0, ch;

  while(!feof(Fin)) {
    ch=fgetc(Fin);
    if(ch == '\n') {
      l++;
    }
  }

  return l;
}

int main(int argc,char **argv)
{
  // Open the input file given from CLI for input
  FILE * Fin= fopen(argv[1], "r");
  // Open the output file given from CLI for output
  FILE * Fout= fopen(argv[2], "w");

  int lines = getFileLines(Fin);

  fseek(Fin, 0, SEEK_SET);

  int i;
  int Line;
  int GC=0;
  int AT=0;

  // Malloc for a 2-dimensional array of strings with
  // 4 lines and MAXLINE of characters per line
  char ** buffer;
  buffer=(char**)malloc(sizeof(char*)*lines);
  for(i=0;i<lines;i++)
  buffer[i]=(char*)malloc(sizeof(char)*MAXLINE );

  size_t len = 0;

  int j=0;

  // read line-by-line the first 4 lines of the file
  // and store each in the array named buffer
  for(Line=0;Line<lines;Line++)
  getline(&buffer[Line], &len, Fin);


  for(j=0; j<lines; j=j+4) {

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

	  //output the nucleotide sequence and the GC content fastq to the output file
	  fprintf(Fout,"%f\t%s",1.0*GC/(GC+AT),buffer[j+1]);

	  GC=0;
	  AT=0;
  }

  //free the allocated memory
  for (i=0;i<lines;i++)
  free(buffer[i]);
  free(buffer);
  exit(0);

  //close the files opened
  fclose(Fin);
  fclose(Fout);

  exit(0);

}
