// Copyright (c) 1995-2019 Faculté polytechnique de Mons (TCTS lab)
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// ============================================================================
// mbe.cpp
//
// functions: InnerProduct	(mbefunc.h)
//			  Interpola		(mbefunc.h)
//			  GetErrorValue	(mbefunc.h)
//			  PosMin		(mbefunc.h)
//			  LowPassFilter	(mbefunc.h)
//			  HammingWindow (mbefunc.h)
//			  Windowing		(mbefunc.h)
//			  AutoCorrelation (mbefunc.h)
//
// classes: MBE_Parameters	 (mbeparam.h)
//			FrameMBEPitch_A  (pitchana.h)
//			Voincing_A		 (pitchana.h)
//			MBEPitchAnalyser_A (pitchana.h)
//			MBEPitchAnalyserNoVoicing_A (pitchana.h) 
//			MBE_Analyser (mbeanaly.h)
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:		08.03.1996: Creation (MBE_Parameters)
//				11.03.1996: Implementation of the members functions
//				15.03.1996: Implementation of some functions (not classes)
//							Pitch analysis classes (from pitchmbe.cpp)
//				25.03.1996: MBE_Analyser_A 
//							Implementation of the private functions
//							Constructor
//				26.03.1996: IO_Def implementation
//							GetSinAmpPhase implementation
//				27.03.1996: Process implementation
//				08.05.1996: Debugging
//				15.05.1996: End of debugging and Setup File implemented for 
//							pitch analysis
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#ifndef _INC_STDLIB
#include <stdlib.h>
#endif

#ifndef _INC_MATH
#include <math.h>
#endif

#ifndef _INC_MBEPARAM
#include "mbeparam.h"
#endif

#ifndef _INC_MBEFUNC
#include "mbefunc.h"
#endif

#ifndef _INC_PITCHANA
#include "pitchana.h"
#endif 

#ifndef _INC_ERROREXC
#include "errorexc.h"
#endif

#ifndef _INC_CONSTANT
#include "constant.h"
#endif

#ifndef _INC_MBEANALY
#include "mbeanaly.h"
#endif

#ifndef _INC_SIGNALPR
#include "signalpr.h"
#endif

// ===========================================================================
// Class MBE_Parameters
// ===========================================================================

// Default constructor
MBE_Parameters::MBE_Parameters()
{
  NMaxTrchs=0;
  NMaxRaies=0;
  NRaiesNV=0;
  PhaseComp=FALSE;
  NTranches=0	;
  NbreRaies=NULL;
  NPoints=NULL;
  F0=NULL;
  Energy=NULL;
  LSError=NULL;
  Amp=NULL;	
  Phases=NULL;
  Sigma=NULL;
};

// ----------------------------------------------------------------------------

// Constructor: Name of the object
//				Number of frames 
//				Max number of frames
//              Max numbers of spectrum lines
//              Phase computation or not
//				Sample frequency
MBE_Parameters::MBE_Parameters(const char *Name,short int NumberOfFrames,
										 unsigned char NMaxFr,unsigned char NMaxRa,
										 unsigned char Comp,AnalyseParameters Fr)
{ 
  int i;
  FMObject::SetName(Name); 
  Param.Frequency=Fr.Frequency;
  Param.Width=Fr.Width;
  Param.Shift=Fr.Shift;
  Param.FirstSample=Fr.FirstSample;
  Param.LastSample=Fr.LastSample;
  NRaiesNV=(unsigned char)((Param.Frequency/2)/150); // Noise bands of 150 Hz
  NMaxRaies=NMaxRa;	
  NMaxTrchs=NMaxFr;
  PhaseComp=Comp;
  try 
	 {	// Memory allocation
		NbreRaies=new Vectoruc(NMaxTrchs,"NbreRaies");
		NPoints=new Vectorus(NMaxTrchs,"NPoints");
		F0=new Vectord(NMaxTrchs,"F0");
		Energy=new Vectord(NMaxTrchs,"Energy");
		LSError=new Vectord(NMaxTrchs,"LSError");
		Amp=new Vectord*[NMaxTrchs];
		Phases=new Vectord*[NMaxTrchs];
		Sigma=new Vectord*[NMaxTrchs];
		for(i=0;i<NMaxTrchs;i++)
		  {
			 Amp[i]=new Vectord(NMaxRaies,"Amp[i]");
			 Sigma[i]=new Vectord(NRaiesNV,"Sigma[i]");
			 Phases[i]=new Vectord(NMaxRaies,"Phases[i]");
		  }
	 }
  catch (FMError& Error1)
	 {	// Create a	new error message
		throw FMError("\nMBE_Parameters constructor\n",ObjectName,Error1);  
	 }
  catch (...)
	 {	
		throw FMError("Error type not supported MBE_Parameters constructor");
	 }
};

