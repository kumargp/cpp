
/*--------------------------------------------------------------------------*\
 * plplotfns-polar.c file
 * .
\*--------------------------------------------------------------------------*/
#define PI 3.1415926535897932384
#define NPTS  1000  //not implimented
#define ra2deg  (180.0/PI)
#define deg2ra (PI / 180.0)


//calculate polar projection of ra, dec.
//RA's are to be passed in decimal hrs,DEC are to be passed in degrees
double polar_rd2xy (PLFLT ra, PLFLT dec, PLFLT *xp, PLFLT *yp) {
     
     PLFLT x1,y1;
     ra=15* ra ;//to degrees from hrs min sec
     *xp=0.,*yp=0.;
     ra =ra *deg2ra;   //or lambda
     dec=dec *deg2ra;     //or phi
     x1 = cos(dec)*sin(ra);
     y1 = cos(dec)*cos(ra);
     *xp = x1*ra2deg*2/5; //2/5 is scaling
     *yp = y1*ra2deg*4/5; //4/5 is scaling

}



//fill area, RA's are to be passed in decimal hrs , not in degrees, but DEC in degrees
void  fillarea_polar_north(PLFLT ra1,PLFLT ra2, PLFLT dec1,PLFLT dec2, PLINT color) {
           int i=0,f,flag=0;
	   float j,k,temp=0,ra1n,ra2n,step;
           PLFLT xp,yp;
	   PLFLT x[5],y[5];
	   step=0.05; //minimum range of ra and dec for filling(in hrs)
//dec should be 60 to 90:
	   if((dec1 < 90)&&(dec2 > 60)) {
		if((dec1 < 60) && (dec2 >60)) dec1 =60; 
		if((dec1 < 90) && (dec2 >90)) dec2 =90;
		ra1n=ra1, ra2n=ra2;

		if(ra2<ra1) { temp = ra2, ra2n=24,flag=1;}  
		for(f=0;f<=flag;f++) {
			if(f==1) {ra1n=0,ra2n=temp;}
			for(j=ra1n;j<ra2n;j+=step) {
				for(k=dec1;k<dec2;k+=4*step) {
					i=0;	      	
					polar_rd2xy(j, k, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					polar_rd2xy(j+step, k, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					polar_rd2xy(j+step, k+4*step, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					polar_rd2xy(j, k+4*step, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					plcol0(color);
					plfill (4, x, y);
				}
			}
		}//f loop
	   }//dec  
        }


//label:RA's are to be passed in decimal hh.hhhh , not in degrees, but DEC in degrees
void   label_rd_polar_north(PLFLT ra1,PLFLT dec1, char *name, PLINT colr) {
	PLFLT xp,yp,just=0;
	   if(!((dec1 < 60) || (dec1 >90))) {
		plcol0(colr);
		polar_rd2xy(ra1+0.05, dec1-1., &xp, &yp);
		plptex (xp , yp , 0. , 0. , just , name);
	      }
	}



//fill area, RA's are to be passed in decimal hh.hhhh , not in degrees, but DEC in degrees
void  fillarea_polar_south(PLFLT ra1,PLFLT ra2, PLFLT dec1,PLFLT dec2, PLINT color) {
           int i=0,f,flag=0;
	   float j,k,ra1s,ra2s,temp=0,step;
           PLFLT xp,yp;
	   PLFLT x[5],y[5];
	   step =0.05; //minimum range of ra and dec for filling(in hrs)
// dec should be -60 to -90 
	   if((dec1 > -90)&&(dec2 < -60)) {
		if((dec1 > -60) && (dec2 < -60)) dec1 =-60; 
		if((dec1 > -90) && (dec2 < -90)) dec2 =-90;

		ra1s=ra1, ra2s=ra2;
		if(ra2<ra1) { temp = ra2, ra2s=24,flag=1;} 
		for(f=0;f<=flag;f++) {
			if(f==1) {ra1s=0,ra2s=temp;}

			for(j=ra1s;j<ra2s;j+=step) {
				for(k=dec1;k>dec2;k-=4*step) {
					i=0;	      	
					polar_rd2xy(j, k, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					polar_rd2xy(j, k-4*step, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					polar_rd2xy(j+step, k-4*step, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					polar_rd2xy(j+step, k, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					plcol0(color);
					plfill (4, x, y);
				}
			}
		}
           }
        }
//label. RA in decimal hrs, DEC in degrees
void   label_rd_polar_south(PLFLT ra1,PLFLT dec1, char *name, PLINT colr) {
	PLFLT xp,yp,just;
	if(!((dec1 > -60) || (dec1 <-90))) {
		plcol0(colr);
		if((ra1 > 5.5) && (ra1<6.5)) { just =0.5;  } //justify 0.5 is half
		else just=0.0;
	
		polar_rd2xy(ra1+0.05, dec1+1.0, &xp, &yp);
		plptex (xp , yp , 0. , 0. , just , name);
// reset color ??
	      }
	}


 
//grid circles-north - 5 degrees gap
void drawgrid_circ_north(PLINT gridcolr) {
   int i, k;
   PLINT colr;
   float j;
   PLFLT xp,yp;
   PLFLT x[2401],y[2401];
   for(i=60;i<=90;i=i+5) {
	if(i % 15==0) {colr = gridcolr;}
	else {colr = 7;}
	for(j=0,k=0;j<=24;j+=0.01,k++) {
		polar_rd2xy(j, i, &xp, &yp);
		x[k]=xp;
		y[k]=yp;
        }
        plcol0(colr);
        plline(k, x, y);
   }
 }

//draw spokes north
void drawgrid_spoke_north(PLINT gridcolr) {
   int k;
   float i,j;
   PLFLT xp,yp;
   PLFLT x[1501],y[1501];
   for(i=0;i<=24;i=i+2) {
	for(j=60,k=0;j<=90;j+=0.04,k++) {
		polar_rd2xy(i, j, &xp, &yp);
		x[k]=xp;
		y[k]=yp;
        }
        plcol0(gridcolr);
        plline(k, x, y);
   }
 }


//grid label. 
void   label_grid_north(PLINT colr) {
	PLFLT xp,yp,x1,y1,just;
	int i;
	char text[5],dectext[5]; 
	plcol0(colr);
//RA
   	for(i=0;i<24;i=i+2) {
		sprintf(text,"%d",i);
		polar_rd2xy(i, 58.5, &xp, &yp);
		if(xp < 0) {plptex (xp,yp,0,0,1,text);}
		else       {plptex (xp,yp,0,0,0,text);}
		}
	polar_rd2xy(1.0, 56, &xp, &yp);	
	x1=xp, y1=yp;
	polar_rd2xy(2, 56, &xp, &yp);	
	plptex (x1,y1,xp-x1,yp-y1,0,"RA->");
//DEC	

  	for(i=62;i<=92;i+=15) {
		strcpy(dectext,"+");
		sprintf(text,"%d",i-2);
		strcat(dectext,text);
		polar_rd2xy(0, i, &xp, &yp);
		plptex (xp,yp,0,0,-0.1,dectext);
		if(i<90) {
			polar_rd2xy(12, i, &xp, &yp);
			plptex (xp,yp,0,0,-0.1,dectext);
		}
		polar_rd2xy(23.8, 63, &xp, &yp);	
		x1=xp, y1=yp;
		polar_rd2xy(23.8, 65, &xp, &yp);	
		plptex (x1,y1,xp-x1,yp-y1,0,"DEC->");
    }
		plcol0(9);
		plmtex("rv", -6, 0.22, 0.5, "Colors: SL");
		plmtex("rv", -6, 0.20, 0.5, "======");
		plmtex("rv", -6, 0.17, 0.5, "blue: <100");
		plmtex("rv", -8, 0.14, 0.5, "cyan: 100-1000");
		plmtex("rv", -9, 0.11, 0.5, "yellow: 1000-5000");
		plmtex("rv", -8.2, 0.08, 0.5, "red: Solar Panel");


/*
		plmtex("rv", -6.5, 0.17, 0.7, "Colors:");
		plmtex("rv", -7.5, 0.14, 0.7, "blue: <100");
		plmtex("rv", -9, 0.11, 0.7, "cyan: 100-1000");
		plmtex("rv", -10, 0.08, 0.7, "yellow: 1000-5000");*/
  }

void drawgrid_north(PLINT gridcolr) {
   drawgrid_circ_north(gridcolr);
   drawgrid_spoke_north(gridcolr);
   label_grid_north(gridcolr);

   }




//grid circles -south
void drawgrid_circ_south(PLINT gridcolr) {
   int i,k=0;
   PLINT colr=0;
   float j;
   PLFLT xp,yp;
   PLFLT x[2401],y[2401];
   for(i=-60;i>=-90;i-=5) {
	if((i%15)==0) {colr = gridcolr;}
	else {colr = 7;}
        for(j=0,k=0;j<=24;j+=0.01,k++) {
		polar_rd2xy(j, i, &xp, &yp);
		x[k]=xp;
		y[k]=yp;
        }
        plcol0(colr);
        plline(k, x, y);
    }
 }


//draw spokes south

void drawgrid_spoke_south(PLINT gridcolr) {
   int k;
   float i,j;
   PLFLT xp,yp;
   PLFLT x[1501],y[1501];
   for(i=0;i<=24;i=i+2) {
	for(j=-60,k=0;j>=-90;j-=0.04,k++) {
		polar_rd2xy(i, j, &xp, &yp);
		x[k]=xp;
		y[k]=yp;
        }
        plcol0(gridcolr);
        plline(k, x, y);
   }
 }


//grid label south
void   label_grid_south(PLINT colr) {
	PLFLT xp,yp,x1,y1,just;
	int i;
	char text[5],dectext[5]; 
	plcol0(colr);
//RA
   	for(i=0;i<24;i=i+2) {
		sprintf(text,"%d",i);
		polar_rd2xy(i, -58.5, &xp, &yp);
		if(xp < 0) {plptex (xp,yp,0,0,1,text);}
		else       {plptex (xp,yp,0,0,0,text);}
		}
	polar_rd2xy(1.0, -56, &xp, &yp);	
	x1=xp, y1=yp;
	polar_rd2xy(2, -56, &xp, &yp);	
	plptex (x1,y1,xp-x1,yp-y1,0,"RA->");
//DEC	

  	for(i=-62;i>=-92;i-=15) {
		sprintf(text,"%d",i+2);
		polar_rd2xy(0, i, &xp, &yp);
		plptex (xp,yp,0,0,-0.1,text);
		if(i>-90) {
			polar_rd2xy(12, i, &xp, &yp);
			plptex (xp,yp,0,0,-0.1,text);
		}
		polar_rd2xy(23.8, -63, &xp, &yp);	
		x1=xp, y1=yp;
		polar_rd2xy(23.8, -65, &xp, &yp);	
		plptex (x1,y1,xp-x1,yp-y1,0,"DEC->");
        }
		plcol0(9);
		plmtex("rv", -6, 0.22, 0.5, "Colors: SL");
		plmtex("rv", -6, 0.20, 0.5, "======");
		plmtex("rv", -6, 0.17, 0.5, "blue: <100");
		plmtex("rv", -8, 0.14, 0.5, "cyan: 100-1000");
		plmtex("rv", -9, 0.11, 0.5, "yellow: 1000-5000");
		plmtex("rv", -8.2, 0.08, 0.5, "red: Solar Panel");

// 		plmtex("rv", -6.5, 0.17, 0.7, "Colors:");
// 		plmtex("rv", -7.5, 0.14, 0.7, "blue: <100");
// 		plmtex("rv", -9, 0.11, 0.7, "cyan: 100-1000");
// 		plmtex("rv", -10, 0.08, 0.7, "yellow: 1000-5000");

  }


void drawgrid_south(PLINT gridcolr) {
   drawgrid_circ_south(gridcolr);
   drawgrid_spoke_south(gridcolr);
   label_grid_south(gridcolr);
   }

//for plenv set for north and south. RA in decimal hrs, DEC in degrees
void xylimits_polar(float *xmin, float *xmax, float *ymin, float *ymax){
	PLFLT xp,yp;
    	polar_rd2xy(18., 60., &xp, &yp);
    	*xmin = xp - 4.5;
//     	*xmin = xp - 20;  //for small fig
    	polar_rd2xy(6., 60., &xp, &yp);
	*xmax = xp +4.5;
// 	*xmax = xp +20; //for small fig
    	polar_rd2xy(0., 60., &xp, &yp);
    	*ymin = yp+2;
//     	*ymin = yp+24; //for small fig
    	polar_rd2xy(12., 60., &xp, &yp);
    	*ymax =yp -2; 
//     	*ymax =yp -24;  //for small fig
     }	


