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

char ProgramInfo[]=
"* This program transforms diphones fron diphone database by normalizing       \n"
"* the energy of each phoneme class                                            \n"
"*                                                                             \n"
"* HISTORY:                                                                    \n"
"*                                                                             \n"
"* 08/08/96: created by Vincent Pagel                                          \n"
"* 28/01/97: Re-implementation of the normalization principle adopted for      \n"
"*           fr1. Energy-based normalization (was : abs. value-based)          \n"
"*           Energy estimation from a fixed frame length at both ends of       \n"
"*             diphones (was : on the whole half-phoneme)                      \n"
"*           Time-linear amplitude normalization (was : time-constant)         \n"
"*           Energy normalization on selected phonemes only                    \n"
"* 5/10/01 : give the directory of .pm files                                   \n";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define int8  unsigned char
#define int16 short
#define int32 long

#define BUFFER_SIZE 32000 /* 2 seconds at 16 kHz ! */
#define MAX_NBR_PHONEMES 2000
#define MAX_NBR_DIPHONES 10000
#define MAX_NBR_DIPHONES_PER_PHONEME 1000
typedef char diphonename[100];
typedef char phonename[50];
typedef struct
{
  phonename name;
  diphonename diphones[MAX_NBR_DIPHONES_PER_PHONEME];
  double diphones_RMS[MAX_NBR_DIPHONES_PER_PHONEME];
  int diphones_VSS[MAX_NBR_DIPHONES_PER_PHONEME];
  double average_RMS;
  double stdev_RMS;
  int nb;
} Phone;

Phone Dico[MAX_NBR_PHONEMES];
int nb_dico=0;
int nsamples=100;
double resamp_ratio;

int16 buffer[BUFFER_SIZE];


void RMS_begin_end(char *wave_name, int n_samples, double *RMS_begin, 
		   double *RMS_end)
{
  FILE *wave_file;
  long i;
  long file_length;
  int n_read;
     
  wave_file=fopen(wave_name,"rb");
  if (wave_file==NULL)
    {
      fprintf(stderr,"FATAL ERROR with %s file !\n",wave_name );
      exit(1);
    }
  
  fseek(wave_file,0,SEEK_SET);
  n_read=fread(buffer,sizeof(int16),n_samples,wave_file);
  if (n_read!=n_samples)
    {
      fprintf(stderr,"FATAL ERROR with %s file !\n",wave_name );
      exit(1);
    }
  
#if defined __powerpc__ || defined __m86k__
  swab((char *)buffer,(char *)buffer,sizeof(int16)*n_samples);
#endif

  *RMS_begin=0;
  for(i=0; i<n_samples; i++)
    *RMS_begin+= buffer[i]*buffer[i];
  *RMS_begin=sqrt(*RMS_begin/n_samples);
  
  fseek(wave_file,0,SEEK_END);
  file_length=ftell(wave_file);
  if (file_length-n_samples*sizeof(int16)<0)
    {
      fprintf(stderr,"FATAL ERROR with %s file !\n",wave_name );
      exit(1);
    }
  fseek(wave_file,file_length-n_samples*sizeof(int16),SEEK_SET);
  fread(buffer,sizeof(int16),n_samples,wave_file);

#if defined __powerpc__ || defined __m86k__
  swab((char *)buffer,(char *)buffer,sizeof(int16)*n_samples);
#endif

  *RMS_end=0;
  for(i=0; i<n_samples; i++)
    *RMS_end+= buffer[i]*buffer[i];
  *RMS_end=sqrt(*RMS_end/n_samples);

  fclose(wave_file);
  return;
}

