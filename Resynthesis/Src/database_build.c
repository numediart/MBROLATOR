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
"* This program builds a mbrola database for 2.02 or upper version             \n"
"* Makes corrections on the case of the file ( MSDOS has no uppercase          \n"
"* filenames)                                                                  \n"
"*                                                                             \n"
"* HISTORY:                                                                    \n"
"*                                                                             \n"
"* 17/09/96: created by Vincent Pagel                                          \n"
"* 02/02/97: accepts replacement diphones in the input text database           \n"
"* 10/04/97: we're no more tangled in msdos diphone rename...UNIX is alive     \n"
"* but make and pmake braindead with ':' -> genere_make is in charge of the    \n"
"* renaming problem :-(                                                        \n"
"* 01/07/97: auto _-_ add when not defined                                     \n"
"*           Mbrola 2.04 -> pitchmark is int32                                 \n"
"* 09/10/97: Mbrola 2.05 -> unlimited phoneme length                           \n"
"* 02/02/98: Mbrola 2.06 -> bug with the resampled dba construction            \n"
"* 02/02/98: Mbrola 3.00 -> sanity checks on the dba                           \n"
"* 11/11/98: Amplification ratio on the fly                                    \n"
"* 25/03/99: Compressor on the fly for amplification                           \n"
"* 07/03/00: .info files bug with \0 + .seg file + .ill file + .build file     \n"
"* 04/09/00: simplication, no more strange renamings, new version on generate_make\n"
"*           give PM path - usefull in combination with diphone_resynth_equalize\n";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define int8  unsigned char
#define int16 short
#define int32 long
#define uint16 unsigned short

/* Number of silent frames for the _-_ diphone automatically added */
#define NB_SILENCE 40

#define BUFFER_SIZE 65536

typedef struct
{
  char *left, *right; /* Name of the diphone     */
  int16 halfseg;      /* position of center of diphone */
  int8  nb_frame;     /* Number of pitch markers */
  int8 nb_wframe;     /* Number of wave frames   */
} DiphoneBase;

typedef DiphoneBase DiphoneTable[10000];
DiphoneBase diphone;
DiphoneTable diphone_table;

short compression_curve[32768];

/* The compression curve map value X onto value Y */
void init_compression_curve(double x,double y)
{
  /*
     First part is linear amplification
     Seconf part -> a log x + b*log x
     Conditions: f(X)=Y   f'(X)=coef
     f'(x)=a/x + b/x-> goes toward flat
     */
  int i,j;
  double coef= y/x;
  double a,b;
  double baseY,restY,restX;

  /* Linear part */
  for (i=0;i<(int)x;i++)
    compression_curve[i]=(short)rint((double)i*coef);

  baseY= (double)i*coef;
  restY=32767.0-baseY;
  restX=32767.0-x;

  a=coef;
  b= (a*restX-restY)/restX/restY;

  for(j=0 ; i<32767 ; i++,j++)
    compression_curve[i]=(short)rint( baseY + a*j/(1+b*j) );
}

