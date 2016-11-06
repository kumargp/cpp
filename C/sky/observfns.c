


void format_time(int *year,int *mnth, int *day,int *hr,int *min) {

	int i;
		if(*min>=60) {
			i=*min/60;
			*min=*min - 60 * i;
			*hr +=i;
		}
		if(*hr==24){
			*hr=0;
			if(*mnth==2) {
				if(*day ==29){
					*day=1;
					*mnth+=1;
				}
				else if(*day ==28){
					if((*year)%4 == 0) *day=29;
					else {
						*day=1;
						*mnth+=1;
					}
				}
			}
			if((*day==31) || ((*day==30) && ((*mnth==4)||(*mnth==6)||(*mnth==9)||(*mnth==11)))) {
				*day=1;
				*mnth+=1;
			}
			else {
				*day+=1;
			}
		}
		if(*mnth>12) {
			*mnth=1;
			*year +=1;
		}
} //format time

void  time2ut( int year,int mnth,int day,int hr,int min, int sec,int ms,int *utyear,int *utmnth,int *utday,int *uthr,int *utmin,int *utsec,int *utms)  {

 	*utms=ms,*utsec=sec,*utmin =30+min,*uthr=5+hr,*utday=day,*utmnth=mnth,*utyear=year;
// 		if(*utms>=1000) {
// 			*utms=*utms-1000;
// 			*sec +=1;
// 		}
// 		if(*sec>=60) {
// 			*sec=*sec-60;
// 			*min +=1;
// 		}	
		format_time(utyear,utmnth, utday,uthr,utmin);

} //time2ut

void  add_frametime(int *year,int *mnth, int *day,int *hr,int *min,int *sec,int *ms) {
		*ms+=128;
		if(*ms>=1000) {
			*ms=*ms-1000;
			*sec +=1;
		}
		if(*sec>=60) {
			*sec=*sec-60;
			*min +=1;
		}	
		format_time(year,mnth, day,hr,min);

} //add_frametime




void  ra_at12_date(int year,int mnth, int day,double *ra) {
//get exact RA based on ecliptic position on elliptic orbit
// location of S/C to be updated, now 82.5E

  double sun_ra=0,sun_dec=0;
  int utyear=0, utmnth=0, utday=0,uthr=0,utmin=0,utsec=0,utms=0;

  time2ut(year,mnth,day,12,0,0,0,&utyear, &utmnth, &utday,&uthr,&utmin,&utsec,&utms);
  SUN_RA_DEC(uthr, utday, utmnth, utyear, &sun_ra, &sun_dec); // pass uthr=12+5.5 ?

  *ra = sun_ra; 
  if(*ra >=360) *ra = *ra -360;
  if(*ra < 0) *ra = *ra + 360;

} //ra_from_date
// printf("sun::local;year=%d,mnth=%d,day=%d,hr=%d,min=%d,sec==%d\n",year,mnth,day,12,0,0);
// printf("sun::ut utyear =%d,utmnth=%d,utday=%d,uthr=%d,utmin=%d,utsec=%d\n",utyear,utmnth, utday,uthr,utmin,utsec);
// printf("sunra=%.4f\n",sun_ra);
// printf(" ra=%.4f\n", *ra);
	
/*	float daynum =0, daytime=0;
//day
	*ra =0;

	daynum = mnth *30 + day;
	*ra = daynum/365.25 * 360;
*/

/* // from idl
yr = long(yr) & mn = long(mn) &  day = long(day)	;Make sure integral
 L = (mn-14)/12		;In leap years, -1 for Jan, Feb, else 0
 julian = day - 32075l + 1461l*(yr+4800l+L)/4 + $
         367l*(mn - 2-L*12)/12 - 3*((yr+4900l+L)/100)/4
 julian = double(julian) + (HR/24.0D) - 0.5D
*/