void VSS_begin_end(char *pm_name, int *VSS_begin, 
		   int *VSS_end, int limitframe)
{
  FILE *pm_file;
  long i,j;
  int8 VSS[100];
  int maxnconcat;
  int n_read;
  
  pm_file=fopen(pm_name,"rb");
  if (pm_file==NULL)
    {
      fprintf(stderr,"FATAL ERROR with %s file !\n",pm_name );
      exit(1);
    }
  
  fseek(pm_file,0,SEEK_SET);
  n_read=fread(VSS,sizeof(int8),100,pm_file);
  if (n_read==0)
    {
      fprintf(stderr,"FATAL ERROR with %s file !\n",pm_name );
      exit(1);
    }
  fclose(pm_file);
  
  for(i=0; i<n_read; i++)
    if ((int8)VSS[i]!=2) break;
  
  if ((i>0) && (i<n_read))
    if ((int8)VSS[i]==3) i++;
  
  for(j=n_read-1; i>=0; j--)
    if ((int8)VSS[j]!=2) break;
  
  if ((j<n_read-1) && (j>=0))
    if ((int8)VSS[j]==3) j--;
  *VSS_begin=i;
  
  maxnconcat=(int)(0.5*n_read+1.0);
  if (maxnconcat<6) maxnconcat=6;
  
  if (*VSS_begin>maxnconcat) *VSS_begin=maxnconcat;
  if (*VSS_begin>limitframe) *VSS_begin=limitframe;
  *VSS_end=n_read-j-1;
  if (*VSS_end>maxnconcat) *VSS_end=maxnconcat;
  if (*VSS_end>(n_read-limitframe)) *VSS_end=limitframe;
  return;
}

void apply_ratio(char *wave_name, char *out_wave_name, char *pm_name, 
                 int nsamples, double coef_l, double coef_r)
{
  FILE *wave_file,*out_file, *pm_file;
  long i,j;
  double tmp;
  int nb_read, nb_pm;
  long curnsamples;
  int8 pmarks[400];
  int prev_v;
  int curpos;
  double coef;

  if ((out_file=fopen(out_wave_name,"wb"))==NULL)
    {
      fprintf(stderr,"FATAL ERROR with %s file !\n",out_wave_name );
      exit(1);
    }

  if ((wave_file=fopen(wave_name,"rb"))==NULL)
    {
      fprintf(stderr,"FATAL ERROR with %s file !\n",wave_name );
      exit(1);
    }

  if ((pm_file=fopen(pm_name,"rb"))==NULL)
    {
      fprintf(stderr,"FATAL ERROR with %s file !\n",pm_name );
      exit(1);
    }

  nb_read=fread(buffer,sizeof(int16),BUFFER_SIZE,wave_file);
  nb_pm=fread(pmarks,sizeof(int8),400,pm_file);
	 
#if defined __powerpc__ || defined __m86k__
  swab((char *)buffer,(char *)buffer,sizeof(int16)*nb_read);
#endif

  prev_v=1; /* fake previous frame = Voiced */
  curpos=0;
  for(i=0; i<nb_pm; i++)
    {
      if ((pmarks[i]==0) || (pmarks[i]==1))
	{
	  if (prev_v==1)
	    curnsamples=2*nsamples;
	  else
	    curnsamples=nsamples;
	  prev_v=0;
	}
      else 
	{
	  curnsamples=nsamples;
	  prev_v=1;
	}
      coef=(i*(coef_r-coef_l)/(nb_pm-1)+coef_l);
      for (j=0;j<curnsamples;j++)
	{
	  tmp=(double) buffer[curpos] * coef;
	  if (tmp>32767.0) 
	    {
	      printf("overflow %s\n",out_wave_name);
	      tmp=32767;
	    }
	  if (tmp<-32767.0) 
	    {
	      printf("overflow %s\n",out_wave_name);
	      tmp=-32767;
	    }
	  buffer[curpos++]= (int16) tmp;
	}
    }
  if (curpos!=nb_read)
    {
      fprintf(stderr,"ERROR in the number of samples of %s file !\n",pm_name );
      exit(1);
    }
#if defined __powerpc__ || defined __m86k__
  swab((char *)buffer,(char *)buffer,sizeof(int16)*nb_read);
#endif
  fwrite(buffer,sizeof(int16),nb_read,out_file);

  fclose(pm_file);
  fclose(wave_file);
  fclose(out_file);
}