// ----------------------------------------------------------------------------

// Constructor: Number of frames 
//				Max number of frames
//              Max numbers of spectrum lines
//              Phase computation or not
//				Sample frequency
MBE_Parameters::MBE_Parameters(short int NumberOfFrames,unsigned char NMaxFr,
										 unsigned char NMaxRa,unsigned char Comp,
										 AnalyseParameters Fr)
{ 	
  int i;
  FMObject::SetName("No Name"); 
  Param.Frequency=Fr.Frequency;
  Param.Width=Fr.Width;
  Param.Shift=Fr.Shift;
  Param.FirstSample=Fr.FirstSample;
  Param.LastSample=Fr.LastSample;
  NRaiesNV=(unsigned char)((Param.Frequency/2)/150); // Noise bands of 150 Hz
  NMaxRaies=NMaxRa;	
  NMaxTrchs=NMaxFr;
  PhaseComp=Comp;
  try 
	 {	// Memory allocation
		NbreRaies=new Vectoruc(NMaxTrchs,"NbreRaies");
		NPoints=new Vectorus(NMaxTrchs,"NPoints");
		F0=new Vectord(NMaxTrchs,"F0");
		Energy=new Vectord(NMaxTrchs,"Energy");
		LSError=new Vectord(NMaxTrchs,"LSError");
		Amp=new Vectord*[NMaxTrchs];
		Phases=new Vectord*[NMaxTrchs];
		Sigma=new Vectord*[NMaxTrchs];
		for(i=0;i<NMaxTrchs;i++)
		  {
			 Amp[i]=new Vectord(NMaxRaies,"Amp[i]");
			 Sigma[i]=new Vectord(NRaiesNV,"Sigma[i]");
			 Phases[i]=new Vectord(NMaxRaies,"Phases[i]");
		  }
	 }
  catch (FMError& Error1)
	 {	// Create a	new error message
		throw FMError("\nMBE_Parameters constructor\n",ObjectName,Error1);  
	 }
  catch (...)
	 {	
		throw FMError("Error type not supported MBE_Parameters constructor");
	 }
};

// ----------------------------------------------------------------------------

// Destructor
MBE_Parameters::~MBE_Parameters()
{	// Free the memory
  int i;
  try
	 {	
		if (Amp!=NULL)
		  {
			 for(i=0;i<NMaxTrchs;i++)
				{
				  delete Amp[i];
		        delete Sigma[i];
		        if (PhaseComp)
					 {
						delete Phases[i];
					 }
				}
			 delete [] Amp;
			 delete [] Sigma;
			 delete [] Phases;
			 delete NbreRaies;
			 delete NPoints;
			 delete F0;
			 delete Energy;
			 delete LSError;
		  }
	 }
  catch (FMError& Error1)	 // Handle for FMError
	 { 
		
		throw FMError("\nError in MBE_Parameters destructor\n",
						  ObjectName,Error1);
	 }
  catch (...)	 // Handle for all errors
	 {
		throw FMError("Error in MBE_Parameters destructor");
	 }
};

// ----------------------------------------------------------------------------