void  ra_from_time(int year,int mnth, int day, int hr,int min, int sec,double *ra) {
//get exact RA based on ecliptic position on elliptic orbit
// location of S/C to be updated, now 82.5E

  double ra_at12=0, diff_deg=1; //1 deg for 24 hrs
  ra_at12_date(year, mnth, day, &ra_at12);
 *ra = ra_at12 + (hr-12 + min/60 +sec/3600)*15*(1+diff_deg/24); //last () is to add 1 deg diff 
  if(*ra >=360) *ra = *ra -360;
  if(*ra < 0) *ra = *ra + 360;
}//ra_from_time


//ra, dec in degrees,not in hrs
void get_straylight(double ra, double dec,int lday,int lmnth, int lyear,double *strlight){

//find mnth no, and phase
  int n1,n2,n3,day;
  double ra_at12=0, ra_dif; //1 deg for 24 hrs
  ra_at12_date(lyear, lmnth, lday, &ra_at12);
  ra_dif=ra-ra_at12;
  n2=(int)(12+ra_dif/15);

//get sl count
  straylight_radec(n2,dec,lmnth,lday,strlight);

//   *strlight =500; // test
}


 void get_jiiter(int mnth,int day,int hr,int min, int sec,int ms, double *jitter){
//thruster ????????
	if((min >=23)&&(min < 30)) {
		*jitter = 0.00001;	
//		function or model
	}

  }


void get_radec_from_simpar(double *rastart, double  *raend, double *deccent) {
	FILE *sim_param=NULL;
	char key[50],key1[20],key2[20],fileline[200];
	if((sim_param = fopen("UVS_skysim_initparams.txt", "r")) == NULL) {
		printf("UVS_skysim_initparams.txt file not found\n");
		exit(0);
	}
	while((fgets(fileline,sizeof(fileline),sim_param))!=NULL) {
		sscanf(fileline,"%s%s%s%s",key,key1,key2);
		if(strcmp(key,"RA_START")==0) {
			*rastart=atof(key2);
		}
		if(strcmp(key,"RA_END")==0) {
			*raend=atof(key2);
		}
		if(strcmp(key,"DEC_START")==0) {
			*deccent=atof(key2);
		}
	}
	fclose(sim_param);
}


//Edit param file.
  void edit_simpar_file(float rastart,float raend, float deccent,float jitter, double darkcount) {

	FILE *simpar_fp=NULL, *simpar_opfp;
	char key[200],key1[20],key2[20],fileline[200];
	
	if((simpar_fp = fopen("UVS_skysim_initparams.txt", "r")) == NULL) {
		printf("UVS_skysim_initparams.txt file not found\n");
		exit(1);
	}

//   system("mv -f UVS_skysim_initparams.txt UVS_skysim_initparams_ren.txt");

  	if((simpar_opfp = fopen("newparam","w"))==NULL) {
	  printf("Sim Par o/p file not created");
  	  exit(1);
  	} 
  	printf("Editing param file:");
   	while((fgets(fileline,sizeof(fileline),simpar_fp))!=NULL) {
		sscanf(fileline,"%s%s%s",key,key1,key2);

		if(strcmp(key,"RA_START")==0) {
	// 		par3 = rastart[n5]);
			fprintf(simpar_opfp, "RA_START = %.3f /*Begin RA */ \n",rastart);	
		}
		else if (strcmp(key,"RA_END")==0) {
			fprintf(simpar_opfp, "RA_END = %.3f /*End RA */ \n",raend);
		}
	
		else if (strcmp(key,"DEC_START")==0) {
			fprintf(simpar_opfp, "DEC_START = %.3f /*DEC_START */ \n",deccent);
		}
		else if (strcmp(key,"JITTER")==0) {
			fprintf(simpar_opfp, "JITTER = %f /*JITTER */ \n",jitter);
		}
		else if (strcmp(key,"DARKCOUNT")==0) {
			fprintf(simpar_opfp, "DARKCOUNT = %f /*DARKCOUNT */ \n",darkcount);
		}
		else  {
			fputs(fileline,simpar_opfp);
		}
		strcpy(fileline,"");
		strcpy(key,"");
	}
   	fclose(simpar_fp);
   	fclose(simpar_opfp);
	system("mv -f newparam UVS_skysim_initparams.txt");
  	printf(" over\n");
}





// //renaming o/p data file
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

