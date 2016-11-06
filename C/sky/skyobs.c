#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


//if needed change printf to fprintf to outfp

  struct strmnth {
	float ph;
	float rstart;
	float rend;

  	};
  struct strphase {
	float tstart;
	float tend;
	struct strmnth strmn[24];
  	};

  struct strslight {
//	char d[10];
	float dec1;   //start dec
	float dec2;   //end dec
	struct strphase strph[15];
//	struct stmed;
  	};
  struct tempstore {
	char p[15];
  	};
  struct scanvalues {
	int best; //if 1 it is best, if zero not best, but within limit or ??
	int monthno; //month
	float imgstart;
	float imgstop;  
	float decval;  //dec
	float slval;  //ph for within limit
  	};



int  main (int argc, char *argv[]) //argv[1]=dec, (ra)
  {

	FILE *slfp=NULL, *outfp=NULL;  //SL file and o/p file

	struct strmnth strmn[25];  // half month
	struct tempstore tem[25];  //temp read
	struct strslight strsl[10];  //for 6 decl ranges
	struct strphase strph[16]; //, strmed[100] , strhigh[100];
	struct scanvalues scanval[100]; //scan values selected within limit

	float lowlim = 100.0, medlim=3000.0;  // now medlim is set. Change to lowlim, or change the no.

  	int n1=0,n2=0,n3=0,n4=0; //dec, phase, month,list
	int i=0,j=0,n=0;
	int decno=0; //find decno
	float ra = 0.0, dec = 0.0; //  ra, dec
	char line[200];//sl file temp store


	char monthstring[24][10]= {"sep21","oct6", "oct21","nov6", "nov21", "dec6","dec21", "jan6","jan21", "feb6","feb21", "march6","march21", "april6","april21", "may6","may21", "june6","june21", "july6", "july21", "aug6", "aug21", "sep6"};


  	if((argc <2) ||(argc> 3))  {
        printf("Arguments: dec [ra] \n The ra is optional; if it is given the result is only for around that ra for given dec\n");
        exit(1);
  	}

   	if(((dec=atof(argv[1])) < -90.0)|| ((atof(argv[1])) > 90.0)) {  //dec =argv[1]
        	printf("dec not in range\n");
        	exit(1);
  	}
	if(argc==3) {
   		if(((ra=atof(argv[2])) < 0.0)|| ((atof(argv[2])) >= 24.0)) {  //dec =argv[1]
        		printf("ra not in range\n");
        		exit(1);
  		}
	}

	if((slfp = fopen("tabsl.txt","r")) == NULL) {
       		printf("File didn't open !\n");
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
//      	printf("Decl line:n1=%d,dec1=%.1f,dec2=%.1f\n",n1,strsl[n1].dec1, strsl[n1].dec2);
	  		n1++;  //ready to store next decl ranges.
	  		n2=0;  //reset phase number to start.
		  	continue;  //next line, if inside this loop
  			} //if

  		else   { 	//line having SL counts
	 		strsl[n1-1].strph[n2].tstart= atof(tem[0].p);
  	 		strsl[n1-1].strph[n2].tend = atof(tem[1].p); 
//    		printf("n2=%d,tstart=%.1f,tend=%.1f\n",n2,strsl[n1-1].strph[n2].tstart,strsl[n1-1].strph[n2].tend);

  	 		for(i=0;i<13;i++) {   //ph starts from thrid column only, from sep21 for 0 --
     	 			j = ((i+18)>=24)?(i+18-24):(i+18); 
	 			strsl[n1-1].strph[n2].strmn[j].ph = strsl[n1-1].strph[n2].strmn[18-i].ph = atof(tem[i+2].p);
	 			} //for

 			for(i=0;i<15;i++) strcpy(tem[i].p,"");  //emptying strings
      			n2++;
			}//else 
   		} //while gets
   	fclose(slfp);



//storing ra values from local time by adding with racent for each month
      	n1 = n2 = 0; //resetting n1 and n2;
   	for(n1=0; n1<6; n1++) {  //6 dec ranges
	      	for(n2=0; n2<15; n2++) { //15 entries in table.
        		for(n3=0; n3<24; n3++) { //24 half months

				strsl[n1].strph[n2].strmn[n3].rstart= strsl[n1].strph[n2].tstart + n3;
				strsl[n1].strph[n2].strmn[n3].rend= strsl[n1].strph[n2].tend + n3;

// 	printf("n1=%d,n2=%d,add=n3=%d", n1,n2, n3);
// 	printf("tstart=%.1f,tend=%.1f", sl[n1-1].strph[n2].tstart, strsl[n1-1].strph[n2].tend);
// 	printf("rstart=%.1f,rend=%.1f\n",strsl[n1].strph[n2].strmn[n3].rstart, strsl[n1].strph[n2].strmn[n3].rend);

				if(strsl[n1].strph[n2].strmn[n3].rstart >=24.0) {
					strsl[n1].strph[n2].strmn[n3].rstart -= 24.0;
					}
				if(strsl[n1].strph[n2].strmn[n3].rend >=24.0) {
					strsl[n1].strph[n2].strmn[n3].rend -= 24.0;
					}	
// 	printf("Corrected ::rstart=%.1f,rend=%.1f,ph=%.1f \n",strsl[n1].strph[n2].strmn[n3].rstart, 	strsl[n1].strph[n2].strmn[n3].rend, strsl[n1].strph[n2].strmn[n3].ph); 
        			}//for n3
      			}//n2
  		}//n1


