
/*--------------------------------------------------------------------------*\
 * plplotfns.c file
 * .
\*--------------------------------------------------------------------------*/

#define PI 3.1415926535897932384
#define NPTS  1000  //not implimented
#define ra2deg  (180.0/PI)
#define deg2rad (PI / 180.0)


//calculate aitoff projection of ra, dec.
//RA in hrs and DEC to be passed in degrees
double ait_rd2xy (PLFLT ra, PLFLT dec, PLFLT *xp, PLFLT *yp) {
     
	ra=(ra > 12) ? (ra =ra-24) : ra;

     PLFLT alpha2,delta,r2,f,cdec,denom,x1,y1;
     *xp=0.,*yp=0.;
     ra=15* ra ; //to degrees from hrs min sec
     alpha2=ra/(2.*ra2deg);  
     delta=dec/ra2deg;   
     r2=sqrt(2.);
     f=2.*r2/PI;
     cdec= cos(delta);
     denom =sqrt(1. + cdec*cos(alpha2));
     x1 = cdec*sin(alpha2)*2.*r2/denom;
     y1 = sin(delta)*r2/denom;
     x1 = x1*ra2deg/f;
     y1 = y1*ra2deg/f;
     *xp = x1/15;
     *yp = y1/15;
}



// RA's are to be passed in decimal hrs, DEC in degrees
void  fillpoly(PLFLT ra1,PLFLT ra2, PLFLT dec1,PLFLT dec2, PLINT color) {
        int i=0;
	float j,k,step;
        PLFLT xp,yp;
	PLFLT x[1801],y[1801];
	step =0.03;
//dec +ve

	if((dec1 < 90)&&(dec2 > -90)) {
		if((dec1 <=0) && (dec2 >0)) dec1 =0; 
		if((dec1 < 90) && (dec2 >90)) dec2 =90;
		if((dec1>=0) && (dec2 >0)) {
			for(j=ra1;j<=(ra2-step);j+=step) {
				for(k=dec1;k<dec2;k+=4*step) {
					i=0;	      	
					ait_rd2xy(j, k, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					ait_rd2xy(j+step, k, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					ait_rd2xy(j+step, k+4*step, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					ait_rd2xy(j, k+4*step, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					plcol0(color);
					plfill (4, x, y);
				}
			}

		}
	}
//If dec is negative 
	if((dec1 > -90)&&(dec2 < 90)) {
		if((dec1 > 0) && (dec2 < 0)) dec1 =0; 
		if((dec1 > -90) && (dec2 < -90)) dec2 =-90;
		if((dec1<=0) && (dec2 <0)) {
			for(j=ra1;j<=(ra2-step);j+=step) {
				for(k=dec1;k>dec2;k-=4*step) {
					i=0;	      	
					ait_rd2xy(j, k, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					ait_rd2xy(j, k-4*step, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					ait_rd2xy(j+step, k-4*step, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					ait_rd2xy(j+step, k, &xp, &yp);
					x[i]=xp, y[i]=yp, i++;
					plcol0(color);
					plfill (4, x, y);
				}
			}
		}
	}
      }


//fill area, RA's are to be passed in decimal hh.hhhh , not in degrees, but DEC in degrees
void  fillarea(PLFLT ra1,PLFLT ra2, PLFLT dec1,PLFLT dec2, PLINT color) {

	ra1=(ra1 > 12) ? (ra1 =ra1-24) : ra1;
	ra2=(ra2 > 12) ? (ra2 =ra2-24) : ra2;
	
	if(ra1 > ra2)  {
		fillpoly(ra1,12,dec1,dec2,color);
		ra1 = -12;
		}
	
	fillpoly(ra1,ra2,dec1,dec2,color);
  }



//label, RA's are to be passed in decimal hh.hhhh , not in degrees, but DEC in degrees
void   label_rd(PLFLT ra1,PLFLT dec1, char *name, PLINT colr) {
	 PLFLT xp,yp,just=0.5;
	 plcol0(colr);
	 dec1 = (dec1 < 0) ? (dec1 +3.5) : (dec1 -1.5);
         ait_rd2xy(ra1+1.0, dec1, &xp, &yp);
         plptex (xp , yp , 0. , 0. , just , name);
	}



//grid horizontals, gridcolr is integer
void drawgrid_hor(PLINT gridcolr) {
   int k;
   float i, j;
   PLFLT xp,yp;
   PLFLT x[2401],y[2401];
   for(i=0;i<=180;i=i+15) {
	for(j=0,k=0;j<=24;j+=0.01,k++) {
		ait_rd2xy(j-12., i-90., &xp, &yp);
		x[k]=xp;
		y[k]=yp;
        }
        plcol0(gridcolr);
        plline(k, x, y);
   }
 }

//grid verticals, gridcolr is integer
void drawgrid_vert(PLINT gridcolr) {
   int k=0;
   float i,j;
   PLFLT xp,yp;
   PLFLT x[3601],y[3601];
   for(j=0;j<=24;j=j+2) {
        for(i=0,k=0;i<=180;i+=0.05,k++) {
		ait_rd2xy(j-12., i-90., &xp, &yp);
		x[k]=xp;
		y[k]=yp;
        }
        plcol0(gridcolr);

// plscol0(0, 0, 0, 0);
        plline(k, x, y);
    }
 }

//grid label. 
void   label_grid_ait(PLINT colr) {
	PLFLT xp,yp,x1,y1,just;
	int i;
	char text[5],dectext[5]; 
	plcol0(colr);
//RA
   	for(i=2;i<24;i=i+2) {
		sprintf(text,"%d",i);
		ait_rd2xy(i, -4, &xp, &yp);
		plptex (xp,yp,0,0,-0.2,text);
		}
	ait_rd2xy(12.001, -4, &xp, &yp);//left 180
	plptex (xp,yp,0,0,0.5,"12");//left 180

	ait_rd2xy(1.2, 5, &xp, &yp);	
	x1=xp, y1=yp;
	plptex (x1,y1,0,0,0,"RA->");
//DEC	

  	for(i=-93;i<0;i+=15) {
		sprintf(text,"%d",i+3);
		ait_rd2xy(0, i, &xp, &yp);
		plptex (xp,yp,0,0,1.0,text);
	}

 	for(i=18;i<=93;i+=15) {
		sprintf(text,"%d",i-3);
		ait_rd2xy(0, i, &xp, &yp);
		plptex (xp,yp,0,0,1.0,text);
	}

		ait_rd2xy(1, -80, &xp, &yp);	
		x1=xp, y1=yp;
		ait_rd2xy(1, -75, &xp, &yp);	
		plptex (x1+0.5,y1+0.5,xp-x1-0.1,yp-y1,-0.4,"DEC->");
		plcol0(9);
		plmtex("rv", -6, 0.22, 0.5, "Colors: SL");
		plmtex("rv", -6, 0.20, 0.5, "======");
		plmtex("rv", -6, 0.17, 0.5, "blue: <100");
		plmtex("rv", -8, 0.14, 0.5, "cyan: 100-1000");
		plmtex("rv", -9, 0.11, 0.5, "yellow: 1000-5000");
		plmtex("rv", -8.2, 0.08, 0.5, "red: Solar Panel");

    }

//plpoin (n , x , y , code );. Plots a character at the speciﬁed points

//grid 
void drawgrid_ait(PLINT gridcolr) {
     drawgrid_hor(gridcolr);
     drawgrid_vert(gridcolr);
     label_grid_ait(gridcolr);
     }




//for plenv set
void xylimits(float *xmin, float *xmax, float *ymin, float *ymax){
	PLFLT xp,yp;
    	ait_rd2xy(-12., 0., &xp, &yp);
    	*xmin = xp-1;
    	ait_rd2xy(12., 0., &xp, &yp);
	*xmax = xp+1;
    	ait_rd2xy(0., -90., &xp, &yp);
    	*ymin = yp-1;
    	ait_rd2xy(0., 90., &xp, &yp);
    	*ymax =yp+1; 
     }	



char *get_filename(char *flt, char *days, char *days2, int pl, char *filename, char *toplbl) {

	strcpy(filename, flt);
	strcat(filename,":");
        strcat(filename, days);
	strcat(filename,"-");
        strcat(filename, days2);
	strcpy(toplbl,filename);
// 	if(pl==0) {strcat(filename,"_ait");}
	if(pl==1) {strcat(filename,"_N");}
	if(pl==2) {strcat(filename,"_S");}
    }

