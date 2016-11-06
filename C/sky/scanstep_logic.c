min&max found; expmin=given
	while(remain){
         //steps--------
		if((dec >= 89.85) && (abs(min-expmin) <=200)) {remain=0;}
		else if(abs(min - (exptime+abs(dec)/90.*8.))<=abs(dec)/90.*8.) remain =0;
		else if(mincheck) {
			if(min < exptime) {
				if(step >0)step -= delta;
				else remain=0;	}
			else if(min > exptime){
				if((step < (maxstep+delta)) && (step>(maxstep-delta)) )remain =0;
				else if(step < maxstep) step +=delta;}
				else remain=0;
			if(step >= 0.9) {step=0.9; remain=1; }
		}
		else {	// mincheck ==0	

				if((step3 ==1)&&(min > expmin)) { 
					if((step5 ==1)&&(step4 ==1))mincheck=1;
					else step +=0.001;
					step4 =1;                }
				else if((step3 ==1)&&(min < expmin)) {
					if((step5 ==1)&&(step4 ==1))mincheck=1;
					else step -=0.0002;
					step5=1;                     }
				else {
					remain =1;
					pstep = step;
					if(step<=0.89)step = (step*min)/expmin; 
					else if((step>0.89)&&(min>expmin))remain=0;
					else if((step>0.89)&&(min<expmin))step-=0.1;
// 					if(step<0.05)step=0.05;
// 					if((abs(pstep-step)<0.0005)&&(step<0.89)&&(min<expmin))step+=0.05;
					if(pstep >step) step1 =1;
					if((step1 ==1) &&(pstep < step)) step2 =1;
					if((step2 ==1) &&(pstep > step)) step3 =1;
					if(step >= 0.9)step=0.9;
				}
		}
		maxstep =step;
	}//while