// Load the parameters from file FileName
int MBE_Parameters::LoadParameters(const char *FileName)
{ 
  /* We don't need it for the moment, moreover it's not compatible with
	  the output format :-/

  FILE *LoadingFile;
  short int i;
  try
	 {
		LoadingFile=fopen(FileName,"rb");
		if (LoadingFile==NULL)
		  {
			 char *Message;
		    int l;
		    l=strlen(ERROR_FILE);
		    l+=strlen("Opening Reading\nMBE_Parameters LoadParameters\n");
	 	    l+=strlen(FileName);
		    Message=new char[l];
		    throw FMError(Message);
		  }
		// Read the analyse parameters
		fread("%i",&(Param.Frequency),LoadingFile);
		
		fscanf(LoadingFile," %i %i %i %i \n",
				 &Param.Width,&Param.Shift,&Param.FirstSample,&Param.LastSample);

		// Read the specific MBE parameters
		fread(&NMaxTrchs,sizeof(unsigned short),1,LoadingFile);
		fread(&NMaxRaies,sizeof(unsigned char),1,LoadingFile);
		fread(&NRaiesNV,sizeof(unsigned char),1,LoadingFile);
		fread(&PhaseComp,sizeof(unsigned char),1,LoadingFile);
		fread(&NTranches,sizeof(unsigned short),1,LoadingFile);
		if (Amp!=NULL)		// If object already used
		  {
			 for(i=0;i<NMaxTrchs;i++)
				{
				  delete Amp[i];
		        delete Sigma[i];
		        if (PhaseComp)
					 {
						delete Phases[i];
					 }
				}
			 delete [] Amp;
			 delete [] Sigma;
			 delete [] Phases;
			 delete NbreRaies;
			 delete NPoints;
			 delete F0;
			 delete Energy;
			 delete LSError;
		  }
		// Memory allocation
		NbreRaies=new Vectoruc(NMaxTrchs,"NbreRaies");
		NPoints=new Vectorus(NMaxTrchs,"NPoints");
		F0=new Vectord(NMaxTrchs,"F0");
		Energy=new Vectord(NMaxTrchs,"Energy");
		LSError=new Vectord(NMaxTrchs,"LSError");
		Amp=new Vectord*[NMaxTrchs];
		Phases=new Vectord*[NMaxTrchs];
		Sigma=new Vectord*[NMaxTrchs];
		for(i=0;i<NMaxTrchs;i++)
		  {
			 Amp[i]=new Vectord(NMaxRaies,"Amp[i]");
			 Sigma[i]=new Vectord(NRaiesNV,"Sigma[i]");
			 if (PhaseComp)
				{
				  Phases[i]=new Vectord(NMaxRaies,"Phases[i]");
				}
		  }
		fread(&(NbreRaies->Value),sizeof(unsigned char),NTranches,LoadingFile);
		fread(&(NPoints->Value),sizeof(unsigned short),NTranches,LoadingFile);
		fread(&(F0->Value),sizeof(float),NTranches,LoadingFile);
		fread(&(Energy->Value),sizeof(float),NTranches,LoadingFile);
		fread(&(LSError->Value),sizeof(float),NTranches,LoadingFile);
		for(i=0;i<NTranches;i++)
		  {
			 fread(&(Sigma[i]->Value),sizeof(double),NRaiesNV,LoadingFile);
			 fread(&(Amp[i]->Value),sizeof(double),(*NbreRaies)[i],LoadingFile);
			 if (PhaseComp)
				{
				  fread(&(Phases[i]->Value),sizeof(double),
						  (*NbreRaies)[i],LoadingFile);
				}
		  }
		fclose(LoadingFile);
	 }
  catch (FMError& Error1)			// Handle for FMError
	 { 
		throw FMError("\nError in MBE_Parameters LoadParameters\n",ObjectName,
						  Error1);
	 }
  catch (...)					 // Handle other errors
	 { 
		throw FMError("\nError in MBE_Parameters LoadParameters\n");
	 }
	 */
  return 1;
};

// ----------------------------------------------------------------------------

