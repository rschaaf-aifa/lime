/*
 *  smooth.c
 *  LIME, The versatile 3D line modeling environment 
 *
 *  Created by Christian Brinch on 08/07/06.
 *  Copyright 2006-2011, Christian Brinch, 
 *  <brinch@nbi.dk>
 *  Niels Bohr institutet
 *  University of Copenhagen
 *	All rights reserved.
 *
 */

#include <qhull/qhull_a.h>
#include "lime.h"


/* Lloyds Algorithm (Lloyd, S. IEEE, 1982) */	
void
smooth(inputPars *par, struct grid *g){
	double mindist;			/* Distance to closest neighbor				*/
	int k=0,j,i;				/* counters																*/
	int sg;							/* counter for smoothing the grid			*/
	int cn;
	int smooth=25;			/* Amount of grid smoothing							*/
	double move[3];			/* Auxillary array for smoothing the grid	*/
	double maxdist;
	double dist;				/* Distance to a neighbor								*/
	
	for(sg=0;sg<smooth;sg++){
	  for(i=0;i<par->ncell && !g[i].sink;i++){
			mindist=1e30;
			cn=-1;
			for(k=0;k<g[i].numNeigh;k++){
				if(g[g[i].neigh[k]].sink==0){
					if(g[i].ds[k]<mindist){
						mindist=g[i].ds[k];
						cn=k;
					}
				}
			}
			
			
			if(par->radius-sqrt(pow(g[i].x[0],2)+pow(g[i].x[1],2)+pow(g[i].x[2],2))<mindist){
				mindist=par->radius-sqrt(pow(g[i].x[0],2)+pow(g[i].x[1],2)+pow(g[i].x[2],2));
				cn=-1;
			}
			
			if(cn>-1) {
				for(k=0;k<DIM;k++){
					move[k] = g[i].x[k] - g[i].dir[cn].x[k]*0.15;
				}			  
				if(sqrt(move[0]*move[0]+move[1]*move[1]+move[2]*move[2])<par->radius &&
				   sqrt(move[0]*move[0]+move[1]*move[1]+move[2]*move[2])>par->minScale){
					for(k=0;k<DIM;k++) g[i].x[k]=move[k];
				}
			}
		}
		
		
		for(i=par->pIntensity;i<par->ncell;i++){
		  mindist=1e30;
		  cn=-1;
		  maxdist=0;
		  for(k=0;k<g[i].numNeigh;k++){	
			if(g[g[i].neigh[k]].sink==1){
			  if(g[i].ds[k]<mindist){
			    mindist=g[i].ds[k];
				cn=k;
			  }
			}
		  }
		  j=g[g[i].neigh[cn]].id; 
		  for(k=0;k<DIM;k++){
		    g[i].x[k] = g[i].x[k] - (g[j].x[k]-g[i].x[k]) * 0.15;;
		  }			
		  dist=par->radius/sqrt(pow(g[i].x[0],2)+pow(g[i].x[1],2)+pow(g[i].x[2],2));	
		  for(k=0;k<DIM;k++){
			g[i].x[k] *= dist;
		  }	
		}
		
		
		qhull(par, g);	
		distCalc(par, g);	    
		if(!silent) progressbar((double)(sg+1)/(double)smooth, 5);	  
	}	
}

