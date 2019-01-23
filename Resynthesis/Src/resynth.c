char ProgramInfo[]=
"* This program does diphone database resynthesis               \n"
"*                                                              \n"
"* HISTORY:                                                     \n"
"*                                                              \n"
"* Created by V. Pagel 15/07/96 , from Dutoit's Turbo pascal 96 \n"
"* 04/12/96 VP: Bug with OmegaT and omegaT2 for phase continuity\n"
"* 18/07/97 VP: debug version with gnuplot, eer, Matlab :-)     \n"
"* 16/09/97 VP: merge database resampling                       \n"
"* 05/12/97 VP: smooth freq above 4Khz to avoid HF hamonicity   \n";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifdef __i386__
 #define LITTLE_ENDIAN
#else
 #define BIG_ENDIAN
#endif


#define BUFFER_SIZE 400 /* resynthesis buffer for a frame  */
#define MAX_HARMO 80
#define MAX_FRAMES 200

#define UNVOICED_BANDS 53		  /* Number of sigma noise */

#define VOICED   2
#define UNVOICED 0
#define TRA 1
#define REG 0

typedef struct
{
  double Sigma_noise[MAX_HARMO];
  double Amp_Harmo[MAX_HARMO];
  double Phase_Harmo[MAX_HARMO];
  double F0;							  /* Fundamental frequency */
  double LSError;	              /* Residual noise after least square optimization */
  double Energy;					  /* Energy of the frame   */
  int NbTeeth;						  /* Nb harmo in amp and phase */
  char Type;							  /* Voiced, unvoiced, Transitory, Stationary */
} A_frame;

typedef struct
{
  int WindLength;	  /* Length of the analysis window in samples */
  int WindShift;	  /* IMPORTANT: it gives frame size in samples */
  long int FirstSpl;	  /* First sample in the orig. s16 file */
  long int LastSpl; 	  /* Last sample in the orig. s16 file */
  int Frame_count;  /* Number of frames in *Frame */
  A_frame *Frame;  /* MBE windows */
} MBE_analysis;

MBE_analysis MbeGlob;					  /* Main variable */

char *mbe_file_name;

double VUVLimVoiced=1.0,
  ElimVoiced=5e8,
  ElimTrans=5e8,
  VUVLimTra=0.55;
int LastPhaseReset=10;

int HarmMatch=-1;	  /* Negative means no matching */

int SourceFreq;  /* Sample rate of original file */

#ifndef RESAMPLING
int TargetFreq;		  /* Target freq is the same as source */
#else
extern int TargetFreq;    /* Alien sample rate for resynthesis */
extern void filter_file(char *name_filtered, char *name_source);
#endif

double FreqRatio;  /* TargetFreq / SourceFreq; */
int Period;				  /* is equal to Windshift modulo resampling */

/*
 * Pseudo random initial phases ;-)
 */
double PhaseResynth[]={
 9.37407054658961E-0001,
 4.80868770629756E-0001,
 1.84859862628903E+0000,
 3.21233255651159E+0000,
 1.26479917629957E-0001,
 2.78410476969065E+0000,
 3.26957766286457E+0000,
 4.81630360778935E-0001,
 3.23609862744839E+0000,
 1.65308085449045E-0001,
 8.98513225198854E-0001,
 2.02706339103931E+0000,
 1.12619328200134E+0000,
 1.69440111142651E+0000,
 6.59436562799956E-0001,
 1.50396430874049E+0000,
 1.68558090288396E+0000,
 1.27764381289258E+0000,
 2.11497888246480E+0000,
 2.44288208082335E+0000,
 7.90573674713342E-0001,
 3.33525883087663E-0001,
 1.83396145872301E+0000,
 4.17068561256962E+0000,
 1.88968681449863E-0001,
 2.07285893966816E+0000,
 4.89770011261941E+0000,
 4.03111577899641E-0001,
 7.30102856332050E-0002,
 2.40908147413371E-0001,
 2.29094917386398E-0001,
 6.50029592035557E-0001,
 5.51733079742248E+0000,
 2.13944741300271E+0000,
 5.29692015063523E-0002,
 1.53888520800138E+0000,
 3.86080896265423E-0001,
 2.08538269371764E+0000,
 2.95965088791240E-0001,
 1.02906489136074E+0000,
 3.01913587000311E+0000,
 2.42660882399278E+0000,
 1.91733167619713E+0000,
 2.80888696061724E+0000,
 1.38683839718842E+0000,
 6.74399641240895E-0002,
 6.37318535062815E-0001,
 2.62843357394941E+0000,
 8.94479714444574E-0002,
 4.89723402103968E+0000,
 2.13257072636083E+0000,
 5.29900223681589E+0000,
 1.25981088019002E+0000,
 1.11281128382061E+0000,
 2.63970891278654E+0000,
 1.54720878512355E+0000,
 4.01549677796167E+0000,
 1.68658440016781E-0001,
 4.72127039150312E-0001,
 1.09019813230487E-0001,
 4.92244260253952E+0000,
 3.12660696184827E+0000,
 5.93667343224316E-0001,
 1.56215789238681E-0001,
 1.52836539405973E-0001,
 1.00754075197623E+0000,
 1.75145701224881E+0000,
 1.26106160942694E+0000,
 1.97502763989165E+0000,
 2.31943466508653E+0000,
 2.81448672847817E+0000,
 4.85735819937036E+0000,
 3.48978328863785E+0000,
 2.83769569238705E-0002,
 3.50049269982085E-0001,
 3.16610735267735E-0001,
 4.52812720230313E-0001,
 2.54043372127026E-0001,
 9.36920039328555E-0001,
 1.37312257900621E-0001
 };