// Save the parameters to the file FileName
int MBE_Parameters::SaveParameters(const char *FileName)
{ 
  FILE *SavingFile; // Pointer to the file 
  short int i,j; // Counter
  try
	 {
		//SavingFile=fopen(FileName,"wb");
		SavingFile=fopen(FileName,"wt");
		if (SavingFile==NULL)	 // If error while opening file
		  {						 // then throw a FMError
			 char *Message;
			 int l;
			 l=strlen(ERROR_FILE);
			 l+=strlen("Opening Writing\nMBE_Parameters SaveParameters\n");
			 l+=strlen(FileName);
			 Message=new char[l];
			 throw FMError(Message);
		  }
		// Write the analyse parameters
		fprintf(SavingFile," %i %i %i %i \n",
				  Param.Width, Param.Shift, Param.FirstSample, Param.LastSample);
		
		for(i=0;i<NTranches;i++)
		  {
			 for(j=0;j<NRaiesNV;j++)
				fprintf(SavingFile," %6.2f ",Sigma[i]->Value[j]);
			 
			 fprintf(SavingFile,"\n");
			 for(j=0;j<(*NbreRaies)[i];j++)
				fprintf(SavingFile," %6.2f ",Amp[i]->Value[j]);
		
			 if (PhaseComp)
				{
				  fprintf(SavingFile,"\n");
				  for(j=0;j<(*NbreRaies)[i];j++)
					 {
						while (Phases[i]->Value[j]>PI)
						  {
							 Phases[i]->Value[j]-=2*PI;
						  }
						fprintf(SavingFile," %f ",Phases[i]->Value[j]);
					 }
				}

			 fprintf(SavingFile,"\n");
			 fprintf(SavingFile," %f \n %f %f \n",
						F0->Value[i],
						LSError->Value[i],
						Energy->Value[i]);
		  }

		fclose(SavingFile);
	 }
  catch (FMError& Error1)			// Handle for FMError
	 { 
		throw FMError("\nError in MBE_Parameters SaveParameters\n",ObjectName,
						  Error1);
	 }
  catch (...)					 // Handle other errors
	 { 
		throw FMError("\nError in MBE_Parameters SaveParameters\n");
	 }
  return 1;
};

// ----------------------------------------------------------------------------

// Copy the parameters of a frame (source) to another (destination)
int MBE_Parameters::CopyFrameParam(short int source,short int destination)
{ 
  short int i;
  if ((source<0)||(destination<0)||
		(source>=NMaxTrchs)||(destination>=NMaxTrchs))  // Index not allowed 
	 {
		throw FMError("MBE_Parameters=>CopyFrame Index not allowed\n",
						  ObjectName);   // throw a FMError
	 }
  for(i=0;i<(*NbreRaies)[source];i++)		// Copy amplitude and
	 { 										// phases if needed
		(*Amp[destination])[i]=(*Amp[source])[i]; 
		if (PhaseComp)
		  {
			 (*Phases[destination])[i]=(*Phases[source])[i];
		  }
	 }
  for(i=0;i<NRaiesNV;i++)   // Copy sigma
	 {
		(*Sigma[destination])[i]=(*Sigma[source])[i];
	 }
  (*NbreRaies)[destination]=(*NbreRaies)[source]; // Copy the other data
  (*NPoints)[destination]=(*NPoints)[source];
  (*Energy)[destination]=(*Energy)[source];
  (*F0)[destination]=(*F0)[source];
  return 1;
};

// ----------------------------------------------------------------------------

// Adapt the parameters to the desired prosody
int MBE_Parameters::MatchProsody(Vectord *Pitch,Vectord *Timing)
{ 
  // Not yet implemented
  return 1;
};


// ============================================================================
// Class MBE_Analyser
// ============================================================================

// Constructor
MBE_Analyser_A::MBE_Analyser_A(const char *Name,
										 unsigned short WindowLength,
										 unsigned short WindowShift,
										 unsigned short NpointsFFT,
										 unsigned short NMax_Raies,
										 unsigned short NMaxRaiesAnal)
{
  try
	 {
		FMObject::SetName(Name);
		NMaxRaies=NMax_Raies;
		NRaiesAnal=NMaxRaiesAnal;
		NFFT=NpointsFFT;
		WinLength=WindowLength;
		WinShift=WindowShift;
		HammingW=new Vectord(WinLength,"MBE_Analyser_A HammingW");
		RealHamSpect=new Vectord(51,"MBE_Analyser_A RealHamSpect");
		ErrorTab=new Vectord(2*NptsError+1);
		// Initialisation of the hamming window
		short int i;
		Vectord Omega(50,"MBE_Analyser_A Omega");
		VectorComplex HamSpect(50,"MBE_Analyser_A HamSpect");
		HammingWindow(&(HammingW->Value[0]),HammingW->GetLength());
		for(i=0;i<50;i++)
		  {
			 Omega[i]=2.0*(double)PI*((double)i/49.0)*2.0/(double)WindowLength;
		  }
		// Compute the real part of the spectrum of the half symetric window
		(*HammingW)[WindowLength/2]=(*HammingW)[WindowLength/2]/2;
		freqz_FIR(*HammingW,WindowLength/2,WindowLength,Omega,&HamSpect);
		(*HammingW)[WindowLength/2]=(*HammingW)[WindowLength/2]*2;
		// Multiply its abs value by two in order to get the spectrum of the 
		// complete window
		for(i=0;i<49;i++)
		  {
			 (*RealHamSpect)[i]=2*fabs(HamSpect[i].Real);
		  }
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nMBE_Analyser_A constructor\n",ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBE_Analyser_A constructor => Error\n",ObjectName);
	 }
};

