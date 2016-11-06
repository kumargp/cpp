/* decstepnew.c  */


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#define PI 3.14159265

void  findstep(float dec,float exptime, float *decstep,float *exp);

int main(int argc, char *argv[]) 
{
  float dec=0,dec1=0,dec2=0,exptime=0,decstep=0,exp=0,decincr=0;
  int i=0;

  dec1=atof(argv[1]),dec2=atof(argv[2]),exptime=atof(argv[3]);

  for(dec=dec1;dec<=dec2;dec+=decincr)
  {
      findstep(dec,exptime,&decstep,&exp);
      i++;
      printf("step %i:decl= %.2f step for next dec=%.3f, with exp=%.1f\n" ,i,dec,decstep,exp);
      decincr=decstep;
   }
   printf("total scans= %i\n",i);
   return 0;
} 

void  findstep(float dec,float exptime, float *decstep,float *expt)
{
	int remain=1,checkn=3,check=1,offaxcross=0,offaxlow=0,i,j,k,n=0,nrepeat=0;
	float step=0,offax=20,offax1=0,offax2=0,offax4=0,angle=0,angle1=0,angle2=0,maxexp=0,t0=215.4,scan=0,exp_next=0,exp=0,this1=0,this2=0;

// 	*exp=0;

	*decstep=0;
	maxexp=t0/(cos(dec*PI/180));
	printf("maxexp=%.1f,exptime_needed=%.1f\n",maxexp,exptime);
/*
	if(exptime <= maxexp) {
		printf("1:only 1 scan\n");
		while(remain){
			printf("1:remains \n");
			n=0,exp=0;
			for(angle=-offax;angle< offax;angle+=0.5){
				exp +=	maxexp * cos((angle)*PI/180);
				n++;
// 				printf("1.0  exp=%.1f\n",maxexp * cos((angle)*PI/180));
			}
			scan = exp/n;
			printf("1,1:n=%i,offax=%.3f,scan =%.1f\n",n,offax,scan);
			if(abs(scan-exptime) <5) remain=0;
			else if((scan < exptime) && (offax > 0)) offax -=1;
			else if((scan > exptime) && (offax < 90)) offax +=1;
 	 		if((offax >90) || (offax <0)){ remain=0;break;}

		}

		while(0*check) {
			check=0;
			offax1 = (90 - offax)/checkn;
			for(j=1;j<checkn;j++){
				angle = j*offax1;
				step = (90 + offax)/200;
				n=0,exp=0;
				for(offax2=-angle; offax2 < angle; offax2+=1){
					printf("1.2.0 angle= %.3f,offax=%.3f,offax2=%.3f\n", angle,offax,offax2);
					exp +=	maxexp * cos((offax+offax2)*PI/180)+ ((t0/(cos((dec+ step)*PI/180))) * cos((90-offax2)*PI/180)) ;
					n++;
				}
				exp_next = exp/n;

				if(exp_next < exptime)	{
					printf("1.2.1 exp_next is less than exp time\n");
					offax -=0.1; //improve it
					check=1;
					break;
				}

				if(exp_next > exptime)	{
					printf("1.2.2 exp_next is more than exp time\n");
					offax +=0.1; //improve it
					check=1;
					break;
				}
				if(abs(exp_next-exptime) <5)	{

					break;
				}
			printf("1.2.3 :scan=%.1f,decstep=%.3f,exp_next=%.1f\n",scan,step,exp_next);					
			}
		}
		*decstep = 0.45+offax/200;
		*expt =scan;
	}

*/



// 	else if(exptime > maxexp) {
// 		printf("2:need >1 scan\n");
		remain=1;
		nrepeat = (int)exptime/maxexp  ;
// 		printf(": 2.1  nrepeat=%i\n",nrepeat);
		if(nrepeat < 1) nrepeat =1;
		offax = 90/nrepeat;
		while(remain){
	
				printf("2.1: rough offax=%.2f\n",offax);	
				scan =0;
// 				offax4=-90+offax/2;
   				n=0,exp=0; //comment this if average

				for(offax4=offax/2; offax4<=90 ;offax4+=offax){	
// 				   n=0,exp=0; //for average
// 				   for(angle = offax4-offax/2; angle <=offax4+offax/2; angle+=0.5){ //for average
// 					printf("2.1.1   offax4=%.0f ",offax4);
				      angle1= dec-offax4/200;
				      angle2= dec+offax4/200; 
				      if(angle1==90) angle1=89.99999;
				      if(angle2==90) angle2=89.99999;
				      exp += this1=(t0/(cos(angle1*PI/180))) * cos(offax4*PI/180);
				      exp += this2=(t0/(cos(angle2*PI/180))) * cos(offax4*PI/180);
	
				      n++;
// 			printf("2.1.1 :exp =%.1f,offax4=%.1f,angle=%.1f\n",this,offax4,angle);//for average
			printf("2.1.1 :this1 =%.1f,this2 =%.1f,offax4=%.1f\n",this1,this2,offax4); //comment if averag

// 				   } //for average
// 				   scan += exp/n; //for average
// 					printf("2.1.2 ::::::::scan =%.1f,offax=%.4f\n",exp/n,offax);  //for average
				}
				   scan = exp; //comment this if average
				    printf("2.1.2 ::::::::scan =%.1f,offax=%.4f\n",scan,offax); //comment if averag

				if(abs(scan-exptime) <=10) {
					printf("2.2 :scan =%.1f,offax=%.4f\n",scan,offax/200);
					printf("2.3 Exp time Reached:Exiting loop\n");	
					remain=0;
				}
				else if((scan < exptime) &&(offax > 0)) {
					printf("2.4 Exp time not reached::refining step\n");	

					if(offaxcross==1) {
						
// 						offax1=offax;
						if((offax-offax/10) >= offax2) {
 							offax -=0.1; 
							printf("2.4.1 offax =%.3f \n",offax);
						}
						else if((offax-offax/100) >= offax2) {
 							offax -=0.01; 
							printf("2.5 offax =%.3f \n",offax);
						}	
						else {
							offax -=0.001; 
							printf("2.6 offax =%.3f \n",offax);
						}
// 						offaxcross=0;
					}
					else offax -= 5;
					offaxlow=1;

				}
				else if((scan > exptime) &&(offax < 180)) {
					printf("2.7 Exp time exceeded::refining step\n");

					if(offaxlow==1) {
						
						offax2=offax;
						offax = offax +0.01; 
					}
					else offax +=5;
					offaxcross=1;
				}
				else if((scan > exptime) && (offax = 180)) {
					printf("2.8 Exp time exceeded with maximum step\n");
					remain=0;
				}

// 				else if((abs(scan-exptime) >5) && ((offax >90) || (offax <0))) {
// 					printf("2.6 Exp time not reached::refining step\n");
// 					offax = offax/2; //improve it
// 					remain=1;
// 				}
			*expt = scan;
			*decstep=offax/200;
		}
// 	}



} //function findstep


