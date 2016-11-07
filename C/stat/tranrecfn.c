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

#define N_PERIOD 100
#define N_PHASE 100
#define TOKEN_NUM 200
#define TOKEN_LEN 200
#define MAXLEN 1000
#define FN_NAME_LEN 100

typedef struct argsst
{

	int jdcol;
	float period[2];
	float deltap;
	float wnoise;
	float rednoise;
	int resphase;
	float dip;
	float qratio;
	int qratio_auto;
	char infile[FN_NAME_LEN];
	char outfile[FN_NAME_LEN];
	int helpfg;
	int verfg;
	int Ntr_min;
	
}argst;

void print_help() 
{ // Usage message
	printf("help \n");
	return;
}

void print_ver() 
{
	printf("# Version: 0.3\n");
	return;
}

int simulate(st)
{

	return 0;
}


void split(char *line, double *jdval, int col, int row)
{
	int j=0,k=0,tokn=0;
	char  res[TOKEN_NUM][TOKEN_LEN];
	char *token;
	const char delim[] = " \t;,:";

	k = 0;
	token = strtok(line, delim);
	while (token)
	{
		strcpy(res[k++],token);
		token = strtok (NULL, delim);
	}
	tokn = k;
	if(tokn<col)
	{
		printf("\n# Warning: Missing entries permitted only at END of rows\n");
	}
	for(j=0; j< tokn; j++){
		if(j == col)
			jdval[row++]= atof(res[j]);
		if(j == col) printf("jdval[%d]=%.1f\n",row-1,jdval[row-1]);
	}
	for(j=0; j< k; j++)
		strcpy(res[j],"");
	return;
}



void file_read(double *jdval, int *rows, argst *st)
{

	FILE *infp=NULL;
	char line[MAXLEN];
	int n=0,rc,row=0,col;
	char pattern[] = "[#/^$&@'~`*()\"]{1,}";
	char infile[FN_NAME_LEN];

	n = row = *rows;
	col = st->jdcol;

	if((infp=fopen(st->infile,"r")) ==NULL)
	{
		printf("# Error opening file\n");
		exit(1);
	}

	regex_t * re = (regex_t *)malloc(sizeof(regex_t));
	rc = regcomp(re, pattern,REG_EXTENDED|REG_NOSUB);

	while((fgets(line, sizeof(line), infp))!=NULL)
	{
		if((strcmp(line," ")==0)||((strcmp(line,"\n")==0)))
			continue;
		if(regexec(re, line, 0 , 0 , 0 ))
		{
			split( line, jdval, col, row);
			row++;
		}
		strcpy(line,"");
	}
	if (!feof(infp))
	{
		printf("# ERROR: bad data item in file\n");
		exit(1);
	}

	*rows =row;
	regfree(re);
	//if(infp!=NULL)fclose(infp);
	return;
}

int checkfn(char *fn)
{
	
	int ret =0,i,match=0;
	char reffn[][FN_NAME_LEN]={"help","infile","outfile","version","col",
								"gauss-noise","rednoise","resphase","dip"};

		for( i=0; i<9; i++ )
		{
			if( strcmp( fn, reffn[i] ) == 0 ) match = 1;
		}

		if(match) 	return 1;
		else 		return 0;
}

void handle_arg(char *fn, char *arg, argst *st)
{
		if( strcmp( fn, "col" )==0 )
		{
			st->jdcol = atoi(arg);
		}
		else if( strcmp( fn, "gauss-noise" )==0 )
		{
			st->wnoise = atof(arg);
		}
		else if( strcmp( fn, "rednoise" )==0 )
		{
			st->rednoise = atof(arg);
		}
		else if( strcmp( fn, "resphase" )==0 )
		{
			st->resphase = atof(arg);
		}
		else if( strcmp( fn, "dip" )==0 )
		{
			st->dip = atoi(arg);
		}
		else if( strcmp( fn, "help" )==0 )
		{
			st->helpfg ==1;
		}
		else if( strcmp( fn, "version" )==0 )
		{
			st->verfg ==1;
		}
		else if( strcmp( fn, "infile" )==0 )
		{
			strcpy(st->infile, arg);
		}
		else if( strcmp( fn, "outfile" )==0 )
		{
			strcpy(st->outfile, arg);
		}

}


