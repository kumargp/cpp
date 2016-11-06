//To be renamed as .c file to do make

 
void fn_poweron(float img) {
	int day=1;
if ((img>=24.0) || (img<=12.0)) { day = 2;}
	printf("\n%02dT%04.1f\t\t%s\t%s\n", day, img, "TVX_12", "A_POWER ON (INIT, STBY mode)");	
	printf("\t\t//(HK mode)\n");
	}

void fn_filters(float img, char flt1[], char flt2[], char flt3[]) {

	int day = 1;
if ((img>=24.0) || (img<=12.0)) { day = 2;}
 	printf("%02dT%04.1f\t\t%s\t%s,\t%s, %s, %s \n", day, img, "TVX_24", "TVX_24 Filters" ,flt1, flt2, flt3);
	}

void fn_mdpmove(float img, char mdpdir[], int mdpsteps) {
	int day = 1;
if ((img>=24.0) || (img<=12.0)) { day = 2;}
	printf("%02dT%04.1f\t\t%s\t%s\t%s\n", day,img-0.4, "TVX_25.1", mdpdir, "MDP movement direction");

if ((img>=24.0) || (img<=12.0)) { day = 2;}
	printf("%02dT%04.1f\t\t%s\t%d\t%s\n", day, img-0.3, "TVX_25.2", mdpsteps,"MDP No. of steps"); 

if ((img>=24.0) || (img<=12.0)) { day = 2;}
	printf("%02dT%04.1f\t\t%s\t%s\n", day, img-0.2, "TVX_25.3", "Activate MDP movement");
//	printf("WAIT MDP-STILL wait STBY mode\n");
	}

void fn_startimg(float img,char filter_state[]) {

	int day = 1;
if ((img>=24.0) || (img<=12.0)) { day = 2;}
//	if(filter_state
	printf("%02dT%.1f\t\t%s\t%s\n", day, img, "TVX_24","Start Imaging (do not move filters)");
	}


void fn_stopimg(float stp, char hvps[], char flt_state[]) {

	int day = 1;
//    if(flt_state eq "hvpson") {
	if((stp>=24.0)||(stp<12.0)) { day = 2; }

 	printf("%02dT%04.1f\t\t%s\t%s\n",day,stp, "TVX_24", "Stop Imaging (Leave HVPS ON, Filters Closed )");
//	}
//    else if(flt_state eq "hvpsoff") {
//	printf("%02dT=%.1f  %s  %s \n",day, stp, "TVX--", "Stop Imaging (HVPS off, filters closed)");
//   else { 
	
// 	   }//else
// 	 } //elseif
     }//end fn

void fn_eclipse(float eclst, float eclend, char flt_change[]) {

	int day = 1;
if (((eclst-0.2)>=24.0) || ((eclst-0.2)<=12.0)) day = 2;
// 	printf("%02dT%04.1f\t\t%s\t%s\n",day, eclst-0.2, "TVX_24", "Stop Imaging (HVPS off)");
if (((eclst-0.01)>=24.0) || ((eclst-0.01)<=12.0)) day = 2;
	printf("%02dT%04.1f\t\t%s\t%s\n",day, eclst-0.01, "TVX_02", "Shut down");
	printf("\t //(HK mode, Eclipse for %.1f hours)\n",eclend-eclst);
	
// if (((eclend)>=24.0) || ((eclend)<=12.0)) day = 2;
// 	printf("%02dT%04.1f\t\t%s\t%s \n",day, eclend, "TVX_12", "Relay: A_POWER_ON wait STBY-mode");

}
 
void fn_thrust_fire(float thrst, float thrdur, char flt1[],char flt2[],char flt3[]) {
	int day = 1;
	if(((thrst-0.2) >=24.0)||((thrst-0.2)<12.0)) {
		day = 2;
		}

//  	printf("%02dT%04.1f\t\t%s\t%s\n",day, thrst,"TVX_24","Stop Imaging (Leave HVPS ON, Filters Closed )");
	printf("\t //(HK mode, Thruster Firing for %.1f hours)\n",thrdur);

// 	if(((thrst + thrdur)>=24.0)||((thrst + thrdur)<12.0)) day = 2;
// 	printf("%02dT%04.1f\t\t%s\t%s %s, %s, %s\n",day, (thrst + thrdur+0.05),"TVX_24","Filters:",flt1, flt2, flt3);
}

void fn_shutdown(float stp) {
	int shut;
	int day = 1;	
if (((stp)>=24.0) || ((stp)<=12.0)) day = 2;
	printf("%02dT%04.1f\t\t%s\t%s\n",day, stp+0.2, "TVX_02", "Shut Down");	
	shut=1;
}


