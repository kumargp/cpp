#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "telefns.c"
#include "simtelefn.c"
#include "straylight_radec.c"
#include "observfns.c" 


#define RAD_PER_DEG 0.0174532925 /*radians per degree*/
#define PI 3.1415926535897932384

int main(int argc, char *argv[]) {


  int lms1=0,lsec1=0,lmin1=23,lhr1=17,lday1=21,lmnth1=7,lyear1 =2008; //to be read from elsewhere
  int lms2=0,lsec2=0,lmin2=30,lhr2=17,lday2=21,lmnth2=7,lyear2 =2008; //to be read from elsewhere

  int lms,lsec,lmin,lhr,lday,lmnth,lyear; //variables
  int utms=0,utsec=0,utmin=0,uthr=0,utday=0,utmnth=0,utyear =0; //82.5 E/15= 5.5 hrs 

  double sun_ra=0, sun_dec=0,sun_angle=-1,racent,deccent = 70,rastart=10,raend=10.01,dec1,dec2;
  double strlight[5],darkcount=0.00001,totalpixels=1048576, limit=50000;
  double jitter=0.001,strlight_start;
  int i=0,j=0,k=0,start=0;
/*
  //if ra dec from sim param file 
  get_radec_from_simpar(&rastart, &raend, &deccent);
  printf("%f,%f,%f\n",rastart, raend, deccent);
*/

lms=lms1,lsec=lsec1,lmin1=atof(argv[5]),lhr1=atof(argv[4]),lday1=atof(argv[3]),lmnth1=atof(argv[2]),lyear1=atof(argv[1]);
lms=lms1,lsec=lsec1,lmin=lmin1,lhr=lhr1,lday=lday1,lmnth=lmnth1,lyear =lyear1;
lmin2=atof(argv[10]),lhr2=atof(argv[9]),lday2=atof(argv[8]),lmnth2=atof(argv[7]),lyear2=atof(argv[6]);
dec1=atof(argv[11]),dec2=atof(argv[12]);

 printf("Start:year=%d,mnth=%d,day=%d,hr=%d,min=%d,sec=%d,ms=%d\n",lyear1, lmnth1, lday1,lhr1,lmin1,lsec1,lms1);
 printf("END::year=%d,mnth=%d,day=%d,hr=%d,min=%d,sec=%d,ms=%d\n",lyear2, lmnth2, lday2,lhr2,lmin2,lsec2,lms2);


// for( ){  //START HERE

//open file for appending parameters of sky sim


	while(1){  //TIME LOOP
		i++;
//incement local time or ut here; 
		add_frametime(&lyear,&lmnth,&lday,&lhr,&lmin,&lsec,&lms);
//   printf("NUM=%d,year=%d,mnth=%d,day=%d,hr=%d,min=%d,sec=%d,ms=%d\n",i,lyear, lmnth, lday,lhr,lmin,lsec,lms);
		time2ut(lyear,lmnth,lday,lhr,lmin,lsec,lms,&utyear, &utmnth, &utday,&uthr,&utmin,&utsec,&utms);
//   printf("utyear=%d,utmnth=%d,utday=%d,uthr=%d,utmin=%d,utsec=%d,utms=%d\n",utyear, utmnth, utday,uthr,utmin,utsec,utms);
	
		if((lmin>=lmin2)&&(lhr>=lhr2)&&(lday>=lday2)&&(lmnth>=lmnth2)&&(lyear >=lyear2)) {
// if(start==0)printf("No region to observe\n");
		break;
		}
	
// get racent,deccent from date and time
		ra_from_time(lyear,lmnth,lday,lhr,lmin,lsec,&racent);
	
//check sunangle
		SUN_RA_DEC(uthr, utday, utmnth, utyear, &sun_ra, &sun_dec);
		angle(sun_ra, sun_dec,racent, deccent,&sun_angle);
		if(sun_angle <90) {
// 	printf("sun ang(= %.2f) is < 90; Searching other decs for obs\n",sun_angle);
//	get_newdec();
		
		} 

//check sl
		get_straylight(racent, deccent,lday,lmnth,lyear, &strlight[0]); 
    printf("SL=%.0lf,limit= %.0lf\n",strlight[0],limit);
		if(strlight[0] > limit) { continue;}
		else {
			start=1;	//?????????????
// 			if(i==0)
			strlight_start=strlight[0];
			if(strlight_start !=strlight[0]) {	
			break;
			}
		}
	
//get jitter from time
//thruster ???????????????????????????
		get_jiiter(lmnth,lday,lhr,lmin,lsec,lms,&jitter);
	
	
	} //TIME LOOP

/*
	if(start==0) {
		printf("Exit::No region to observe\n");
		exit(0);
	}*/
// darkcount=strlight/totalpixels;


//decide start and end obs ra's  and pass it to log file section


//edit sim param file
// eclipse, jitter, thruster contamination, SP in FOV, ----------------------
//get time and ra that is not affected, then affected, .....
	deccent=dec1;
  	edit_simpar_file(rastart,raend,deccent,jitter,darkcount); 


//run skysim to get data
//check param file contains values, program installed and catalog present 
  	system("uvs_simulate_sky");
  	printf("***Created Events file\n");
//rename file 
//append ra's,dec,jitter,darkcnt,fwhm to a file


// simtelemetry
  	simtelemetry(utyear,utmnth,utday,uthr,utmin,sun_angle);
  	printf("***Created Telemetry\n");
// sim log files

// sim commands

//cat files


// }// END HERE


return 0;
}