void parse_arg(char *fn, char *arg, argst *st)
{
	char *token, *pc=NULL;
	const char delim[] = " :,;-";
	char toks[TOKEN_NUM][TOKEN_LEN], substr[TOKEN_LEN];
	int i=0,j=0,alpha=0,tokn=0,iftok=0,col_list[TOKEN_NUM];

	printf(" fn =%s, arg = %s\n", fn,arg);
	if( !( checkfn(fn) || strcmp( fn, "Period" ) == 0 || strcmp( fn,"qratio")
	== 0) )
	{
		printf("# Invalid Arguments\n");
		exit(1);
	}
	else 
	{
		if( checkfn(fn) )
			handle_arg(fn, arg, st);
		else
		{
			if(strcmp( fn,"qratio")== 0)
			{
				printf("q \n");
				if(strcmp( arg,"auto")== 0)
				{
					st->qratio_auto = 1;
				}
				else
				{
					st->qratio = atof(arg);
				}
			}
			if(strcmp( fn,"Period")== 0)
			{
				if(arg!=NULL)
				{	
					for(j=0,pc= arg; j <=strlen(arg); j++, pc++)
					{
						if(isalpha(*pc)) alpha = 1;
					}
				}
				if(alpha)
				{
					printf("Argument of period contains non-digits !!");
					return;
				}
				i=0;
				token = strtok(arg, delim);
				while (token)
				{
					strcpy(toks[i++], token);
					iftok = 1;
					token = strtok (NULL, delim);
				}
				tokn = (iftok==1)?i:1;	//ie tokn >= 1
				for(i=0; i<tokn; i++)
				{
					if(i<2)
					{
						st->period[i] = atof(toks[i]);
					}
					if(i==2)
						st->deltap = atof(toks[2]);
				}
			}
		}
	}
	return;
}



void commd_parse(int argc, char *argv[], argst *st)
{
	char ch, fn[FN_NAME_LEN]="";
	int l=-1,li=0;
	static int longind;
	static const char *optString = "hvi:o:P:n:m:r:d:q:c:";

	static const struct option longOpts[] =
	{
		{ "help", 0, &longind, 0 },
		{ "version", 0, 0, 'v' },
		{"infile", 1, 0 ,'i'},
		{"outfile", 1, 0 ,'o'},
		{"Period", 1, 0 ,'P' },
		{"gauss-noise", 1, 0 ,'n' },
		{"red-noise", 1, 0, 'm' },
		{"resphase", 1, 0, 'r' },
		{"dip", 1, 0, 'd' },	
		{"qratio", 1, 0, 'q' },
		{"col", 1, 0, 'c' },
		{ 0,0,0, 0 },
	};

	// manage command line arguments
	while(( ch = getopt_long( argc, argv, optString, longOpts, &li ))!= -1 )
	{
		switch( ch )
		{
			case 'h':
				l = 0;
				break;
			case 'v':
				l = 1;
				break;
			case 'i':
				l = 2;
				break;
			case 'o':
				l = 3;
				break;
			case 'P':
				l = 4;
				break;
			case 'n':
				l = 5;
				break;
			case 'm':
				l = 6;
				break;
			case 'r':
				l = 7;
				break;
			case 'd':
				l = 8;
				break;
			case 'q':
				l = 9;
				break;
			case 'c':
				l = 10;
				break;
			case 0:	//to handle first longopt
				l = 0;
				break;
		
			default:
				break;
		}

		if((li>0)&&(l=-1))
			strcpy(fn,longOpts[li].name);//if long except li==0
		else if (l!=-1)
			strcpy(fn,longOpts[l].name); //if long li==0 & short opt

			parse_arg(fn, optarg, st);

		strcpy(fn,"");
		l=-1,li=0;
	}
	return;
}



void get_var(double *list,  int listn, double qty, double *var)
{
	double  sumsqr, dev;
	int i=0, num;

	num = listn;

	for(i = 1; i<=num; i++)// variance calculation
	{
		dev = list[i] - qty;
		sumsqr += dev*dev;
	}
	*var = sumsqr/(double)num;
	return;
}


void sort(float **trvisits,int *trvlen, int intcycle)
{
	int i,j,k,repeat=0,trvl[intcycle];
	
	for(i=0; i< intcycle; i++)
		trvl[i] = trvlen[i];

	for(i=0; i< intcycle; i++)
	{
		for(j=0; j< trvl[i]; j++)
		{
			for(k=j+1; k< trvl[i]; k++)
				if(trvisits[i][j] == trvisits[i][k])
					trvlen[i]--;
		}
	}
}

