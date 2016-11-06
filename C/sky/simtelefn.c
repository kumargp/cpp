#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
// #include "telefns.c"
#define RAD_PER_DEG 0.0174532925 /*radians per degree*/
#define PI 3.1415926535897932384

// void main(int argc, char *argv[]) {
// void simtelemetry() {
void simtelemetry(int utyr,int utmth,int utd,int uth,int utmn,float sun_angl) {
	FILE *fout = NULL, *params=NULL;

	float raincr=0.0005330,racent=0,deccent = 70;//raincr=0.000533
	float scanrate=0.004167,rastart=12.0,raend=12.10;

	unsigned long int frame=0,noofframes =0; //27800
	unsigned long int obt=0,obtstart=0,obtoffset= 50000;  //change to long double

	char ipkey[10],key[50],key1[20],key2[20],fileline[200];
	char sync[10]="AC CA 1F";//,ut[100]="2008-6-15-19-10";  

	int dynblkno=0,spare=0,sysmod1=3,sys_status=112,sys_test=255, tel_status =56;

	char dynblk[]="AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
	int T1flt=0,T2flt=0,T3flt=0,flt_state =36,i,binlen=0, num=0,valarr[3],lenarr[3];//filt
	char *flt[]={"BBF","SF1","SF2","SF3","NBF3"};	

	int busvolt=7,therm_stat=0,obt_resetflag =0;


	int sysmod2=1,thrwarn=0,shutwarn=0,dhmtx=0,dhmb=0,num_sysmod2=0,hkmod=1, mdpflag=0, thrflag=0;
	int sys2arr[7];
	int sys2len[]={1,1,1,1,1,1,1};

	int thrfire_uthr[]={10,23},thrfire_utmin[]={2,9},thrfire_utsec[]={10,15};

	int mdpang =16000,spangle =500;
	double sun_ra=0,sun_dec = 0;;
	float sun_angle=1000;
	double moon_ra=0,moon_dec =0,moon_angle = 630;
	int lmin=23,lhr=17,lday=21,lmnth=7,lyear =2008; //to be read from elsewhere
	int utms_start=0,utms=-128,utsec=0,utmin=0,uthr=0,utday=0,utmnth=0,utyear =0; //82.5 E/15= 5.5 hrs 
	unsigned long int xcoord=2000,ycoord=1000, zcoord = 36000000;




	utmin=utmn,uthr=uth,utday=utd,utmnth=utmth,utyear =utyr;

// 2008-6-13-3-30

// 	if(argc ==4) {
// 		 rastart= atof(argv[1]);
// 		 raend= atof(argv[2]);
// 		 deccent= atof(argv[3]);
// 	}
	
/*	else if (argc < 4) {
		printf("If you want to give input enter 'y', else press any other key\n");
		scanf("%s",&ipkey);		

	    if(strcmp(ipkey,"y")==0) {
		printf("Enter rastart, raend, dec, UT(UT format:year-month#-day-hr-min\n");
		printf("RA start:");
		scanf("%f",&rastart);
		printf("RA end:");
		scanf("%f",&raend);
		printf("DEC:");
		scanf("%f",&dec);
// 		printf("time:");
// 		scanf("%s",&ut);
	   }
	} 
*/	

// // 	if(argc <4) {
// 		if((params = fopen("UVS_skysim_initparams.txt", "r")) == NULL) {
// 			printf("UVS_skysim_initparams.txt file not found\n");
// 			exit(0);
// 		}
// 		while((fgets(fileline,sizeof(fileline),params))!=NULL) {
// 			sscanf(fileline,"%s%s%s%s",key,key1,key2);
// 			if(strcmp(key,"RA_START")==0) {
// 				rastart=atof(key2);
// 			}
// 			if(strcmp(key,"RA_END")==0) {
// 				raend=atof(key2);
// 			}
// 			if(strcmp(key,"DEC_START")==0) {
// 				deccent=atof(key2);
// 			}
// 		}
// // 	}








	if(noofframes==0) {
		noofframes=(raend-rastart)/raincr;   // ===========================24 hrs ??
	}
	
	fout = fopen("telemetry.txt", "w");    
	fprintf(fout,"#This is TAUVEX Telemtry file for ");
	fprintf(fout,"RA start = %.4f, RAend = %.4f, ",rastart,raend);
	fprintf(fout,"DEC = %.4f\n",deccent);


//time
// 	if(year 		//exit if i/p is wrong

// 		uthr=strncpy(
// 		utmin=strncpy(

// 	obt = (uthr+utmin/60)*60*60*1000/4;
// 	obt = obtoffset; 


//MDP angle;lsb = 1/157°; Range: ±91; +91°=28574,0°=14287, -91°=0
	mdpang = (91+deccent)*14287/91; //14287/91 =157 . This is steps to move ??


// SP angle
	spangle =20;
// 			if((uthr-5) >=12) {
// 		
// 				if(utmin>=30) spangle = (uthr-5 -12) *15;
// 				if(utmin < 30) spangle = (uthr-5- 13) *15;
// 				if (spangle < 0) spangle += 360;
// 			}
// 			else {
// 				if(utmin>=30) spangle = (uthr -5 + 12) *15;	
// 				if(utmin < 30) spangle = (uthr -5 + 11) *15;	
// 			}
// 			if((utmin >=23)&&(utmin < 30)) {
// 				spangle = spangle + (15*10*((utmin-22)*60+(utsec-utsec%5)))/(7*60);
// 			}



if(utmin ==30) spangle = 0;




// print telemetry file
// 		fprintf(fout,"#Telemtry file for frame # %d\n\n",frame);
		fprintf(fout,"$frame %lu\n\n",frame);
// 		fprintf(fout,"#RA = %f, DEC = %.4f, frame=%d\n\n\n",ra,dec,frame);
		fprintf(fout," #bytes 0-2 , Telemtry SYNC \n TELE_SYNC = %s \n\n",sync);
		fprintf(fout," #byte 3 \n DYNAMIC_BLOCK_NO = %d \t\t\t // Range: 1-7\n\n",dynblkno);
		fprintf(fout," #byte 4 \n SPARE_4 = %d\n\n",spare);
		fprintf(fout," #Static Telemetry (bytes 5-16) \n\n");

//sysmode

/*
System mode Bits# 0-3  1-A, 1 INIT mode
                      0x0D, 2 STBY mode
                      0x0F  3 IMAGE mode
                            4 CLBR mode
                            5 BIT mode
                            6 OUTGAS mode
                            7 PRE-SHUT mode
                            9 PRE-CLBR submode
                            A POST-CLBR (sub mode)
                            D PRE-IMAGE (sub-mode)
                            F POST-IMAGE (submode)
1553 status Bit #4          0/1  0 Communication OK
                            1 No updates from GSAT for the last 768 mSec
Spare       Bits#5-7    0
*/




//system modes, thrusters, 
//if thrwarn=1,thruster will be fired in next 15 min

		thrwarn=0;
		for(i=0;i<2;i++) {
			if(((uthr - thrfire_uthr[i]) <= 0) && ((uthr - thrfire_uthr[i]) >= -1)) {
				if((((uthr - thrfire_uthr[i]) == 0) &&((utmin - thrfire_utmin[i]) >= -15)) || (((uthr - thrfire_uthr[i]) == -1)&&((60 -utmin+thrfire_utmin[i]) <= 15))) {
					thrwarn=1;
					break;
				}
			}
		}

		sys2arr[0]=hkmod,sys2arr[1]=mdpflag,sys2arr[2]=thrflag,sys2arr[3]=thrwarn,sys2arr[4]=shutwarn,sys2arr[5]=dhmtx,sys2arr[6]=dhmb;

		num_sysmod2 = (sizeof(sys2len)) /(sizeof(sys2len[0]));
		sysmod2=int2bin2int(sys2arr, sys2len, num_sysmod2, 7);

		fprintf(fout," #byte 5 \n SYSTEM_MODE = %d\n\n",sysmod1);
		fprintf(fout," #byte 6 \n SYSTEM_MODE2 = %d \t\t\t //HKI mode \n\n",sysmod2);
		fprintf(fout," #byte 7 \n SYSTEM_STATUS2 = %d  \n \n",sys_status);
		fprintf(fout," #Telescopes status \n");
		fprintf(fout," #byte 8 \n TELESCOPE_STATUS = %d  \t\t // 3 telescopes active \n\n",tel_status);

//filters state
		fprintf(fout," #Filters status\n #byte 9\n");
		fprintf(fout,"  # T1 filter state=0 (2,bit) 0=BBF,1=SF1,2=SF2,3=Close \n");
		fprintf(fout,"  # T2 filter state=1 (2,bit) 0=Close,1=SF1,2=NBF3,3=SF3\n");
		fprintf(fout,"  # T3 filter state=2 (2,bit) 0=BBF,1=Close,2=SF2,3=SF3\n");
		fprintf(fout," #Spare =0 (2,bit) \n");

		T1flt=0,T2flt=1,T3flt=2;  //get these from obs parameters
		lenarr[0]=2;lenarr[1]=2,lenarr[2]=2;
		valarr[0]=T1flt,valarr[1]=T2flt,valarr[2]=T3flt;
		num = (sizeof(lenarr)) /(sizeof(lenarr[0]));
		for(i=0,binlen=0; i<num; i++) {
			binlen += lenarr[i];
		}
		flt_state= int2bin2int(valarr, lenarr, num, 6); //6 -> binlen
	// 	if(frame ==0) printf("in main:fltstat=%d, ",flt_state);
		fprintf(fout," FILTER_STATUS =%d \t\t\t ",flt_state); 
		fprintf(fout,"// %s %s %s \n\n",flt[T1flt],flt[T2flt],flt[T3flt]);


		fprintf(fout," # Thermal modes status\n");
		fprintf(fout," #byte 10 \n THERMAL_STATUS =%d \t\t\t //Everything normal \n\n",therm_stat); 
		fprintf(fout," #byte 11-12 \n MDP_ANGLE =%d \t\t\t //lsb = 1/157°; Range: ±91; +91°=28574,0°=14287, -91°=0\n\n",mdpang); 
		fprintf(fout," # System tests results\n");
		fprintf(fout," #byte 13 \n SYSTEM_TEST =%d \t\t\t //All pass \n\n",sys_test); 
		fprintf(fout," #byte 14 \n BUS_VOLTAGE = %d\n\n", busvolt);
		fprintf(fout," #bytes 15-16 \t\t\t\t // Range =0 \n SPARE_15  = %d\n\n", spare);
	
		fprintf(fout," #byte 17-66 \n # Dynamic telemetry block %d (50 bytes)\n DYNAMIC_BLOCK = %s \n\n", dynblkno, dynblk);
	
		fprintf(fout," #byte 67-68 \n # SUN_ANGLE \t\t\t\t//Range:0-3600\n SUN_ANGLE = %.0f\n\n", sun_angle);
		fprintf(fout," #byte 69-70 \n # MOON_ANGLE \t\t\t\t//Range:0-3600\n MOON_ANGLE = %.0f\n\n", moon_angle);
		fprintf(fout," #byte 71-74 \n # Pointing center RA \t\t\t//Range:0-360000\n FOV_RA  = %.0f\n\n", racent*1000);
		fprintf(fout," #byte 75-78 \n # Pointing center DEC \t\t\t//Range:0-180000\n FOV_DEC  = %.0f\n\n", (90 + deccent)*1000);
	
		fprintf(fout," #bytes 79-82  \t\t\t\t //\n SPACECRAFT_X  = %lu\n\n", xcoord);
		fprintf(fout," #bytes 83-86  \t\t\t\t //\n SPACECRAFT_Y  = %lu\n\n", ycoord);
		fprintf(fout," #bytes 87-90  \t\t\t\t //\n SPACECRAFT_Z  = %lu\n\n", zcoord);
	
		fprintf(fout," #UT/GRT(9 bytes)\n");
		fprintf(fout," #bytes 91-92 \t\t\t\t // UT year ,Unsigned\n YEAR  = %d\n\n", utyear);
		fprintf(fout," #bytes 93 \t\t\t\t // UT month, 1-12 Month,Unsigned\n MONTH  = %d\n\n", utmnth);
		fprintf(fout," #bytes 94 \t\t\t\t // UT day,  1-31 Day of month,Unsigned\n DAY  = %d\n\n", utday);
		fprintf(fout," #bytes 95 \t\t\t\t // UT hrs.,  0-23  Hours,Unsigned\n HOURS  = %d\n\n", uthr);
		fprintf(fout," #bytes 96 \t\t\t\t // UT min.,  0-59 minutes,Unsigned\n MINUTES  = %d\n\n", utmin);
		fprintf(fout," #bytes 97 \t\t\t\t // UT sec.,  0-59  seconds,Unsigned\n SECONDS  = %d\n\n", utsec);
	
		fprintf(fout," #bytes 98-99 \t\t\t\t // UT milli sec.  0-999 Milliseconds,Unsigned\n MILLISECONDS  = %d\n\n", utms);
		fprintf(fout," #bytes 100-103 \t\t\t // GSAT4 Onboard time. lsb = 8ms\n OBT  = %lu\n\n", obt);
		fprintf(fout," #bytes 104 \t\t\t\t // Unsigned, 1=OBT Reset; once in 198 days\n OBT_RESET_FLAG  = %d\n\n", obt_resetflag);
		fprintf(fout," #bytes 105-106 \t\t\t // Range: 0-3600, Unsigned, lsb = 0.1 deg \n SOLAR_PANEL  = %d\n\n", spangle);
		fprintf(fout," #bytes 107-127 \t\t\t //  21 bytes (Range =0) \n SPARE_107  = %d\n\n", spare);
		fputs("\n#=========================================================\n",fout);
//     	}  //frame  

//  return 0;
 }

//Moon angle,  OBT reset flag, Observation mode,lamp state
