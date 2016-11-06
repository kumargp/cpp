

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#define PI 3.14159265

unsigned int debug=0;

void findexp(double *finarr, float dec,float dec1,float dec2, unsigned long int *pixels ); 

int main(int argc, char *argv[]) {


	int i,j,k,pn,stepflag=0,decflag=-1;
	unsigned long int q,pixels=0;
	float startdec=-91,arr[200],decarr[200],enddec=-91,dec ;
	double  finarr[102500];
	char filename[50],tem[50],line[100];
	FILE *fp =NULL, *fp1=NULL;

	printf("type FILENAME\n");
	scanf("%s",filename);
	printf("If file contains absolute decs, type \"dec\"\n");
	printf("Or if file contains only steps, type  START_DEC\n");
	scanf("%s", tem);
	if(strcmp(tem,"dec")==0) {
		decflag =1;
	}
	else {
		startdec = atof(tem);
		stepflag=1;
	}
 
	if((fp1 = fopen(filename,"r")) ==NULL) {
		printf("Couldn't open the specified File :(");
		exit(1);
	}
	if((fp = fopen("exp_times","w")) ==NULL) {
		printf("Could not create file, Check permission of disk space\n");
		exit(1);
	}



        // read line from  file into line[]
        strcpy(tem,"");
	i=0,j=0;
        while ((fgets(line,sizeof(line),fp1))!=NULL) {
        	sscanf(line, "%s", tem);
                if((strcmp(tem,"")==0)||(strcmp(tem,"#")==0)||(strcmp(tem,"#-")==0)||(strcmp(tem,"$")==0))continue;
 		arr[i]=atof(tem);
		i++;
	printf("arr[0]=  %.3f \n",arr[0]);
	}
	if(i==0) {
		printf("File doen't contain numbers !\n");
		exit(1);
	}
	fclose(fp1);


	if(stepflag==1) {
			decarr[0] = startdec;
			for(j=0;j<i;j++) {
				decarr[j+1] = decarr[j]+arr[j];   //check for -ve dec
			}
		startdec = decarr[0];	
		enddec = decarr[i-1];
	}else {
			for(j=0;j<i;j++) {
				decarr[j] = arr[j];   //check for -ve dec
			}
	
		startdec = arr[0];	
		enddec = arr[i-1];	
	}


	if(stepflag)printf("Input is steps \n" );
	else printf("Input is DECs\n" );

// pixtotal += scanpixn[pn];
// pixtot[pn] = pixtotal;
	printf("startdec=  %.3f \n",arr[0]);

	pn=0;
	for (k=0;k<i;k++) {
		findexp(finarr,decarr[k],startdec,enddec,&pixels);
		printf("\n ::decarr[%d]=%.3f " ,k,decarr[k] );

// 	for(q=pixels-100;q<= pixels-80;q++){
// 	printf("%d  %.0f \n",q,finarr[q]);
// 	}

	}

	for(q=0;q<= pixels;q++){
// 		if((q <20) || (q > pixels-20) || (q%50==0) || ( (finarr[q])< exptime) ) { 			
// 			if(debug==0)fprintf(fp,"%d  %.0f \n",q,finarr[q]);
// 			if(debug==0)if(q%5 ==0)fprintf(fp,"\n");
// 		}
		fprintf(fp,"%d  %.0f \n",q,finarr[q]);
// 		if(q==pixtot[pn]){
// 			if(debug==0)fprintf(fp,"\n# scan %d\n", pn );
// 			pn++;
// 		}
	}
	printf("\n\n");

   	return 0;
} 



void findexp(double *finarr, float dec,float dec1,float dec2, unsigned long int *pixels) {

	float scandec,pix,pixs,pixe,this,pixang,exp=0,t0=216.;
	unsigned long int n;		
	
// 	unsigned int debug=0;	

	scandec =dec;

	if(scandec > 90) printf("Findexp> scan = %0.3f is >90 ******Error******\n",scandec);
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

	printf("> startdec=  %.3f \n",dec1);

	printf("> n_start=%d,scandec =%.3f; pixs=%.3f; pixe=%0.3f \n", n,scandec,pixs,pixe);
 
	for(pix = pixs; pix < pixe; pix+=0.9/1024.0){
//  		for(pix = (dec-0.45);pix < (dec+0.45); pix+=0.9/1024.0){
		pixang = pix -scandec;
		if(pixang > 0.45) pixang = 0.45 ;
		if(pixang < -0.45) pixang = -0.45 ;
		exp += this=(t0/(cos(scandec*PI/180))) * cos(pixang*200.*PI/180.);
		finarr[n] +=this;
// 			if((debug==2) && ((n==1)||(n==20)))printf("2.1:: pix=%.1f;pixang=%.3f, time[%d][%d]=%.0f \n ",pix,pixang*200 ,m,n,time[m][n]);
// 	if(n%30==0)printf("Findexp::this[%d] =%.0f;finarr[%d]=%.0f\n", n,this,n,finarr[n]);

		n++;
	} 
	*pixels =n-1;
	if(debug==1)printf("  n_end= %d \n", n);
// 			if((debug==2) &&(pixang <= 0.45)&&(pixang >= -0.45))printf("2.1-:: pixang=%.3f,thisexp=%.0f \n ",pixang*200,this);

}
