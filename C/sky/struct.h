
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
  struct tempstore {
	char p[15];
  	};
 


