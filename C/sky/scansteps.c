/* scansteps.c  */


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#define PI 3.14159265

unsigned int debug=0;//if(argc ==5) debug =atoi(argv[5]);

void  findstep(float ,float,float,float,float *, float *,float *,float *,float *,float *, int, int*, float*, float*, float*,int);
void findexp(double *, float ,float ,float ,int *);
int number_type(char *);




int main(int argc, char *argv[]) {

	float dec=0,dec1=0,dec2=0,exptime=0,decstep=0,exp=0,expmin=0,expmax=0,decincr=0,decarr[365],steparr[366], pixst=0,pixen=0,timetot[20000];

	int i=0,j,k,dir=1,pixels=0,scanpixn[100],pn=0,pixtotal=0;

	double finarr[184500];

	unsigned long int p,q,pixtot[184500];

	FILE *fp = NULL,*fp1=NULL,*fp2=NULL;


 	if((argc < 4)||(argc >5)) {
		printf("Arguments: decl_start, decl_end, exposure_time(sec)\n");
		printf("Example  : ./scansteps 30 50 1000 \n\n");
		exit(1);
	}



	if(number_type(argv[1]) || number_type(argv[2]) || number_type(argv[3])) {		
		printf("Invalid Number\n");
		exit(1);
	}

	dec1=atof(argv[1]),dec2=atof(argv[2]),exptime=atof(argv[3]);

	if((argc==5)&&(strcmp(argv[4],"debug")==0))debug =1;


	if((dec1<-90) || (dec2<-90) || (dec1>90) || (dec2>90) || (exptime <=0)) {
		printf("Values are not realistic\n");
		exit(1);
	}


	if((dec1>89.85714)||(dec2>89.85714))printf("Resetting DEC > 89.85714  to 89.85714\n");

	if((dec1<-89.85714)||(dec2<-89.85714))printf(" Resetting DEC < -89.85714  to -89.85714\n");

	if(dec1>89.85714)dec1=89.85714;

	if(dec2>89.85714)dec2=89.85714;

	if(dec1 < -89.85714)dec1 = -89.85714;

	if(dec2 < -89.85714)dec2 = -89.85714;

  	printf("\ndecl steps for dec from %.2f deg. to %.2f deg\n for exposure time of %.1f sec are: \n----------------------------------------\n", dec1, dec2,exptime);
	printf(" accum_av = Average exposure(in sec)/pixel between this and next step from  all accumulated scans \n ");
	printf("max = The highest value of exposure(in sec) between this and next step from all accumulated scans \n ");
	printf("min = The least value of exposure(in sec) between this and next step from all accumulated scans \n ");
	printf("single_max = The highest value of exposure(in sec) at the centre of this one scan \n\n");

	dir=(dec1>dec2)?-1:1;
	pn=0;

  	for(dec=dec1;(dec1<dec2)?(dec<=dec2):(dec>=dec2);dec+=dir*decincr) {

//       	if(i==0) printf("%d: Scan start with decl= %.2f \n",i+1, dec);
      		decarr[i]=dec;
		steparr[i]=decstep;
		if(debug)printf("# dec=  %.3f\n",dec);
// 		if(dec2 < (dec+decstep))dec = dec2;    //???????????????

      		findstep(dec,dec1,dec2,exptime,&decstep,decarr,steparr,&exp,&expmax,&expmin,i,&scanpixn[pn],&pixst,&pixen,timetot,pixtotal);

		pixtotal += scanpixn[pn];
		pixtot[pn] = pixtotal;

// 		fprintf(fp1,"# dec=  %.3f\n",dec);
// 		printf("scanpixn[pn-1]=%d,scanpixn[pn] = %d\n",scanpixn[pn-1],scanpixn[pn]  );

// 		for(j = (pn >=1)?pixtot[pn-1]:0 ; j<pixtot[pn]; j++){
// 			fprintf(fp1," %d j-pixtot[pn-1]= %d; %.0f\n",j,j-pixtot[pn-1],timetot[j-pixtot[pn-1]]);
// 		}

//       	printf("\n>>--pixst= %.3f, pixen = %.3f, scanpixn[%d]= %d, pixtot= %d\n",pixst,pixen,pn,scanpixn[pn], pixtot[pn]);

//       	printf("step %i: decl= %.2f; next step =%.3f\n" ,i,dec,decstep);

		printf("%d: DEC = %.3f, step =%.3f, accum_av=%.1f, max=%.0f, min=%.0f, single_max=%.0f\n" ,i+1,decarr[i],decstep,exp,expmax,expmin, 216./(cos(dec*PI/180)));

// 		fprintf(fp2,"%d   %.3f  %.3f  \n",i,decarr[i],exp);
      		i++,pn++;
      		decarr[i]=dec+dir*decstep;
      		decincr=decstep;

   	}

   	printf("total scans (days)= %i\n\n",i);



//final 
	pn=0;
// 	for (k=0;k<=i;k++) {
		//findexp(finarr,decarr[k],dec1,dec2,&pixels);
// 		printf("findexp::decarr[%d]=%.3f\n" ,k,decarr[k]);
// 	}

// 	for(q=0;q<= pixels;q++){
// 		if((q <20) || (q > pixels-20) || (q%50==0) || ( (finarr[q])< exptime) ) { 			
// 			if(debug==0)fprintf(fp,"%d  %.0f \n",q,finarr[q]);
// 			if(debug==0)if(q%5 ==0)fprintf(fp,"\n");
// 		}
// 		fprintf(fp,"%d  %.0f \n",q,finarr[q]);
// 		if(q==pixtot[pn]){
// 			if(debug==0)fprintf(fp,"\n# scan %d\n", pn );
// 			pn++;
// 		}
// 	}
// 	printf("\n\n");

   	return 0;
} 



