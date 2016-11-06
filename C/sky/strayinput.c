
void editparams() {

 /* for(n2=0;n2<5;n2++) {  //Change n2 max limit====================
     n1=1,n3=0;   //GET these =========================
     if (strsl[n1].strph[n2].strmn[n3].ph < medlim) {
	     sl=strsl[n1].strph[n2].strmn[n3].ph;
	     rastart[n4]=strsl[n1].strph[n2].tstart;
	     raend[n4]=strsl[n1].strph[n2].tend;
	     decstart[n4]=15.0; //GET it ==========================

	     printf("%.1f,%.1f,%.1f,%.1f\n", rastart[n4],raend[n4],decstart[n4],sl);
	     n4++;
	     }//if
     } //fo*/r


   
// for sim loop
  for(n5=0;n5<n4;n5++) {
  printf("simulation loop:\n");	
//Edit param file.
  printf("Editing param file:\n");	

  system("mv -f UVS_skysim_initparams.txt UVS_skysim_initparams_ren.txt");
  
  if((simparfp = fopen("UVS_skysim_initparams_ren.txt","r"))==NULL) {
	  printf("Sim Par file not opened");
  	  exit(1);
  	  }
  if((simparopfp = fopen("UVS_skysim_initparams.txt","w"))==NULL) {
	  printf("Sim Par o/p file not created");
  	  exit(1);
  	  } 

  while((fgets(parline,sizeof(parline),simparfp))!=NULL) {
  sscanf(parline,"%s",par1);

  if(strcmp(par1,"RA_START")==0) {
//	par3 = rastart[n5]);
	fprintf(simparopfp, "RA_START = %.1f /*Begin RA */ \n",rastart[n5]);	
  	}
  else if (strcmp(par1,"RA_END")==0) {

	fprintf(simparopfp, "RA_END = %.1f /*Begin RA */ \n",raend[n5]);
	}

  else if (strcmp(par1,"DEC_START")==0) {

	fprintf(simparopfp, "DEC_START = %.1f /*Begin RA */ \n",decstart[n5]);
	}
  else  {
	fputs(parline,simparopfp);
	}
        strcpy(par1,"");
//   JITTER       =  0.00001   
//   DARKCOUNT    =  0.000005



  }//while fgets par file
  fclose(simparfp);
  fclose(simparopfp);

  printf("Running Sim with RAstart=%.1f, RAend=%.1f,dec1=%.1f\n", rastart[n5],raend[n5],decstart[n5]);
  system("./uvs_simulate_sky");

// file renaming
  sprintf(file_ren,"%.1f",decstart[n5]); 

  strcat(file_ren,"-1");  
  strcpy(file_ren2,"UVS_skysimdata-C_DEC");
  strcat(file_ren2,file_ren); //file_ren2 is source file
  
  strcpy(file_ren3,file_ren2);
  strcat(file_ren3, "-"); 
  sprintf(file_ren4,"%d",n5+1); 
  strcat(file_ren3,file_ren4);  //file_ren3 dest file
  sprintf(file_ren5,"mv %s %s",file_ren2, file_ren3);
  printf("renaming %s\n\n",file_ren5);
  system(file_ren5);


  } //for sim loop


return ;
}//main



    