// ----------------------------------------------------------------------------

// Destructor
MBE_Analyser_A::~MBE_Analyser_A()
{
  try
	 {
		delete RealHamSpect;
		delete HammingW;
		delete ErrorTab;
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nMBE_Analyser_A destructor\n",ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBE_Analyser_A destructor\n",ObjectName);
	 }
};

// ----------------------------------------------------------------------------

// Define the I/O
void MBE_Analyser_A::IO_Def(SoundFile *Son,PitchObject *Pitch,
									 MBE_Parameters *MBE)
{
  son=Son;  
  pitch=Pitch;
  mbe=MBE;
};

// ----------------------------------------------------------------------------

// Process
int MBE_Analyser_A::Process()
{
  try
	 {
		short *Sig;
		short NTranches;
		long int AnShift;
		double temp;
		short PosMinimum;
		CComplex tempC;
		short i,j;
		Sig=new short[mbe->Param.Width];
		SigDoub=new Vectord(mbe->Param.Width);
		SigSpectrum=new VectorComplex(NFFT/2+1,"SigSpectrum");
		SigSpectrum2=new VectorComplex(NFFT/2+1,"SigSpectrum2");
		OffSet=(mbe->Param.Width-mbe->Param.Shift)/2;
		Sommw=sum_V((*HammingW));
		eltmult_V((*HammingW),(*HammingW),SigDoub);
		Sommw2=sum_V((*SigDoub));
		Sommw4=norm2_V((*SigDoub))/Sommw2/Sommw2;
		NTranches=(short)floor(minab((mbe->Param.LastSample-mbe->Param.FirstSample+1)/
											  mbe->Param.Shift+1,mbe->NMaxTrchs));
		mbe->NTranches=NTranches;
		for(i=0;i<NTranches;i++)
		  {
			 DebutFen=mbe->Param.FirstSample+i*mbe->Param.Shift-OffSet+1;
			 if (DebutFen<1)
				{
				  fprintf(stderr,"READ %i before begin of file\n",DebutFen);
				  exit(2);
				  
				  AnShift=-DebutFen+1;
				  DebutFen=1;
				}
			 else
				{
				  AnShift=0;
				}
			 if ((DebutFen+mbe->Param.Width)>son->Length())
				{
				  fprintf(stderr,"READ %i %i after end of file\n",DebutFen,son->Length());
				  exit(2);
				  
				  DebutFen=son->Length()-mbe->Param.Width+1;
				}
			 son->LoadSamples(mbe->Param.Width,DebutFen-1,Sig);
			 //son->LoadSamples(Sig);
			 for(j=0;j<mbe->Param.Width;j++)
				{
				  (*SigDoub)[j]=Sig[j];
				}
			 mbe->F0->Value[i]=pitch->GetPitch_Step(mbe->Param.FirstSample+
																 (i-1)*mbe->Param.Shift+
																 mbe->Param.Shift/2+1);
			 
			 mbe->NPoints->Value[i]=mbe->Param.Shift;
			 mbe->Energy->Value[i]=norm2_V(*SigDoub);
			 FirstRaieSpect=round(maxab((int)(40/mbe->Param.Frequency*NFFT),
												 (int)(mbe->F0->Value[i]/2/
														 mbe->Param.Frequency*NFFT)))+1;
			 LastRaieSpect=round(minab((int)(NFFT/2),(int)((floor(mbe->Param.Frequency/
																					mbe->F0->Value[i]/2)+0.5)*mbe->F0->Value[i]/
																		  mbe->Param.Frequency*NFFT)));
			 eltmult_V(*SigDoub,*HammingW,SigDoub);
			 fft_R(*SigDoub,SigSpectrum);
			 // Forward time-shift of frame length /2 samples to avoid linear
			 // phase component
			 SearchFreq=2*PI*((double)mbe->Param.Width/(double)2)/(double)NFFT;
			 double RealTemporaire;
			 for(j=1;j<(NFFT/2);j++)
				{
				  Norm2Sig=SearchFreq*j;
				  tempC.Real=cos(Norm2Sig);
				  tempC.Imaginary=sin(Norm2Sig);
				  RealTemporaire=(*SigSpectrum)[j].Real;
				  (*SigSpectrum)[j].Real=(*SigSpectrum)[j].Real*tempC.Real-
					 (*SigSpectrum)[j].Imaginary*tempC.Imaginary;
				  (*SigSpectrum)[j].Imaginary=RealTemporaire*tempC.Imaginary+
					 (*SigSpectrum)[j].Imaginary*tempC.Real;
				}
			 Norm2Sig=norm2_CV(SigSpectrum,FirstRaieSpect,LastRaieSpect);
			 // Compute the unbiased error function on the pitxh period range  +++
			 for(j=-NptsError;j<NptsError;j++)
				{
				  SearchFreq=mbe->Param.Frequency/mbe->F0->Value[i]; // Initial 
				  // pitch period
				  temp=SearchFreq+j*0.125;
				  SearchFreq=mbe->Param.Frequency/temp; // 1/8 sample precision
				  //printf("\n avant geterrorval %i\n",j);
				  (*ErrorTab)[j+NptsError]=GetErrorVal(SearchFreq,FALSE,mbe,i);
				  //printf("\n apres geterrorval");
				  (*ErrorTab)[j+NptsError]/=(1-temp*Sommw4);
				}
			 // Make a choice
			 SearchFreq=mbe->Param.Frequency/mbe->F0->Value[i];
			 PosMinimum=PosMin(ErrorTab->Value,NptsError*2);
			 PosMinimum=PosMinimum-NptsError;
			 temp=SearchFreq+PosMinimum*0.125;
			 SearchFreq=mbe->Param.Frequency/temp;
			 mbe->F0->Value[i]=SearchFreq; // Store the value
			 //printf("\n Avant erreur possible");
			 // Compute ³S(w)-S^(w)³ý and the sines amplitude again
			 mbe->LSError->Value[i]=GetErrorVal(SearchFreq,TRUE,mbe,i);
			 mbe->LSError->Value[i]/=(1-temp*Sommw4);
			 // Amplitude of the sines = 2 times the amplitudes of the complex 
			 // exponentials detected in the spectrum
			 //printf("\n Apres erreur probable");
			 for(j=0;j<mbe->NbreRaies->Value[i];j++)
				{
				  mbe->Amp[i]->Value[j]*=2;
				}
			 // Phase correction if frame shift at startup
			 double tempor2;
			 for(j=0;j<mbe->NbreRaies->Value[i];j++)
				{
				  tempor2=(mbe->Phases[i]->Value[j]-AnShift/mbe->Param.Frequency*
							  2*PI*j*mbe->F0->Value[i])/(2*PI);
				  mbe->Phases[i]->Value[j]=(tempor2-floor(tempor2))*2*PI;
				}
			 //printf("\n presque fini");
			 GetNoiseBandSigma(mbe,i);
			 //printf("\n fini");
		  }
		delete SigSpectrum2;
		delete SigSpectrum;
		delete SigDoub;
		delete [] Sig;
		return 1;
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nMBE_Analyser_A Process\n",ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBE_Analyser_A Process => Error\n",ObjectName);
	 }
};