#ifdef DEBUG
/* DEBUG globals !! */
FILE* matlab_file; /* resynth data for matlab viewing */
FILE* matlab_mbe;  /* mbe data for matlab */
#endif

float weight[512];    /* Hamming weighting window */

/*
 * Fills mbe structure
 */
void read_mbe(char *file_name,MBE_analysis *mbe)
{
  FILE *mbe_file;
  char line[1024];
  int i,pos,new_pos;
  int tot_frame;
  double value;

 if ((mbe_file=fopen(file_name,"rb"))==NULL)
	{
	  fprintf(stderr,"FATAL ERROR with MBE file %s !\n",file_name );
	  exit(1);
	}

 if (4!=fscanf(mbe_file," %i %i %li %li \n",
		  &mbe->WindLength,&mbe->WindShift,&mbe->FirstSpl,&mbe->LastSpl))
	{
	  fprintf(stderr,"PANIC: not a MBE file 1 %s\n",file_name);
	  exit(1);
	}

 tot_frame= ((mbe->WindLength - mbe->WindShift)/2 + mbe->LastSpl - mbe->FirstSpl)
	/ mbe->WindShift;

 mbe->Frame=(A_frame *) calloc(tot_frame+1,sizeof(A_frame));

 mbe->Frame_count=0;
 while (!feof(mbe_file))
	{
	  /* Read noise bands */
	  fgets(line,sizeof(line),mbe_file);
	  pos=0;

	  for(i=0;i<MAX_HARMO;i++)
		 {
			if (1==sscanf(&line[pos]," %lf %n",&value,&new_pos))
			  {
				 if (!(abs(value)>1e27))
					{
					  mbe->Frame[mbe->Frame_count].Sigma_noise[i]=value;
					  pos+=new_pos;
					}
				 else
					{
					  fprintf(stderr,"FATAL ERROR MBE file %s !\n",file_name );
					  fprintf(stderr,"at line %s \n",line);
					  exit(1);
					}
			  }

			else
			  break;
		 }

	  sscanf(&line[pos]," %n",&new_pos);
	  if (new_pos+pos!= strlen(line))
		 {
			fprintf(stderr,"FATAL ERROR MBE file %s !\n",file_name );
			fprintf(stderr,"at line %s \n",line);
			exit(1);
		 }

	  if (i!=UNVOICED_BANDS)
		 {
			fprintf(stderr,"PANIC: Harmo number changes %i %s\n",i,file_name);
			exit(1);
		 }

	  /* Read Harmonics amplitude */
	  fgets(line,sizeof(line),mbe_file);
	  pos=0;

	  for(i=0;i<MAX_HARMO;i++)
		 {
			if (1==sscanf(&line[pos]," %lf %n",&value,&new_pos))
			  {
				 if (!(abs(value)>1e27))
					{
					  mbe->Frame[mbe->Frame_count].Amp_Harmo[i]=value;
					  pos+=new_pos;
					}
				 else
					{
					  fprintf(stderr,"FATAL ERROR MBE file %s !\n",file_name );
					  fprintf(stderr,"at line %s \n",line);
					  exit(1);
					}
			  }
			else
			  break;
		 }

	  sscanf(&line[pos]," %n",&new_pos);
	  if (new_pos+pos!= strlen(line))
		 {
			fprintf(stderr,"FATAL ERROR MBE file %s !\n",file_name );
			fprintf(stderr,"at line %s \n",line);
			exit(1);
		 }

	  mbe->Frame[mbe->Frame_count].NbTeeth=i;

	  /* Read Harmonics' phase */
	  fgets(line,sizeof(line),mbe_file);
	  pos=0;

	  for(i=0;i<MAX_HARMO;i++)
		 {
			if (1==sscanf(&line[pos]," %lf %n",&value,&new_pos))
			  {
				 if (!(abs(value)>1e27))
					{
					  mbe->Frame[mbe->Frame_count].Phase_Harmo[i]=value;
					  pos+=new_pos;
					}
				 else
					{
					  fprintf(stderr,"FATAL ERROR MBE file %s !\n",file_name );
					  fprintf(stderr,"at line %s \n",line);
					  exit(1);
					}
			  }
			else
			  break;
		 }

	  sscanf(&line[pos]," %n",&new_pos);
	  if (new_pos+pos!= strlen(line))
		 {
			fprintf(stderr,"FATAL ERROR MBE file %s !\n",file_name );
			fprintf(stderr,"at line %s \n",line);
			exit(1);
		 }

	   if (i!=mbe->Frame[mbe->Frame_count].NbTeeth)
		 {
			fprintf(stderr,"PANIC: Harmo number changes2 %i %s\n",i,file_name);
			exit(1);
		 }

		/* Read the last 2 vals */
		if (3!=fscanf(mbe_file," %lf \n %lf %lf \n",
						  &mbe->Frame[mbe->Frame_count].F0,
						  &mbe->Frame[mbe->Frame_count].LSError,
						  &mbe->Frame[mbe->Frame_count].Energy ))
		  {
			 fprintf(stderr,"PANIC: not a MBE file 2 %s\n",file_name);
			 exit(1);
		  }

		if ((++ mbe->Frame_count)>tot_frame)
		  {
			 fprintf(stderr,"PANIC: frame count %s\n",file_name);
			 exit(1);
		  }
	}
}


