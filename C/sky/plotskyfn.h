
#ifndef PLOTSKYFN_H
#define PLOTSKYFN_H

#define PI 3.1415926535897932384
#define NPTS  1000  //not implimented
#define ra2deg  (180.0/PI)
#define deg2rad (PI / 180.0)


double ait_rd2xy (PLFLT ra, PLFLT dec, PLFLT *xp, PLFLT *yp);
void  fillpoly(PLFLT ra1,PLFLT ra2, PLFLT dec1,PLFLT dec2, PLINT color,PLFLT refdec);
void  fillarea(PLFLT ra1,PLFLT ra2, PLFLT dec1,PLFLT dec2, PLINT color, PLINT refdec);
void   label_rd(PLFLT ra1,PLFLT dec1, char *name, PLINT colr);
void drawgrid_hor(PLINT gridcolr, PLFLT dec1, PLFLT dec2);
void drawgrid_vert(PLINT gridcolr, PLFLT dec1, PLFLT dec2);
void   label_grid_ait(PLINT colr,PLFLT dec1,PLFLT dec2);
void drawgrid_ait(PLINT gridcolr, PLFLT dec1, PLFLT dec2);
void xylimits(float *xmin, float *xmax, float *ymin, float *ymax, float dec1, float dec2);
char *get_filename(char *flt, char *days, char *days2, int pl, char *filename, char *toplbl);




#endif