void findexp(double *finarr, float dec,float dec1,float dec2, int *pixels) {

	float scandec,pix,pixs,pixe,this,pixang,exp,t0=216.;
	unsigned long int n;		
	
// 	unsigned int debug=0;	

	scandec =dec;

// 	if(scandec > 90) printf("Findexp> scan = %0.3f is >90 ******Error******\n",scandec);
	if(scandec > 89.85714) scandec=89.85714;
	if(scandec <-89.85714) scandec=-89.85714;

	if(dec1 > (scandec -0.45))pixs = dec1;
	else pixs = scandec - 0.45;
// 	if(dec2 < (scandec +0.45))pixe = dec2;
// 	else 
	pixe = scandec + 0.45;
	if(pixe >dec2) pixe =dec2;
	if(pixe >90)pixe =90;
	if(pixs < -90)pixs =-90;

	n = (int)((pixs -dec1)*1024./0.9);
// 	printf("\tnstart = %d  \n", n);
// 	if(debug==1)
// 	printf("Findexp> n_start= %d,scandec =%.3f; pixs=%.3f; pixe=%0.3f \n", n,scandec,pixs,pixe);
 
	for(pix = pixs; pix < pixe; pix+=0.9/1024.0){
//  		for(pix = (dec-0.45);pix < (dec+0.45); pix+=0.9/1024.0){
		pixang = pix -scandec;
		if(pixang > 0.45) pixang = 0.45 ;
		if(pixang < -0.45) pixang = -0.45 ;
		exp += this=(t0/(cos(scandec*PI/180))) * cos(pixang*200.*PI/180.);
		finarr[n] +=this;
// 		if((debug==2) && ((n==1)||(n==20)))printf("2.1:: pix=%.1f;pixang=%.3f, time[%d][%d]=%.0f \n ",pix,pixang*200 ,m,n,time[m][n]);
// 		if(n%30==0)printf("Findexp::this[%d] =%.0f;finarr[%d]=%.0f\n", n,this,n,finarr[n]);

		n++;
	} 
	*pixels =n-1;
// 	if(debug==0)printf("  n_end= %d \n", n);
// 	if((debug==2) &&(pixang <= 0.45)&&(pixang >= -0.45))printf("2.1-:: pixang=%.3f,thisexp=%.0f \n",pixang*200,this);

}