//Find decno, given dec.
	n1=0;		//reset n1
 	for(i=0;i<6;i++) {
		if((dec >= (15.0*i))&&(dec<(15.0*(i+1)))) {
			decno =i; 
			}
		}
  	n1=decno;


// find month and RA ranges for given dec. n1 is set just above
	if(argc==2) {
	n4=0;
	printf("At given DEC of %.1f for SL limit=%.1f\n",dec,medlim);
	printf("NOTE:In time and RA, numbers are in decimal points \n");
	for(n3=0; n3<24; n3++) {
	   	for(n2=0;n2<15;n2++){

		 		if((strsl[n1].strph[n2].strmn[n3].ph) < medlim){
			// store rastart and raend in new arrays here ?
			//storing phases in new arrays
	     	    			if(strsl[n1].strph[n2].tstart != strsl[n1].strph[n2].tend) { //check
			  			scanval[n4].monthno = n3;
						scanval[n4].imgstart=strsl[n1].strph[n2].tstart;
	     					scanval[n4].imgstop=strsl[n1].strph[n2].tend;
	     		  			scanval[n4].decval=n1; //check it 
	     		  			scanval[n4].slval=strsl[n1].strph[n2].strmn[n3].ph;
			  			scanval[n4].best = (scanval[n4].slval < lowlim) ? 1:0; // best values
			  			}//if

		printf("During:%7s-%6s, <RA>: %.1f - %.1f    ", monthstring[n3],((n3+1)==24)? monthstring[0]:monthstring[n3+1], strsl[n1].strph[n2].strmn[n3].rstart, strsl[n1].strph[n2].strmn[n3].rend);

		printf("<LocalTime>: %.1f - %.1f, \n", scanval[n4].imgstart, scanval[n4].imgstop);
// 		printf("SL=%.1f",scanval[n4].slval);

					n4++;
		       			}// if medlim

	     			}//second for
		} //outermost for
	}//if argv[2]
		

// 	printf("scan: Local Times===================================\n");
// 	for(i=0;i<n4;i++) {
//  	     	printf("During ::%7s-%6s, start_t=%.1f, end_t=%.1f, \n", monthstring[scanval[i].monthno], ((scanval[i].monthno+1)==24)? monthstring[0]:monthstring[scanval[i].monthno], scanval[i].imgstart, scanval[i].imgstop);
// 		}



//	To find RA ranges for given dec and ra, add this. n1 is set above
  	if(argc==3) {

  		printf("At given Dec = %.1f and around RA =%.1f, for SL limit=%.1f \n",dec,ra,medlim);
		printf("NOTE:In time and RA, numbers are in decimal points \n");
		n4=0;
		for(n3=0; n3<24; n3++) {
	   	    for(n2=0;n2<15;n2++){

			if(((strsl[n1].strph[n2].strmn[n3].rstart > strsl[n1].strph[n2].strmn[n3].rend) && 
			((strsl[n1].strph[n2].strmn[n3].rstart < ra) ||(strsl[n1].strph[n2].strmn[n3].rend > ra))) 
			||
			((strsl[n1].strph[n2].strmn[n3].rstart) <= ra) && 
			((strsl[n1].strph[n2].strmn[n3].rend) >= ra))	{
		 		if((strsl[n1].strph[n2].strmn[n3].ph) < medlim){
				    if(strsl[n1].strph[n2].tstart != strsl[n1].strph[n2].tend) { //check
					printf("During:%7s-%6s, <RA>: %.1f - %.1f      ", monthstring[n3],((n3+1)==24)? monthstring[0]:monthstring[n3+1], strsl[n1].strph[n2].strmn[n3].rstart, strsl[n1].strph[n2].strmn[n3].rend);

					printf("<LocalTime> %.1f - %.1f\n",  strsl[n1].strph[n2].tstart,strsl[n1].strph[n2].tend);

					} //if
				    }//if medlim
				}//if ra check
			}//for n2
		    }//for n3
		}

} //main
