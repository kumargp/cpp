#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "functions.h"
#include "struct.h"

//Function prototypes
// Allocate memory


int  main (int argc, char *argv[]) {

  FILE *slfp=NULL ;//,*simparfp=NULL,*simparopfp, *startlogfp=NULL, *endlogfp=NULL, *telemfp=NULL, *outfp=NULL;

  struct strslight strsl2[10];
  struct strslight strsl[10];
  struct strmnth strmn[26],strmn2[26]; 
  struct strphase strph[16],strph2[16];
  struct tempstore tem[20];
// struct strmnth strmn2[25]; 
// struct strphase strph2[16];

  char line[200];//,temp1[30],temp2[30],temp3[30];//sl file temp store
  char *parline, *par1;//sim_par temp store
  char *file_ren, *file_ren2,*file_ren3,*file_ren4,*file_ren5;
  float lowlim = 100.0, medlim=1000.0; 

//   char *line,*temp1,*temp2,*temp3;//sl file temp store
//   char parline[300],par1[20];//sim_par temp store
//  float scanstart[50],scanend[50],decstart[50], sl[50]; // phases observ. list
//   float t36st[15],t36stp[15],sl36[15], decno36[6];// for passing to commands
//   char file_ren[30], file_ren2[30],file_ren3[30],file_ren4[30],file_ren5[30];
//   char month[20];   // ??
// float *t36st,*t36stp,*sl36, *decno36;// for passing to commands


	char *monthstring[]= {"sep21","oct6", "oct21","nov6", "nov21", "dec6","dec21", "jan6","jan21", "feb6","feb21", "march6","march21", "april6","april21", "may6","may21", "june6","june21", "july6", "july21", "aug6", "aug21", "sep6"};


  int n1=0,n2=0,n3=0,n4=0,d1=0,d2=0; //dec, phase, month,list,dec1,dec2
  int i=0,j=0,m,n=0;
  float ra = 0.0, dec = 0.0,dec1 =0.0, exptime=0; // pass dec

int decno; //find decno
char *flt1 = "SF1"; 
char *flt2 ="SF2"; 
char *flt3 ="SF3"; 
char *mdpdir;
char *mdpdirn ="NORTH";
char *mdpdirs ="SOUTH";
//int  mdpsteps=12234;
char *flt_state = " ";
float eclst =23.5,eclend=24.5;

char *hvps= "ON";
char *flt_change = "no";

unsigned int pon=0, newdec=0;
int jold=0,mold=0,thrno=0;
int mdpsteps=0,filtset =0,mdpmove=0; 		//if mdpsteps != 0; mpdmove to be set to 1
//pass mdpsteps,filtset,mdpos,
float mdpos=0.0;
float thrust[]={19.0,0.5,1.5};
float thrdur = 0.1;
float imglist[20],stplist[20];
int t=0,st=0,k=0,th=0,stp=0,filt=0;
float tmp=0;


	if((slfp = fopen("tabsl.txt","r")) == NULL) {
       		printf("File open error !\n");
       		exit(1);
       		}//fopen

	while ((fgets(line,sizeof(line),slfp))!=NULL) {
  	sscanf(line, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", tem[0].p,tem[1].p, tem[2].p,tem[3].p, tem[4].p,tem[5].p, tem[6].p,tem[7].p, tem[8].p,tem[9].p,tem[10].p,tem[11].p,tem[12].p,tem[13].p,tem[14].p);

  		if(strcmp(tem[0].p,"")==0) continue; //skip blank lines
  
  		if(strcmp(tem[0].p,"Decl")==0){   //First word is Decl. Change this if needed.
			strsl[n1].dec1= atof(tem[1].p);
	  		strsl[n1].dec2= atof(tem[2].p);
	  		for(i=0;i<3;i++) { 
				strcpy(tem[i].p,"");  //clear strings
				}
//      	printf("\nDecl line:n1=%d,dec1=%.1f,dec2=%.1f\n",n1,strsl[n1].dec1, strsl[n1].dec2);
	  		n1++;  //ready to store next decl ranges.
	  		n2=0;  //reset phase number to start.
		  	continue;  //next line, if inside this loop
  			} //if

  		else   { 	//line having SL counts
	 		strsl[n1-1].strph[n2].tst= atof(tem[0].p);
  	 		strsl[n1-1].strph[n2].tstp = atof(tem[1].p); 
			strsl[n1-1].strph[n2].t36st = (atof(tem[0].p) < 12.0) ? (atof(tem[0].p) +24.0) : atof(tem[0].p);
			strsl[n1-1].strph[n2].t36stp = (atof(tem[1].p) < 12.0) ? (atof(tem[1].p) +24.0) : atof(tem[1].p);
			strsl[n1-1].decno=n1-1;

//    		printf("\nn2=%d,tstart=%.1f,tend=%.1f\n",n2,strsl[n1-1].strph[n2].tst,strsl[n1-1].strph[n2].tstp);

  	 		for(i=0;i<13;i++) {   //ph starts from thrid column only, from sep21 for 0 --
     	 			j = ((i+18)>=24)?(i+18-24):(i+18); 
	 			strsl[n1-1].strph[n2].strmn[j].ph = strsl[n1-1].strph[n2].strmn[18-i].ph = atof(tem[i+2].p);
// 			printf("%.1G ",strsl[n1-1].strph[n2].strmn[j].ph);
	 			} //for

 			for(i=0;i<15;i++) strcpy(tem[i].p,"");  //emptying strings
      			n2++;
			}//else 
   		} //while gets
   	fclose(slfp);

// printf("\n===========================================\n");

//storing ra values from local time by adding with racent for each month
      	n1 = n2 = 0; //resetting n1 and n2;
   	for(n1=0; n1<6; n1++) {  //6 dec ranges
	      	for(n2=0; n2<15; n2++) { //15 entries in table.
        		for(n3=0; n3<24; n3++) { //24 half months

				strsl[n1].strph[n2].strmn[n3].rstart= strsl[n1].strph[n2].tst + n3;
				strsl[n1].strph[n2].strmn[n3].rend= strsl[n1].strph[n2].tstp + n3;

// 	printf("n1=%d,n2=%d,n3=%d", n1,n2, n3);
// 	printf("tstart=%.1f,tend=%.1f", strsl[n1-1].strph[n2].tst, strsl[n1-1].strph[n2].tstp);
// 	printf("rstart=%.1f,rend=%.1f",strsl[n1].strph[n2].strmn[n3].rstart, strsl[n1].strph[n2].strmn[n3].rend);

				if(strsl[n1].strph[n2].strmn[n3].rstart >=24.0) {
					strsl[n1].strph[n2].strmn[n3].rstart -= 24.0;
					}
				if(strsl[n1].strph[n2].strmn[n3].rend >=24.0) {
					strsl[n1].strph[n2].strmn[n3].rend -= 24.0;
					}	
// 	printf("rstart=%.1f,rend=%.1f,ph=%.1G \n",strsl[n1].strph[n2].strmn[n3].rstart, 	strsl[n1].strph[n2].strmn[n3].rend, strsl[n1].strph[n2].strmn[n3].ph); 
        			}//for n3
      			}//n2
  		}//n1
	
//========================================
//Find decno, given decstart, decend.
//   for(dec=decstart; ; ) {  dec=decend
	n1=0;		//reset n1
 	for(i=0;i<6;i++) {
		if((dec >= (15.0*i))&&(dec<(15.0*(i+1)))) {
			decno =i; 
			}
		}
  	n1=decno;


//sorting for SL < limit, and store in new str.
// 	printf("At given DEC of %.1f for SL limit=%.1f\n",dec,medlim);
// 	printf("NOTE:In time and RA, numbers are in decimal points \n");

	for(n1=0;n1<6;n1++) {
		for(n3=0; n3<24; n3++) {
	   		for(n2=0;n2<15;n2++){
			
		 		if((strsl[n1].strph[n2].strmn[n3].ph) < medlim){
	     	    			if(strsl[n1].strph[n2].tst != strsl[n1].strph[n2].tstp) { 
			  			
				//COPY STRUCTURE ???
// 						strcut strsl strsl2;
// 						strsl2[n1]=strsl[n1];
// 						strsl2[n1].strph[n2] = strsl[n1].strph[n2];
// 						strsl2[n1].strph[n2].strmn[n3]=strsl[n1].strph[n2].strmn[n3];

						strsl2[n1].strph2[n2].tst=strsl[n1].strph[n2].tst;
	     					strsl2[n1].strph2[n2].tstp=strsl[n1].strph[n2].tstp;

	     		  			strsl2[n1].strph2[n2].strmn2[n3].ph=strsl[n1].strph[n2].strmn[n3].ph;
						strsl2[n1].strph2[n2].strmn2[n3].rstart=strsl[n1].strph[n2].strmn[n3].rstart;
						strsl2[n1].strph2[n2].strmn2[n3].rend=strsl[n1].strph[n2].strmn[n3].rend;
			  			strsl2[n1].strph2[n2].t36st=strsl[n1].strph[n2].t36st;
						strsl2[n1].strph2[n2].t36stp=strsl[n1].strph[n2].t36stp;
						strsl2[n1].decno=strsl[n1].decno;
						}//if

// 		printf("During:%7s-%6s, <RA>: %.1f - %.1f    ", monthstring[n3],((n3+1)==24)? //monthstring[0]:monthstring[n3+1], strsl[n1].strph[n2].strmn[n3].rstart, strsl[n1].strph[n2].strmn[n3].rend);

// 		printf("<LocalTime>: %.1f - %.1f, \n", scanval[n4].imgstart, scanval[n4].imgstop);
// 		printf("SL=%.1f",scanval[n4].slval);
					}// if medlim
				else 	{	
// 					strsl2[n1].strph2[n2].tst=0;
// 					strsl2[n1].strph2[n2].tstp=0;
// 					strsl2[n1].strph2[n2].strmn2[n3].ph=0;
// 					strsl2[n1].strph2[n2].strmn2[n3].rstart=0;
// 					strsl2[n1].strph2[n2].strmn2[n3].rend=0;
					strsl2[n1].strph2[n2].t36st=0;
					strsl2[n1].strph2[n2].t36stp=0;
					strsl2[n1].decno=11;
// 					strsl2[n1].dec1=0;
// 					strsl2[n1].dec2=0;
					}
// printf("During:%7s-%6s Local:%.1f-%.1f, <RA>:%.1f-%.1f, SL:%.1G\n", monthstring[n3], ((n3+1)==24)? monthstring[0]:monthstring[n3+1], strsl2[n1].strph2[n2].t36st, strsl2[n1].strph2[n2].t36stp, strsl2[n1].strph2[n2].strmn2[n3].rstart, strsl2[n1].strph2[n2].strmn2[n3].rend,strsl2[n1].strph2[n2].strmn2[n3].ph);

	     			}//n2
			} //n3
		}//for n1




//COMMANDS ======START===========//
// mdpsteps=1234,
mdpdir=mdpdirn,mdpmove=0;

// thrust[4]= {18.0,20.3,04.2,06.6}; //input

thrno=3;//sizeof(thrust[]


	for(i=0; i<thrno; i++) { 
			
		thrust[i]=(thrust[i] <12.0 ) ? (thrust[i] +24.0) : thrust[i]; 
		}

printf("\n===========================================\n");
printf("\n=======COMMANDS=======\n");
printf("\n===========================================\n");

printf("Thr:");
for(i=0; i<thrno; i++) { 
	printf(" %.1f, ",thrust[i]);
	}
//	decno for dec1, dec2 ?. 

//redefining ecl times
	eclst = (eclst <12.0)? (eclst+24.0): eclst;
	eclend= (eclend <12.0)? (eclend+24.0): eclend;
// printf("\nEclipse:%.1f,%.1f\n",eclst,eclend);



// 	for(j=0;j<n1;j++) {
j=0;
// 		for(n=0; n<n3; n++) {
n=0;

t=0,st=0;
	   		for(m=0;m<9;m++) {


// 				printf("\nm=%d\n",m);
				dec = dec1;

				if((eclst < (strsl2[j].strph2[m].t36st)) &&(eclend > (strsl2[j].strph2[m].t36stp-0.2))) {
					continue;
				}

				printf("\nBegin:obs %.1f-%.1f\n",strsl2[j].strph2[m].t36st, strsl2[j].strph2[m].t36stp);

				if(fabs(strsl2[j].strph2[m].t36st - strsl2[j].strph2[m].t36stp)<0.1) {	
					jold=j;	

// 					printf("in1==:jold= %d, j= %d\n",jold,j);

					for(j=0;j<6;j++)  {

// 						printf("In2 ==:decno= %d,  j=%d\n",strsl2[j].decno,j);
// 						
// 						if(j==strsl2[jold].decno) { j++; }
// 						printf("In 3 ==:decno= %d,j=%d\n",strsl2[j].decno,j);

						if(strsl2[j].strph2[m].t36stp > strsl2[j].strph2[m].t36st) {        
// 					&&(strsl2[j].strph[m+1].t36stp > strsl2[j].strph[m+1].t36st)) 
						//=====exact  dec from decno
// 							printf("In 4== %.1f-%.1f\n",strsl2[j].strph2[m].t36st, strsl2[j].strph2[m].t36stp);
						    	dec= j * 15.0;	
// 							printf("in 5==:dec= %.1f\n",dec);
							if(j!=jold) {
								newdec=1;
// 								printf("in 6==:jold= %d, j= %d\n",jold,j);
							}
							break;
						}
					}
					if(newdec==0) {
						j=jold;
// 						printf("in 7==:jold= %d, j= %d\n",jold,j);
					}
						
				}
			




// NEW LIST

// float imglist[16],thrlist[20],eclst1,eclstp1;
// int t=0,th=0,eno=0;


				if(strsl2[j].strph2[m].t36st != strsl2[j].strph2[m].t36stp) {
					printf("Newlist:obs %.1f-%.1f\n",strsl2[j].strph2[m].t36st, strsl2[j].strph2[m].t36stp);


					if((eclst <= (strsl2[j].strph2[m].t36st)) && 
						((eclend >= (strsl2[j].strph2[m].t36st)) && (eclend <= (strsl2[j].strph2[m].t36stp-0.2) ))) {

						imglist[t]= eclend;

						t++;
						stp=0;
// 						printf("1 imglist[%d]: %.1f\n",t,imglist[t-1]);
						
					}	

					else if(((t>0) && (imglist[t-1] != strsl2[j].strph2[m].t36st)) || (t==0))  {
						imglist[t]= strsl2[j].strph2[m].t36st;

// 						if((imglist[t])>0) 
// 						printf("2 imglist[%d]: %.1f\n",t,imglist[t]);
						t++;
						stp=0;
					}

 					if((stp==0)&&((strsl2[j].strph2[m].t36st +0.2) <= eclst) && (strsl2[j].strph2[m].t36stp > eclst)) {
						stplist[st]=eclst;
// 						printf("3 stplist[%d]: %.1f\n",st,stplist[st]);
						st++;
						stp=1;
					

						if ((strsl2[j].strph2[m].t36stp -0.2) > eclend) {
							imglist[t]=eclend; 
// 							printf("4 imglist[%d]: %.1f\n",t,imglist[t]);
							t++;
							stp=0;	
						}

					}





					for(th=0;th<thrno;th++) {

						if(((strsl2[j].strph2[m].t36st-thrdur) > thrust[th]) || (strsl2[j].strph2[m].t36stp < thrust[th])) {
							continue;
						}
						if((eclst < thrust[th]) && (eclend > (thrust[th]+thrdur))) {
							continue;
						}

		// 			if(((fabs(strsl2[j].strph2[m].t36st-eclst)) > 0.2) && ((fabs(strsl2[j].strph2[m].t36st-thrust[th])) > 0.1)) {
		// 				imglist[t]= strsl2[j].strph2[m].t36stp;
		// 				t++;
		// 				}
			
		// 				if ((fabs(strsl2[j].strph2[m].t36st-thrust[th])) < 0.2){ 
		// 					thrlist[th]= strsl2[j].strph2[m].t36st;
		// 					th++;	
		// 				}


						if((strsl2[j].strph2[m].t36st <= thrust[th]) && (strsl2[j].strph2[m].t36st + 0.2 > thrust[th]) ) {

							stplist[st]=strsl2[j].strph2[m].t36st;
//  							printf("5 stplist[%d]: %.1f\n",st,stplist[st]);
							st++;
							imglist[t]=thrust[th]+thrdur; 
							printf("1 Start img delayed\n");
// 							printf("6 imglist[%d]: %.1f\n",t,imglist[t]);
							t++;
						}


						if((strsl2[j].strph2[m].t36st +0.2 <= thrust[th]) && (strsl2[j].strph2[m].t36stp - 0.2 > thrust[th]) ) { 
		
							stplist[st]=thrust[th];
// 							printf("7 stplist[%d]: %.1f\n",st,stplist[st]);
							st++;
							imglist[t]=thrust[th]+thrdur; 
							printf("2 Start img delayed\n");
// 							printf("8 imglist[%d]: %.1f\n",t,imglist[t]);
							t++;
						}



						if((thrust[th] > (strsl2[j].strph2[m].t36stp -0.2)) && (thrust[th] < strsl2[j].strph2[m].t36stp))  {
	
							stplist[st]=thrust[th];
// 							printf("9 stplist[%d]: %.1f\n",st,stplist[st]);
							st++;
							stp=1;
						}

					}



					if(stp==0) {
						stplist[st]=strsl2[j].strph2[m].t36stp;

// 						printf("10 stplist[%d]: %.1f\n",st,stplist[st]);
						st++;
					}

// 					printf("t=%d,st=%d \n",t,st);		
					
// 					for(i=0;i<t;i++) {
// 						printf("Before: imglist[%d]: %.1f\n",i,imglist[i]);
// 					}
// 					for(i=0;i<st;i++) {
// 						printf("Before:stplist[%d]: %.1f\n",i,stplist[i]);
// 					}

					for(i=0;i<t-1;i++) {
// 						if(i==t-1) break;
// 						printf("In sort imglist\n");	
						if(imglist[i] > imglist[i+1]) {
							tmp = imglist[i];
							imglist[i] = imglist[i+1];
							imglist[i+1] = tmp;
						}
					}

					for(i=0;i<st-1;i++) {
// 						if(i==st-1) break;
// 						printf("In sort stplist\n");
						if(stplist[i] > stplist[i+1]) {
							tmp = stplist[i];
							stplist[i] = stplist[i+1];
							stplist[i+1] = tmp;
						}
					}	

// 					for(i=0;i<t;i++) {
// 						printf("imglist[%d]: %.1f\n",i,imglist[i]);
// 						imglist[i] =0;
// 					}	
						
// 					for(i=0;i<st;i++) {
// 						printf("stplist[%d]: %.1f\n",i,stplist[i]);
// 						stplist[i] =0;
// 					}
				}
				j=jold;
// 			}



//FIlters set
				if((filtset == 0)) {     //&&(imglist[i]!=0.0)
					if((strsl2[j].strph2[m].strmn2[n].ph > lowlim) && (strsl2[j].strph2[m].strmn2[n].ph < medlim)) {
						filtset = 2;  
// 						printf("j=%d,m=%d,n=%d,filt set 1(%d)\n",j,m,n,filtset);
					}
					else if(strsl2[j].strph2[m].strmn2[n].ph < lowlim) {
						filtset = 3;
// 						printf("filt set 2(%d)\n",filtset);

					}
				}
			}
// 		printf("j=%d,m=%d,n=%d,filt set no=%d\n",j,m,n,filtset);		
		
//	found scan ranges dec or new dec
// 	if((fabs(imglist[t]-eclst)>0.2) && (fabs(imglist[t] - thrust[th])>0.1)) {
				for(k=0;k<t;k++) {	
					st=0;
					for(i=0;i<t;i++) {
// 						printf("Passing: imglist[%d]=%.1f,stplist[%d]=%.1f\n",i,imglist[i],i,stplist[i]);
					}
				if(imglist[k]!=stplist[k]) {
					if(pon ==0) {
						if(k==0) {
							fn_poweron(imglist[k]-0.5);
						}
// 						printf("Power on loop\n");
						else {
							fn_poweron(imglist[k]);
						}
						pon=1;
					}
			
						//	dec += decstep;  //get dec. decstep from decsteps.c
			
					if(mdpsteps==0) {
						if(mdpos != dec) {
							mdpmove=1;
							if((mdpos - dec) > 0.0) {
								mdpdir = mdpdirs;
							}
							else if((mdpos - dec) < 0.0) {
								mdpdir = mdpdirn;
							}
							mdpsteps= 157.0*fabs(mdpos - dec); 
						}
					}
					if(mdpmove == 1) {
// 						printf("move MDP\n");
						fn_mdpmove(imglist[k],  mdpdir, mdpsteps); 
						mdpos = dec;
						mdpmove =0;
					}
			
		
		
// 			printf("filt set =%d\n",filtset);

					if((filtset == 2)&&(filt==0)) {
// 						printf("filt set 1\n");
						fn_filters(imglist[k], flt2, flt1, flt2);  
					}
					else if((filtset == 3)&&(filt==0)) {
// 						printf("filt set 2\n");
						fn_filters(imglist[k], flt1, flt2, flt3);  //////
					}
					
// 				printf("t=%d,k=%d,imglist= %.1f,stplist=%.1f\n",t,k,imglist[k],stplist[k]);
				if((k==0)||( (k>0)&&(imglist[k] != stplist[k-1]))) {

//  ||((k<t)&&(imglist[k+1]!=stplist[k]))) {		

					fn_startimg(imglist[k], flt_state);
				}
				if((k!=t)&&(imglist[k+1] != stplist[k])) {

					fn_stopimg(stplist[k], hvps, flt_state);
				}
					filt=1;

					if(stplist[k] == eclst )  {
						printf("\t\t//Postpone obs. due to eclipse\n");
						fn_eclipse(eclst, eclend, flt_change);
						filt=0;
						pon=0;
					}
				for(th=0;th<thrno;th++) {
					if(thrust[th] == stplist[k]) {
						printf("\t\t::Thruster\n");
						fn_thrust_fire(thrust[th], thrdur, flt1, flt2, flt3);
						filt=0;
// 						pon=0;
						}
				}
			
		//filters closed if no obs 
// 					if((stplist[k] - imglist[k]) >2.0) {
// 						fn_filters(imglist[k], flt2, flt1, flt2);  
// 						filtset == 0;
// 						}
		
					if(k==t-1) {
						fn_shutdown(stplist[k]);
						pon ==0;
						}
				}
			}//i
// 		}//thruster	

// for(i=0;i<t;i++) {
// printf("imglist: %.1f ",imglist[i]);
// }

return(0);
}



/*
					if((fabs(strsl2[j].strph2[m].t36st-eclst)) < 0.2)  {
						fn_eclipse(eclst, eclend, flt_change);
// 						eclwarn=1;
						mold=m;
						for(m=mold;m<=14;m++) {
						if(strsl2[j].strph2[m].t36stp > eclend) {
						// m is new
						printf("\t new m =%d\n",m); 
						printf("by %.1f hrs\n",eclend-strsl2[j].strph2[mold].t36st);
							strsl2[j].strph2[m].t36st = eclend;	
printf("\tEclipse affects Img: New st=%.1f\n",strsl2[j].strph2[m].t36st);
							break;	
							}
						}
						
// 						printf("by %.1f hrs\n",eclend-strsl2[j].strph2[m].t36st);
							
						}
					if((fabs(strsl2[j].strph2[m].t36st-thrust[thrno])) <0.1) {
						printf("\t\t//Postpone obs. due to thruster firing");
						if(strsl2[j].strph2[m+1].t36st > thrust[thrno]+thrdur) {
						printf("by %.1f hrs\n",thrust[thrno]+thrdur-strsl2[j].strph2[m].t36st);
							strsl2[j].strph2[m].t36st = thrust[thrno]+thrdur;			
							}
						else    {
							m++;
						printf("by %.1f hrs\n",thrust[thrno]+thrdur-strsl2[j].strph2[m].t36st);
							}
						}

// 			if(eclwarn==1) {
// 				fn_eclipse(eclst, eclend, flt_change);
// 				}
// 
// 			if(thrwarn==1) {
// 				fn_thrust_fire(thrust[thrno], thrdur, flt1, flt2, flt3);
// 				}
// 
// 			if((eclwarn==0)&&(thrwarn==0)) {
// printf("start img\n");
// 				fn_startimg(strsl2[j].strph2[m].t36st, flt_state);
// 				}

					if(((fabs(strsl2[j].strph2[m].t36st-eclst)) >= 0.2) && 					(fabs(strsl2[j].strph2[m].t36st-thrust[thrno])) >=0.1 )  {

							fn_startimg(strsl2[j].strph2[m].t36st, flt_state);
							 
							
	
						if((eclst-0.1) < strsl2[j].strph2[m].t36stp) {
printf("Eclipse\n");
							fn_eclipse(eclst, eclend, flt_change);
							
							filtset=0; // or filt not closed. then filtset=1
							if(eclend < strsl2[j].strph2[m].t36stp) {  //scan remaining time
								if((eclend+0.2)  < thrust[th]) {
printf("::Eclipse start img\n");
							//mdp move ? thrust ??
									fn_startimg(eclend +0.2, flt_state);
									}
								}		
							else if(eclend >= strsl2[j].strph2[m].t36stp) {
								//assign ?? j=jold,--
								//break;
								}
							}
				
						if((thrust[th]-0.1) < strsl2[j].strph2[m].t36stp) {

printf("::Thruster\n");
							fn_thrust_fire(thrust[thrno], thrdur, flt1, flt2, flt3);
							thrno++;
							if(eclend < strsl2[j].strph2[m].t36stp) {  //continue scan

printf("::Thruster start img\n");
				
							//again thrust ??
								fn_startimg(eclend +0.2, flt_state);
								}
						
							else if(eclend >= strsl2[j].strph2[m].t36stp) {
								//continue;
								}
						
							}
		
						fn_stopimg(strsl2[j].strph2[m].t36stp, hvps, flt_state);

								

						if((newdec==1)&&(j!=jold)) {
							j=jold;
							}
						m++;
						}
		// Last phase



					if(m==n2) {
							fn_shutdown(strsl2[j].strph2[m].t36stp);
							pon ==0;
							}
				

				if(m==n2){
					jold++;
					}*/

// 				}//n
// 			}//m
// 		}//j


//=======delete from here=====================
// //	exptime 
// 	exp_perscan= 
// 	no_scans=	
// 	no_scan_best=
// 	no_scan_lim=
// 	scan_start_date=
// 	sca_end_date=
// 	decstep=

//=======delete till here=====================


//decstep ??

//====== uncomment for simulation===========   
// //sim loop- 
// for(n5=0;n5<n4;n5++) {
//   printf("simulation loop:\n");	
// //Edit param file.
//   printf("Editing param file:\n");	
// 
//   system("mv -f UVS_skysim_initparams.txt UVS_skysim_initparams_ren.txt");
//   
//   if((simparfp = fopen("UVS_skysim_initparams_ren.txt","r"))==NULL) {
// 	  printf("Sim Par file not opened");
//   	  exit(1);
//   	  }
//   if((simparopfp = fopen("UVS_skysim_initparams.txt","w"))==NULL) {
// 	  printf("Sim Par o/p file not created");
//   	  exit(1);
//   	  } 
// 
//   while((fgets(parline,sizeof(parline),simparfp))!=NULL) {
//   sscanf(parline,"%s",par1);
// 
//   if(strcmp(par1,"RA_START")==0) {
// //	par3 = rastart[n5]);
// 	fprintf(simparopfp, "RA_START = %.1f /*Begin RA */ \n",scanval[n5].imgstart);	
//   	}
//   else if (strcmp(par1,"RA_END")==0) {
// 
// 	fprintf(simparopfp, "RA_END = %.1f /*End RA */ \n",scanval[n5].imgstop);
// 	}
// 
//   else if (strcmp(par1,"DEC_START")==0) {
// 
// 	fprintf(simparopfp, "DEC_START = %.1f /*DEC_START */ \n",scanval[n5].decval);
// 	}
//   else  {
// 	fputs(parline,simparopfp);
// 	}
//         strcpy(par1,"");
//   // JITTER       =  0.00001   
//  //  DARKCOUNT    =  0.000005
//   }//while fgets par file
// 
//   fclose(simparfp);
//   fclose(simparopfp);

// //renaming par file
//   strcpy(file_ren,"UVS_skysim_initparams");
//   strcpy(file_ren2,file_ren);
//   strcat(file_ren, ".txt"); 
//   strcat(file_ren2, "-"); 
//   sprintf(file_ren3,"%d",n5+1); 
//   strcat(file_ren2,file_ren3);
//   strcat(file_ren2,".txt");  //file_ren3 dest file
//   sprintf(file_ren4,"cp -f %s %s",file_ren, file_ren2);
//   printf("renaming %s\n\n",file_ren4);
//   system(file_ren4);


//startlog , endlog, temetry

//   printf("Running Sim with RAstart=%.1f, RAend=%.1f,dec1=%.1f\n", imgstart[n5],imgstop[n5],decstart[n5]);
//   system("./uvs_simulate_sky");

// // file renaming
//   sprintf(file_ren,"%.1f",decstart[n5]); 
//   strcat(file_ren,"-1");  
//   strcpy(file_ren2,"UVS_skysimdata-C_DEC");
//   strcat(file_ren2,file_ren); //file_ren2 is source file
//   strcpy(file_ren3,file_ren2);
//   strcat(file_ren3, "-"); 
//   sprintf(file_ren4,"%d",n5+1); 
//   strcat(file_ren3,file_ren4);  //file_ren3 dest file
//   sprintf(file_ren5,"mv -f %s %s",file_ren2, file_ren3);
//   printf("renaming %s\n\n",file_ren5);
//   system(file_ren5);


// } //for sim loop


//      if (fpointer == NULL)
//       {
//           printf("Error: Unable to create file \"%s\"\n",filename);
// 	  exit(0);
//           }


// return(0);
// }//main



    