void write_mbe(char *file_name,MBE_analysis *mbe)
{
  FILE *mbe_file;
  int i,j;

  if ((mbe_file=fopen(file_name,"w"))==NULL)
	 {
		fprintf(stderr,"FATAL ERROR with MBE file %s !\n",file_name );
		exit(1);
	 }

  fprintf(mbe_file," %i %i %li %li \n",
			 mbe->WindLength,mbe->WindShift,mbe->FirstSpl,mbe->LastSpl);

  for(i=0;i<mbe->Frame_count;i++)
	 {
		for(j=0;j<53;j++)
		  fprintf(mbe_file," %f ",mbe->Frame[i].Sigma_noise[j]);
		fprintf(mbe_file,"\n");

		for(j=0;j<mbe->Frame[i].NbTeeth;j++)
		  fprintf(mbe_file," %f ",mbe->Frame[i].Amp_Harmo[j]);
		fprintf(mbe_file,"\n");
		for(j=0;j<mbe->Frame[i].NbTeeth;j++)
		  fprintf(mbe_file," %f ",mbe->Frame[i].Phase_Harmo[j]);
		fprintf(mbe_file,"\n");

		fprintf(mbe_file," %f \n %f %f \n",
				  mbe->Frame[i].F0,
				  mbe->Frame[i].LSError,
				  mbe->Frame[i].Energy );
	 }

  fclose(mbe_file);
}


/*
 * Synthesize a voiced frame
 */
void Harmonic_synthesis_order0(double F0,
										 double *Amp,
										 double *Phases,
										 int NbPoints,
										 int NbSin,
										 int ResynthFreq,
										 double *Buffer)
{
  int i,k;
  double tmp=2*M_PI*F0/(double)ResynthFreq;

  /* init the buffer */
  for (i=0; i<NbPoints; i++)
	 Buffer[i]=0;

  /* Add cosinus */
  for(k=0; k<NbSin; k++)
	 {
		// Below an attempt to disminish the contribution of HF on the borderof the frames
		/* test if we're higher than 4Khz */
		//		if ( (k+1)*F0 > 4000 )
		// {
		// /* b is 1 at 4000 and 0 at Resynth Freq */
		// /* smoothing triangular win is ax+b, corners lowering slowly */
		// double b= 1.0 - ((k+1)*F0-4000.0)/(double)ResynthFreq;
		// double a= (1.0 - b)*2.0/(double)NbPoints;
		//
		// if ( (b>1.0) || (b<0.0))
		//	{
		//	  fprintf(stderr,"coef panic\n");
		//	  exit(1);
		//	}
		// for (i=0;i<NbPoints/2;i++)
		//	weight[i]= (a*(double)i + b);
		// }
		// else
		//  {
		//	 for (i=0;i<NbPoints;i++)
		//		weight[i]= 1.0;
		// }
		for (i=0; i<NbPoints; i++)
			 Buffer[i]+= Amp[k]*cos( Phases[k] + i*((k+1)*tmp));
	 }
}

/*
 * Duplicate source in res, and change F0 to Match_Freq
 */
