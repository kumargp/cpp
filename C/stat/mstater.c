/* Copyright: HATNet project, Harvard-Smithsonian CFA

 mstater.c for statistics handling  multiple  coulms 

Usage: ./mstater -h
 Author: Gopakumar Perumpilly: gopan[dot]p[at]gmail[dot]com

*/



#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>   
#include <math.h>  
#include <unistd.h> 
#include <string.h>
#include <stdarg.h>
#include <getopt.h>
#include <fcntl.h>
#include <regex.h>
#include <sys/types.h> 


#include "mstatfn.h"
#ifndef STEP
	#define STEP 10
#endif

int main(int argc, char ** argv)
{
	myst **st = NULL;
	flgst *flst = NULL;
	FILE *outfp=NULL, *infp=NULL;;
	long int frows=0;
	double **file_data;
	char line[200];
	int i=0,rows=0, tot_col=0, stnum=0, sti=0;
	int *col,stn=0,*storder;	


	if (argc < 1)   
	{
		print_help();
		exit(1);
	}

	st = malloc(sizeof(myst *)*STEP);
	for(i=0;i<STEP;i++){
		st[i] =  malloc(sizeof(myst));
	}

	flst = malloc(sizeof(flgst));

	if((st==NULL)||(flst ==NULL) )
	{
		printf("Memory error \n");
		exit(1); 
	}

	commd_parse(argc, argv,  st, &stnum, flst);

	for(tot_col=0,sti =0; sti<stnum; sti++){//cols to be read from file
		
		if( (((*st)+sti)->colum) > tot_col) tot_col=((*st)+sti)->colum;
		if(((*st)+sti)->ccorrel >tot_col) tot_col=((*st)+sti)->ccorrel;
	}
	if((tot_col ==0)&&(stnum >0))tot_col =1;
	
	if(flst->infg){
		if((infp=fopen(flst->infname,"r")) ==NULL){//open and read file
			printf("Error with file\n");  
			exit(1);
		} 
		while((fgets(line, sizeof(line), infp))!=NULL)	frows ++;
	}
	if(infp!=NULL)fclose(infp);


	file_data = malloc(sizeof(double *)*frows);
	for(i=0;i<frows;i++){
		file_data[i] = malloc(sizeof(double )*(tot_col+1));
	}
	if(file_data ==NULL)
	{
		printf("Memory error \n");
		exit(1); 
	}
	
	if( (tot_col>0) && ((flst->infg)==1) ) {//Read file
		file_read(flst->infname, file_data, tot_col+1, &rows);
	}
	if(stnum>0)calc_struct(file_data, rows, tot_col, st, stnum, flst);

	if(flst->helpfg==1) { //for -h option
		print_help();
		exit(0);
	} 
	
	if(flst->versionfg==1) { // for -v
		print_ver();
		exit(0);
	} 

	/* To sort o/p */
	col = malloc(stn * sizeof(int));
	storder = malloc(stn * sizeof(int));	

	
	sort_st(st, stnum, col, &stn);

	output(st, stn, col, flst);//sort and print results
	
	if(outfp!=NULL)fclose(outfp);

	return 0;
}