void  findstep(float dec,float dec1, float dec2, float exptime, float *decstep,float *decarr,float *steparr,float *expt,float *expmax, float *expminim, int loop, int *scanpixn,  float *pixst,  float *pixen, float *timetot, int prevtotpix)
{
	int remain=1,checkn=3,check=1,mincheck,offaxcross=0,offaxlow =0,i,j,k=0,n=0,m,s, nrepeat=0,step1=0, step2=0, step3=0,step4=0,step5=0,last=0,whilen=0,arrst=0,arrend=0,arrpos=0,nextnum=0,dir=1,step9=0;

	float scancent,scancent1,step=0,pstep=0,maxstep,decs,dece,pixs,pixe,pixang=0,pix,delta, offax1=0,offax2=0,offax=0,angle=0, angle1=0,angle2=0,expmin,maxexp=0,t0=215.4,scan=0, exp_next=0,exp=0,this1=0,this2=0,timet[1027], time[20][1027],max=0,min=0,pmin=0;

	float pixarr[20][1027],scanarr[50];
	double avexp,totexp;

	char opn[20]="op";
	FILE *fp2 =NULL,*fp3 =NULL;
	float margin =1.0;

	if(dec1>dec2)dir =-1;
	if(!loop) {
		maxexp=t0/(cos(dec*PI/180));
		nrepeat = (int)exptime/maxexp  ;

		if(nrepeat < 1) nrepeat =1;
		step = 90./nrepeat * 1/200.;
	}
	else{
		step = *decstep;
	}
// 	printf("1.0:loop =%d\n",loop);


// 	*decstep=0;	
	expmin = 1.0*exptime;
	remain=1,mincheck =0,whilen=0;
	while(remain){
		whilen ++;


		for(s=0;s<20;s++){for(n=0;n<=1026;n++) {time[s][n]=0;}}
		for(n=0;n<=1026;n++) {timet[n]=0;}
		scan =0,m=0,last=0;

		k=0;
		if((dir==1)&&(dec <= dec1+0.45)) {
			decs=dec1;arrst=1;
// 			printf("1 arrst = %d\n",arrst);
		}//arrst[0]=0  
		else if((dir==-1)&&(dec >= dec1-0.45)) decs=dec1;  		
		else { 
			for(k=loop;k>=0;k-=1){
					if((0.45+step) < ((dir==1)?(dec-decarr[k]):(decarr[k]-dec))) break ;
				}
			k++;
			decs = decarr[k];  
// 			arrst=(dir==1)?(k+1):(k);
			arrst = k+1;
			if(debug)printf("2 arrst = %d\n",arrst);
// 			if(debug)printf("\n\n first;;arrst=k=%d,step=%d,dec=%.3f,decarr[k-1=%d]=%.3f\n" ,arrst,step,dec,k-1,decarr[k-1]);
		}
		if(debug)printf("1.1:k =%d,decs=decarr[k]=%.3f\n",k,decarr[k]);

 
		if((dir==1)&&(dec2 <= dec + step +0.45))dece=dec2+0.45;	     //????????
		else if((dir==-1)&&(dec2 >= dec - step -0.45))dece=dec2-0.45;	 
		else { 
			for(k=0;;k++){
					if((0.45+step) <= (step *k)) break ;    //added = ,check it ???
				}
			dece = dec+ k*dir*step;
		}
		if(dece >90)dece=90;
		if(dece <-90)dece=-90;

		if(debug)printf("\n\n======FS:dec=%.3f;decs=%.3f;dece=%.3f;step=%.3f;k=%d\n",dec,decs,dece,step,k);


		pixs = dec;
 		if((dir==-1)&&(dec2 > (dec+dir*step)))pixe = dec2;
		else if((dir==1)&&(dec2 < (dec+dir*step)))pixe = dec2;
		else pixe = dec+dir*step;
		if(pixe >90)pixe=90;
		if(pixe <-90)pixe=-90;
		*pixst=pixs;
		*pixen=pixe;



		for(scancent=decs,arrpos=0,nextnum=0; (decs<dece)?(scancent <= dece):(scancent>=dece);) {
// 			if(whilen<=10)printf("1: for  scancent=%.3f\n",scancent);
			scancent1 =scancent;
// 			if((scancent > dec2)&&(last==1)) scancent1 =dec2;
			angle1= scancent1;
			if(angle1 > 89.85714) angle1 = 89.85714;
			if(angle1 < -89.85714) angle1 = -89.85714;
			if(debug)printf("\n---------Scancent Loop start::scancent1=%.3f:\n",scancent1);
			n=0,exp=0; // 


			if(debug)printf("2.0.1:: pixs=%.3f; pixe=%0.3f \n", pixs,pixe);
			for(pix = pixs; (decs<dece)?(pix <= pixe):(pix >= pixe); pix+=dir*0.9/1024.0){
				pixang = pix -scancent1;
				if(pixang > 0.45) pixang = 0.45 ;
				if(pixang < -0.45) pixang = -0.45 ;
				time[m][n]=(t0/(cos(angle1*PI/180))) * cos(pixang*200.*PI/180.);
				pixarr[m][n] = pixang;
				*scanpixn = n;
				n++;

// 				if((debug>=2) && ((n<20)||(n>400)||(n%20==0)))printf("2.1:: pix=%.3f;pixang=%.3f, time[%d][%d]=%.0f\n ",pix,pixang*200 ,m,n-1,time[m][n-1]);
 			} //for pix	
			scanarr[m]=scancent1;
			m++;

// 			if((debug>=2) &&(pixang <= 0.45)&&(pixang >= -0.45))printf("2.1- ::pix=%.3f ,pixang=%.3f,thisexp=%.0f \n ",pix,pixang*200,time[m-1][n-1]);
// 			if(debug)printf("scancent_find1:step=%.3f,steparr[%d]=%.3f,arrst=%d,arrpos=%d\n", step,arrst+arrpos,steparr[arrst+arrpos],arrst,arrpos);

			if(dir==1){
// 				printf("up\n");
				if(scancent==dec) {
					scancent += dir*step;
					if(debug)printf("scancent==dec: scancent +=%.3f\n",dir*step);
				}
				else if(scancent<dec) {
					scancent += dir*steparr[arrst+arrpos];
					if(debug)printf("scancent<dec: scancent +=%.3f, step=%.3f,steparr[%d]=%.3f,arrst=%d,arrpos=%d\n", dir*steparr[arrst+arrpos], step, arrst+arrpos, steparr[arrst+arrpos],arrst,arrpos);

					arrpos++;

				}
				else if(scancent>dec) {
					nextnum ++;
					scancent +=dir*step*(1.+ 0.0*nextnum);
					if(debug)printf("scancent>dec :scancent +=%.3f, step=%.3f\n",dir*step,step);
				}
			}
			else if(dir==-1){
// 				printf("down\n");
				if(scancent==dec) scancent += dir*step;
				else if(scancent>dec) {
					scancent += dir*steparr[arrst+arrpos];
					arrpos++;
					if(debug)printf("scancent_find5:step=%.3f,steparr[%d]=%.3f, arrst=%d,arrpos=%d\n", step,arrst+arrpos,steparr[arrst+arrpos],arrst,arrpos);
				}
				else if(scancent<dec) {
					nextnum ++;
					scancent +=dir*step*(1.+ 0.0*nextnum);
					if(debug)printf("scancent_find6:step=%.3f,steparr[%d]=%.3f, arrst=%d,arrpos=%d\n", step,arrst+arrpos,steparr[arrst+arrpos],arrst,arrpos);
				}
			}
			if(debug)printf(" Got scancent=%.3f\n",scancent);
			if(debug)printf("step-1st = %0.3f \n",step);
		}
		for(j=0;j<n;j+=1) { timet[j] =0;}

// 		sprintf(opn,"%d",loop +100);
// 		strcat(opn,"opn.txt");
// 		fp3 = fopen(opn,"w");

// 		fprintf(fp3,"\n# Loopst;dec= %0.3f;dec+step=%0.3f;decs =%.3f;dece =%.3f; step= %.3f\n",dec,dec+step, decs,dece,step);

// 		fprintf(fp3," pixs=%.3f; pixe=%0.3f \n", pixs,pixe);
// 	 	for(s=0;s<m;s+=1) {
// 			fprintf(fp3,"# %.3f ", scanarr[s]);
// 		}
// 		fprintf(fp3,"\n\n");


		for(j=0;j<n;j+=1) {
	 		for(s=0;s<m;s+=1) {
 				timet[j] +=time[s][j];
// 		    		fprintf(fp3," %.3f %.0f  ", pixarr[s][j],time[s][j]);
		    	}
// 		    	fprintf(fp3,": %d  %.0f \n",prevtotpix+j,timet[j]);

		}
// 		fprintf(fp3," pixs=%.3f; pixe=%0.3f \n", pixs,pixe);
// 	 	for(s=0;s<m;s+=1) {
// 			fprintf(fp3," %.3f ", scanarr[s]);
// 		}
// 		fclose(fp3);


		max = timet[0];
		min = timet[0];

		totexp =0,avexp=-1;
		for(j=0;j<n;j++) {
// 			if((debug==2) && (j%50 ==0))printf("2.2:time[%d]=%.0f\n",j,timet[j]);	
			if(max < timet[j]) max =timet[j];
 			if(min > timet[j]) min =timet[j];
			totexp += timet[j];
		}


		*expminim =min;
		*expmax =max;
		if(n>1)avexp =totexp/(n-1);

		if(debug) printf("\n2.3:max=%.0f::min=%.0f\n",max,min);

	   	delta =0.0005;

		if(((dec >= 89.85714)||(dec <= -89.85714)) && (abs(min-expmin) <=5)) {remain=0;}
// 		else if(abs(min - expmin)<=abs(dec)/90.*margin) remain =0;
		else if(abs(min - expmin)<=2) remain =0;
		else if(mincheck) {

			if(min < expmin) {
				if(debug)printf("Mincheck1;step=%.4f\n",step);
				if(step >0)step -= delta;
				else remain=0;	}
			else if(min > expmin){
				if(debug)printf("Mincheck2;step=%.4f \n",step );
				if((step < (maxstep+delta)) && (step>(maxstep-delta)) )remain =0;
				else if(step < maxstep) step +=delta;}
				else remain=0;
				if(step >= 0.9) {step=0.9; remain=1; }
		}
		else {	// mincheck ==0	
				if(debug)printf("Mincheck else;step=%.4f \n",step );
				if((step3 ==1)&&(min > expmin)) { 
					if(debug)printf("Mincheck else2;step=%.4f \n",step );
					if((step5 ==1)&&(step4 ==1))mincheck=1;
					else step +=0.001;
					step4 =1;                }
				else if((step3 ==1)&&(min < expmin)) {
					if(debug)printf("Mincheck else3;step=%.4f \n",step );
					if((step5 ==1)&&(step4 ==1))mincheck=1;
					else step -=0.0002;
					step5=1;                     }
				else {
					if(debug)printf("Mincheck else4;step=%.4f,pstep=%.4f  \n",step,pstep );
					remain =1;
					pstep = step;

					if((step<0.899)&&(step9!=2))step = (step*min)/expmin; //don't change it to 0.9
					if(debug)printf("Mincheck else5;step=%.4f, min=%.0f,expmin=%.0f, remain=%d\n",step,min,expmin,remain);
// 					if(step<0.05)step=0.05;
// 					if((abs(pstep-step)<0.0005)&&(step<0.89)&&(min<expmin))step+=0.05;
					if((step2 ==1) &&(pstep >= step)) step3 =1;
					if((step1 ==1) &&(pstep <= step)) step2 =1;
					if(pstep >=step) step1 =1;
					if(debug)printf("1 step=%.4f\n",step);
					if(step >= 0.899){
						if(debug)printf("2 step=%.4f\n",step);
 						if(debug)printf("Mincheck else6;step=%.4f, min=%.0f,remain=%d,step9=%d,step1=%d,step2=%d,step3=%d\n", step,min,remain,step9,step1,step2,step3);

						if((step9==2)&&(min>expmin))step +=100*delta;

						if((step9>=1)&&(remain==1)&&(min<expmin)){
							step -=delta;step9=2;
  							if(debug)printf("Mincheck else7;step=%.4f, min=%.0f,remain=%d,step9=%d,step1=%d,step2=%d,step3=%d\n", step,min,remain,step9,step1,step2,step3);
						}
						else if (step9==0){
							step9=1;remain=1;step=0.9;
// 						
 							if(debug)printf("Mincheck else8;step=%.4f, min=%.0f,remain=%d,step9=%d,step1=%d,step2=%d,step3=%d\n", step,min,remain,step9,step1,step2,step3);
						}
					}

				}
		}
		maxstep =step;
		pmin=min;

/*
		if(mincheck) {
// 		if(debug) printf("2.3:Mincheck;avexp=%.1f; exptime+abs(dec)/90.*5.)=%.1f;abs(avexp - (exptime+abs(dec)/90.*5.))=%.1f\n",avexp, exptime+abs(dec)/90.*5.,abs(avexp - (exptime+abs(dec)/90.*5.)));

			if(abs(min - (exptime+abs(dec)/90.*5.))<=abs(dec)/90.*10.) remain =0;
			else if(min < exptime) {
				if(step >0)step -= delta;
				else remain=0;
			}
			else if(min > exptime){
				if((step < (maxstep+delta)) && (step>(maxstep-delta)) )remain =0;
				else if(step < maxstep) step +=delta;
// 				if(debug)printf("-------->step=%.4f, maxstep=%.4f\n",step,maxstep);
			}
			
			if(step >= 0.9) {step=0.9; remain=1; }

		}
		else {
			if(debug) printf("2.3:Else Mincheck\n");
// 			printf("expmin+abs(dec)/90.*10.=%.3f\n",expmin+abs(dec)/90.*10.);	
			if((dec >= 89.85) && (abs(min-expmin) <=200)) {
				if(debug==2)printf("2.4 :min =%.1f,step=%.4f\n",min,step );
				printf("2.5 Exp time Reached:Exiting loop\n");	
				remain=0;
				step1=0,step2=0,step3=0;
			}
			else if(abs(min-(expmin+abs(dec)/90.*5.)) <= abs(dec)/90.*10.) {
				if(debug==2)printf("2.6 :min =%.1f,step=%.4f\n",min,step );
				if(debug)printf("2.7 Exp time Reached:Exiting loop\n");	
// 				mincheck=1;
				remain=0;
				step1=0,step2=0,step3=0;
			}
			else { //if(((min < expmin) )  || ((min > expmin) ) ){
				if(debug)printf("2.8 Exp time not reached::refining step\n\n");	
// 				if((step3 ==1)&&(scancent > dec2)) {
// 					printf("#####scancent > dec2 \n");
// 					if(last==1)break;
// 					if(min>=expmin) break;
// 					else {	scancent1 = dec2;last=1;}
// 				}
// 				printf("->step=%.4f, min=%.0f \n",step,min);
				if((step3 ==1)&&(min > expmin)) { 
					if((step5 ==1)&&(step4 ==1))mincheck=1;
					else step +=0.001;
					step4 =1;
					if(debug)printf("if((step3 ==1)&&(min > expmin)) \n" );
				}
				else if((step3 ==1)&&(min < expmin)) {
					if((step5 ==1)&&(step4 ==1))mincheck=1;
					else step -=0.0002;
					step5=1;
					if(debug)printf("else if((step3 ==1)&&(min < expmin))\n");
				}
				else {
					remain =1;
					pstep = step;
// 					printf("last;; step=%.3f \n",step);

					if(step<=0.89)step = (step*min)/expmin; 
					else if((step>0.89)&&(min>expmin))remain=0;
					else if((step>0.89)&&(min<expmin))step-=0.1;
					if(step<0.05)step=0.05;
// 					printf("last2;; step=%.3f,min=%.0f \n",step,min);
					if(pstep >step) step1 =1;
					if((step1 ==1) &&(pstep < step)) step2 =1;
					if((step2 ==1) &&(pstep > step)) step3 =1;
					if(debug)printf("step =%.3f,pstep =%.3f,step1 = %d,step2 = %d,step3 = %d ::",step,pstep,step1,step2,step3);
					if(step >= 0.9)step=0.9;

					if(debug)printf("step =%.3f\n",step);
				}
			}
		}
		maxstep =step;
*/
		if((remain==0)&&(debug))printf(" Got scancent=%.3f\n",scancent);
		if(debug)printf("======While Loop End; Next step = %0.3f \n",step);
	}  //while 
	*expt = avexp;
	*decstep=step;
  
  


 
// 	sprintf(opn,"%d",loop);
// 	strcat(opn,"opn.txt");
// 	fp2 = fopen(opn,"w");

// 	for(j=0;j<n;j++) {
// 		*(timetot+j) =timet[j];
// 		 fprintf(fp2,"last: %d   %.0f\n",j,  *(timetot+j));
// 	}

} //function findstep



int number_type(char *st)
{
    char *point;
    double doub;
    long lon;

    doub = strtod(st, &point);
    if ((st != point) && (*point == '\0'))return 0;
    else return 1;  
}