void MatchFreq(double NewF0, MBE_analysis *source, MBE_analysis *dest)
{
  int NewNbTeeth;
  int i,j,k;
  double CurFreq,OldFreq,OldF0,a;
  double PrevDeltaF0=0;
  double omegaT=0.0;

  /* Clone */
  *dest= *source;

  dest->Frame= (A_frame *) calloc(source->Frame_count,sizeof(A_frame));

  for (i=0; i< source->Frame_count; i++)
	 {
		/* Copy the bunch, we'll modify it later anyway */
		dest->Frame[i]=source->Frame[i];

		NewNbTeeth= TargetFreq/NewF0/2; /* Don't overcome 16000Hz/2 */
		if (NewNbTeeth>MAX_HARMO)
		  NewNbTeeth=MAX_HARMO;

		CurFreq=NewF0;
		OldF0=OldFreq= source->Frame[i].F0;

		/*
		 * Linear interpolation of harmonic's amplitudes
		 */
		for (j=0,k=0 ; j< NewNbTeeth; j++,CurFreq+= NewF0 )
		  {
			 /* Lack data on the left side ? -> duplicate first */
			 if (CurFreq < OldFreq)
				dest->Frame[i].Amp_Harmo[j] = source->Frame[i].Amp_Harmo[0];
			 else
				{
				  /* Move the Old cursor for linear interpolation */
				  while (CurFreq > OldFreq + OldF0)
					 {
						OldFreq+=OldF0;
						k++;
					 }

				  /* We're still in the defined area ? */
				  if (k < source->Frame[i].NbTeeth-1)
					 {
						/* linear slope */
						a=(source->Frame[i].Amp_Harmo[k+1] - source->Frame[i].Amp_Harmo[k])
						  /source->Frame[i].F0;

						dest->Frame[i].Amp_Harmo[j]= a* (CurFreq-OldFreq)
						  + source->Frame[i].Amp_Harmo[k];
					 }
				  else
					 {
						/* Duplicate last tooth one time (MODIF Pagel) */
						dest->Frame[i].Amp_Harmo[j]=source->Frame[i].Amp_Harmo[source->Frame[i].NbTeeth-1];
						/* Random phases for new harmonics */
						dest->Frame[i].Phase_Harmo[j]= 2*M_PI*drand48();
						NewNbTeeth=j+1;
						break;
					 }
				}
		  }

		/* Tune the real number of available harmonics */
		dest->Frame[i].F0=NewF0;
		dest->Frame[i].NbTeeth=NewNbTeeth;

		/*
		 * Manage something with harmonic's phases
		 * First frame is left untouched
		 */
		if (i!=0)
		  {
			 double omegaT2;

			 /* 2pi*omega* T */
			 omegaT += 2*M_PI*PrevDeltaF0/SourceFreq * source->WindShift;

			 /* Modify phases previously copied */
			 for(j=0; j<source->Frame[i].NbTeeth; j++)
				dest->Frame[i].Phase_Harmo[j]+= (j+1) * omegaT;

			 /* Create phases that did'nt exist previously */
			 omegaT2= 2*M_PI* dest->Frame[i-1].F0/SourceFreq * source->WindShift;
			 for( ; j<NewNbTeeth; j++)
				{
				  if (j< dest->Frame[i-1].NbTeeth)
					 /* If we can, copy from previous frame + delta continuity
						 else leave random phases
					  */
					 dest->Frame[i].Phase_Harmo[j]= dest->Frame[i-1].Phase_Harmo[j]
						+ (j+1) * omegaT2;
				}

			 /* Fit phases modulo 2Pi */
			 for(j=0; j<MAX_HARMO; j++)
				{
				  while (dest->Frame[i].Phase_Harmo[j] > 2*M_PI)
					 dest->Frame[i].Phase_Harmo[j]-= 2.0*M_PI;

				  while (dest->Frame[i].Phase_Harmo[j] < 0)
					 dest->Frame[i].Phase_Harmo[j]+= 2.0*M_PI;
				}
		  }
		PrevDeltaF0= NewF0-OldF0;
	 }
}


/*
 * Tag the frames with Voiced and Unvoiced attribute, and
 * make decision for Transitory or stationary mode
 */