void add_info(char *phone, char *in_diphone, double rms, int nvss)
{
  int i;
  
  for(i=0;i<nb_dico;i++)
    if (strcmp(phone,Dico[i].name)==0) break;
  
  if (nb_dico == MAX_NBR_PHONEMES )
    {
      fprintf(stderr,"FATAL ERROR Too many MAX_NBR_PHONEMES %i\n", MAX_NBR_PHONEMES);
      exit(1);
    }

  /* Was'nt there, add */
  if (i==nb_dico)
    {
      Dico[i].nb=0;
      strcpy(Dico[i].diphones[Dico[i].nb],in_diphone);
      Dico[i].diphones_RMS[Dico[i].nb]=rms;
      Dico[i].diphones_VSS[Dico[i].nb]=nvss;
      strcpy(Dico[i].name, phone);
      nb_dico++;
      Dico[i].nb++;
    }
  else
    {
      strcpy(Dico[i].diphones[Dico[i].nb],in_diphone);
      Dico[i].diphones_RMS[Dico[i].nb]=rms;
      Dico[i].diphones_VSS[Dico[i].nb]=nvss;
      Dico[i].nb++;
    }
  if ( Dico[i].nb == MAX_NBR_DIPHONES_PER_PHONEME )
    {
       fprintf(stderr,"FATAL ERROR too many MAX_NBR_DIPHONES_PER_PHONEME %i!\n",MAX_NBR_DIPHONES_PER_PHONEME);
       exit(1);
    }
}

void compute_info()
{
  int i,j;
  
  for(i=0;i<nb_dico;i++)
    {
      for(j=0;j<Dico[i].nb;j++)
	Dico[i].average_RMS+=Dico[i].diphones_RMS[j];
      Dico[i].average_RMS/=Dico[i].nb;
      for(j=0;j<Dico[i].nb;j++)
	Dico[i].stdev_RMS+=(Dico[i].diphones_RMS[j]-Dico[i].average_RMS)
	  * (Dico[i].diphones_RMS[j]-Dico[i].average_RMS);
      Dico[i].stdev_RMS=sqrt(Dico[i].stdev_RMS/Dico[i].nb);
    }
}

int retrieve_index(char *phone)
{
  int i;
  
  for(i=0;i<nb_dico;i++)
    if (strcmp(phone,Dico[i].name)==0) break;
  
  if (i==nb_dico)
    {
      printf("FATAL BUG %s\n",phone);
      exit(1);
    }
  return i;
}

