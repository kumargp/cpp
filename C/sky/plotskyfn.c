
/*--------------------------------------------------------------------------*\

\*--------------------------------------------------------------------------*/

#include "plotskyfn.h"




//calculate aitoff projection of ra, dec.
//RA in hrs and DEC to be passed in degrees
double ait_rd2xy (PLFLT ra, PLFLT dec, PLFLT *xp, PLFLT *yp) {
     
	ra=(ra > 24) ? (ra =ra-24) : ra;

     PLFLT alpha2,delta,r2,f,cdec,denom,x1,y1;
     *xp=0.,*yp=0.;
    //to degrees from hrs min sec

     *xp =ra ;
     *yp =dec;


/*     alpha2=ra/(2.*ra2deg);  
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
      *yp = y1/15;*/
}



// RA's are to be passed in decimal hrs, DEC in degrees
void  fillpoly(PLFLT ra1,PLFLT ra2, PLFLT dec1,PLFLT dec2, PLINT color,PLFLT refdec) {
        int i=0,n=0;
	float j,k,step;
        PLFLT xp,yp;
	PLFLT x[1801],y[1801];
	step =0.03;

	refdec=0;

//dec +ve

	if((dec1 < 90)&&(dec2 > -90)) {
		if((dec1 <=0) && (dec2 >0)) dec1 =0; 
		if((dec1 < 90) && (dec2 >90)) dec2 =90;
		if((dec1>=0) && (dec2 >0)) {


			for(j=ra1;j<=(ra2-step);j+=step) {

				for(k=dec1;k<dec2;k+=4*step) {

					i=0;	      	
					ait_rd2xy(j, k, &xp, &yp);
					x[i]=xp, y[i]=yp-refdec, i++;
					ait_rd2xy(j+step, k, &xp, &yp);
					x[i]=xp, y[i]=yp-refdec, i++;

					ait_rd2xy(j+step, k+4*step, &xp, &yp);
					x[i]=xp, y[i]=yp-refdec, i++;
					ait_rd2xy(j, k+4*step, &xp, &yp);
					x[i]=xp, y[i]=yp-refdec, i++;
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
void  fillarea(PLFLT ra1,PLFLT ra2, PLFLT dec1,PLFLT dec2, PLINT color, PLINT refdec) {

	ra1=(ra1 >= 24) ? (ra1 =ra1-24) : ra1;
	ra2=(ra2 >= 24) ? (ra2 =ra2-24) : ra2;
	
	if(ra1 > ra2)  {
		fillpoly(ra1,24,dec1,dec2,color,refdec);
		ra1 = 0;
		}
	
	fillpoly(ra1,ra2,dec1,dec2,color,refdec);
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
void drawgrid_hor(PLINT gridcolr, PLFLT dec1, PLFLT dec2) {
   int k;
   float i, j;
   PLFLT xp,yp;
   PLFLT x[2401],y[2401];
   for(i=dec1;i<=dec2;i=i+0.5) {
	for(j=0,k=0;j<=24;j+=0.5,k++) {
		ait_rd2xy(j, i, &xp, &yp);
		x[k]=xp;
		y[k]=yp;
        }
        plcol0(gridcolr);
        plline(k, x, y);
// printf("hor:x[0],y[0]=%f %f;x[k],y[k]=%f %f\n",x[0],y[0],x[k],y[k]);
   }
 }

//grid verticals, gridcolr is integer
void drawgrid_vert(PLINT gridcolr, PLFLT dec1, PLFLT dec2) {
   int k=0;
   float i,j;

   PLFLT xp,yp;
   PLFLT x[3601],y[3601];
   for(j=0;j<=24;j=j+1) {
        for(i=dec1,k=0;i<=dec2;i+=0.5,k++) {
		ait_rd2xy(j, i, &xp, &yp);
		x[k]=xp;
		y[k]=yp;
        }
        plcol0(gridcolr);
// printf("ver:x[0],y[0]=%f %f;x[k],y[k]=%f %f\n",x[0],y[0],x[k],y[k]);

// plscol0(0, 0, 0, 0);
        plline(k, x, y);
    }
 }

//grid label. 
void   label_grid_ait(PLINT colr,PLFLT dec1,PLFLT dec2) {
	PLFLT xp,yp,x1,y1,just,dec,space;
	int i;
	char text[5],dectext[5]; 
	float j;
	plcol0(colr);
//RA

	for(dec=dec1+0.5;dec<=dec2;dec+=2.){
		for(i=0;i<=24;i+=4) {
			if(i==24)sprintf(text,"%d",0);
			else sprintf(text,"%d",i);
			if(i<10)space =-0.2;
			else space = -0.5;
			ait_rd2xy(i+space, dec-0.1, &xp, &yp);
			plptex (xp,yp,0,0,-0.2,text);
		}
	}
//DEC	
  	for(j=dec1;j<=dec2+0.001;j+=1) {
		sprintf(text,"%.1f",j);
		ait_rd2xy(0, j, &xp, &yp); 
		plptex (xp,yp,0,0,1.2,text);
	}



// 		ait_rd2xy(1, -80, &xp, &yp);	
// 		x1=xp, y1=yp;
// 		ait_rd2xy(1, -75, &xp, &yp);	
// 		plptex (x1+0.5,y1+0.5,xp-x1-0.1,yp-y1,-0.4,"DEC->");
// 		plcol0(9);
// 		plmtex("rv", -6, 0.22, 0.5, "Colors: SL");
// 		plmtex("rv", -6, 0.20, 0.5, "======");
// 		plmtex("rv", -6, 0.17, 0.5, "blue: <100");
// 		plmtex("rv", -8, 0.14, 0.5, "cyan: 100-1000");
// 		plmtex("rv", -9, 0.11, 0.5, "yellow: 1000-5000");
// 		plmtex("rv", -8.2, 0.08, 0.5, "red: Solar Panel");

    }

//plpoin (n , x , y , code );. Plots a character at the speciﬁed points

//grid 
void drawgrid_ait(PLINT gridcolr, PLFLT dec1, PLFLT dec2) {

     drawgrid_hor(gridcolr, dec1,dec2);
     drawgrid_vert(gridcolr, dec1,dec2);
     label_grid_ait(gridcolr,dec1,dec2);
     }




//for plenv set
void xylimits(float *xmin, float *xmax, float *ymin, float *ymax, float dec1, float dec2){
	PLFLT xp,yp;
    	ait_rd2xy(0., dec1, &xp, &yp);
    	*xmin = xp;
    	ait_rd2xy(24., dec1, &xp, &yp);
	*xmax = xp;
    	ait_rd2xy(0, dec1, &xp, &yp);
    	*ymin = yp;
    	ait_rd2xy(24., dec2, &xp, &yp);
    	*ymax =yp; 
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

