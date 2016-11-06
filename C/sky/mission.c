#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define PI 3.14159265

double  exposure(double ,double,double *);

int main(int argc, char *argv[]) 
{
  double dec,deccom,dec1,dec2,decarray[200],steparray[200];
  double exptime,decstep,totexp=0.0,decincr;
  int i,j=0,k,n, sa=0;
  int commands=0,dayno=1;
  int day,hour,min,sec;
  char y[]="y";
  char ch;
  float strcount=0;
  float strlimit_high=2000.0,strlimit_med=1000.0,strlimit_low=100;
  char *code_pon;
  char *code_mdp_dir;
  double mdp_num;
  FILE *filestray;

  filestray=fopen("straytable.txt","r");
  if(filestray==NULL) {
	printf("cannot open file");
	exit(1);
  }  

  if((argc < 4)||(argc>5))  {
        printf("Arguments: dec_start, dec_end, exposure_time, y (for command)\n");
        exit(1);
  }

  if((argv[4])!=0){
  	if(!strcmp(y,argv[4])) {
  	commands=1;
  	}
 
 	else{
 	printf("4th argument should be 'y' to print commands\n");
	exit(1);
 	}
  }
 
  if((dec1<-90)||(dec2>90)) {
        printf("dec not in range\n");
        exit(1);
  }
  
  if((dec1=atof(argv[1]))>(dec2=atof(argv[2]))) {
        printf("Please type dec in increasing range\n");
        exit(1);
   }

  if((exptime=atof(argv[3]))<=0) {
        printf("Exposure time should be >1\n");
        exit(1);
   }

 
  printf("\ndecl steps for dec from %.2f deg. to %.2f deg.for mean exposure time of %.1f sec are: \n", dec1, dec2,exptime);
 
for(dec=dec1;dec<=dec2;dec+=decincr){
   if((dec>=90)||(dec<-90)) 
   break;

   for(decstep=0.001;decstep<=0.9;decstep+=0.001) {
       exposure(dec,decstep,&totexp);
       decincr=1;

       if((totexp-exptime)<1) {
//          printf("1-dec= %.1f, step=%.3f\n" ,dec,decstep);
	    
	    decstep > 0.7 ? decstep=0.7:decstep;
	    decarray[sa]=dec;
	    steparray[sa]=decstep;
	    sa+=1;
            break;
       }
       else  if(totexp>exptime) {
	    decstep+=0.001;
 	    exposure(dec,decstep,&totexp);
	    
            if(totexp<exptime) {
//            printf("2-dec= %.1f, step=%.3f\n",dec,decstep);
	    	decstep > 0.7 ? decstep=0.7:decstep;
	    	decarray[sa]=dec;
	    	steparray[sa]=decstep;
	    	sa+=1;
            	break;
            }
       }

     else if(totexp<exptime){
	   decstep+=-0.001;
	   exposure(dec,decstep,&totexp);
           if(totexp>exptime) {
//           printf("3-dec= %.1f, step=%.3f\n", dec,decstep);
	   	decstep > 0.7 ? decstep=0.7:decstep;
	   	decarray[sa]=dec;
	   	steparray[sa]=decstep;
	   	sa+=1;
           	break;
           }//inner if
      } // this elseif

    } //inner for
 }// outer for
  

//print arrays
   for(n=0;n<sa ;n+=1) {
   	printf("decl= %.1f,next_step= %.3f\n",decarray[n],steparray[n]); 
   }
//   printf("For dec steps > 0.7 exposure time for middle regions will be much less than other regions\n");
//=====================================================

   if(commands==1) {
   	printf("\nCommand for Scan bet dec1=%0.1f and dec2= %0.1f:\n\n",dec1,dec2);
  	deccom=dec1;

//get hour. hour = start obs hour - 30 min. (stray light table)
	if(strcount<=strlimit_high) {
		ch=fgetc(filestray);

//		if(ch==EOF)
//		break;
	}
// get day no 
	if((hour<24)&&(hour>12)) {
   		day=1;
	}
	else {
		day=2;
	}

	hour=17,min=30,sec=0; 
	code_pon="TVX_12";

   	printf("%02dT%02d:%02d:%02d   %s 	A_POWER ON (INIT, STBY mode)\n", day,hour, min, sec, code_pon);

	min=45,code_pon="TVX_25.1",code_mdp_dir="NORTH";
	printf("%02dT%02d:%02d:%02d   %s     %s	 MDP movement direction\n", day,hour, min, sec, code_pon,code_mdp_dir);

 	sec=3,code_pon="TVX_25.2", mdp_num=6833.0;//,no_steps="NORTH";
	printf("%02dT%02d:%02d:%02d   %s   %.0f MDP no. of steps\n", day,hour, min, sec, code_pon, mdp_num);

	sec=6,code_pon="TVX_25.3";
	printf("%02dT%02d:%02d:%02d   %s   Activate  MDP movement\n", day,hour, min, sec, code_pon);
  
	
    }//while loop to write commands

   return 0;
 } //main

// subroutines
 double  exposure(double dec,double decstep,double *totexp){
	double offax,factor,t0=216;
	double exp,exp2;
	*totexp=0;
	for(offax=-0.45;offax<=0.45;offax+=decstep){
		factor=t0/(cos(dec*PI/180));
		exp=cos(offax*200*PI/180);
		exp2=factor*exp;
		*totexp=*totexp + exp2;
	}
    }