int main(int argc, char **argv)
{
  FILE *dba_in;
  FILE* illfile;
  char name[255],wave[255] ,
    name_s16[255], name_s16_target[255], name_pm[255], 
    left_phone[255], right_phone[255],
    line[255];
  int i,j;
  long begin, end,middle;
  double rms1,rms2;
  int nvss1, nvss2;
  int nline;
  char datafilename[255];  
  phonename illegals[50];
  int nb_illegals=0;
  int limitframe;
  FILE* eqdata;
  double ratio_left, ratio_right;

  if ( (argc!=6) && 
       (argc!=8))
    {
      fprintf(stderr,ProgramInfo);
      fprintf(stderr,"\n %s DbaName.seg Path_wav Path_normalized_wav nsamples illegal_phonemes.ill [orig_sample_rate dest_samp_rate]\n",argv[0]);
      /*                 0      1         2                3            4               5                6                 7     */
      exit(1);
    }
  
  if (argc==8)
    resamp_ratio= atof(argv[7])/atof(argv[6]);
  else
    resamp_ratio=1.0;
  
  nsamples= atoi(argv[4]);
  
  if ((dba_in=fopen(argv[1],"rb"))==NULL)
    {
      fprintf(stderr,"FATAL ERROR with dba file !\n" );
      exit(1);
    }

  /* First pass */
  nline=0;
  while (!feof(dba_in))
    {
      strcpy(line,"");  /* needed to avoid bad processing of blank lines 
                           at the end of the file. */
      fgets(line, 255,dba_in);
      if (sscanf(line,"%s %s %s %li %li %li",wave,left_phone,right_phone,&begin,&end,&middle)==6)
	{
	  begin*=resamp_ratio;
	  end*=resamp_ratio;
	  middle*=resamp_ratio;

	  /* Complete name of the wave file */
	  sprintf(name,"%i",nline);
	  sprintf(name_s16,"%s%s.s16", argv[2], name); 
	  sprintf(name_pm,"%s%s.pm", argv[2], name);
			 
	  /* Left and right phone */
	  RMS_begin_end(name_s16, nsamples, &rms1, &rms2);
          limitframe= (middle-begin)/ nsamples;
          VSS_begin_end(name_pm, &nvss1, &nvss2,limitframe);
			 
	  /* printf("Left=%s : Right=%s Wave=%s e1=%f e2=%f\n",left_phone, right_phone,name_s16,energ1,energ2); */

	  nline++;			 
          add_info(left_phone, name, rms1, nvss1);
	  add_info(right_phone, name, rms2, nvss2);
			 
	  printf("LEFT=%s %f RIGHT=%s %f\n",left_phone, rms1, right_phone, rms2);
	}
    }
  fclose(dba_in);
  
  compute_info();
  
  if ((illfile=fopen(argv[5],"rt"))==NULL)
    {
      fprintf(stderr,"FATAL ERROR with illegal phonemes file !\n" );
      exit(1);
    }
  
  while (!feof(illfile))
    {
      fscanf(illfile,"%s",illegals[nb_illegals]);
      nb_illegals++;
    }
  fclose(illfile);
  
  sprintf(datafilename,"%s.eq",argv[1]);
  if ((eqdata=fopen(datafilename,"wt"))==NULL)
    {
      fprintf(stderr,"FATAL ERROR with intermediate results file !\n" );
      exit(1);
    }
  
  /* Second  pass */
  if ((dba_in=fopen(argv[1],"rb"))==NULL)
    {
      fprintf(stderr,"FATAL ERROR with dba file !\n" );
      exit(1);
    }
  
  nline=0;
  while (!feof(dba_in))
    {
      strcpy(line,"");  /* needed to avoid bad processing of blank lines 
                           at the end of the file. */
      fgets(line, 255,dba_in);
      if (sscanf(line,"%s %s %s %li %li %li",wave,left_phone,right_phone,&begin,&end,&middle)==6)
	{
	  begin*=resamp_ratio;
	  end*=resamp_ratio;
	  middle*=resamp_ratio;

	  /* Complete name of the wave file */
	  sprintf(name,"%i",nline);
	  sprintf(name_s16,"%s%s.s16", argv[2], name); 
	  sprintf(name_s16_target,"%s%s.s16", argv[3], name); 
	  sprintf(name_pm,"%s%s.pm", argv[2], name);
	  
	  /* Left phone then right phone */
	  RMS_begin_end(name_s16, nsamples, &rms1, &rms2);

	  i= retrieve_index(left_phone);
          ratio_left= Dico[i].average_RMS / rms1;

	  j= retrieve_index(right_phone);
          ratio_right= Dico[j].average_RMS / rms2;

	  /* NO AMPLIFICATION ON FORBIDDEN PHONEMES !!! */
	  /* Generally _ p t k ... */
          for(i=0;i<nb_illegals;i++)
	    {
	      if (strcmp(left_phone,illegals[i])==0) 
		{
		  if (ratio_left>=1.0)
		    ratio_left=1.0;
		}
	    }
			 
          for(i=0;i<nb_illegals;i++)
	    {
	      if (strcmp(right_phone,illegals[i])==0) 
		{
		  if (ratio_right>=1.0)
		    ratio_right=1.0;
		}
	    }
          
	  /* 
	     printf("%s COEF=%f %f %f\n", out_wave_name, coef,coef_l,coef_r);
	     printf("NAME=%s energ=%f nb=%i\n",Dico[i].name,Dico[i].energ,Dico[i].nb); 
	     */
			 
          if ((ratio_right/ratio_left<5) && (ratio_right/ratio_left>0.2))
	    {
	      fprintf(eqdata, "%s left_ratio=%3.3f\t rightratio=%3.3f\t\n", name, ratio_left, ratio_right);
	      apply_ratio(name_s16, name_s16_target, name_pm, nsamples, ratio_left, ratio_right);
	    }
          else  
	    {
	      fprintf(eqdata, "%s NOT equalized left_ratio=%3.3f\t rightratio=%3.3f\t\n", name, ratio_left, ratio_right);
	      apply_ratio(name_s16, name_s16_target, name_pm, nsamples, 1, 1);
	    }
	  printf("%d %s\n",nline++,name);
	}
    }
  fclose(dba_in);
  fclose(eqdata);

  return 0;
}
