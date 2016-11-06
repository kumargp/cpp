/*
GNU Licence

straylight_radec.c

This program gives straylight counts for given RA, DEC for all filters.

It needs file:
sl_table_north.txt (straylight file)


Arg1: 
Arg2: 
Arg3: 

author: Gopakumar
*/

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <math.h>
// #include <ctype.h>


#define PI 3.1415926535897932384
#define ra2deg  (180.0/PI)
#define deg2rad (PI / 180.0)


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
// int  main (int argc, char *argv[]) {
void straylight_radec(int n2temp,double dec,int lmnth,int lday,double *strlight){
	FILE *slfp=NULL ;
	struct strslight strsl[10];
	struct strmnth strmn[26];
	struct strphase strph[16];

	
	int i=0,j=0,n1=0,n2=0,n3=0,n3s=0,n31=0,pl=0,plt=0,fltno=0,rasouth,phase=0,day=0,month=0,plotno=0,plotlimit=0,list=1;//,n2temp=0; //n1=dec, n2=phase,n3=month,n3s=n3south
	float xmin=0,xmax=0,ymin=0,ymax=0;
	char filename[50],line[200], tem[50][50];
	float ra1[1000],ra2[1000],dec1[1000],dec2[1000];

	//Stray Light limits
	double count[5]; 
// 	double lim[] = {-1,100,1000,5000,1E+20,1E+25};
//  	double lim[] = {-1,1000,5000,10000,20000,1E+20,1E+25}; //0,lowlim,medlim,highlim,allowed,sp

	//filters
	float fltratio[]= {1.,1111,35,8.88,116};  //bbf/bbf, bbf/sf1, bbf/sf2, bbf/sf3, bbf/nbf3. last-ETC.
	char *flt[] = {"BBF", "SF1", "SF2", "SF3", "NBF"};
	
// 	if(argv[1]) day=atoi(argv[1]);
// 	if(argv[2]) month=atoi(argv[2]);
// 	if(argv[3]) fltno=-1+atoi(argv[3]);

/*	
	if((argc <4)||(day>31) || (day < 1) || (month>12) || (month < 1) || (fltno>4) || (fltno < 0) ) {

		printf("Arguments are wrong. Correct Usage is:\n \tExample:   ./regions_for_date 2 4 3 \n \tArg1: 1-31 (day) ,\n \tArg2: 1-12 (month) \n \tArg3: 1:BBF, 2:SF1, 3:SF2, 4: SF3, 5: NBF3 (filter number) \n");   

		exit(0);
	}
*/
	// open file
	if((slfp = fopen("sl_table_north.txt","r")) == NULL) {
       		printf("Error: SL file not opened !\n");
       		exit(1);
       	}//fopen

	// read line from SL file into line[], and store in tem structure
	n1=-1, n2=0; //starting n1 and n2
	while ((fgets(line,sizeof(line),slfp))!=NULL) {
  	sscanf(line, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", tem[0],tem[1], tem[2],tem[3], tem[4],tem[5], tem[6],tem[7], tem[8],tem[9], tem[10],tem[11], tem[12],tem[13], tem[14]);

  		if(strcmp(tem[0],"")==0) continue;  //skip blank lines
  		if(strcmp(tem[0],"#")==0) continue; //skip comments
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
        		}//for n3
      		}//n2
  	}//n1


// //day,month from arguments
	n3=0;
	day=lday;
// 	n3= atof(argv[3]) *2 +6; 
	n3= lmnth *2 +6; 
	if((day>=6) && (day<=21)) n3 = n3-1;
	if(day< 6) n3 = n3-2;
	if(n3>=24) n3 = n3-24;

// 	n1 = (int)(atof(argv[2])/15);  //dec
	n1=(int)(dec/15);
// 	n2temp = (int)(atof(argv[1]));
	for(i=0;i<15;i++) {
		if(n2temp > strsl[0].strph[0].strmn[0].rstart)
		if(n2temp <=  strsl[0].strph[0].strmn[0].rend)
			n2=i;
	}



//sorting for SL < limit

	for(fltno=0;fltno<5;fltno++) { //1=bbf,2=sf1,3=sf2,3=sf3,4=nbf3

//      		if(strsl[n1].strph[n2].tst != strsl[n1].strph[n2].tstp) { 
// limit north
			count[fltno]=strsl[n1].strph[n2].strmn[n3].ph/fltratio[fltno];
			ra1[fltno]=strsl[n1].strph[n2].strmn[n3].rstart;
			ra2[fltno]=strsl[n1].strph[n2].strmn[n3].rend;
			dec1[fltno]=15.*strsl[n1].decno;
			dec2[fltno]=15.*(1+strsl[n1].decno); //north
			
// 		}// if north limit

// limit south 
		n3s= (n3 >=12)? (n3-12) :(n3+12);
				
			count[fltno]=strsl[n1].strph[n2].strmn[n3s].ph/fltratio[fltno];
			ra1[fltno]=12+strsl[n1].strph[n2].strmn[n3s].rstart;
			ra2[fltno]=12+strsl[n1].strph[n2].strmn[n3s].rend;
			if(ra1[fltno]>24) {ra1[fltno]-=24;}
			if(ra2[fltno]>24) {ra2[fltno]-=24;}
			dec1[fltno]=-15.* strsl[n1].decno;
			dec2[fltno]=-15.* (1 + strsl[n1].decno); //south
	}//flts

//write SL o/p
//      printf("# RA =%.4f, DEC=%.4f, SL(BBF)=%g,SL(SF1)=%g, SL(SF2)=%g,SL(SF3)=%g,SL(NBF)=%g\n", ra,dec, count[0], count[1], count[2], count[3], count[4]);

*strlight =count[0];

//      for(i=0;i<5;i++) {&strlight[i] =count[i]; }
//    return(0);
}//end main

