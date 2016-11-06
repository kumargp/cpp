//scanstep-findstep.c

extern unsigned int debug;

void  findstep(float dec,float dec1, float dec2, float exptime, float *decstep,float *decarr,float *steparr,float *expt,float *expmax, float *expminim, int loop, int *scanpixn,  float *pixst,  float *pixen, float *timetot, int prevtotpix)
{
	int remain=1,checkn=3,check=1,mincheck,offaxcross=0,offaxlow =0,i,j,k=0,n=0,m,s, nrepeat=0,step1=0, step2=0, step3=0,step4=0,step5=0,last=0,whilen=0,arrst=0,arrend=0,arrpos=0,nextnum=0;

	float scancent,scancent1,step=0,pstep,pstep_orig=0,maxstep,decs,dece,pixs,pixe,pixang=0,pix,delta,allow, offax1=0,offax2=0,offax=0,angle=0, angle1=0,angle2=0,expmin,maxexp=0,t0=215.4,scan=0, exp_next=0,exp=0,this1=0,this2=0,timet[1027], time[20][1027],max=0,min=0,fit;

	float pixarr[20][1027],scanarr[50];
	double avexp,totexp;

	char opn[20]="op";
	FILE *fp2 =NULL,*fp3 =NULL;
// 	unsigned int debug=2;

	if(!loop) {
	maxexp=t0/(cos(dec*PI/180));
	nrepeat = (int)exptime/maxexp  ;

	if(nrepeat < 1) nrepeat =1;
	step = 90./nrepeat * 1/200.;
	}
	else{
		step = *decstep;
	}
// printf("1.0:loop =%d\n",loop);

// 	*decstep=0;	
	allow = 0.05;
	expmin = (1-allow)*exptime;
	remain=1,mincheck =0;
	while(remain){

		whilen ++;


		for(s=0;s<20;s++){for(n=0;n<=1026;n++) {time[s][n]=0;}}
		for(n=0;n<=1026;n++) {timet[n]=0;}
		scan =0,m=0,last=0;

		k=0;
		if(dec <= dec1+0.45) decs=dec1;  //avoid starting with dec < dec1
		else { //find previous entries from decsteps array for finding exp time
			for(k=loop;k>=0;k-=1){
					if((0.45+step) < dec-decarr[k]) break ;
				}
			k++;  // because k-=1
			decs = decarr[k]; 
			arrst=k;
		}
// printf("1.1:k =%d,decs=decarr[k]=%.3f\n",k,decarr[k]);


//do fitting for previous decs to find the variation. Now assume 5 %=0.05
		fit=0.02;
 
		if(dec2 <= dec + step +0.45){ //end dec 
	 		dece=dec2+0.45;	     //?????????????????????????????????
		}
		else { 
			for(k=0;;k++){
					if((0.45+step) < ((step+fit)*k)) break ;
				}
			dece = dec+ k*(step+fit);
		}
		if(dece >90)dece=90;
// 		printf("2k =%d\n",k);


		pixs = dec;  //start pixel from current dec; end is next step
 		if(dec2 < (dec+step))pixe = dec2;
		else pixe = dec+step;
		if(pixe >90)pixe=90;
		*pixst=pixs;
		*pixen=pixe;





//  	printf("step=%.3f: decs=%.3f,dece=%.3f\n",step,decs,dece);

		for(scancent=decs,arrpos=0,nextnum=0; scancent <= dece;) {
// 			printf("1: for  scancent=%.3f\n",scancent);
			scancent1 =scancent;
// 			if((scancent > dec2)&&(last==1)) scancent1 =dec2;
			angle1= scancent1;
			if(angle1 > 89.85714) angle1=89.85714;

			if(debug)printf("---------:angle1 =%.3f; scancent=%.3f:\n",angle1,scancent);

			n=0,exp=0; // 

// printf("2.0.1:: pixs=%.3f; pixe=%0.3f \n", pixs,pixe);
			if(debug)printf("2.0.1:: pixs=%.3f; pixe=%0.3f \n", pixs,pixe);
			for(pix = pixs; pix <= pixe; pix+=0.9/1024.0){
				pixang = pix -scancent1;
				if(pixang > 0.45) pixang = 0.45 ;
				if(pixang < -0.45) pixang = -0.45 ;
				time[m][n]=(t0/(cos(angle1*PI/180.))) * cos(pixang*200.*PI/180.);
				pixarr[m][n] = pixang;
				*scanpixn = n;
				n++;

				if((debug==3) && ((n<20)||(n>400)||(n%20==0)))printf("2.1:: pix=%.3f;pixang=%.3f, time[%d][%d]=%.0f\n ",pix,pixang*200 ,m,n-1,time[m][n-1]);
 			} //for pix	
			scanarr[m]=scancent1;
			m++;

			if((debug==2) &&(pixang <= 0.45)&&(pixang >= -0.45))printf("2.1- ::pix=%.3f ,pixang=%.3f,thisexp=%.0f \n ",pix-0.9/1024.0,pixang*200,time[m-1][n-1]);


			if(scancent==dec) scancent += step;
			else if(scancent<dec) {
				scancent += steparr[arrst+arrpos];
				arrpos++;
			}
			else if(scancent>dec) {
				nextnum ++;
// 				scancent +=step*(1.+ 0.0*nextnum);
				scancent +=step*(1+fit);
			}
		}
		for(j=0;j<n;j+=1) { timet[j] =0;}


/*
		sprintf(opn,"%d",loop +100);
		strcat(opn,"opn.txt");
		fp3 = fopen(opn,"w");

		fprintf(fp3,"\n# Loopst;dec= %0.3f;dec+step=%0.3f;decs =%.3f;dece =%.3f; step= %.3f\n",dec,dec+step, decs,dece,step);

		fprintf(fp3," pixs=%.3f; pixe=%0.3f \n", pixs,pixe);
	 	for(s=0;s<m;s+=1) {
			fprintf(fp3,"# %.3f ", scanarr[s]);
		}
		fprintf(fp3,"\n\n");
*/


		for(j=0;j<n;j+=1) {
	 		for(s=0;s<m;s+=1) {
 				timet[j] +=time[s][j];
//   if(loop==0)		fprintf(fp3," %.3f %.0f  ", pixarr[s][j],time[s][j]);
		    	}
//   if(loop==0) 		    	fprintf(fp3,": %d  %.0f \n",prevtotpix+j,timet[j]);

		}


//  		fprintf(fp3," pixs=%.3f; pixe=%0.3f \n", pixs,pixe);
// 	 	for(s=0;s<m;s+=1) {
//if(loop==0) 			fprintf(fp3," %.3f ", scanarr[s]);
// 		}
// 		fclose(fp3);

		//to sort
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
		else avexp =min;
		if(debug)printf(";; min=%.0f, max=%.0f, avexp =%0.1f \n",min,max,avexp);


	   	delta =0.0005;
		if(mincheck) {
			if(avexp == exptime){ 
				remain =0; 
				if(debug==2)printf("findstep1:avexp==exptime\n");
				}
			else if(avexp < exptime) {
				if(step >0)step -= delta;
				else remain=0;
				if(debug==2)printf("findstep2:avexp<exptime\n");
			}
			else if(avexp > exptime){
				if((step < (maxstep+delta)) && (step>(maxstep-delta)) ){
					remain =0;
					if(debug==2)printf("findstep3:avexp>exptime\n");
					}
				else if(step < maxstep) {step +=delta;
					if(debug==2)printf("findstep4:avexp>exptime; step<maxstep\n");}
				else if(debug==2)printf("findstep5:avexp>exptime::Attention!!!!!!\n");
// 				printf("-------->step=%.4f, maxstep=%.4f\n",step,maxstep);
			}
			

			if(step >= 0.9) {
				if(pstep_orig>=0.9)remain=0;
				else remain =1;
				pstep_orig =step;
				step=0.9;
				if(debug==2)printf("findstep6:avexp>exptime::step=0.9 Attention!!!!!!\n");
			} 
		}
		else {
	
	
			if((dec >= 89.5) && (abs(min-expmin) <=200)) {
				if(debug==2)printf("2.4 :min =%.1f,step=%.4f\n",min,step );
				if(debug)printf("2.5 Exp time Reached:Exiting loop\n");	
				mincheck=1;
				step1=0,step2=0,step3=0;
			}
	
// 			else if(abs(min-(expmin-1.)) <= 1.) {
			else if((min>expmin)&&(min<exptime)) {
				if(debug==2)printf("2.6 :min =%.1f,step=%.4f\n",min,step );
				if(debug)printf("2.7 Exp time Reached:Exiting loop\n\n\n");	
				mincheck=1;
				step1=0,step2=0,step3=0;
			}
			else { //if(((min < expmin) )  || ((min > expmin) ) ){
				if(debug)printf("2.8 Exp time not reached::refining step\n");	
	
// 				if((step3 ==1)&&(scancent > dec2)) {
// 					printf("#####scancent > dec2 \n");
// 					if(last==1)break;
// 					if(min>=expmin) break;
// 					else {
	// 					scancent1 = dec2;
// 						last=1;
// 					}
// 				}


// 				printf("->step=%.4f, min=%.0f \n",step,min);


				if((step3 ==1)&&(min > expmin)) { 
					if((step5 ==1)&&(step4 ==1))mincheck=1;
					else step +=0.001;
					step4 =1;
				}
				else if((step3 ==1)&&(min < expmin)) {
					if((step5 ==1)&&(step4 ==1))mincheck=1;
					else step -=0.0002;
					step5=1;
				}
				else {
					pstep = step;
// printf("last;; step=%.3f \n",step);

					step = (step*min)/expmin; 
					if(step<delta)step=0.05;
// printf("last2;; step=%.3f,min=%.0f \n",step,min);
					if(pstep >step) step1 =1;
					if((step1 ==1) &&(pstep < step)) step2 =1;
					if((step2 ==1) &&(pstep > step)) step3 =1;
					if(debug)printf("minkcheck==0&exptime_not;;dec==%.3f, step =%.3f,pstep =%.3f,step1 = %d,step2 = %d,step3 = %d ::",dec,step,pstep,step1,step2,step3);
					if(step >= 0.9) {
						if(pstep_orig>=0.9)remain=0;
						else remain =1;
						pstep_orig =step;
						step=0.9;
						if(debug==2)printf("findsteplast:avexp>exptime::step=0.9 Attention!!!!!!\n");
					}
					if(debug)printf("step =%.3f\n",step);
				}
			}
		}
		maxstep =step;
		if(debug==2)printf("@@@@findstep mincheck loop last\n\n");

	}  //while 
	*expt = avexp;
	*decstep=step;
  
  


/* 
	sprintf(opn,"%d",loop);
	strcat(opn,"opn.txt");
	fp2 = fopen(opn,"w");
*/
	for(j=0;j<n;j++) {
		*(timetot+j) =timet[j];
// 		 fprintf(fp2,"last: %d   %.0f\n",j,  *(timetot+j));
	}


} //function findstep

