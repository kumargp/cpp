
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#define PI 3.14159265

void  findexp(double dec,double decstep, double *avexp);

int main(int argc, char *argv[]) 
{
   double dec=0,dec1=0,dec2=0,exptime=0,decstep=0,exp=0,avexp=0,decincr=0;
   int i=0;

   dec=atof(argv[1]),decstep=atof(argv[2]);

//    printf("Usage example: \n ./exec 34 0.2\n arguments: \n 1. dec1 \n 2. step\n");
//    printf("----------------------------------------\n");

   findexp(dec,decstep,&avexp);
//    printf("avexp=%.0f\n",avexp);

   return 0;
} 

void  findexp(double dec,double decstep,double *avexp)
{
	int i=0,j=0,k=0,n=0,imin=0,imax=0;
	double exp1=0,totexp,min,max, offax=0,offaxcent=0,pix=0, dec1,dec2,strip=0.9,exp=0,step=0, t0=215.4,pixels[1025],pixs[50][1025],num,tt[125],decarr[1025],offaxarr[1025],scancent[50];

// 	printf("IN findstep :: dec=%.4f,decstep=%.4f  \n",dec,decstep);
	for(i=0;i<1025;i++) pixels[i] =0;

	pix=0.9/1024;
	j=-1;
	for(dec1=dec-strip;dec1<dec+strip;dec1+=decstep){

		j++;
		offaxcent=dec-dec1;
		scancent[j]=dec1;
// 		printf("dec1=%.4f,offaxcent= %.4f\n",dec1,offaxcent);
		i=-1;
		for(offax=offaxcent-0.45; offax<offaxcent+0.45;offax+=pix){
			i++;			
			if(offax >0.45 || offax < (-0.45)) continue;
			dec2=dec1;
			if(dec1 > 89.85) dec2=89.85;
			if(dec1 <-89.85) dec2=-89.85;
			exp =(t0/(cos(dec2*PI/180))) * cos(offax*200*PI/180);
			pixels[i] +=exp;
			pixs[j][i]=exp;
			decarr[i]=dec2+offax;
			offaxarr[i]=offax;
// 			if(i%50==0)printf("scan%d: pixels[%d]=%.0f\n",j,i,exp);
// 			if(i%50==0) {printf("offax= %.3f, offaxdec= %.3f,exp=%.1f ,pixels[%d]= %.1f\n",offax, dec1+offax,exp,i, pixels[i]);
			}

		}

/*
	for(i=0;i<1024;i++) {
// 		if(i%10==0) printf("pixels[%d]=%.0f\n",i,pixels[i]);
	// 	printf("%.0f\n",i,pixels[i]);
		totexp +=pixels[i];
		if(i==0)min=pixels[i];
		if(pixels[i] < min) {min=pixels[i];imin=i;}
		if(pixels[i] > max) {max=pixels[i];imax=i;}
	}
*/
// for(i=0;i<=j;i++)printf("#scan%d center DEC=%.4f \n",i,scancent[i]);

printf("#========================================================\n");

 printf("#scanDEC_cent in next line is for each scan whose exp given from third column till last but one\n#");
 for(i=0;i<=j;i++){printf("%.4f,",scancent[i]);if(i==j/2)printf("\n#");}
 printf("\n");

printf("#pix;pixDEC; ******exposre times for each scan****;Tot_exp\n");
	for(i=0;i<1024;i++) {
// 	   if(i%20==0)printf(" [%d] ",i);
printf("%d ",i);
printf(" %.4f ",decarr[i]);
	   for(k=0;k<=j;k++) {
		printf("%.0f ",pixs[k][i]);
		tt[i]+=pixs[k][i];
	   }

	 printf(" %.0f\n",tt[i]);
		if(i==0)min=tt[i];
		totexp +=tt[i];
		if(tt[i] < min) {min=tt[i];imin=i;}
		if(tt[i] > max) {max=tt[i];imax=i;}
	}

	*avexp =totexp/1024;
	printf("\n#avexp=%.0f\n",*avexp);
	printf("\n#maxexp[%d]=%.0f\n",imax,max);
	printf("#  minexp[%d]=%.0f\n",imin,min);

}

