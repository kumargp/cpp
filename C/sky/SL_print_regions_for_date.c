/*
This program is part of TAUVEX observation tools

regions_for_date.c

This program gives observable regions for a date and filter. The o/p lists RA, DEC and Straylight count. The sorted list is given for four straylight limits. It is a trimmed version of plotskymap used to plot map based on straylight.

It needs file:
sl_table_north.txt (straylight file)

Usage:
=====
 Interactive mode  
./exec

author: Gopakumar
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>




 struct strmnth {
	double ph;
	float rstart;
	float rend;
  	};
  struct strphase {
	float tst;
	float tstp;
	float t36st;
	float t36stp;
	struct strmnth strmn[24];
  	};
  struct strslight {
	float dec1;  
	float dec2;
	int decno;
	struct strphase strph[15];
  	};



//start main
int  main (int argc, char *argv[]) {

	FILE *slfp=NULL ;
	struct strslight strsl[10];
	struct strmnth strmn[26];
	struct strphase strph[16];
// 	struct tempstore tem[20];
	
	int i=0,j=0,n1=0,n2=0,n3=0,n3s=0,n31=0,n32=0,n321=0,n33=0,pl=0,plt=0,fltno=0,rasouth,phase=0,day=0,day1=0,day2=0,month=0,month1=0,month2=0,plotno=0,plotlimit=0,list=1,day2ra,day2ra1,day2ra2,day2ralast,set=0,cross24; //n1=dec, n2=phase,n3=month,n3s=n3south
	float xmin=0,xmax=0,ymin=0,ymax=0,t1=0,limit=10000;
	char filename[50],line[200], tem[25][50];
	float ra1[1000],ra2[1000],dec1[1000],dec2[1000];

	//Stray Light limits
	double count[1000]; 
// 	double lim[] = {-1,100,1000,5000,1E+20,1E+25};
 	double lim[] = {-1,1000,5000,10000,20000,1E+20,1E+25}; //0,lowlim,medlim,highlim,allowed,sp
	int color[1000],colr[]= {9,11,2,0,1};
	//filters
	float fltratio[]= {1.,1111,35,8.88,116};  //bbf/bbf, bbf/sf1, bbf/sf2, bbf/sf3, bbf/nbf3. last-ETC.
	char *flt[] = {"BBF", "SF1", "SF2", "SF3", "NBF"};
/*	
	if(argv[1]) day=atoi(argv[1]);
	if(argv[2]) month=atoi(argv[2]);
	if(argv[3]) fltno=-1+atoi(argv[3]);
	if((argc <4)||(day>31) || (day < 1) || (month>12) || (month < 1) || (fltno>4) || (fltno < 0) ) {
*/	


	
	printf("This program lists observable DEC for each local time or RA between two input dates.\n Enter values:\n");

	printf("Start: Month(1 -12), Day(1-31) \n");
	scanf("%s",tem[0]);
	month1 = month = atoi(tem[0]);

// 	printf("Start: Day of the month: 1-31 \n");
	scanf("%s",tem[0]);
	day1 = day = atoi(tem[0]);


	printf("End: Month(1 -12), Day(1-31) \n");
	scanf("%s",tem[0]);
	month2 = atoi(tem[0]);

// 	printf("End: Day of the month: 1-31 \n");
	scanf("%s",tem[0]);
	day2 = atoi(tem[0]);

	printf("Filter number: 1- 5 (1:BBF, 2:SF1, 3:SF2, 4: SF3, 5: NBF) \n");
	scanf("%s",tem[0]);
	fltno = -1+atoi(tem[0]);

// 	printf("Straylight Limit for RA or DEC sorting\n");
// 	scanf("%s",tem[0]);
// 	limit = atof(tem[0]);
	limit =10000.;

// 	printf("List type: 1-3 (1: RA or time sorted, 2: SL sorted, 3: DEC sorted) \n");
// 	scanf("%s",tem[0]);
// 	list = atoi(tem[0]);
 	list =1;

// 	if(list ==1) {
// 		printf("Any choice of DEC?  y/n  \n");
// 		scanf("%s",tem[0]);
// 		if( strcmp(tem[0],"y") ==0)listra = 1;
// 	}
	