void ComputeVNV(MBE_analysis *mbe)
{
  int i,j;
  double CstFreq=100.0;
  double  VoicedEnergy[MAX_FRAMES],
	 UnvoicedEnergy[MAX_FRAMES],
	 VNVRatio[MAX_FRAMES];
  MBE_analysis mbe_temp;

  /* Spectrum at 100Hz for normalization purpose */
  MatchFreq(CstFreq,mbe,&mbe_temp);

  if (mbe_temp.Frame_count>MAX_FRAMES)
	 {
		fprintf(stderr,"FATAL FRAME ERROR\n");
		exit(1);
	 }

  for(i=0;i<mbe_temp.Frame_count;i++)
	 {
		VoicedEnergy[i]=UnvoicedEnergy[i]=0.0;
		for(j=0; j<mbe_temp.Frame[i].NbTeeth/8; j++)
		  {
			 VoicedEnergy[i]+= mbe_temp.Frame[i].Amp_Harmo[j] * mbe_temp.Frame[i].Amp_Harmo[j];
			 UnvoicedEnergy[i]+= mbe_temp.Frame[i].Sigma_noise[j] * mbe_temp.Frame[i].Sigma_noise[j] ;
		  }

		VoicedEnergy[i] *= CstFreq * M_PI * M_PI;	/* Overestimation of the voiced Energy */
		UnvoicedEnergy[i] *= 150;

		if (VoicedEnergy[i]!=0.0)
		  VNVRatio[i]= UnvoicedEnergy[i]/VoicedEnergy[i];
		else
		  VNVRatio[i]=0.0;

		/* Voiced Unvoiced decision */
		if ( ((UnvoicedEnergy[i]==0.0) || ((VoicedEnergy[i]/UnvoicedEnergy[i]) < VUVLimVoiced))
			  || ((VoicedEnergy[i]+UnvoicedEnergy[i])< ElimVoiced))
		  mbe->Frame[i].Type= UNVOICED;
		else
		  mbe->Frame[i].Type= VOICED;

		/* Stationary or transitory decision */
		if (mbe->Frame[i].Type==UNVOICED)
		  {
			 if (UnvoicedEnergy[i]> ElimTrans)
				mbe->Frame[i].Type+= TRA;
			 else
				mbe->Frame[i].Type+= REG;
		  }
		else
		  {
			 if (VNVRatio[i]> VUVLimTra)
				mbe->Frame[i].Type+= TRA;
			 else
				mbe->Frame[i].Type+= REG;
		  }

#ifdef DEBUG
		{ /* DEBUG */
		  int type;

		  switch (mbe->Frame[i].Type)
			 {
			 case 0:
				type=0;
				break;
			 case 1:
				type=1;
				break;
			 case 2:
				type=3;
				break;
			 case 3:
				type=2;
				break;
			 }

		  /* DEBUG */
		  fprintf(matlab_file," %f %f %f %i ; \n",
					 VNVRatio[i],
					 VoicedEnergy[i],
					 UnvoicedEnergy[i],
					 type
					 );
		}
#endif
	 }
}