// ----------------------------------------------------------------------------

// Reset the block
void MBE_Analyser_A::Reset()
{
  // Not yet implemented
};

// ----------------------------------------------------------------------------

// Estimation of the magnitude of the sinusoidal wave 
void MBE_Analyser_A::GetSinAmpPhase(double SinFreq,
												unsigned short int FirstRaie,
												unsigned short int LastRaie,int FreqRelease,
												double *SinAmplitude,double *SinPhase)
{
  try
	 {
		int i,j,k; // counter
		Vectord RealHamSpectResampled(LastRaie-FirstRaie+1,"RealHamResampled");
		// Spectral vector
		double NewInd,PasFreq;
		CComplex CompCoef;
		Vectord RelatError(21,"RelatError");
		double Norm2SigBand;
		double Norm2SinBand;
		int FirstIndex;
		short int fr,wl,ws; // Sample frequency, window length and window shift
		// Begin of the function
		fr=mbe->Param.Frequency;
		wl=mbe->Param.Width;
		ws=mbe->Param.Shift;
		PasFreq=(double)fr/(double)NFFT;
		Norm2SigBand=norm2_CV(SigSpectrum,FirstRaie,LastRaie);
		if (FreqRelease==TRUE)
		  {
			 FirstIndex=10;
		  }
		else
		  {
			 FirstIndex=0;
		  }
		// +++
		for(j=-FirstIndex;j<FirstIndex+1;j++)
		  {
			 for(i=FirstRaie;i<=LastRaie;i++)
				{
				  NewInd=(double)(2*(double)fr/(double)wl);
				  NewInd=(double)((double)fabs((double)(SinFreq+(double)j*0.5
																	 -(double)i*PasFreq))*49.0/NewInd);
				  if (NewInd>49) 
					 {
						RealHamSpectResampled[i-FirstRaie]=0; // Out of the main 
					 }
				  else 
					 {
						RealHamSpectResampled[i-FirstRaie]=
						  Interpola(RealHamSpect->Value,50,NewInd);
					 }
				}
			 VectorComplex V2(LastRaie-FirstRaie+1,"V2");
			 for(k=0;k<(LastRaie-FirstRaie+1);k++)
				{
				  V2[k]=(*SigSpectrum)[k+FirstRaie];
				}
			 div_CVV(V2,RealHamSpectResampled,&CompCoef);
			 (*SinAmplitude)=CompCoef.GetModule();
			 if (mbe->PhaseComp==TRUE)
				{
				  (*SinPhase)=CompCoef.GetPhase();
				}
			 Norm2SinBand=norm2_V(RealHamSpectResampled);
			 RelatError[j+FirstIndex]=1-(*SinAmplitude)*(*SinAmplitude)*
				Norm2SinBand/Norm2SigBand;
		  }
  		if (FreqRelease==TRUE)
		  {
			 FirstIndex=PosMin(RelatError.Value,21)-FirstIndex;
			 for(i=FirstRaie;i<=LastRaie;i++)
				{
				  NewInd=(2*(double)fr/(double)wl);
				  NewInd=fabs(SinFreq+FirstIndex*0.5-i*PasFreq)*49/NewInd;
				  if (NewInd>49) 
					 {
						RealHamSpectResampled[i-FirstRaie]=0; // Out of the main 
					 }
				  else 
					 {
						RealHamSpectResampled[i-FirstRaie]=
						  Interpola(RealHamSpect->Value,50,NewInd);
					 }	
				}
			 VectorComplex V22(LastRaie-FirstRaie+1,"V22");
			 for(k=0;k<(LastRaie-FirstRaie+1);k++)
				{
				  V22[k]=(*SigSpectrum)[k+FirstRaie];
				}
			 div_CVV(V22,RealHamSpectResampled,&CompCoef);
			 (*SinAmplitude)=CompCoef.GetModule();
			 if (mbe->PhaseComp==TRUE)
				{
				  (*SinPhase)=CompCoef.GetPhase();
				}
		  }
		for(i=FirstRaie;i<LastRaie;i++)
		  {
			 (*SigSpectrum2)[i].Real=(*SigSpectrum)[i].Real-
				CompCoef.Real*RealHamSpectResampled[i-FirstRaie];
			 (*SigSpectrum2)[i].Imaginary=(*SigSpectrum)[i].Imaginary-
				CompCoef.Imaginary*RealHamSpectResampled[i-FirstRaie];
		  }
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nMBE_Analyser_A GetSinAmpPhase\n",ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBE_Analyser_A GetSinAmpPhase => Error\n",ObjectName);
	 }
};

