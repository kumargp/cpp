
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define NUM 1



int main(int argc, char **argv)
{


	int i, j, k, Ntr, elem, phcount, phnum, count, trvnum, new;
	float p, ph, q, dur, SN=0, SNcumul, dp, recov_rate, recov;
	float E0=50000, Ecurr, jdph, phcent, phmin, phmax, dm, dip, prom;
	int *trvlen, intran=0, outtran=0, trvln, intran_min, Ntr_min;
	float nterm1=0, nterm2=0, rnoise, wnoise, cadence;
	float tranfrac_obs, recov_SN, p1, p2, resphase;
	double jd, cycle, **trvisits, *temp;
	long int jdi, intcycle, intcyclem=0, ii, *intcyc_arr, intcyc_num;
	long int intcyc_max,arr_max,jdn;
	double jd_arr[10000];
	int debug = 0, flag,intold ,intcount;
	


	for(i=0; i<10000 ; i++)
		jd_arr[i] = 53000+sqrt(i)*0.0005*rand()/RAND_MAX;
	
	jdn = i;
	
	resphase = 0.01;
	q = 0.2;
	p1 =0.01, p2=1.5, dp=0.01;

	phnum = (int)1/resphase;
	
	for(p=p1; p<p2 ; p+=dp)//period
	{
		dur = p * q;
		if(dur < 0.0001)
		{
			printf("Transit Duration = 0: Modify P or q !!!\n");
			exit(1);
		}
		if(debug==1)printf("\n\n Period=%.4f  ---- dur = %.4f\n",p,dur);
		recov = 0;
		SNcumul = 0;

		trvnum = 0;
		arr_max = 0;
		phnum = (int)1/resphase;
		for(phcount=0,ph =0; phcount<phnum ; phcount++)//phase
		{

			trvlen = malloc(sizeof(int));
			intcyc_arr = malloc(sizeof(long int));
			trvisits = malloc(sizeof(double *));
			trvisits[0] = malloc(sizeof(double));


			ph = phcount * resphase;
			trvnum = 0;
			intran = 0;
			outtran = 0;

			intcyc_num = 0;
			intcyc_max = 0;

			Ecurr = E0 +  ph/resphase * p;
			phcent = 0.5;
			phmin = phcent - q / 2.0;
			phmax = phcent + q / 2.0;
			
				flag = 1;
				intold = 0;
				intcount = 0;
			
if(debug==1)printf("ph=%.2lf,phmax=%.2lf,phmin=%.2lf,Ecurr=%.2lf\n\n",
ph,phmax,phmin,Ecurr);
			//printf("=====\n");
			for(jdi=0; jdi<jdn ; jdi++)//time
			{

				cadence = (jdi < jdn-1)?(jd_arr[jdi+1]-jd_arr[jdi]):cadence;
				//cadence = cadence/86400.0;
				
				jd = jd_arr[jdi];
				cycle = (jd - Ecurr) / p;
				intcycle =  (int)cycle;

				jdph  = cycle - intcycle ;
				//if(debug==1)
//printf("\n\njdph=%.3lf,cycle=%.4lf,jd=%.4lf,Ecurr=%.2lf,p=%.2lf\n",jdph,
//cycle,jd,Ecurr,p);
				if( ((jdph >= phmin) && (jdph <= phmax) && (phmin < phmax))
								  || ( (jdph >= phmin || jdph <= phmax) &&
(phmin > phmax) ) )
				{
					intran++;
					new = 1;

					if(debug==1)
printf("\n>>..Intran..jdi=%ld,intcyc_max=%ld;intran=%d,\
	intcycle=%ld;arr_max=%ld\n",jdi,intcyc_max,intran,intcycle,arr_max);
					


					for(i=0; i< intcyc_max; i++)//intcyc_num
					{
						for(j=0; j< trvlen[i]; j++)//trvnum
						{
							if(debug==1)	printf("jd=%.4lf;trvisits[%ld][%d]\
=%.4lf\n",	 jd,intcyc_arr[i],j,trvisits[i][j]);
							if(trvisits[i][j] == jd)
							{
								new = 0;
								break;
							}
						}
					}

					intcyc_num = intcyc_max ;
					
					for(i=0; i<intcyc_max ; i++)
					{
						if(debug==1)printf("intcyc_arr[%d]=%ld;intcycle=%ld\n",
						   i,intcyc_arr[i],intcycle);
						if(intcyc_arr[i] == intcycle)
						{
							break;
						}
					}
					intcyc_num = i;//if not exists then max=next num
					
					if(debug==1)printf(":intcyc_num=%ld\n",intcyc_num);

					if((!new) || (intcyc_num<intcyc_max ))
					{
						trvnum = trvlen[intcyc_num];
						//if(debug==1)
						if(debug==1)	printf("old:trvnum=%d,intcyc_num\
=%ld,trvlen[intcyc_num]=%d \n",trvnum,intcyc_num,trvlen[intcyc_num]);
					}
					else if(new)
					{
						trvnum = 0;
						if(debug==1)	printf("new\n");
					}
					if(new)
					{
						trvisits = realloc(trvisits,sizeof(double*)
						*(intcyc_max+1)*(trvnum+1));
						if(intcyc_num==intcyc_max )
							trvisits[intcyc_max] = malloc(sizeof(double));
						else
							trvisits[intcyc_num] =
							realloc(trvisits[intcyc_num], sizeof(double)*
 							(trvnum+1));
						trvlen = realloc(trvlen,sizeof(int)*(intcyc_max+1));
						intcyc_arr = realloc(intcyc_arr,sizeof(long
int)*(intcyc_max+1));
					}


					
					trvisits[intcyc_num][trvnum] = jd;
					intcyc_arr[intcyc_num] = intcycle;

					if(debug==1)printf(" trvnum=%d,intcyc_num\
	=%ld,trvlen[intcyc_num]=%d\n",trvnum,intcyc_num,trvlen[intcyc_num]);
					if(debug==1)printf(" trvisits[%ld][%d]=%.4lf\n"
,intcyc_num,trvnum,trvisits[intcyc_num][trvnum] );
						

					if(debug==1)printf("trvnum=%d\n",trvnum);
					trvnum++;
					trvlen[intcyc_num] = trvnum;

				
if(debug==1)printf("trvlen[%ld]=%d\n",intcyc_arr[intcyc_num],
trvlen[intcyc_num]);

if(debug==1)printf("intran>>trvlen[%ld]=%d;intcyc_arr[%ld]=%ld,jdi=%ld\n"
			,intcyc_num,trvlen[intcyc_num],intcyc_num,intcycle,jdi);

					intcyc_num ++;
					intcyc_max = intcyc_num;

				}


			}
		if(debug==1){
			for(i=0; i< intcyc_max; i++)//intcyc_num
			{
				for(j=0; j< trvlen[i]; j++)//trvnum
				{
					if(debug==1)
						printf("list::ph= %.3f;trvisits[%ld][%d]=%.4lf\n"
,ph,intcyc_arr[i],j,trvisits[i][j]);

				}
			}
		}

			
			Ntr = 0;
		
if(debug==1)if(intcyc_max>0)printf("---->intcyc_max=%ld\n",intcyc_max);
			for(ii=0; ii< intcyc_max; ii++)
			{
				if(debug==1)printf("----\n");
				trvln = trvlen[ii];

				if(debug==1)
					printf(" trvlen[%ld]=%d ; jdph=%.2lf;intcyc_max =%ld\n"
							,ii,trvln ,jdph,intcyc_max);
	
				if(dur > 0)
				{
					tranfrac_obs = trvln * cadence / dur;
				}
				else
				{
				printf("Duration = 0: Period and/or qratio is too small\n");
					exit(1);
				}
			

				if( tranfrac_obs > q && trvln >4)
				{ //intran_min=4
					Ntr++;

					if(debug==1)
						printf("Ntr =%d\n",Ntr);
				}

				//if(Ntr>0)printf("Ntr= %d; Ntr_min= %d\n",Ntr, Ntr_min);
				if (Ntr > 3)
				{//3
					recov++;
					SNcumul += SN;
					if(debug==1)
						printf("!!!! Recovered recov =%.0lf\n",recov);
					break;
				}
			}


			free(trvlen);
			free(intcyc_arr);
			for(i=0; i<intcyc_max; i++)
				free(trvisits[i]);

		//printf(">>>>>>>>>>>\n");
		}//phase
		//
		recov_rate = recov /phnum;//* resphase;
		if(debug==1)printf(" p= %.3f;recovrate =%.2lf, recov=%.4f\n"
,p,recov_rate,recov);
	}//p
	return 0;

}
 