double resynth(char *s16_file_name, char *output_s16, char *output_pm, double gain)
{
  FILE *orig_file, *output_file, *pm_file;
  double ConstF0;
  MBE_analysis mbe;
  int current_sample;
  int i,j;
  double buffer[BUFFER_SIZE];
  double max_clip=0.0;
  short ibuff[BUFFER_SIZE];

#ifdef RESAMPLING
 char name_filtered[255];

#ifdef RESAMPLING16_8
 sprintf(name_filtered,"%s.s8",s16_file_name);
#endif
#ifdef RESAMPLING16_11
 sprintf(name_filtered,"%s.s11",s16_file_name);
#endif

  /* Won't compute again if it's allready resampled */
  filter_file(name_filtered, s16_file_name);

  /* Pretend everything is as usual ! */
  s16_file_name=name_filtered;
#endif

  if ((orig_file=fopen(s16_file_name,"rb"))==NULL)
	 {
		fprintf(stderr,"FATAL ERROR with orig file %s!\n",s16_file_name);
		exit(1);
	 }

  ComputeVNV(&MbeGlob);

  ConstF0= (double)SourceFreq / MbeGlob.WindShift;
  MatchFreq(ConstF0,&MbeGlob,&mbe);

  if (mbe.WindShift>BUFFER_SIZE)
    {
      fprintf(stderr,"FATAL ERROR Shift value>%i\n",BUFFER_SIZE);
      exit(1);
    }

  if ((output_file=fopen(output_s16,"w"))==NULL)
	 {
		fprintf(stderr,"FATAL ERROR with file %s!\n",output_s16);
		exit(1);
	 }
  if ((pm_file=fopen(output_pm,"w"))==NULL)
	 {
		fprintf(stderr,"FATAL ERROR with file %s!\n",output_pm);
		exit(1);
	 }

  current_sample= (double) (mbe.FirstSpl - mbe.WindShift / 2) * FreqRatio;

  for(i=0; i<mbe.Frame_count; i++)
	 if (mbe.Frame[i].Type&VOICED)
		{	/* Voiced Frame -> synthesis */

		  /* shift the real phases to match the fixed one */
		  double deltaphi0;
		  if (HarmMatch<0)		  /* No harmonic to match */
			 deltaphi0 = 0;
		  else
			 deltaphi0 = (mbe.Frame[i].Phase_Harmo[HarmMatch] - PhaseResynth[HarmMatch])/(HarmMatch+1);

		  /* FEMALE Hack */
#ifdef FEMALE
		  /* Copy upper frequency phases (the first are fixed)  */
		  for(j=LastPhaseReset; j<mbe.Frame[i].NbTeeth ;j++)
			 PhaseResynth[j]=mbe.Frame[i].Phase_Harmo[j/2];
#else
		  /* Copy upper frequency phases (the first are fixed)  */
		  for(j=LastPhaseReset; j<mbe.Frame[i].NbTeeth ;j++)
			 PhaseResynth[j]=mbe.Frame[i].Phase_Harmo[j] - deltaphi0*(j+1);
#endif

		  /* Synthesize one period */
		  Harmonic_synthesis_order0(ConstF0,
											 mbe.Frame[i].Amp_Harmo,PhaseResynth,
											 Period,
											 mbe.Frame[i].NbTeeth,
											 TargetFreq,
											 buffer);

		  for(j=0; j<Period ;j++)
			 {
				buffer[j]*=gain;

				if ((buffer[j]>32767.0)||(buffer[j]<-32767.0))
				  {
					 fprintf(stderr,"ONE CLIP %f FILE:%s\n",buffer[j], mbe_file_name);
					 if (fabs(buffer[j])>max_clip)
						max_clip=fabs(buffer[j]);

					 if (buffer[j]>32767.0)
						ibuff[j]= 32767;
					 else
						ibuff[j]= -32767;
				  }
				else
				  ibuff[j]= (short) rint(buffer[j]);
			 }


			 /* For suns */
#ifndef LITTLE_ENDIAN
			 swab( (char *)ibuff, (char *)ibuff, Period*sizeof(short));
#endif

		  if (fwrite(ibuff,sizeof(short),Period,output_file) != Period)
			 {
				fprintf(stderr,"FATAL writing %i %s",i,output_s16);
				exit(1);
			 }
		  current_sample+= Period;
		}
	 else
		{	/* Unvoiced Frame -> read original */
		  int Shift;

		  if (fseek(orig_file, sizeof(short)*(current_sample-1),SEEK_SET)==-1)
			 {
				fprintf(stderr,"ERREUR Seek %i current_sampl=%i %s",i,current_sample,s16_file_name);
				exit(1);
			 }

		  /* If it's the first, or the previous one is voiced */
		  if ((i==0) || (mbe.Frame[i-1].Type&VOICED))
			 Shift= 2*Period;
		  else
			 Shift= Period;

		  if (fread(ibuff,sizeof(short),Shift,orig_file) != Shift)
			 {
				fprintf(stderr,"FATAL reading %i %s",i,s16_file_name);
				exit(1);
			 }

		  /* For suns */
#ifndef LITTLE_ENDIAN
		  swab( (char *)ibuff, (char *)ibuff,Shift*sizeof(short));
#endif

#ifdef SYBIL
		  /* Try to reduce sybilance */
		  for(j=0; j<Shift ; j++) ibuff[j] *= gain/2;
#else
		  for(j=0; j<Shift ; j++) ibuff[j] *= gain;
#endif

		  /* For suns */
#ifndef LITTLE_ENDIAN
		  swab( (char *)ibuff, (char *)ibuff, Shift*sizeof(short));
#endif

		  if (fwrite(ibuff,sizeof(short),Shift,output_file) != Shift)
			 {
				fprintf(stderr,"FATAL writing %i %s",i,output_s16);
				exit(1);
			 }
		  current_sample+= Shift;

		  /* If it's the last, tradeoff for initial 2 frames */
		  if ( (i!=mbe.Frame_count) && (mbe.Frame[i+1].Type&VOICED))
			 current_sample-=Period;
		}
  fclose(output_file);

  /* Pitch mark type */
  for(i=0;i<MbeGlob.Frame_count; i++)
	 fwrite(&MbeGlob.Frame[i].Type,sizeof(char),1,pm_file);

  fclose(pm_file);

  return(max_clip);
}

