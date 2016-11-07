
/*
 mstatfn.c
 Copyright: HATNet project, Harvard-Smithsonian CFA

 mstater.c for statistics handling  multiple  coulms 

Usage: ./mstater -h
 Author: Gopakumar Perumpilly: gopan[dot]p[at]gmail[dot]com

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <stdarg.h>
#include <getopt.h>
#include <regex.h>
#include <sys/types.h> 
#include <math.h>
#include <fcntl.h>
#include <unistd.h> 

#include "mstatfn.h"

#ifndef STEP
	#define STEP 10
#endif




void print_help() { // Usage message
	printf("\nUsage: \n \t --stat or -sci[cj...]:fn[,fm,...,ck...:fp,fq...\
]\nwhere ci, cj,.. are coulm number(eg: c4,c6);  i,j,k, are any positive\
numbers; fn,fm,.. are function names(eg: mean, median,..);\n\t --input\
or -i <file> \n\t  --output or -o <file> \n\t --mean or -m ci[cj...] \n\t\
--median or -d ci[cj...]  \n \t --mode or -p ci[cj...]\n\t --correl or -c\
ci[cj...] \n\t --meandev or -M  ci[cj...]\n\t --mediandev or -D ci[cj...]\
\n\t --moddev or -K ci[cj...]\n\n");

}


void print_ver() {
	printf("# Version: 0.2\n");
	return;
}


void split(char *line, double **col_arr, int coln, int row)
{
	int j=0,k=0,tokn=0;
	char  res[200][50];
	char *token;
	const char delim[] = " \t;,:";

	k=0;
	token = strtok(line, delim);
	while (token)
	{
		strcpy(res[k++],token);	
		token = strtok (NULL, delim);
	}
	tokn =k;
	k=0;
	for(j=0; j< coln; j++){
		col_arr[row][j]= atof(res[j]);
	}
	return;
}



void file_read(char *fname, double **col_arr, int coln,  int *rows)
{

	FILE *infp=NULL;
	char line[200];
	int n=0,rc,row=0;
	char pattern[] = "[#/^$&@'~`*()\"]{1,}";

	n = row = *rows;

	if((infp=fopen(fname,"r")) ==NULL){//open and read file
		printf("# Error opening file\n");  
		exit(1);
	} 


	regex_t * re = (regex_t *)malloc(sizeof(regex_t)); 
	rc = regcomp(re, pattern,REG_EXTENDED|REG_NOSUB);


	while((fgets(line, sizeof(line), infp))!=NULL)
	{

		if((strcmp(line," ")==0)||((strcmp(line,"\n")==0)))continue;
		if(regexec(re, line, 0 , 0 , 0 ))
		{
			split( line, col_arr,coln, row);
			row++; 
		}
		strcpy(line,"");
	}
	if (!feof(infp)) {
		printf("# ERROR: bad data item in file\n");
		exit(1);
	}

	*rows =row;

	//regfree(re); 
	if(infp!=NULL)fclose(infp);
	return;
}



void add2st(char *fun, int  col_list[], int coln, myst ** st, int *stnum) 
{
	int i,j,n=0,cfg1=0,cfg2=0,stn,fnn,fni,k,new=1,fnrepeat =0;
	myst **tmp;
	
	k = *stnum;
	if(k>STEP)n = k;
	
	for(j=0; j < coln; j++){
		for(stn =0; stn< (*stnum); stn++){//if st exists
	
			if( ((*st)+stn)->colum == col_list[j] ) {
				new = 0;
				break; //from stn loop, keep stn value
			}
			else new = 1;
		}	
		
		if(new){
				if ((k>STEP)&&(k >= n)) {
					if ((tmp = realloc(st, sizeof(myst *)*(n + STEP))) == NULL)
					{
						printf("# Realloc error! \n");
					}
					for (i = n; i < n + STEP; i++) {
						tmp[i] = malloc(sizeof(myst));
					}
					n += STEP;
					st = tmp;
					free(tmp);
				}

			if(strcmp(fun,"correl")==0) {
				if (cfg1==0){
					if(coln >0){

						((*st)+stn)->colum = col_list[0];
						((*st)+stn)->ccorrel =col_list[1];
						((*st)+stn)->fnn =1; 
						
						strcpy(((*st)+stn)->fn[0], fun);
						cfg1=1;
						*stnum +=1;//icremented
					}
					if(coln==1){
						printf("# Correl needs two columns !!\n");
					}else if ((coln >2)&&(cfg1=1))printf("Correl takes FIRST\
two columns only!!\n");

				}
					
			}else{
				((*st)+stn)->colum = col_list[j];
				((*st)+stn)->fnn =1; 
				strcpy(((*st)+stn)->fn[0], fun);
				*stnum +=1;//icremented
			}	

		}else {

				fnn = (((*st)+stn)->fnn); //if st extists
				for(fni =0; fni< fnn; fni++){ //if fn exits in the same st
					if (strcmp( (((*st)+stn)->fn[fni]),fun )==0) {
						fnrepeat =1; 
						break; 
					}
				}
				if(!fnrepeat){

					if(strcmp(fun,"correl")==0){
						if((cfg1==0)&&(cfg2==0)){
							if(coln>0){

								((*st)+stn)->ccorrel =col_list[0];
								strcpy(((*st)+stn)->fn[fnn], fun);
								((*st)+stn)->fnn +=1; 
								cfg2=1;
							}
							if(coln==1){
								printf("# Correl needs two columns !!\n");
							}else if (coln >2)printf("# correl takes FIRST two\
columns only!!\n");
						}
					}else {
						strcpy(((*st)+stn)->fn[fnn], fun);
						((*st)+stn)->fnn +=1; 
					}
				}
				fnrepeat =0;
		}
		
	}
	return;

}



void parse_argterm(char *fn, char *com_arg, myst **st, int *stnum)
{//
	char *token;
	const char delim[] = ",:;";
	char res[100][50], copy[100];
	int j,k=0,n=0,tokn,iftok=0,stn=0;
	myst **tmp;

	k = *stnum;
	n = k;
	
	if(k>=STEP){
		if ((tmp = realloc(st, sizeof(myst *)*(k+1) )) == NULL) {
			printf("# Memory realloc error! \n");
		} 
		tmp[k] = malloc(sizeof(myst));
		st = tmp;
		free(tmp);
	}

	if(k ==0 )k=1;

	strcpy(((*st)+n)->fn[0], fn);
	((*st)+n)->term=1;

	strcpy(copy, com_arg);
	
	j=0;
	token = strtok(copy, delim);
	while (token)
	{
		strcpy(res[j++], token);
		iftok = 1;
		token	 = strtok (NULL, delim);

	}
	tokn = (iftok==1)?j:1;
	((*st)+n)->valn=tokn;
	((*st)+n)->term=1;

	strcpy(((*st)+stn)->fn[0], fn);
	((*st)+stn)->fnn=1;

	for(j=0; j <tokn; j++)
	{
		((*st)+n)->tvalues[j]= atof(res[j]);
	}

	*stnum =k;
	return;
}


void parse_argstat(char *com_arg, myst **st, int *stnum)
{
	char copy[100],*token, *pc;
	const char delim[] = ":,;-";
	char res[100][50], substr[50];
	char fns[100][50];

	int i=0,j=0,k=0,f=0,nfn,cn=0,cflg=0,fflg=0,tokn,iftok=0,cint1=-1;
	int cols[50][50], col_list[50];
	
	strcpy(copy,com_arg);
	i=0;
	token = strtok(copy, delim);
	while (token)
	{
		strcpy(res[i++], token);
		iftok = 1;
		token = strtok (NULL, delim);

	}
	tokn = (iftok==1)?i-1:0;

	for(j=0; j <=tokn; j++)
	{
		strcpy(substr,res[j]);
		pc = strchr(substr, 'c'); // position of match is p
		while (pc != NULL) {
			sscanf( substr+(pc-substr), "c%d", &cint1);//if c[0-9]
			if(cint1 != -1){ //if c[0-9]
				if(cflg ==0)cn=0;
				col_list[cn++]=cint1;
				cols[j][k++] = cint1;
				cflg = 1;
			}
			cint1=-1;
			pc = strchr(pc + 1, 'c'); //search continues
		}

		if(cflg ==0)
		{
			strcpy(fns[f++],res[j]);
			fflg = 1;
		}
		if(fflg==1){
			if(checkfn(fns[f-1], "st"))add2st( fns[f-1], col_list, cn,st,stnum);
			fflg=0;
		}
		k=0,cflg=0;
	}
	nfn = f;
}




void add2flst(char fn[], char thisarg[], flgst *flst)
{
	char *token;
	int iftok=0;
	const char delim[] = " ,:;";

	token = strtok(thisarg, delim);
	
	while (token)
	{
		if(strlen(thisarg)>strlen(token)) iftok = 1;
		token = strtok (NULL, delim);
	}

	if(iftok){
		printf("# Argument(non-function) is Wrong\n");
		exit(1);
	}

	if(strcmp(fn,"help")==0) flst->helpfg=1;
	if(strcmp(fn,"version")==0) flst->versionfg=1;

	if(strcmp(fn,"infile")==0){
		flst->infg=1;	
		strcpy(flst->infname,thisarg);
			
	}
	
	if(strcmp(fn,"outfile")==0){
		flst->outfg=1;
		strcpy(flst->outfname,thisarg);
	}
	
	if((strcmp(fn,"rejmean")==0)|| (strcmp(fn,"rejmedian")==0)||(strcmp(fn,
"rejmode")==0)){
		flst->rejn = atof(thisarg);
		strcpy(flst->rejfn,fn);
	}

	return;
}


void parse_arg(char *fn, char *arg, myst **st, int *stnum, flgst * flst)
{
	char thisarg[100],*token, *pc=NULL;
	const char delim[] = " :,;-";
	char toks[100][50], substr[50];

	int i=0,j=0,cn=0,alpha=0,cflg=0,tokn,iftok=0,cint1=-1;
	int col_list[100];
	
	if( checkfn(fn, "flst")) add2flst(fn, arg, flst);	
	else {
	
		strcpy(thisarg, arg);
		if(thisarg!=NULL){

			for(j=0,pc= thisarg; j <=strlen(thisarg); j++, pc++)
			{
				if(isalpha(*pc)) alpha=1;
			}
		}
		i=0;
		token = strtok(thisarg, delim);
		while (token)
		{
			strcpy(toks[i++], token);
			iftok = 1;
			token = strtok (NULL, delim);
		}
		tokn = (iftok==1)?i:1;	//ie tokn >= 1 

		for(j=0,cn=0; j <tokn; j++)
		{
			strcpy(substr,toks[j]);
			pc = strchr(substr, 'c'); // position of match is pc
			while (pc != NULL) {
				sscanf( substr+(pc-substr), "c%d", &cint1);//if c[0-9]
				if(cint1 != -1){ //if c[0-9]
					col_list[cn++]=cint1;
					cflg = 1;
				}
				cint1=-1;
				pc = strchr(pc + 1, 'c'); //search continues
			}
		}
		if((!alpha)&&(cflg !=1)) {
			parse_argterm(fn, arg, st, stnum); 
		}
		else if(cflg == 1) {
			if(checkfn(fn, "st"))add2st(fn, col_list, cn, st, stnum);
		}
		else {
			printf("# Invalid Arguments\n");
			exit(1);
		}
	}

	return;
}



int checkfn(char *fn, char sttype[]){
	
	int ret =0,i;
	char stfn[][20]={"mean","meandev","median","mode","moddev","correl"};
	char flfn[][20]={"help","infile","outfile","rejmean","rejmedian","rejmode",
"version"};

	if(strcmp(sttype,"st")==0)for(i=0; i<6; i++) if(strcmp(fn,stfn[i])==0)
		ret =1;
	if(strcmp(sttype,"flst")==0)for(i=0; i<7; i++) if(strcmp(fn,flfn[i])==0)
		ret =1;
	
	if(ret== 1)	return 1;
	else {
		return 0;	
	}	
}


void commd_parse(int argc, char *argv[], myst **st, int *stnum, flgst * flst) 
{
	char ch, fn[20]="";
	int l=-1,li=0;
	static int longind;
	static const char *optString = "hi:o:m:M:d:D:p:K:c:a:b:t:vs:";

	static const struct option longOpts[] = { 
		{ "help", 0, &longind, 0 },
		{"infile", 1, 0 ,'i'},
		{"outfile", 1, 0 ,'o'},
		{"mean", 1, 0 ,'m' },
		{"meandev", 1, 0 ,'M' },
		{"median", 1, 0, 'd' },
		{"mediandev", 1, 0, 'D' },
		{"mode", 1, 0, 'p' },	
		{"moddev", 1, 0, 'K' },
		{"correl", 1, 0, 'c' },
		{"rejmean", 1, 0, 'a' },
		{"rejmedian", 1, 0, 'b' },
		{"rejmode", 1, 0, 't' },
		{ "version", 0, 0, 'v' },
		{ "stat", 1, 0, 's' },
		{ 0,0,0, 0 },
		};

	// manage command line arguments
	while(( ch = getopt_long( argc, argv, optString, longOpts, &li ))!= -1 )  {
	switch( ch ) 
	{
		case 'h': 
			l=0;
			break; 
		case 'i': 
			l=1;
			break;
		case 'o': 
			l=2; 
			break;

		case 'm':
			l=3;
			break;
		case 'M': 
			l=4;
			break;			    
		case 'd': 
			l=5;
			break;
		case 'D': 
			l=6;
			break;
		case 'p': 
			l =7;
			break;    
		case 'K': 
			l=8;
			break;
		case 'c': 
			l=9;
			break;
		case 'a': 
			l=10;
			break;
		case 'b': 
			l=11;
			break;
		case 't': 
			l=12;
			break;
		case 'v': 
			l=13;
			break;	
		case 's': 
			l=14;	
			break;

		case 0:		//to handle first longopt
			l=0;    
			break;
	
		default:  
			break;
		}

		if((li>0)&&(l=-1))strcpy(fn,longOpts[li].name);//if long except li==0
		else if (l!=-1)strcpy(fn,longOpts[l].name); //if long li==0 & short opt

		if(strcmp(fn,"stat")==0)parse_argstat(optarg, st, stnum);
		else if(!(strcmp(fn,"")==0))parse_arg(fn, optarg, st, stnum, flst); 

		strcpy(fn,"");
		l=-1,li=0;

	}
	return;
}



	 
void get_mean(double *listin, int listn, double rejn, char *rejfn, double
*mean)//mean 
{
	int i, listn2=0;
	double av=0, list[listn];
	

	if(rejn !=0){
		sigma_reject(listin, listn, rejn, rejfn, list, &listn2);
	}else{
		for(i=0;i<listn;i++) { 
			list[i]=listin[i]; 
		}
		listn2 = listn;
	}
	
	for(i=0;i<listn2;i++)
	{
		av+= list[i];
	}
	*mean = av/listn2;	
}



void get_med(double *listin, int listn, double rejn, char * rejfn, double *med)
{

	double temp, list[listn];
	int i,j,num,listn2=0;

	num = listn;
	if(rejn !=0){
		sigma_reject(listin, listn, rejn, rejfn, list, &listn2);
	}else{
		for(i=0;i<num;i++)list[i]=listin[i];
	}
	
	for(i=0;i<num;i++)// sort to ascending order
	{
		for(j=i+1;j<num;j++)
		{
			if(list[i]>list[j])
			{
				temp=list[j];
				list[j]=list[i];
				list[i]=temp;
			}
		}
	}

	if(num%2==0) *med = (list[num/2]+list[num/2-1])/2;
	else 	*med = list[num/2];
	return;

}


void get_mode(double *listin, int listn, double rejn, char * rejfn, double *mod)
{

	double temp,mode, list[listn];
	int i,j,num,count=0,max=0,listn2=0;

	num = listn;
	if(rejn !=0){
		sigma_reject(listin, listn, rejn, rejfn, list, &listn2);
	}else{
		for(i=0;i<num;i++)list[i]=listin[i];
	}
	
	for(i=0;i<num;i++)// sort to ascending order
	{
		for(j=i+1;j<num;j++)
		{
			if(list[i]>list[j])
			{
				temp=list[j];
				list[j]=list[i];
				list[i]=temp;
			}
		}
	}
	
	for(i=1; i<num-1; i++)//find mode. This returns only 1 value 
	{
		if(temp!=list[i])
		{
			temp=list[i];
			count=1;
		}
		else
		count++;
		if(count>max)
		{
			max=count;
			mode=temp;
		}
	}
	*mod = mode;
	return;
} 


void sigma_reject(double *list, int listn, double rejn, char fn[], double 
*listnew, int *listn2)
{
	double res=0, dev=0, temp[listn], temp2[listn];
	int i,j=0,k,n,try=1;
	
	n =listn;
	
	for(k=0; k< n; k++)temp[k] = list[k];
	
	while(try){
		
		if( strcmp(fn,"rejmean")==0 ) {

			get_mean(temp, n, 0, "", &res);
			get_meandev(temp, n, 0, "", &dev);
		}
		else if( strcmp(fn,"rejmedian")==0 ) {

			get_med(temp, n, 0, "", &res);
			get_mediandev(temp, n, 0, "", &dev);

		}
		else if( strcmp(fn,"rejmode")==0 ) {

			get_mode(temp, n, 0, "", &res);
			get_moddev(temp, n, 0, "", &dev);
		}

		for(i=0; i< n; i++){
			if( abs(temp[i]-res) <= rejn*dev) {
				temp2[j++] = temp[i];
			}
		}
		if(j == n) try=0;
		else if(j< n) {
			try=1;
			for(k=0; k< j; k++) temp[k] = temp2[k];
			n = j;
			j=0;
		}
	}

	for(k=0; k< j; k++) {
		listnew[k] = temp2[k];
	}
	*listn2 = k;
	return;	
}	



void get_meandev(double *list,  int listn, double rejn, char * rejfn, double 
*meandev)
{
	double var=0, mean=0;
	get_mean(list, listn, 0, "mean", &mean);
	get_var(list,  listn, mean, &var);
	*meandev = sqrt(var);
	return;

}


void get_mediandev(double *list,  int listn, double rejn, char * rejfn, double 
*mediandev)
{
	double var=0, median=0;
	get_med(list, listn, 0, "median",&median);
	get_var(list,  listn, median, &var);
	*mediandev = sqrt(var);
	return;

}


void get_moddev(double *list,  int listn, double rejn, char * rejfn, double 
*moddev)
{
	double var=0, mode=0;
	get_mode(list, listn,0, "mode", &mode);
	get_var(list,  listn, mode, &var);
	*moddev = sqrt(var);
	return;

}


void get_var(double *list,  int listn, double qty, double *var)
{
	double  sumsqr, dev;
	int i=0, num;

	num = listn;	
	
	for(i=1; i<=num; i++)// variance calculation
	{
		dev = list[i] - qty;
		sumsqr += dev*dev;
	}
	*var = sumsqr/(double)num;	
	
	return;
}


void cross_corr(double *lst1, int lstn1, double *lst2, int lstn2, double rejn, 
char rejfn[], double *r)
{ 	//Correlation(r) = NΣXY - (ΣX)(ΣY) / Sqrt([NΣX^2 - (ΣX)^2][NΣY^2 - (ΣY)^2])

	int i=0,n,n1;
	double sumx=0,sumy=0,sumxy=0,sumx2=0,sumy2=0,numer,denom;
	double ls1[100], ls2[100];


	if(rejn !=0){
		sigma_reject(lst1, lstn1, rejn, rejfn, ls1, &n);
		sigma_reject(lst2, lstn2, rejn, rejfn, ls2, &n1);
	}else {
		n = lstn1;
		n1 = lstn2;
		for(i=0; i< lstn1; i++)ls1[i] = lst1[i];
		for(i=0; i< lstn2; i++)ls2[i] = lst2[i];
	}

	if(n != n1)
	{
		printf("# Error in Cross-corr: column lengths differ\n");
		return;
	}


	for (i=0;i<n;i++) {
		sumx += ls1[i];
		sumy += ls2[i];
		sumxy +=ls1[i]*ls2[i];
		sumx2 += ls1[i]*ls1[i];  
		sumy2 += ls2[i]*ls2[i]; 
	}

	numer = n*sumxy - sumx*sumy;// Calculate the numerator

	// Calculate the denom
	denom = sqrt( (n*sumx2 -sumx*sumx)*(n*sumy2 - sumy*sumy) );

	*r = numer/denom;
	return;
}

void corr_list2_file(double **col_arr, int rows, myst **st, int sti, double 
*list1, double *list2)
{
	int col1,col2,i,j,row1,row2;

	row1=0;
	row2= rows;
	col1= ((*st)+sti)->colum;
	col2= ((*st)+sti)->ccorrel;

	for(j=0,i=row1;i<row2;i++){
		list1[j++]=col_arr[i][col1];

	}
	for(j=0,i=row1;i<row2;i++){
		list2[j++]=col_arr[i][col2];
	}
}



void get_list_term(myst **st, int sti, int fi,double *list, int *listn, double 
*rejn, char rejfn[], flgst * flst)
{
	int n,i,j;

	*rejn = flst->rejn;
	strcpy(rejfn,flst->rejfn);

	n = ((*st)+sti)->valn;

	for(j=0,i=0;i<n;i++){
		list[j++]=((*st)+sti)->tvalues[i];
	}
	*listn = j;

}


void get_list_file(double **fdata, int rows, myst **st, int sti, int fi, double 
*list, int *listn, double *rejn, char rejfn[], flgst * flst)
{
	int col,i,j;//,row1,row2;

	*rejn = flst->rejn;
	strcpy(rejfn,flst->rejfn);
	col= ((*st)+sti)->colum;
	
	for(j=0,i=0;i<rows;i++){
		list[j++]=fdata[i][col];
	}
	*listn = j;

}



void calc_struct(double **fdata, int rows, int cols, myst **st, int stnum, flgst
 *flst)
{
	int sti,listn,i,j;
	double rejn,mean,median, mode, meandev, meddev, moddev, correl, list[100],
list1[100],list2[100] ;
	char rejfn[20];

	for(sti=0 ; sti< stnum;sti++ ){ //cn or term

		for(i=0; i<((*st)+sti)->fnn; i++){//functions lookup

			if(((*st)+sti)->term ==1) get_list_term(st, sti, i,list, &listn, 
&rejn, rejfn, flst);
			else get_list_file(fdata, rows, st, sti, i,list, &listn, &rejn, 
rejfn,flst);

			if( strcmp(((*st)+sti)->fn[i], "mean") ==0 ){

				get_mean(list,  listn, rejn, rejfn , &mean);
				((*st)+sti)->result[i]= mean;
			}
			if( strcmp(((*st)+sti)->fn[i], "median") ==0 ){

				get_med(list,  listn, rejn, rejfn , &median);
				((*st)+sti)->result[i]= median;
			}
			if( strcmp(((*st)+sti)->fn[i], "mode") ==0 ){
				get_mode(list,  listn, rejn, rejfn , &mode);
				((*st)+sti)->result[i]= mode;
			}
			if( strcmp(((*st)+sti)->fn[i], "meandev") ==0 ){
				get_meandev(list,  listn, rejn, rejfn , &meandev);
				((*st)+sti)->result[i]= meandev;
			}
			if( strcmp(((*st)+sti)->fn[i], "mediandev") ==0 ){
				get_mediandev(list,  listn, rejn,rejfn , &meddev);
				((*st)+sti)->result[i]= meddev;
			}
			if( strcmp(((*st)+sti)->fn[i], "moddev") ==0 ){
				get_moddev(list,  listn, rejn, rejfn , &moddev);
				((*st)+sti)->result[i]= moddev;
			}
			if( strcmp(((*st)+sti)->fn[i], "correl") ==0 ){
				if(((*st)+sti)->term ==1){
					for(j=0; j<listn/2; j++)list1[j] = list[j];
					for(j=listn/2; j<listn; j++)list2[j-listn/2] = list[j];
					cross_corr(list1, listn/2, list2, listn/2, rejn, rejfn, 
&correl);
				}else{
					corr_list2_file(fdata,rows, st, sti, list, list2);
					cross_corr(list, rows, list2, rows, rejn, rejfn, &correl);
				}
				((*st)+sti)->result[i]= correl;
			}
		}
	}
	return;
}




void 	sort_st(myst **st, int stnum, int col[], int *stn){

	int i,j,temp;
 
	for(i=0,j=0;i<stnum;i++){//sort o/p 

		if (((*st)+i)->fnn > 0) {
			col[j] = ((*st)+i)->colum;
			//storder[j]=i;
		}
		j++;
	}
	*stn = j;


	for(i=0;i<(*stn);i++)// to ascending 
	{
		for(j=i+1;j<(*stn);j++)
		{
			if(col[i]>col[j])
			{
				temp=col[j];
				col[j]=col[i];
				col[i]=temp;
				//tem = storder[j];
			}
		}
	}
}





	/*========================================================================
	Sorting o/p. Relies on one struct for one column, and function name	doesn't
	repeat in a structure. Also the order of the result is same as that	of the
	function stored in the same structure.
	=========================================================================*/
	void output(myst **st, int stn, int col[], flgst *flst){

		int i,j,k,l,m,n,fnn;
		char str[20][20],temch[20];
		
		FILE *outfp=NULL;
		
		if((flst->outfg)==1) { // for output file
			outfp = fopen(flst->outfname,"w");
			if(outfp ==NULL){
				printf("Output file error !\n");
				exit(1);
			}
		} 
		
		for(i=0; i<stn; i++){//Col sorted order
			k = col[i];	
			for(j=0; j<stn; j++){
				if( (((*st)+j)->colum ==k ) &&( ((*st)+i)->term !=1 )) {
					fnn =((*st)+j)->fnn;
					for(n=0; n<fnn; n++) {//get fns
						strcpy(str[n],((*st)+j)->fn[n]); /*store func in str
array*/
					}	

					for(n=0; n<fnn; n++){ /*sort func for first 3 char only*/
						for(m=n+1; m<fnn; m++){
							for(l=0; l<3; l++){
								if(str[n][l]>str[m][l]){
									strcpy(temch, str[m]);
									strcpy(str[m], str[n]);
									strcpy(str[n], temch);
								}
							}
						}
					}

					for(m=0; m<fnn; m++){ 
						for(n=0; n<fnn; n++){ 
							if(strcmp(str[m],((*st)+j)->fn[n]) ==0){
								if(strcmp(((*st)+j)->fn[n], "correl") ==0){
									if(outfp==NULL)printf("#c%d c%d: %s=%.4lf\n"
									,((*st)+j)->colum,((*st)+j)->ccorrel,
									((*st)+j)->fn[n],((*st)+j)->result[ n]);
									else fprintf(outfp,"#c%d c%d: %s = %.4lf\n",
									((*st)+j)->colum,((*st)+j)->ccorrel,
									((*st)+j)->fn[n],((*st)+j)->result[n]);	
								}else{
									if(outfp ==NULL)printf("#c%d: %s =%.4lf\n", 
									   ((*st)+j)->colum, ((*st)+j)->fn[n],	
										 ((*st)+j)->result[n]);
									else fprintf(outfp,"#c%d: %s = %.4lf\n", 
										((*st)+j)->colum,((*st)+j)->fn[n],
										((*st)+j)->result[n]);
								}
							}
						}
					}		
				}
			}
		
		
			if( ((*st)+i)->term ==1 ) {
				fnn =((*st)+i)->fnn;
				for(n=0; n<fnn; n++) {//get fns
					strcpy(str[n],((*st)+i)->fn[n]); /*store func in str array*/
				}	

				for(n=0; n<fnn; n++){ /*sort func for first 3 char only*/
					for(m=n+1; m<fnn; m++){
						for(l=0; l<3; l++){
							if(str[n][l]>str[m][l]){
								strcpy(temch, str[m]);
								strcpy(str[m], str[n]);
								strcpy(str[n], temch);
							}
						}
					}
				}

				for(m=0; m<fnn; m++){ 
					for(n=0; n<fnn; n++){ 
						if(strcmp(str[m],((*st)+i)->fn[n]) ==0){
							if(strcmp(((*st)+i)->fn[n], "correl") ==0){
								if(outfp ==NULL)printf("#  %s  =%.4lf\n",
								   ((*st)+i)->fn[n],((*st)+i)->result[n]);
								else fprintf(outfp,"# %s = %.4lf\n",
								((*st)+i)->fn[n],((*st)+i)->result[n]);	
							}else{
								if(outfp ==NULL)printf("#  %s  = %.4lf\n", 
								   ((*st)+i)->fn[n],((*st)+i)->result[n]);
								else fprintf(outfp,"#  %s = %.4lf\n",
								((*st)+i)->fn[n],((*st)+i)->result[n]);
							}
						}
					}
				}		
			}
		
		}
		return;
	}