void prominace(double *list,  int listn, double qty, double *prom)
{
	double  sum, dev;
	int i=0, num;

	num = listn;

	for(i = 1; i<=num; i++)// variance calculation
	{
		dev = list[i] - qty;
		sum += dev;
	}
	*prom = sum/(double)num;
	return;
}


void simulate_transit(argst *st, double *jd_arr, int jdn)
{
	int jdi,i,j,k=0,recov,recov_rate,Ntr,intcycle,elem;
	float jd,p,ph,q,dur,SN,SNcumul,cycle,dp;
	float E0=50000,Ecurr,jdph,phcent,phmin,phmax;
	int *trvlen,intran=0,outtran=0,resphase,l=0,ii,trvln,intran_min;
	float trand_frac,nterm1,nterm2,rnoise,wnoise,cadence;
	float **trvisits,*trvis,*lc,dm,dip,prom;
	float tranfrac_obs,recov_SN,p1,p2;
	
	dip = st->dip;
	resphase = st->resphase;

	lc = malloc(sizeof(float) * 100);
	trvis = malloc(sizeof(float *));
	trvlen = malloc(sizeof(int));
	trvisits = malloc(sizeof(float *));

	for(p=p1; p<p2 ; p+=dp)//period
	{

		if(st->qratio == 0)//set it if 'auto', outside
			q = 0.07566 * pow(p,2/3.0);
		else q = st->qratio;
		
		dur = p * q;
		recov = 0;
		SNcumul = 0;

		for(ph=0; ph<resphase ; ph++)//phase
		{
			i = 0;
			intran = 0;
			outtran = 0;
			Ecurr = E0 +  ph / resphase * p;
			phcent = 0.5;
			phmin = phcent - q / 2.0;
			phmax = phcent + q / 2.0;

			for(jdi=0; jdi<jdn ; jdi++)//time
			{
				cadence = (jdi < jdn-1)?jd_arr[jdi+1]-jd_arr[jdi]:cadence;
				jd = jd_arr[jdi];
				cycle = (jd - Ecurr) / p;
				intcycle =  (int)cycle;
				if(elem != intcycle) i = 0;
				trvisits = realloc(trvisits, sizeof(float *) * (intcycle+1));
				trvlen = realloc(trvlen, sizeof(int) * (intcycle+1));	
		
				jdph  = cycle - intcycle ;
		
				if( ((jdph >= phmin) && (jdph <= phmax) && (phmin < phmax))
				 || (jdph >= phmin || jdph <= phmax) && phmin > phmax ){
				
					intran++;
					//lc[l++] = jd[0];
					//lc[l++] = intcycle;
					trvisits[intcycle] = realloc(trvisits[intcycle],
						sizeof(float)*(i+1));
					trvisits[intcycle][i++] = jd;
					trvlen[intcycle] = i;
					elem = intcycle;

				}else {
					outtran++;
					//lc[l++] = jd[1];
					//lc[l++] = intcycle;
				}
			}	

			nterm1 = wnoise*wnoise/intran;
			nterm2 = wnoise*wnoise/outtran;		
			dm = dip;


			SN = dm / sqrt(nterm1 + nterm2 + rnoise*rnoise);	

			//prominace(list, listn, mean,  *prom)

			// Find if Ntr> Ntr_min transits			
			Ntr = 0;
			sort(trvisits, trvlen,intcycle);

			for(ii=0; ii< intcycle; ii++) {
			
				trvln = trvlen[ii];				
				tranfrac_obs = trvln * cadence / dur;
				
				if( tranfrac_obs > st->qratio && trvln >
intran_min ){//>0.2,>4
					Ntr++;
				}
				if (Ntr > (st->Ntr_min)) { //>2
					recov++;
					SNcumul += SN;
					printf("Recovered \n");
					break;
				}
			}
		}//phase		
		// Recovery rate 

		recov_rate = recov / resphase;
		
		if (recov > 0) {
			recov_SN = SNcumul / recov;
		} else {
			recov_SN = 0;
		}

		
		//printf("p=%.3f; recov_rate=%d;q=%.3f; p*q=%.3f;recov_SN= %.2f",p,
			//recov_rate,q, p*q,recov_SN);

	}//p
	return;
}




void output(argst *st, int stn)
{
	int i,j,k;
	FILE *outfp=NULL;

	if(strcmp(st->outfile, "")!=0) { // for output file
		outfp = fopen(st->outfile,"w");
		if(outfp ==NULL){
			printf("Cannot create output file !\n");
			exit(1);
		}
	}

	for(i=0; i<stn; i++)
	{
	
	
	}
	
	
	
	
	
	return;
}
	