int main(int argc, char **argv)
{
  char *s16_file_name, output_s16[255],output_pm[255],output_matlab[255];
  double gain;

  if ((argc!=5)&&(argc!=10))
	 {
		fprintf(stderr,ProgramInfo);
		fprintf(stderr,"USAGE: %s input_s16_file input_mbe_file ouput_filename\n"
				  "[ Freq=%i PhaseReset=%i VUVtra=%f VUVvoice=%f ElimTra=%f ElimVoice=%f ]\n",
				  argv[0], SourceFreq, LastPhaseReset, VUVLimTra, VUVLimVoiced,
				  ElimTrans, ElimVoiced);
		exit(1);
	 }

  if (argc==10)
	 {
		SourceFreq=atoi(argv[4]);
		LastPhaseReset=atoi(argv[5]);

		VUVLimTra= atof(argv[6]);
		VUVLimVoiced= atof(argv[7]);

		ElimTrans= atof(argv[8]);
		ElimVoiced= atof(argv[9]);
	 }
  else
	 {
		FILE *init_file;
		int read_phase=0;

		if ((init_file=fopen(argv[4],"rt"))==NULL)
		  {
			 fprintf(stderr,"FATAL ERROR with file %s!\n",argv[4]);
			 exit(1);
		  }

		while (!feof(init_file))
		  {
			 char line[1024];
			 if (!fgets(line,sizeof(line),init_file))
				break;

			 if ( (sscanf(line," Freq = %i ",&SourceFreq)!=1) &&
					(sscanf(line," LastPhaseReset = %i ",&LastPhaseReset)!=1) &&
					(sscanf(line," HarmMatch = %i ",&HarmMatch)!=1) &&
					(sscanf(line," VUVLimTra = %lf ",&VUVLimTra)!=1) &&
					(sscanf(line," VUVLimVoiced = %lf ",&VUVLimVoiced)!=1) &&
					(sscanf(line," ElimTrans = %lf ",&ElimTrans)!=1) &&
					(sscanf(line," ElimVoiced = %lf ",&ElimVoiced)!=1))
				{
				  if (strncmp(line,"PhaseList",9)==0)
					 {
						read_phase=1;
						break;
					 }

				  fprintf(stderr,"Param: Freq LastPhaseReset VUVLimTra VUVLimVoiced ElimTrans ElimVoiced \n");
				  exit(1);
				}
		  }

		/*
		fprintf(stderr," Freq = %i ",SourceFreq);
		fprintf(stderr," LastPhaseReset = %i ",LastPhaseReset);
		fprintf(stderr," HarmMatch = %i ",HarmMatch);
		fprintf(stderr," VUVLimTra = %lf ",VUVLimTra);
		fprintf(stderr," VUVLimVoiced = %lf ",VUVLimVoiced);
		fprintf(stderr," ElimTrans = %lf ",ElimTrans);
		fprintf(stderr," ElimVoiced = %lf ",ElimVoiced);
		*/

		if (read_phase)
		  {
			 int i=0;
			 while (!feof(init_file))
				{
				  double phase;
				  if (fscanf(init_file," %lf ",&phase)==1)
					 PhaseResynth[i++]= phase;
				}
		  }
	 }

  /* No resampling -> initialize the Target as the Source */
#ifndef RESAMPLING
  TargetFreq=SourceFreq;
#endif

  FreqRatio = (double) TargetFreq / (double) SourceFreq;

  mbe_file_name=argv[2];
  s16_file_name=argv[1];

  read_mbe(mbe_file_name,&MbeGlob);

  /* Period of the resynthesized frames */
  Period= (int)((double) MbeGlob.WindShift*FreqRatio);

  /* Extension .s16 and .pm */
  strcpy(output_s16,argv[3]);
  strcpy(output_pm,argv[3]);
  strcat(output_s16,".s16");
  strcat(output_pm,".pm");

#ifdef DEBUG
  /* DEBUG */
  strcpy(output_matlab,argv[3]);
  strcat(output_matlab,".m");
  matlab_file=fopen(output_matlab,"wt");
  fprintf(matlab_file,"matrix=[ ");

  {
	 int i;
	 int j;

	 /* Donne'es debuggage MBE */
	 strcat(output_matlab,"m");
	 matlab_mbe=fopen(output_matlab,"wt");

	 fprintf(matlab_mbe,"F0=[ ");
	 for(i=0; i<MbeGlob.Frame_count; i++)
		fprintf(matlab_mbe,"%f ",MbeGlob.Frame[i].F0);
	 fprintf(matlab_mbe,"];\n Error=[");
	 for(i=0; i<MbeGlob.Frame_count; i++)
		fprintf(matlab_mbe,"%f ",MbeGlob.Frame[i].LSError);
	 fprintf(matlab_mbe,"];\n Energy=[");
	 for(i=0; i<MbeGlob.Frame_count; i++)
		fprintf(matlab_mbe,"%f ",MbeGlob.Frame[i].Energy);
	 fprintf(matlab_mbe,"];\n");
	 fprintf(matlab_mbe,
				"wav_file=fopen('%s','r','l'); "
				"fseek(wav_file,%i,-1); "
				"data=fread(wav_file,%i,'short');\n ",
				s16_file_name,
				MbeGlob.FirstSpl*2,
				MbeGlob.LastSpl-MbeGlob.FirstSpl);

	 fprintf(matlab_mbe,
				"figure(1); "
				"zoom on;"
				"subplot(411); plot(F0); grid; xlabel f0; "
				"subplot(412); plot(Energy); grid; xlabel energy;"
				"subplot(413); plot(Error); grid; xlabel error;"
				"subplot(414); plot(data); grid; xlabel %s;\n"
				"pause;\n",
				output_s16 );

	 for(j=0; j<MbeGlob.Frame_count; j++)
		{
		  fprintf(matlab_mbe,
					 "fseek(wav_file,%i,-1); "
					 "data=fread(wav_file,%i,'short');\n "
					 "data=data.*hamming(%i); ",
					 (MbeGlob.FirstSpl+ j*MbeGlob.WindShift - (MbeGlob.WindLength-MbeGlob.WindShift)/2 )*2,
					 MbeGlob.WindLength,
					 MbeGlob.WindLength );

		  fprintf(matlab_mbe,
					 "figure(2); "
					 "zoom on; "
					 "subplot(211); plot(data); grid; xlabel %s; \n",
					 output_s16);

		  fprintf(matlab_mbe,
					 "subplot(212); ");

		  fprintf(matlab_mbe,"resynth=zeros(%i,1);\n", MbeGlob.WindLength);

		  for(i=0; i<MbeGlob.Frame[j].NbTeeth; i++)
			 {
				if (MbeGlob.Frame[j].Phase_Harmo[i]>=0)
				  fprintf(matlab_mbe,
							 "resynth=resynth + cos( (%i:%i)'/16000*%f*2*pi + %f)* %f; \n ",
							 -MbeGlob.WindLength/2,
							 MbeGlob.WindLength/2-1,
							 MbeGlob.Frame[j].F0*(i+1),
							 MbeGlob.Frame[j].Phase_Harmo[i],
							 MbeGlob.Frame[j].Amp_Harmo[i] );
				else
				  fprintf(matlab_mbe,
							 "resynth=resynth+cos((%i:%i)'/16000*%f*2*pi %f)* %f ;\n",
							 -MbeGlob.WindLength/2,
							 MbeGlob.WindLength/2-1,
							 MbeGlob.Frame[j].F0*(i+1),
							 MbeGlob.Frame[j].Phase_Harmo[i],
							 MbeGlob.Frame[j].Amp_Harmo[i] );

			 }
		   fprintf(matlab_mbe,
					  "resynth=resynth.*hamming(%i); "
					  "plot(resynth); "
					  "xlabel %s-%i; "
					  "grid; "
					  "figure(3); "
					  "amp=20*log(abs(fft(data,1024))); "
					  "plot(amp(1:512),'c'); grid; xlabel f0=%f; \n"
					  "hold on; \n"
					  "data_rest= data-resynth; "
					  "amp=20*log(abs(fft(data_rest( %i : %i ),1024))); "
					  "plot(amp(1:512),'r'); hold off; "
					  "fprintf(1,'data=%%f synth=%%f sub=%%f\\n',std(data), std(resynth), std(data-resynth)); \n"
					  "pause; ",
					  MbeGlob.WindLength,
					  output_s16,
					  j,
					  MbeGlob.Frame[j].F0,
					  (MbeGlob.WindLength- MbeGlob.WindShift)/2,
					  (MbeGlob.WindLength+ MbeGlob.WindShift)/2,
					  j);
		}
	 fclose(matlab_mbe);
  }
#endif

  gain=resynth(s16_file_name,output_s16,output_pm,1.0);

#ifdef DEBUG
  /* DEBUG */
  fprintf(matlab_file,"];\n");
  fprintf(matlab_file,
          "subplot(411);"
			 "plot(matrix(:,2:3)); "
			 "xmax=max(matrix(:,2)); "
			 "axis([1 %i 0 xmax]); "
			 "xlabel VUVEner; "
			 "grid; \n ",
			 MbeGlob.Frame_count);

  fprintf(matlab_file,
			 "subplot(412); "
			 "xmax=max(matrix(:,1)); "
			 "if xmax>3\n xmax=3; \n end; \n"
			 "plot(matrix(:,1),'c'); "
			 "axis([1 %i 0 xmax]); "
			 "hold on; "
			 "line([0,%i],[%f,%f]); "
			 "line([0,%i],[%f,%f]); "
			 "hold off; "
			 "xlabel UV/V; "
			 "grid; \n ",
			 MbeGlob.Frame_count,
			 MbeGlob.Frame_count,
			 1/VUVLimVoiced,
			 1/VUVLimVoiced,
			 MbeGlob.Frame_count,
			 VUVLimTra,
			 VUVLimTra);

  fprintf(matlab_file,
			 " subplot(413);"
			 "grid;"
			 "plot(matrix(:,4));"
			 "axis([1 %i 0 3]);"
			 "xlabel 'Decision(0=UV,3=V)'; "
			 "grid;\n "
			 "subplot(414); \n",
			 MbeGlob.Frame_count);

  fprintf(matlab_file,
		  " wav_file=fopen('%s','r','l');"
		  "fseek(wav_file,%i,-1); "
		  "data=fread(wav_file,%i,'short');\n ",
		  s16_file_name,
		  MbeGlob.FirstSpl*2,
		  MbeGlob.LastSpl-MbeGlob.FirstSpl);

  fprintf(matlab_file,
			 "plot(data); "
			 "grid; "
			 "axis([1 %i -20000 20000]); "
			 "xlabel %s; "
			 "zoom on; "
			 "pause;\n",
			 MbeGlob.LastSpl-MbeGlob.FirstSpl,
			 output_s16);
  fclose(matlab_file);
#endif

  if (gain>1.0)
	 {
		fprintf(stderr,"GAIN LOWERED TO: %f",gain);
		gain=resynth(s16_file_name,output_s16,output_pm,32000.0/gain);
	 }
  return(0);
}