// ----------------------------------------------------------------------------

// Estimation of the standard deviation of the noise 
double MBE_Analyser_A::GetNoiseSigma(unsigned short FirstRaie,
												 unsigned short LastRaie)
{
  // Frequency averaged periodogram : Daniel's spectral estimator 
  // Average ³X(w)³`ý, then divide it by the sum of squared weighing elements
  // in order to approximate the (flat) Power spectrum = aý at any frequency.
  // Take the square root to get a.
  try
	 {
		double temp;
		int l;
		l=LastRaie-FirstRaie+1;
		temp=norm2_CV(SigSpectrum2,FirstRaie,LastRaie);
		temp=sqrt(temp/l/Sommw2);
		return temp;
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nMBE_Analyser_A GetNoiseSigma\n",ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBE_Analyser_A GetNoiseSigma => Error\n",ObjectName);
	 }
};

// ----------------------------------------------------------------------------

// Estimation of the standard deviation of the noise in contiguous 
// frequency bands (bandwith 150 Hz)
void MBE_Analyser_A::GetNoiseBandSigma(MBE_Parameters* AnalysisParam,
													short frame)
{
  try
	 {
		double temp;
		int i;
		temp=150.0/AnalysisParam->Param.Frequency*NFFT;
		for(i=0;i<AnalysisParam->NRaiesNV;i++)
		  {
			 AnalysisParam->Sigma[frame]->Value[i]=
				GetNoiseSigma((unsigned short)ceil(i*temp),
								  (unsigned short)floor((i+1)*temp));
		  }
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nMBE_Analyser_A GetNoiseBandSigma\n",ObjectName,
						  Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBE_Analyser_A GetNoiseBandSigma => Error\n",
			           ObjectName);
	 }
};