int main(int argc, char **argv)
{
  FILE *dba_in,*dba_out,*raw_out,*pm_out,*tmp,*big_one;
  char *Path, *PathPm, *Version, *dba_name;
  char name_left[255], name_right[255];
  char name[255];
  char copy_nameleft[255], copy_nameright[255];
  char name_raw[255], sys[255];
  char line[255];

  double volume_ratio; /* quick hack to let you push the volume */

  int16 Freq;
  int8 OldPeriod;
  int16 Period;
  int8 Coding;
  int32 SizeMrk=0;
  int16 SizeDba=0;
  int32 SizeRaw=0;

  int begin,end,middle;
  int32 indice_raw=0;
  int32 indice_pm=0;
  int indice_diphone=0;
  int32 wave_size;

  int incident=0;
  int i,j;
  char buffer[BUFFER_SIZE];
  short audio_in[BUFFER_SIZE];
  short audio_out[BUFFER_SIZE];
  double resamp_ratio;
  int nb_read;
  int init_silence=0;			  /* True if _-_ has been encountered */
  int separator,count;

  if ( (argc!=8) &&
       (argc!=9) &&
       (argc!=10))
    {
      fprintf(stderr,ProgramInfo);
      printf("\n %s DbaName Path_for_s16 Path_for_pm Version Sample_rate MBRPeriod Code [volume_ratio [source_srate]]\n",argv[0]);
      exit(1);
    }

  dba_name= argv[1];
  Path= argv[2];
  PathPm = argv[3];
  Version= argv[4];
  Freq= atoi(argv[5]);
  Period= atoi(argv[6]);
  Coding= atoi(argv[7]);

  if (argc>=9)
    volume_ratio= atof(argv[8]);
  else
    volume_ratio= 1.0;

  if (volume_ratio>1.0)
    {
      /* We may need compression */
      double max_val= rint(32768.0/volume_ratio);
      double mapped_to= (32768+max_val)/2;   /* rise linear until remain + 2 */
      init_compression_curve( max_val, mapped_to );
    }

  if (argc==10)
    resamp_ratio= (double) Freq / atof(argv[9]);
  else
    resamp_ratio=1;

  if (strlen(Version)>5)
    {
      fprintf(stderr,"MBR version number format is xxxxx : %s\n",Version);
      return 1;
    }
  if (Freq<=0)
    {
      printf("Wrong sampling frequency : %s\n",argv[5]);
      return 1;
    }
  if ( (Period<=0) || (Period>500))
    {
      printf("Wrong Period: %s\n",argv[6]);
      return 1;
    }
  if (Coding<=0)
    {
      printf("Wrong Coding: %s\n",argv[7]);
      return 1;
    }

  /* Open the label.seg file */
  sprintf(name,"%s.seg",dba_name);
  if ((dba_in=fopen(name,"rb"))==NULL)
    {
      fprintf(stderr,"CAN'T OPEN %s!\n",name);
      exit(1);
    }

  if ( ((dba_out=fopen("SORTIE.dba","wb"))==NULL) ||
       ((pm_out=fopen("SORTIE.mrk","wb"))==NULL) ||
       ((raw_out=fopen("SORTIE.raw","wb"))==NULL) )
    {
      fprintf(stderr,"FATAL ERROR output files SORTIE.DBA,  !\n");
      exit(1);
    }

  /* First part -> scan normal diphone definition */
  while (!feof(dba_in))
    {
      strcpy(line,"");  /* needed to avoid bad processing of blank lines at the end of the file. */
      fgets(line, 255,dba_in);

      /* file_name diphleft diphright begin_sample end_sample middle_sample */
      if (sscanf(line,"%*s %s %s %i %i %i",name_left,name_right,&begin,&end,&middle) !=5 )
	{
	  fprintf(stderr,"Ooops on line (.seg file) : %s\n",line);
	}
      else
	{
	  /* definition of _-_ */
	  if ( (strcmp(name_left,"_")==0) &&
	       (strcmp(name_right,"_")==0))
	    { init_silence= indice_diphone; }

	  diphone_table[indice_diphone].left=strdup(name_left);
	  diphone_table[indice_diphone].right=strdup(name_right);

	  diphone_table[indice_diphone].halfseg= (int16) ((middle - begin)*resamp_ratio);

	  /*
	   * Open the diphone soundfile ( ex: a-a.s16 )
	   */
	  sprintf(name_raw,"%s%i.s16", Path, indice_diphone);
	  if ((tmp=fopen(name_raw,"rb"))==NULL)
	    {
	      fprintf(stderr,"FATAL ERROR input files %s !\n",name_raw);
	      exit(1);
	    }
	  while (!feof(tmp))
	    {
	      int saturation= 0; /* Saturation indicator */
	      double val_satur=0;

	      int t;
	      j=fread(audio_in, sizeof(short), BUFFER_SIZE, tmp);
	      indice_raw+=j;
#ifndef __i386__
	      swab((char*)audio_in, (char*)audio_in, j*2);
#endif
	      for(t=0; t<j ; t++)
		{
		  double val = volume_ratio * (double) audio_in[t];

		  if ( (val>32767) ||
		       (val<-32768))
		    {
		      saturation=1;
		      if (val>val_satur)
			val_satur=val;
		      if (-val>val_satur)
			val_satur=-val;
		    }
		  audio_out[t]= val;
		}

	      /* Plug the compression */
	      if (saturation)
		{
		  fprintf(stderr,"saturation on %s %lf\n",name_raw, val_satur);
		  for(t=0; t<j ; t++)
		    {
		      /*
			if (audio_in[t]>0)
			audio_out[t]= compression_curve[ audio_in[t]  ];
			else
			audio_out[t]= compression_curve[ -audio_in[t]  ];
		      */
		      double val= volume_ratio * (double) audio_in[t] / val_satur * 32767.0;
		      audio_out[t]= val;
		    }
		}

#ifndef __i386__
	      swab((char*)audio_out, (char*)audio_out, j*2);
#endif
	      fwrite(audio_out, sizeof(short), j, raw_out);
	    }
	  wave_size=ftell(tmp)/2;
	  diphone_table[indice_diphone].nb_wframe= wave_size/Period;

	  /* Check the wave size is a multiple of the pitch period */
	  if ((wave_size-(int32)diphone_table[indice_diphone].nb_wframe*Period)!=0)
	    {
	      fprintf(stderr,"Wavesize=%li Period=%i Frame=%i Rest=%li on %s-%s file %s\n",
		      wave_size,  Period,diphone_table[indice_diphone].nb_wframe,
		      wave_size-(int32)diphone_table[indice_diphone].nb_wframe*(int32)Period,
		      diphone_table[indice_diphone].left,
		      diphone_table[indice_diphone].right,
		      name_raw);
	      incident++;
	    }
	  fclose(tmp);

	  /* Open the diphone pitchmark file ( ex: a-a.pm ) */
	  sprintf(name_raw,"%s%i.pm", PathPm, indice_diphone);
	  if ((tmp=fopen(name_raw,"rb"))==NULL)
	    {
	      fprintf(stderr,"FATAL ERROR input files %s !\n",name_raw);
	      exit(1);
	    }

	  while (!feof(tmp))
	    {
	      j=fread(buffer,sizeof(char),BUFFER_SIZE,tmp);
	      indice_pm+=j;
	      fwrite(buffer,sizeof(char),j,pm_out);
	    }

	  diphone_table[indice_diphone].nb_frame = (int8) ftell(tmp);
	  fclose(tmp);

	  printf("READ: %s-%s %i %i %i end_raw=%li %li nb_frame=%i physical=%i\n",
		 diphone_table[indice_diphone].left,
		 diphone_table[indice_diphone].right,
		 begin,end,middle,indice_raw, indice_pm,
		 diphone_table[indice_diphone].nb_frame,
		 diphone_table[indice_diphone].nb_wframe);

	  /* Sanity check -> physical frame number is at least equal to the
	   * logical frame number
	   */
	  if (diphone_table[indice_diphone].nb_frame > diphone_table[indice_diphone].nb_wframe)
	    {
	      fprintf(stderr,"**** Physical frame number / logical frame number on %s \n", name_raw);
	      incident++;
	    }

	  /* Sanity check -> */
	  if ( (diphone_table[indice_diphone].halfseg<=0) ||
	       (diphone_table[indice_diphone].nb_wframe<=0 )
	       )
	    {
	      fprintf(stderr,"**** HALFSEG OR NBFRAME IS WRONG on %s\n",name_raw);
	      incident++;
	    }

	  /* LITTLE_ENDIAN is now the standard database format */
#ifndef __i386__
	  diphone_table[indice_diphone].halfseg= ((diphone_table[indice_diphone].halfseg&0xFF00)>>8) |
	    ((diphone_table[indice_diphone].halfseg&0xFF)<<8);
#endif

	  /* Write the diphone name, append 0 for Zstrings */
	  fprintf(dba_out,"%s",diphone_table[indice_diphone].left);
	  fputc(0,dba_out);
	  fprintf(dba_out,"%s",diphone_table[indice_diphone].right);
	  fputc(0,dba_out);

	  fwrite(&diphone_table[indice_diphone].halfseg,
		 sizeof(diphone.halfseg),1,dba_out);
	  fwrite(&diphone_table[indice_diphone].nb_frame,
		 sizeof(diphone.nb_frame),1,dba_out);
	  fwrite(&diphone_table[indice_diphone].nb_wframe,
		 sizeof(diphone.nb_wframe),1,dba_out);
	  indice_diphone++;
	}
    }

  /*
   * Automatically define the _-_ diphone
   */
  if (init_silence==0)
    {
      int i;
      int nb_silence=NB_SILENCE;

      /* Write a wave of 0 */
      indice_raw+=(nb_silence+1)*Period;
      for(i=0; i<(nb_silence+1)*Period*2; i++)
	buffer[i]=0;

      fwrite(buffer,sizeof(short),(nb_silence+1)*Period,raw_out);

      /* Write to NV_REG pitchmarks -> 0 0 */
      indice_pm+=nb_silence;
      fwrite(buffer,sizeof(char),nb_silence,pm_out);

      fprintf(stderr,"NO _-_ diphone, automatically adds it\n");
      diphone_table[indice_diphone].left="_";
      diphone_table[indice_diphone].right="_";

      diphone_table[indice_diphone].halfseg= (int16) (nb_silence*Period/2*resamp_ratio);
      diphone_table[indice_diphone].nb_wframe= nb_silence+1;
      diphone_table[indice_diphone].nb_frame= nb_silence;


      printf("_-_: %s-%s %i %i %i end_raw=%li %li nb_frame=%i\n",
	     diphone_table[indice_diphone].left,
	     diphone_table[indice_diphone].right,
	     begin,end,middle,indice_raw, indice_pm,
	     diphone_table[indice_diphone].nb_frame);

      /* LITTLE_ENDIAN is now the standard database format */
#ifndef __i386__
      diphone_table[indice_diphone].halfseg= ((diphone_table[indice_diphone].halfseg&0xFF00)>>8) |
	((diphone_table[indice_diphone].halfseg&0xFF)<<8);
#endif

      fprintf(dba_out,"%s",diphone_table[indice_diphone].left);
      fputc(0,dba_out);
      fprintf(dba_out,"%s",diphone_table[indice_diphone].right);
      fputc(0,dba_out);

      fwrite(&diphone_table[indice_diphone].halfseg,
	     sizeof(diphone.halfseg),1,dba_out);
      fwrite(&diphone_table[indice_diphone].nb_frame,
	     sizeof(diphone.nb_frame),1,dba_out);
      fwrite(&diphone_table[indice_diphone].nb_wframe,
	     sizeof(diphone.nb_wframe),1,dba_out);
      /* One more has been inserted ! */
      indice_diphone++;
    }
  else if ( init_silence != indice_diphone-1 )
    {
      fprintf(stderr,"******************************************************\n");
      fprintf(stderr,"SEVERE WARNING: mbrola assumes _-_ is the last diphone\n");
      fprintf(stderr,"_-_ is defined at line %i\n", init_silence);
    }
  fclose(dba_in);

  /*
   * Open the diphone copy file
   */
  sprintf(name,"%s.cpy",dba_name);
  if ((dba_in=fopen(name,"rb"))==NULL)
    {
      fprintf(stderr,"NO DIPHONE COPIES!\n");
    }
  else
    {
      /* Second part -> scan duplicate diphone definition */
      while (!feof(dba_in))
	{
	  strcpy(line,"");  /* needed to avoid bad processing of blank lines at the end of the file. */
	  fgets(line, 255,dba_in);

	  if (sscanf(line,"%s %s %s %s",name_left,name_right,copy_nameleft,copy_nameright) !=4 )
	    { fprintf(stderr,"Ooops on line (.cpy) : %s\n",line); }
	  else
	    {
	      /* The format is the one of a diphone copy */
	      fprintf(dba_out,"%s",name_left);
	      fputc(0,dba_out);
	      fprintf(dba_out,"%s",name_right);
	      fputc(0,dba_out);

	      fprintf(dba_out,"%s",copy_nameleft);
	      fputc(0,dba_out);
	      fprintf(dba_out,"%s",copy_nameright);
	      fputc(0,dba_out);

	      printf("Copy %s-%s into %s-%s\n",name_left,name_right,copy_nameleft,copy_nameright);		 	       indice_diphone++;
	    }
	}
      fclose(dba_in);
    }

  fclose(dba_out);
  fclose(raw_out);
  fclose(pm_out);

  /*
   * And now ... ladies and gentlemen, something completely different
   */

  /* check if we can safely continue */
  if (incident!=0)
    {
      fprintf(stderr,"****** INCIDENT=%i\n",incident);
      exit(1);
    }

  /* Write the header */
  strcpy(name,dba_name);
  if ((big_one=fopen(name,"wb"))==NULL)
    {
      fprintf(stderr,"CAN'T WRITE %s!\n",name);
      exit(1);
    }

  fprintf(big_one,"MBROLA%5s",Version);

  SizeMrk= indice_pm;
  SizeRaw= indice_raw*2;
  SizeDba= indice_diphone; /*new database format (.2,02)*/

#ifndef __i386__
  SizeDba=((SizeDba&0xFF00)>>8) | ((SizeDba&0xFF)<<8);

  SizeMrk=(((SizeMrk&0xFF)<<24) |
	   ((SizeMrk&0xFF00)<<8) |
	   ((SizeMrk&0xFF0000)>>8) |
	   ((SizeMrk>>24)&0xFF));
  Freq=((Freq&0xFF00)>>8) | ((Freq&0xFF)<<8);
  SizeRaw=(((SizeRaw&0xFF)<<24) |
	   ((SizeRaw&0xFF00)<<8) |
	   ((SizeRaw&0xFF0000)>>8) |
	   ((SizeRaw>>24)&0xFF));
#endif

  fwrite(&SizeDba,sizeof(int16),1,big_one);

  {
    int16 newdba=0;
    fwrite(&newdba,sizeof(int16),1,big_one);
  }

  fwrite(&SizeMrk,sizeof(int32),1,big_one);
  fwrite(&SizeRaw,sizeof(int32),1,big_one);
  fwrite(&Freq,sizeof(int16),1,big_one);

  /* Hack for periods > 255
     This can easily be the case if the sampling rate is higher than 16Khz
     */
  if (Period>255)
    {
      OldPeriod=0;
      fwrite(&OldPeriod,sizeof(int8),1,big_one);
      fwrite(&Period,sizeof(int16),1,big_one);
    }
  else
    {
      OldPeriod=Period;
      fwrite(&OldPeriod,sizeof(int8),1,big_one);
    }

  fwrite(&Coding,sizeof(int8),1,big_one);
  /* NB : Header = 25 bytes */

  /* Copy the database */
  tmp=fopen("SORTIE.dba","rb");
  while ((nb_read=fread(buffer,1,BUFFER_SIZE,tmp))!=0)
    fwrite(buffer,1,nb_read,big_one);
  fclose(tmp);

  /* Copy and compress in a stream the pitch marks */
  {
    unsigned char pmval=0;
    int pmshift=0;

    tmp=fopen("SORTIE.mrk","rb");

    while ((nb_read=fread(buffer,1,BUFFER_SIZE,tmp))!=0)
      {
	for (i=0;i<nb_read;i++)
	  {
	    /* Shift 2 by 2 */
	    pmval=pmval>>2;

	    /* Voiced=1 Unvoiced=0 */
	    pmval+= buffer[i]<<6;
	    pmshift+=2;

	    if (pmshift==8)
	      {
		fwrite(&pmval,1,1,big_one);
		pmshift=0;
		pmval=0;
	      }
	  }
      }

    /* Flush the bitstream */
    if (pmshift!=0)
      {
	pmval= pmval>>(8-pmshift);
	fwrite(&pmval,1,1,big_one);
      }
    fclose(tmp);
  }

  /* Copy the waves */
  tmp=fopen("SORTIE.raw","rb");
  while ((nb_read=fread(buffer,1,BUFFER_SIZE,tmp))!=0)
    fwrite(buffer,1,nb_read,big_one);
  fclose(tmp);

  /* Trailing Information notice if the file exists */
  sprintf(name,"%s.info",dba_name);
  if ( (tmp=fopen(name,"rb")) != 0)
    {
      printf("INFO FILE %s DETECTED\n",name);
      while ( (count=fread(buffer,1,BUFFER_SIZE,tmp))!= 0 )
	{
	  fwrite(buffer,count,1,big_one);
	}
      fclose(tmp);
    }

  if ( (tmp=popen("date","r")) != 0)
    {
      while (fgets(buffer,BUFFER_SIZE,tmp)!=NULL)
	{
	  fprintf(big_one,"Built on %s",buffer);
	  printf("Built on %s",buffer);
	}
      pclose(tmp);
    }

  /* Database building info, F0 Parameters */
  sprintf(name,"%s.f0",dba_name);
  if ( (tmp=fopen(name,"rb")) != 0)
    {
      separator=0;
      putc(separator,big_one);
      separator=255;
      putc(separator,big_one);  /* Escape sequence ! */
      printf("F0 FILE %s DETECTED\n",name);
      while ( (count=fread(buffer,1,BUFFER_SIZE,tmp))!= 0 )
	{
	  fwrite(buffer,count,1,big_one);
	}
      fclose(tmp);

      sprintf(name,"%s.mbe",dba_name);
      if ( (tmp=fopen(name,"rb")) != 0)
	{
	  printf("MBE FILE %s DETECTED\n",name);
	  while ( (count=fread(buffer,1,BUFFER_SIZE,tmp))!= 0 )
	    {
	      fwrite(buffer,count,1,big_one);
	    }
	  fclose(tmp);
	}

      sprintf(name,"%s.syn",dba_name);
      if ( (tmp=fopen(name,"rb")) != 0)
	{
	  printf("SYN FILE %s DETECTED\n",name);
	  while ( (count=fread(buffer,1,BUFFER_SIZE,tmp))!= 0 )
	    {
	      fwrite(buffer,count,1,big_one);
	    }
	  fclose(tmp);
	}
      sprintf(name,"%s.ill",dba_name);
      if ( (tmp=fopen(name,"rb")) != 0)
	{
	  printf("ILL FILE %s DETECTED\n",name);
	  while ( (count=fread(buffer,1,BUFFER_SIZE,tmp))!= 0 )
	    {
	      fwrite(buffer,count,1,big_one);
	    }
	  fclose(tmp);
	}
      sprintf(name,"%s.build",dba_name);
      if ( (tmp=fopen(name,"rb")) != 0)
	{
	  printf("BUILD FILE %s DETECTED\n",name);
	  while ( (count=fread(buffer,1,BUFFER_SIZE,tmp))!= 0 )
	    {
	      fwrite(buffer,count,1,big_one);
	    }
	  fclose(tmp);
	}
    }

  /* Trailing Copyright notice */
  separator=0;
  putc(separator,big_one);
  fprintf(big_one,"Copyright (c) 1995-2019 Faculté Polytechnique de Mons - TCTS lab\n");
  fclose(big_one);

  sprintf(sys,"/bin/rm -f SORTIE.*"); /* well well... */
  system(sys);

  return 0;
}


