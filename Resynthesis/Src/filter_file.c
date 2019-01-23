/* Copyright (c) 1995-2019 Faculté polytechnique de Mons (TCTS lab)
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifndef __i386__
#define SWAP_MACHINE
#endif

#ifdef RESAMPLING16_8

/* Alien sample rate for resynthesis */
int TargetFreq=8000;

/* Coef such that in_coef*in_len=out_coef*out_len */
int  in_coef=1;	 /* 16000*1 = 8000*2 */
int  out_coef=2;

double filter_coef[]={ 
#include "COEF_FILTER/filtr16_8" 
};
#endif

#ifdef RESAMPLING16_11
/* Alien sample rate for resynthesis */
int TargetFreq=11000;

/* Coef such that in_coef*in_len=out_coef*out_len */
int  in_coef=11;	 /* 16000*11 = 11000*16 */
int  out_coef=16;

double filter_coef[]={ 
#include "COEF_FILTER/filtr16_11" 
};
#endif

#define NB_COEF sizeof(filter_coef)/sizeof(double) 

/* 
 * Resample a Little Endian audio file
 */
void filter_file(char *name_filtered, char *name_source)
{
  FILE *filtered, *source;
  short *in_buff, *out_buff;
  long in_pos,out_pos;     /* Position in in_buff and out_buff (step are different) */
  long coef_base;			  /* Position of the first coef being used */
  long in_base;				  /* Position of the first in value being used */
  int Order;				  /* Order of the filter */
  int HalfOrder;
  long i, nb_read,in_length;
  

  /* Check if the work is already done */
  if ((filtered=fopen(name_filtered,"r"))!=NULL)
	 {
		fclose(filtered);
		fprintf(stderr,"\nALLREADY RESAMPLED %s!\n",name_filtered);
		return;
	 }
  
  if ((filtered=fopen(name_filtered,"w"))==NULL)
	 {
		fprintf(stderr,"Fatal error with %s!\n",name_filtered);
		exit(1);
	 }
  
  if ((source=fopen(name_source,"r"))==NULL)
	 {
		fprintf(stderr,"Fatal error with %s!\n",name_source);
		exit(1);
	 }
  
  Order= NB_COEF;
  
  fseek(source,(long)0,SEEK_END);
  in_length= ftell(source) / sizeof(short);
  fseek(source,(long)0,SEEK_SET);
  
  in_buff=(short *) calloc(in_length+Order+2, sizeof(short));
  out_buff= (short *) calloc( ((in_length+Order+1)*in_coef)/out_coef, sizeof(short));
  
  /* Head Filled up to Half Order with 0 */
  for(i=0; 2*i< Order/in_coef; i++)
	 in_buff[i]=0;
  HalfOrder=i;
  
  nb_read= fread(&in_buff[HalfOrder], sizeof(short), in_length, source);
#ifdef SWAP_MACHINE
  swab((char *) &in_buff[HalfOrder], (char *) &in_buff[HalfOrder], sizeof(short)*in_length);
#endif

  /* Tail filled up to half order with 0 */
  for(i=0; i< HalfOrder; i++)
	 in_buff[i+nb_read+HalfOrder]=0;	
    
  /* Loop on output values */
  for(coef_base=0,out_pos=0,in_base=0 ; 
		out_pos*out_coef < nb_read*in_coef; 
		out_pos++)
	 {
		double out_double;
		
		/* Step in the input buffer ? */
		while (in_base*in_coef< out_pos*out_coef)
		  in_base++;
		
		coef_base= in_base*in_coef - out_pos*out_coef;
		
		out_double=0.0;
		for(in_pos=0; in_pos*in_coef+coef_base < Order; in_pos++)
		  out_double+= filter_coef[in_pos*in_coef+coef_base] 
			 * (double) in_buff[in_pos+in_base];
		
		out_double*= in_coef;
		
		if (out_double>32768.0)
		  {
			 fprintf(stderr,"CLIPPING %f\n",out_double);
			 out_double=32767;
		  }
		if (out_double<-32767.0)
		  {
			 fprintf(stderr,"CLIPPING %f\n",out_double);
			 out_double=-32767;
		  }
		out_buff[out_pos]= (short) out_double;
	 }

#ifdef SWAP_MACHINE
  swab((char*) out_buff,(char *) out_buff, sizeof(short) *out_pos);
#endif
  fwrite(out_buff, sizeof(short), out_pos, filtered);
  
  fclose(filtered);
  fclose(source);
}

/*  
main()
{
filter_file("test1.tmp","test2.tmp");
}
*/