// 		printf("Arguments are wrong. Correct Usage is:\n \tExample:   ./regions_for_date 2 4 3 \n \tArg1: 1-31 (day) ,\n \tArg2: 1-12 (month) \n \tArg3: 1:BBF, 2:SF1, 3:SF2, 4: SF3, 5: NBF3 (filter number) \n");   
// 		exit(0);
// 	}

	// open file
	if((slfp = fopen("sl_table_north.txt","r")) == NULL) {
       		printf("Error: SL file not opened !\n");
       		exit(1);
       	}//fopen

	// read line from SL file into line[], and store in tem structure
	strcpy(tem[0],"");
	n1=-1, n2=0; //starting n1 and n2
	while ((fgets(line,sizeof(line),slfp))!=NULL) {
  	sscanf(line, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", tem[0],tem[1], tem[2],tem[3], tem[4],tem[5], tem[6],tem[7], tem[8],tem[9], tem[10],tem[11], tem[12],tem[13], tem[14]);

  		if(strcmp(tem[0],"")==0) continue;  //skip blank lines
  		if(strcmp(tem[0],"#")==0) continue; //skip comments
  		if(strcmp(tem[0],"#-")==0) continue; //skip comments
  		if(strcmp(tem[0],"Decl")==0){       //if Dec range line.,store decstart,decend 
			n1++;
			strsl[n1].dec1= atof(tem[1]);
	  		strsl[n1].dec2= atof(tem[2]);
			strsl[n1].decno=n1;
	  		for(i=0;i<3;i++) { 
				strcpy(tem[i],"");  //clear strings
		  		n2=0;  //reset phase number for values.
			}
  		} //if
		else {
	 		strsl[n1].strph[n2].tst= atof(tem[0]);
  	 		strsl[n1].strph[n2].tstp = atof(tem[1]); 
			strsl[n1].strph[n2].t36st = (atof(tem[0]) < 12.0) ? (atof(tem[0]) +24.0) : atof(tem[0]);  //shift to 12-36
			strsl[n1].strph[n2].t36stp = (atof(tem[1]) < 12.0) ? (atof(tem[1]) +24.0) : atof(tem[1]);

  	 		for(i=0;i<13;i++) {   //ph starts from thrid column only, from sep21 for 0 --
     	 			j = ((i+18)>=24)?(i+18-24):(i+18); 
	 			strsl[n1].strph[n2].strmn[j].ph = strsl[n1].strph[n2].strmn[18-i].ph = atof(tem[i+2]);
	 		} //for
 			for(i=0;i<15;i++) strcpy(tem[i],"");  //clearing strings
      			n2++;
		}//else 
   	} //while gets
   	fclose(slfp);



	//storing ra values from local time for each month by adding to racent 
   	for(n1=0; n1<6; n1++) {  //6 dec ranges
	      	for(n2=0; n2<15; n2++) { //15 entries in table.
        		for(n3=0; n3<24; n3++) { //24 x 15 days

				strsl[n1].strph[n2].strmn[n3].rstart= strsl[n1].strph[n2].tst + n3;
				strsl[n1].strph[n2].strmn[n3].rend= strsl[n1].strph[n2].tstp + n3;

				if(strsl[n1].strph[n2].strmn[n3].rstart >=24.0) {
					strsl[n1].strph[n2].strmn[n3].rstart -= 24.0;
				}
				if(strsl[n1].strph[n2].strmn[n3].rend >=24.0) {
					strsl[n1].strph[n2].strmn[n3].rend -= 24.0;
				}	
 				
// 				if(n3==0) printf("tst=%.1f , tstp= %.1f:: rast=%.1f , rastp= %.1f \n  ",strsl[n1].strph[n2].tst,strsl[n1].strph[n2].tstp,strsl[n1].strph[n2].strmn[n3].rstart,strsl[n1].strph[n2].strmn[n3].rend);
        		}//for n3
      		}//n2
  	}//n1




	//	day,month 
	// 	n3=0;
	n3= month *2 +6; 
	if((day>=6) && (day<=21)) n3 = n3-1;
	if(day< 6) n3 = n3-2;
	if(n3>=24) n3 = n3-24;

	n31= month1 *2 +6; 
	if((day1>=6) && (day1<=21)) n31 = n31-1;
	if(day1< 6) n31 = n31-2;
	if(n31>=24) n31 = n31-24;

	printf("month1=%d, day1=%d \n",month1,day1);

	n32= month2 *2 +6; 
	if((day2>=6) && (day2<=21)) n32 = n32-1;
	if(day2< 6) n32 = n32-2;
	if(n32>=24) n32 = n32-24;
	printf("month2=%d, day2=%d \n",month2,day2);

	printf("\n ***** Warning: -ve SL means it is not from calculation****\n");


	if(list ==2) { //SL order list
//sorting for SL < limit
// 	for(fltno=0;fltno<5;fltno++) { //1=bbf,2=sf1,3=sf2,3=sf3,4=nbf3
// 	  for(n3=0; n3<24; n3++) {  //15 days
	    phase=0;	
	    for(i=0;i<6;i++) {		//5 limits
		if(i==4) continue;
		for(n1=0;n1<6;n1++) {  //6 dec ranges
	   		for(n2=0;n2<15;n2++){  //15 phases
		
		 		if(((strsl[n1].strph[n2].strmn[n3].ph/fltratio[fltno]) > lim[i]) && ((strsl[n1].strph[n2].strmn[n3].ph/fltratio[fltno])  <= lim[i+1])){
	     	    			if(strsl[n1].strph[n2].tst != strsl[n1].strph[n2].tstp) { 

// limit north
						count[phase]=strsl[n1].strph[n2].strmn[n3].ph/fltratio[fltno];
						ra1[phase]=strsl[n1].strph[n2].strmn[n3].rstart;
						ra2[phase]=strsl[n1].strph[n2].strmn[n3].rend;
						dec1[phase]=15.*strsl[n1].decno;
						dec2[phase]=15.*(1+strsl[n1].decno); //north
						color[phase]=colr[i];
						phase++;
					}//if
				}// if north limit

// limit south 
				n3s= (n3 >=12)? (n3-12) :(n3+12);
				
		 		if(((strsl[n1].strph[n2].strmn[n3s].ph/fltratio[fltno]) > lim[i]) && ((strsl[n1].strph[n2].strmn[n3s].ph/fltratio[fltno]) <= lim[i+1])){
	     	    			if(strsl[n1].strph[n2].tst != strsl[n1].strph[n2].tstp) { 

						count[phase]=strsl[n1].strph[n2].strmn[n3s].ph/fltratio[fltno];
						ra1[phase]=12+strsl[n1].strph[n2].strmn[n3s].rstart;
						ra2[phase]=12+strsl[n1].strph[n2].strmn[n3s].rend;
						if(ra1[phase]>24) {ra1[phase]-=24;}
						if(ra2[phase]>24) {ra2[phase]-=24;}
						dec1[phase]=-15.* strsl[n1].decno;
						dec2[phase]=-15.* (1 + strsl[n1].decno); //south
						color[phase]=colr[i];
						phase++;					
					}//if
				}// if south limit
	     		}//n2
		} //n1
	     } // for i - all limits

//write list 
 

           printf("\n\n # ======================================================== \n");
	   printf("# RA, DEC regions within following Straylight(SL) limits \n for:: day=%i, month=%i,filter=%s\n",day,month,flt[fltno]);
	   printf("# ======================================================== \n");



	   for(j=1;j<5;j++) {
		printf("\n# SL Range = %g - %g \n",lim[j-1]+1,lim[j]);
		printf("# ==================== \n");
		for(i=0;i<phase;i++) {
			if((dec2[i]>0)&& (count[i]<= lim[j]) && (count[i] > lim[j-1])) {
				if(ra1[i]==24) ra1[i]=0;
				if(ra2[i]==24) ra2[i]=0;
				if(dec1[i]!=0 && dec1[i] != dec1[i-1]) {
					printf("\n");
					printf("ra: %.1f to %.1f, dec: %.0f to %.0f, SL=%g \n",ra1[i],ra2[i],dec1[i],dec2[i],count[i]); 
				}
			}
		}
		for(i=0;i<phase;i++) {
			if((dec2[i]<0)&& (count[i]<= lim[j]) && (count[i] > lim[j-1])) {
				if(ra1[i]==24) ra1[i]=0;
				if(ra2[i]==24) ra2[i]=0;
				if(dec1[i]!=0 && dec1[i] != dec1[i-1]) {
					 printf("\n");
					printf("ra: %.1f to %.1f, dec: %.0f to %.0f, SL=%g \n",ra1[i],ra2[i],dec1[i],dec2[i],count[i]); 
				}
			}
		}
	   }
	   printf("\n\n");
	} //SL order list


	if(list ==3) { //DEC in order: List
	   printf("\nDEC in order: List\n");

		phase =0;
		for(n1=0;n1<6;n1++) {  //6 dec ranges
		   	for(n2=0;n2<15;n2++){  //15 phases
 
//   north
				count[phase]=strsl[n1].strph[n2].strmn[n3].ph/fltratio[fltno];
				ra1[phase]=strsl[n1].strph[n2].strmn[n3].rstart;
				ra2[phase]=strsl[n1].strph[n2].strmn[n3].rend;
				dec1[phase]=15.*strsl[n1].decno;
				dec2[phase]=15.*(1+strsl[n1].decno); //north
				color[phase]=colr[i];
				if(count[phase] <= limit)printf("LT: %.1f to %.1f; ra: %.1f to %.1f; dec: %.0f to %.0f; SL=%g\n",strsl[n1].strph[n2].tst,strsl[n1].strph[n2].tstp ,ra1[phase],ra2[phase],dec1[phase],dec2[phase],count[phase]);
				phase++;
	     		}//n2
		} //n1

//  South

		for(n1=0;n1<6;n1++) {  //6 dec ranges
		   	for(n2=0;n2<15;n2++){  //15 phases

				n3s= (n3 >=12)? (n3-12) :(n3+12);

				count[phase]=strsl[n1].strph[n2].strmn[n3s].ph/fltratio[fltno];
				ra1[phase]=12+strsl[n1].strph[n2].strmn[n3s].rstart;
				ra2[phase]=12+strsl[n1].strph[n2].strmn[n3s].rend;
				if(ra1[phase]>24) {ra1[phase]-=24;}
				if(ra2[phase]>24) {ra2[phase]-=24;}
				dec1[phase]=-15.* strsl[n1].decno;
				dec2[phase]=-15.* (1 + strsl[n1].decno); //south
				color[phase]=colr[i];
				if(count[phase] <= limit) printf("LT: %.1f to %.1f; ra: %.1f to %.1f, dec: %.0f to %.0f, SL=%g\n",strsl[n1].strph[n2].tst,strsl[n1].strph[n2].tstp ,ra1[phase],ra2[phase],dec1[phase],dec2[phase],count[phase]);
				phase++;					
	     		}//n2
		} //n1
	   } //DEC order list





	if(day1 >21) day2ra1 = day1-21;
	else if(day1 >5) day2ra1 = day1-6;
	else if(day1 <=5) day2ra1 = day1 + 10;
	if(day2 >21) day2ralast = day2-21;
	else if(day2 >5) day2ralast = day2-5;
	else if(day2 <=5) day2ralast = day2 + 10;
	day2ra2 =day2ra1;
  	printf("1:day2ra1 =%d, day2ralast=%d, day2ra2=%d\n",day2ra1, day2ralast, day2ra2);


// RA ranges, DEC  and values

	if(list ==1) { //RA ordered list
	   printf("\nLT or RA ordered List\n");
	   i =0;
	   set =0;
	   cross24 =0;
	   if((n31 > n32)||((n31==n32)&&(day2ra2>day2ra1) )) {
		n321 =  24;
		cross24=1;
	   }
	   else n321 =n32;

	   for (n33=n31;n33<=n321;n33++) {
	     for(day2ra=day2ra1;day2ra<=day2ra2;day2ra++){  //between exact days
		n3 =n33;
		if(n3 ==24) n3=0;
		i++;
		printf("\n\nday: %d \n=========\n",i);
// 		printf("Month= %d: Day= %d\n",n3,n3);
  		printf("2:day2ra =%d,,day2ra1 =%d, day2ralast=%d, day2ra2=%d\n",day2ra, day2ra1, day2ralast, day2ra2);

		phase =0;
		t1 =-1;
	   	for(n2=0;n2<15;n2++){  //15 phases



			     for(n1=0;n1<6;n1++) {  //6 dec ranges

// limit north
				count[phase]=strsl[n1].strph[n2].strmn[n3].ph/fltratio[fltno];
				ra1[phase]=strsl[n1].strph[n2].strmn[n3].rstart + day2ra/15.;
				ra2[phase]=strsl[n1].strph[n2].strmn[n3].rend + day2ra/15.;
				if(ra1[phase]>=24) {ra1[phase]-=24;}
				if(ra2[phase]>=24) {ra2[phase]-=24;}
				dec1[phase]=15.*strsl[n1].decno;
				dec2[phase]=15.*(1+strsl[n1].decno); //north
				color[phase]=colr[i];
				if(count[phase] <= limit) {
				   	if(t1 == strsl[n1].strph[n2].tst) {
// 						printf("\t\t\t\t dec: %.0f to %.0f , SL=%g\n", dec1[phase],dec2[phase],count[phase] );
			        	}
// 					else printf("LT: %.1f to %.1f; ra: %.3f to %.3f, dec: %.0f to %.0f , SL=%g\n",strsl[n1].strph[n2].tst,strsl[n1].strph[n2].tstp ,ra1[phase],ra2[phase],dec1[phase],dec2[phase],count[phase] );
						   
					t1 = strsl[n1].strph[n2].tst;
				}
				phase++;
 			     }
//South
			     for(n1=0;n1<6;n1++) {  //6 dec ranges

				n3s= (n3 >=12)? (n3-12) :(n3+12);
				
	     	    			if(strsl[n1].strph[n2].tst != strsl[n1].strph[n2].tstp) { 

						count[phase]=strsl[n1].strph[n2].strmn[n3s].ph/fltratio[fltno];
						ra1[phase]=12+strsl[n1].strph[n2].strmn[n3s].rstart + day2ra/15.;
						ra2[phase]=12+strsl[n1].strph[n2].strmn[n3s].rend + day2ra/15.;
						if(ra1[phase]>=24) {ra1[phase]-=24;}
						if(ra2[phase]>=24) {ra2[phase]-=24;}
						dec1[phase]=-15.* strsl[n1].decno;
						dec2[phase]=-15.* (1 + strsl[n1].decno); //south
						color[phase]=colr[i];

						if(count[phase] <= limit) {
						   if(t1 == strsl[n1].strph[n2].tst) {
// 							printf("\t\t\t\t dec: %.0f to %.0f, SL=%g \n", dec1[phase],dec2[phase],count[phase] );
							}
// 						   else printf("LT: %.1f to %.1f; ra: %.3f to %.3f, dec: %.0f to %.0f, SL=%g \n",strsl[n1].strph[n2].tst,strsl[n1].strph[n2].tstp ,ra1[phase],ra2[phase],dec1[phase],dec2[phase],count[phase] );
						   
						   t1 = strsl[n1].strph[n2].tst;
						}
						phase++;					
					}//if
	     		     }//n1

			} //n2

			if((n33==n32)&&(cross24)){
// 				day2ra1=1;
				day2ra2 =15;
			}
			else if(n33==n32)day2ra2 = day2ralast;
			else if(day2ra ==15) {
				day2ra1=1;
				day2ra2 =15;
			printf("Reset:day2ra1 =%d, day2ra2=%d \n", day2ra1,day2ra2);
			}
			else if((!set)&&(day2ra <15)) {
				day2ra2 =15;
				set =1;
			printf("1stset:day2ra1 =%d, day2ra2=%d \n", day2ra1,day2ra2);
			}
  			printf("2:n33=%d,n31=%d,n32=%d,n321=%d\n",n33,n31,n32,n321);
  			printf("2:same:day2ra =%d,,day2ra1 =%d, day2ralast=%d, day2ra2=%d\n",day2ra, day2ra1, day2ralast, day2ra2);
		     } // for each day

		     if((cross24)&&(n33 == 24)) {  //2nd loop,if end month no > start month no
		  		n33 =0; 
				n321 =n32; 
  				printf("2:reset:n33=%d,n31=%d,n32=%d,n321=%d\n",n33,n31,n32,n321);
		     }
  		} //for selected 15 days 

	}// RA ordered list

 
   return(0);
}//end main