// ----------------------------------------------------------------------------

// Estimation of the minimal relative error of modelisation
double MBE_Analyser_A::GetErrorVal(double SinFreq,int FreqRelease,
											  MBE_Parameters* AnalysisParam,short frame)
{
  try
	 {
		int i;
		unsigned short FirstRaie,LastRaie;
		double temp;
		double Value;
		short int  Nbr;
		for(i=0;i<SigSpectrum->Length;i++)
		  {
			 (*SigSpectrum2)[i].Real=(*SigSpectrum)[i].Real;
			 (*SigSpectrum2)[i].Imaginary=(*SigSpectrum)[i].Imaginary;
			 //(*SigSpectrum2)[i].Module=(*SigSpectrum)[i].Module;
			 //(*SigSpectrum2)[i].Phase=(*SigSpectrum)[i].Phase;
		  }
		temp=LastRaieSpect;				   // ++
		AnalysisParam->NbreRaies->Value[frame]=
		  round(minab((int)floor(temp*AnalysisParam->Param.Frequency/
										 SinFreq/NFFT),AnalysisParam->NMaxRaies));
		Nbr=AnalysisParam->NbreRaies->Value[frame];
		// First spectrum line
		FirstRaie=FirstRaieSpect;
		LastRaie=(unsigned short)floor(SinFreq*1.5/
												 AnalysisParam->Param.Frequency*NFFT);
		GetSinAmpPhase(SinFreq,FirstRaie,LastRaie,FreqRelease,
							&(AnalysisParam->Amp[frame]->Value[0]),
							&(AnalysisParam->Phases[frame]->Value[0]));
		(*SigSpectrum2)[LastRaie].Real=0.0;
		(*SigSpectrum2)[LastRaie].Imaginary=0.0;
		// Spectrum lines
		for(i=1;i<Nbr-1;i++)
		  {
			 FirstRaie=LastRaie+1;
			 LastRaie=(unsigned short)floor(SinFreq*(i+1.5)/
													  AnalysisParam->Param.Frequency*NFFT);
			 GetSinAmpPhase(SinFreq*(i+1),FirstRaie,LastRaie,
								 FreqRelease,&(AnalysisParam->Amp[frame]->Value[i]),
								 &(AnalysisParam->Phases[frame]->Value[i]));
			 (*SigSpectrum2)[LastRaie].Real=0.0;
			 (*SigSpectrum2)[LastRaie].Imaginary=0.0;
		  }
		// Last spectrum line
		FirstRaie=LastRaie+1;
		LastRaie=LastRaieSpect;
		GetSinAmpPhase(SinFreq*Nbr,
							FirstRaie,LastRaie,
							FreqRelease,&(AnalysisParam->Amp[frame]->Value[Nbr-1]),
							&(AnalysisParam->Phases[frame]->Value[Nbr-1]));
		(*SigSpectrum2)[LastRaie].Real=0.0;
		(*SigSpectrum2)[LastRaie].Imaginary=0.0;
		// Error determination
		double temp2;
		temp2=norm2_CV(SigSpectrum2,FirstRaieSpect,LastRaieSpect);
		Value=sqrt(temp2/Norm2Sig);
		return Value;
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nMBE_Analyser_A GetErrorVal\n",ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBE_Analyser_A GetErrorVal => Error\n",ObjectName);
	 }
};
