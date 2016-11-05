#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>


#include "tranrecfn.c"
#define MAX_LINE_LEN 1000



int main(int argc, char **argv)
{

	FILE *outfp=NULL, *inf=NULL;
	argst *st = NULL;
	int i,col=-1, rows=0;
	char line[MAX_LINE_LEN];
	double *jd_arr;

	

	if (argc ==1)
	{
		print_help();
		exit(1);
	}
	st = malloc(sizeof(argst));

	commd_parse(argc, argv,  st);

	if(!simulate(st))
	{
		//printf("Input is insufficient to do work !!\n");
		//exit(1);
	}
	printf("st->infile=%s\n",st->infile);
	if( !(strcmp(st->infile,"")==0) ){
		if( (inf=fopen((st->infile),"r")) == NULL ){
			printf("# Error Opening File\n");
			exit(1);
		}
		while((fgets(line, sizeof(line), inf))!=NULL) rows++;
	}
	if(inf!=NULL)fclose(inf);

printf("rows= %d\n",rows);
	jd_arr = malloc(sizeof(double)*rows);
	if(jd_arr ==NULL)
	{
		printf("Memory error \n");
		exit(1); 
	}

	if( inf!=NULL)
	{
		file_read(jd_arr, &rows, st);
		simulate_transit(st, jd_arr, rows);
	}

	if(st->helpfg==1) { //for -h option
		print_help();
		exit(0);
	}

	if(st->verfg==1) { // for -v
		print_ver();
		exit(0);
	}

	if(outfp!=NULL)fclose(outfp);
	return 0;
}

