#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "plplot.h"
#include "struct.h"
#include "plotskyfn.h"


#define PI 3.1415926535897932384
#define ra2deg  (180.0/PI)
#define deg2rad (PI / 180.0)

//start main
int  main (int argc, char *argv[]) {

	FILE *slfp=NULL ;
	struct strslight strsl[10];
	struct strmnth strmn[26];
	struct strphase strph[16];
	struct tempstore tem[20];
	
	int i,j,n1,n2,n3=0,n3s,n31,pl,fltno=0,rasouth,phase=0,day=0,month=0,plotno=0,plotlimit=0; //n1=dec, n2=phase,n3=month,n3s=n3south
	float xmin=0.,xmax=0.,ymin=0.,ymax=0.;
	char filename[50],line[200];
	float ra1[1000],ra2[1000],dec1[1000],dec2[1000],decs=0,decend=0,decss,decee,refdec;

	//Stray Light limits
	float lim[] = {-1,100,1000,5000,1E+20,1E+25};//0,lowlim,medlim,highlim,>highlim,sp
	int color[1000],colr[]= {9,11,2,0,1};
	//filters
	float fltratio[]= {1.,1111,35,8.88,116};  //bbf/bbf, bbf/sf1, bbf/sf2, bbf/sf3, bbf/nbf3. last-ETC.
	char *flt[] = {"BBF", "SF1", "SF2", "SF3", "NBF"};
	
	//surveys:HDF,CDFS,CDFN,HELLAS2XMM,Goods,SDSS,2df,ALFALFA		
	  //     char *survey[] = {"HDF","CDFS","CDFN","HELLAS2XMM","Goods", "SDSS","2df", "ALFALFA"};
	
	//objects
	float obj_ra1[]={5.39,      0.877,  0.712,   1.564,  12.996   , 12.442};
	float obj_dec1[]={-69.756, -72.8,  41.269,  30.66,  27.981,  12.723};
	char *object[] = {"LMC","SMC","M31","M33","COMA","VIRGO"};
	int objectcolr[] ={3,3,3,3,3,3};
	float obj_ra2[10],obj_dec2[10],objraspread=0.15,objdecspread=0.40;
	
	//eclipse
	  //  void plot_eclips(int n3,int ecl_colr) {
	  //  int ecl_colr =0;
	
	char *days[]= {"Sep21","Oct6", "Oct21","Nov6", "Nov21", "Dec6","Dec21", "Jan6","Jan21", "Feb6","Feb21", "Mar6","Mar21", "Apr6","Apr21", "May6","May21", "Jun6","Jun21", "Jul6", "Jul21", "Aug6", "Aug21", "Sep6"};
	char *xlbl= "<- RA ->", *ylbl="<- Decl->", toplbl[50]="";


	// open file
	if((slfp = fopen("tabsl.txt","r")) == NULL) {
       		printf("Error: SL file not opened !\n");
       		exit(1);
       	}//fopen

		if( (argc <6)|| ((day=atoi(argv[1]))>31) || (day < 1) || ((month=atoi(argv[2]))>12) || (month < 1) || ((fltno=-1+atoi(argv[3]))>4) || (fltno < 0) || (decs=atof(argv[4])) <-90. || (decend=atof(argv[5]))>90.) {
	
	//	sscanf(date,%c%c%c%c%c,&day[0],&day[1],&month[0],&month[0],&month[1]); //third is not used
			printf("Usage:\n \t Arg 1: date (1-31),\n \t Arg2: month (1-12) \n \tArg3: filter number (1:BBF, 2:SF1, 3:SF2, 4: SF3, 5: NBF3), \n  \t Arg4: dec_start \n \t Arg5: dec_end\n");   
	
			exit(0);
		}



	// read line from SL file into line[], and store in tem structure
	n1=-1, n2=0; //starting n1 and n2
	while ((fgets(line,sizeof(line),slfp))!=NULL) {
  	sscanf(line, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", tem[0].p,tem[1].p, tem[2].p,tem[3].p, tem[4].p,tem[5].p, tem[6].p,tem[7].p, tem[8].p,tem[9].p, tem[10].p,tem[11].p, tem[12].p,tem[13].p, tem[14].p);

  		if(strcmp(tem[0].p,"")==0) continue;  //skip blank lines
  		if(strcmp(tem[0].p,"#")==0) continue; //skip comments
  		if(strcmp(tem[0].p,"Decl")==0){       //if Dec range line.,store decstart,decend 
			n1++;
			strsl[n1].dec1= atof(tem[1].p);
	  		strsl[n1].dec2= atof(tem[2].p);
			strsl[n1].decno=n1;
	  		for(i=0;i<3;i++) { 
				strcpy(tem[i].p,"");  //clear strings
		  		n2=0;  //reset phase number for values.
			}
  		} //if
		else {
	 		strsl[n1].strph[n2].tst= atof(tem[0].p);
  	 		strsl[n1].strph[n2].tstp = atof(tem[1].p); 
			strsl[n1].strph[n2].t36st = (atof(tem[0].p) < 12.0) ? (atof(tem[0].p) +24.0) : atof(tem[0].p);  //shift to 12-36
			strsl[n1].strph[n2].t36stp = (atof(tem[1].p) < 12.0) ? (atof(tem[1].p) +24.0) : atof(tem[1].p);

  	 		for(i=0;i<13;i++) {   //ph starts from thrid column only, from sep21 for 0 --
     	 			j = ((i+18)>=24)?(i+18-24):(i+18); 
	 			strsl[n1].strph[n2].strmn[j].ph = strsl[n1].strph[n2].strmn[18-i].ph = atof(tem[i+2].p);
	 		} //for
 			for(i=0;i<15;i++) strcpy(tem[i].p,"");  //clearing strings
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
	



//day,month from arguments
	n3=0;  //if n3 is not supplied, the first month
        //don't change the position, since n3 is modified in previous loop.
	n3= month *2 +6; 
	if((day>=6) && (day<=21)) n3=n3-1;
	if(day< 6) n3=n3-2;
	if(n3>=24) n3= n3-24;



//sorting for SL < limit, and plotting

// for(fltno=0;fltno<5;fltno++) { //1=bbf,2=sf1,3=sf2,3=sf3,4=nbf3
// 	for(n3=0; n3<24; n3++) {  //15 days

	    phase=0;	
	    for(i=0;i<5;i++) {		//5 limits
		if(i==3) continue;
		for(n1=0;n1<6;n1++) {  //6 dec ranges
	   		for(n2=0;n2<15;n2++){  //15 phases
		
		 		if(((strsl[n1].strph[n2].strmn[n3].ph/fltratio[fltno]) > lim[i]) && ((strsl[n1].strph[n2].strmn[n3].ph/fltratio[fltno])  <= lim[i+1])){
	     	    			if(strsl[n1].strph[n2].tst != strsl[n1].strph[n2].tstp) { 

// limit north
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

						ra1[phase]=12+strsl[n1].strph[n2].strmn[n3s].rstart;
						ra2[phase]=12+strsl[n1].strph[n2].strmn[n3s].rend;
						if(ra1[phase]>=24) {ra1[phase]-=24;}
						if(ra2[phase]>=24) {ra2[phase]-=24;}
						dec1[phase]=-15.* strsl[n1].decno;
						dec2[phase]=-15.* (1 + strsl[n1].decno); //south
						color[phase]=colr[i];
						phase++;					
					}//if
				}// if south limit
	     		}//n2
		} //n1
	     } // for i - all limits


//Projections

	refdec=decs;
	plotno= pl;
	plotlimit= pl;
	    for(pl=plotno;pl<=plotlimit;pl++) {
			n31=(n3==23)?0:(n3+1);
			get_filename(flt[fltno], days[n3], days[n31], pl, filename,toplbl);
			plsdev("gif");//	plsdev("png");
			plsfnam(filename);
			plscol0(3, 0,0,0); 
			plscolbg (255, 255, 255);
			plinit();
			xylimits(&xmin,&xmax,&ymin,&ymax, decs, decend);

			plenv(xmin-1.5, xmax+0.5, ymin-0.5, ymax+1., 0, -2);

// printf("xmin =%.1f,xmax =%.1f,ymin =%.1f,ymax =%.1f\n",xmin,xmax,ymin,ymax);
        		plschr(0.0, 0.6);  //text or char size
	
			for(i=0;i<6;i++) {  //object spread
				obj_ra2[i]= obj_ra1[i] + objraspread;
			}
			for(i=0;i<6;i++) {
				obj_dec2[i]= (obj_dec1[i] < 0) ? (obj_dec1[i]-objdecspread):(obj_dec1[i] + objdecspread);
			}

//Type

			pllab ("", ylbl, "");
			for(i=0;i<phase;i++){ //SL
				if((((dec1[i]) >(decs-15)) && ((dec2[i])>decs)) && (dec2[i] < (decend +15))) { 
// 					fillarea(ra1[i],ra2[i],dec1[i],dec2[i],color[i]);
					decss = (dec1[i] < decs)? decs: dec1[i];
					decee = (dec2[i] > decend)? decend: dec2[i];
					fillarea(ra1[i],ra2[i],decss,decee,color[i],refdec);
// printf("ra1[i]=%.1f,ra2[i]=%.1f,dec1[i]=%.1f,dec2[i]=%.1f\n",ra1[i],ra2[i],decss,decee);
				}

			}
			for(i=0;i<6;i++){ //objects
				if((dec1[i]>=decs) && (dec2[i] <=decend)) fillarea(obj_ra1[i],obj_ra2[i],obj_dec1[i],obj_dec2[i],8,refdec);
				label_rd(obj_ra1[i],obj_dec1[i],object[i],objectcolr[i]);
			}
			drawgrid_ait(3,decs,decend);  //bracket: color code

	     		pladv(0);
	     		plend();
	     }
// 	}//for n3
//    } //filters loop
   exit(0);
   return(0);
}//end main
