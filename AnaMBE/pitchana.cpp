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

#define MinPer_8000 28  // These definitions are used in the
#define MaxPer_8000 100 // class FramePitch_A 

// ============================================================================
// Class FrameMBEPitch_A
// ============================================================================

// Constructor: Initialise the input/output, 
//              the parameters and the coefficients
//              of the filter
FrameMBEPitch_A::FrameMBEPitch_A(short int width,int freq)
{ 
  FMObject::SetName("No Name");
  SigDoub=NULL;
  PitchValue=NULL;
  SampleFrequency=freq;
  WindowWidth=width;
  // Coefficients for the low pass filtering Fe= 8 kHz
  LP_8000[0]=0.394201;  LP_8000[1]=0.297354; LP_8000[2]=0.093671;
  LP_8000[3]=-0.051602; LP_8000[4]=-0.063723; LP_8000[5]=-0.004565;
  LP_8000[6]=0.030659; LP_8000[7]=0.016900; LP_8000[8]=-0.007430;
  LP_8000[9]=-0.013775; LP_8000[10]=-0.002174;
  // Coefficients for the low pass filtering Fe= 16 kHz
  LP_16000[0]=2.0456169e-001; LP_16000[1]=1.9019267e-001; 
  LP_16000[2]=1.5109153e-001; LP_16000[3]=9.7067761e-002;
  LP_16000[4]=4.1101007e-002; LP_16000[5]=-4.6188073e-003;
  LP_16000[6]=-3.2194882e-002; LP_16000[7]=-3.9766186e-002;
  LP_16000[8]=-3.1219316e-002; LP_16000[9]=-1.4090978e-002;
  LP_16000[10]=3.3838699e-003; LP_16000[11]=1.4988023e-002;  
  LP_16000[12]=1.8092057e-002; LP_16000[13]=1.3718861e-002;
  LP_16000[14]=5.3275554e-003; LP_16000[15]=-3.0076626e-003;
  LP_16000[16]=-8.2351944e-003; LP_16000[17]=-9.230003e-003;
  LP_16000[18]=-6.7465991e-003; LP_16000[19]=-2.6768421e-003;
  LP_16000[20]=9.7616003e-004; LP_16000[21]=2.9265469e-003;
  LP_16000[22]=6.2065720e-003; 
};

// ----------------------------------------------------------------------------

// Constructor: Initialise the input/output, 
//              the parameters and the coefficients
//              of the filter
//				and give a name to the object
FrameMBEPitch_A::FrameMBEPitch_A(const char *Name,short int width,int freq)
{ 
  FMObject::SetName(Name);
  SigDoub=NULL;
  PitchValue=NULL;
  SampleFrequency=freq;
  WindowWidth=width;
  // Coefficients for the low pass filtering Fe= 8 kHz
  LP_8000[0]=0.394201;  LP_8000[1]=0.297354; LP_8000[2]=0.093671;
  LP_8000[3]=-0.051602; LP_8000[4]=-0.063723; LP_8000[5]=-0.004565;
  LP_8000[6]=0.030659; LP_8000[7]=0.016900; LP_8000[8]=-0.007430;
  LP_8000[9]=-0.013775; LP_8000[10]=-0.002174;
  // Coefficients for the low pass filtering Fe= 16 kHz
  LP_16000[0]=2.0456169e-001; LP_16000[1]=1.9019267e-001; 
  LP_16000[2]=1.5109153e-001; LP_16000[3]=9.7067761e-002;
  LP_16000[4]=4.1101007e-002; LP_16000[5]=-4.6188073e-003;
  LP_16000[6]=-3.2194882e-002; LP_16000[7]=-3.9766186e-002;
  LP_16000[8]=-3.1219316e-002; LP_16000[9]=-1.4090978e-002;
  LP_16000[10]=3.3838699e-003; LP_16000[11]=1.4988023e-002;  
  LP_16000[12]=1.8092057e-002; LP_16000[13]=1.3718861e-002;
  LP_16000[14]=5.3275554e-003; LP_16000[15]=-3.0076626e-003;
  LP_16000[16]=-8.2351944e-003; LP_16000[17]=-9.230003e-003;
  LP_16000[18]=-6.7465991e-003; LP_16000[19]=-2.6768421e-003;
  LP_16000[20]=9.7616003e-004; LP_16000[21]=2.9265469e-003;
  LP_16000[22]=6.2065720e-003; 
};

// ----------------------------------------------------------------------------

// Destructor: Free the memory
FrameMBEPitch_A::~FrameMBEPitch_A()
{ 
  try
	 {
		if (ForwErrTab2)
		  { 
			 delete [] ForwErrTab2;
		  }
		if (ForwErrTab) 
		  { 
			 delete [] ForwErrTab;
		  }
		for(i=0;i<3;i++)
		  { 
			 if (ErrorFunctions[i])
				{ 
				  delete [] ErrorFunctions[i]; 
				}
		  }
		if (AutoCorr)
		  { 
			 delete [] AutoCorr;
		  }
		if (Hamming) 
		  { 
			 delete [] Hamming;
		  }
	 }
  catch (...)
	 {
		throw FMError("\nFrameMBEPitch destructor => Error\n",ObjectName);
	 }
};

// ----------------------------------------------------------------------------

// IO_Def: Define the input/output and do some initialisation 
//         operations 
void FrameMBEPitch_A::IO_Def(Vectord *SoundFrame,
                             double *pitch)
{ 
  try
	 {
		SigDoub=SoundFrame;
		PitchValue=pitch;
		// Select the low pass filter to use [f(sampling frequency)]
		temp1=(double)SampleFrequency;
		if (SampleFrequency==8000) 
		  {
			 CoefLP=LP_8000;
		    LPOrder=11;
		  }
		else 
		  {
			 if (SampleFrequency==16000)
				{
				  CoefLP=LP_16000;
				  LPOrder=23;
            }
			 else
				{
				  throw 
					 FMError("\nFrameMBEPitch IO_Def => Fe not allowed\n",
								ObjectName);
				} // Sampling Frequency not allowed
		  }
		// Memory allocation and initialisation
		MinPeriod2=(int)round(MinPer_8000*(double)temp1/(double)8000)*2;
		MaxPeriod2=(int)round(MaxPer_8000*(double)temp1/(double)8000)*2;
		LocLength=(WindowWidth)+2*(LPOrder-1);  
		Hamming=new double[WindowWidth];
		AutoCorr=new double[2*WindowWidth];
		for(i=0;i<3;i++)
		  { 
			 ErrorFunctions[i]=new double[MaxPeriod2-MinPeriod2+1];
        }
		ForwErrTab=new double[MaxPeriod2-MinPeriod2+1];
		ForwErrTab2=new double[MaxPeriod2-MinPeriod2+1];
		P_1=P_2=100*(SampleFrequency/8000);
		Err_1=Err_2=0;
		// Hamming window normalisation
		HammingWindow(Hamming,WindowWidth);
		Norm=sqrt(InnerProduct(Hamming,Hamming,WindowWidth));
		for(i=0;i<WindowWidth;i++)
		  { 
			 Hamming[i]/=Norm;
		  }
		Sommw4=0;
		for(i=0;i<WindowWidth;i++)
		  { 
			 Sommw4+=Hamming[i]*Hamming[i]*Hamming[i]*Hamming[i];
		  }
		i=0;
		ChosenPitch=0;
		ForwErr=0;
		BackErr=0;
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nFrameMBEPitch IO_Def\n",ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nFrameMBEPitch IO_Def => Error\n",ObjectName);
	 }
};

// ----------------------------------------------------------------------------

// Process: DO the pitch analysis (delay of 2 frames)
int FrameMBEPitch_A::Process()
{ 
  // Begin of the MBE pitch analysis
  if (SigDoub==NULL)
	 {
		throw FMError("\nFrameMBEPitch_A Process => IO_Def not called\n",
						  ObjectName);
    }
  try
	 {
		// Beginning of the process      
		// Low Pass Filtering
		LowPassFilter(CoefLP,LPOrder,&(SigDoub->Value[0]),      
						  WindowWidth+LPOrder-1);
		// Evaluate the biased autocorrelation function
		Windowing(&(SigDoub->Value[LPOrder-1]),Hamming,WindowWidth);
		Norm=InnerProduct(&(SigDoub->Value[LPOrder-1]),
								&(SigDoub->Value[LPOrder-1]),WindowWidth);
		Windowing(&(SigDoub->Value[LPOrder-1]),Hamming,WindowWidth);
		Autocorrelation(&(SigDoub->Value[LPOrder-1]),AutoCorr,
							 WindowWidth,WindowWidth-1);
		for(j=WindowWidth-2;j>0;j--)
		  { 
			 AutoCorr[(2*j)]=AutoCorr[j];
			 AutoCorr[(2*j-1)]=(AutoCorr[(j-1)]+AutoCorr[j])/2;
		  } 
		// Determine the error function on the pitch period range
		for(j=MinPeriod2;j<=MaxPeriod2;j++)
		  { 
			 ErrorFunctions[2][j-MinPeriod2]=GetErrorValue(j,
																		  WindowWidth,Norm,Sommw4,AutoCorr);
		  } 
		// Dynamic algorythm
		if (i>=2)
		  { 
			  LocMin=round(maxab(MinPeriod2,round(0.8*P_1)));
			  LocMax=round(minab(MaxPeriod2,round(1.2*P_1))); 
			  PB=PosMin(&(ErrorFunctions[0][LocMin-MinPeriod2]),
						  LocMax-LocMin+1)+LocMin;
			 BackErr=ErrorFunctions[0][PB-MinPeriod2]+Err_1+Err_2;
			 // Compute the forward pitch period and cumulative error
			 for(j=0;j<=(MaxPeriod2-MinPeriod2);j++)
				{ 
				  LocMin=round(maxab(MinPeriod2, round(0.8*(j+MinPeriod2))));
				  LocMax=round(minab(MaxPeriod2, round(1.2*(j+MinPeriod2))));
				  PF=PosMin(&(ErrorFunctions[2][LocMin-MinPeriod2]),
							LocMax-LocMin+1)+1;
				  ForwErrTab[j]=ErrorFunctions[1][j]+
					  ErrorFunctions[2][PF+LocMin-MinPeriod2-1];
				}
			 for(j=0;j<=(MaxPeriod2-MinPeriod2);j++)
			 { 
				 LocMin=round(maxab(MinPeriod2,
									round(0.8*(j+MinPeriod2))));
				 LocMax=round(minab(MaxPeriod2,
									round(1.2*(j+MinPeriod2))));
				 PF=PosMin(&ForwErrTab[LocMin-MinPeriod2],
						   LocMax-LocMin+1)+1;
				 ForwErrTab2[j]=ErrorFunctions[0][j]+
					 ForwErrTab[PF+LocMin-MinPeriod2-1];
			 }
			 PF=PosMin(ForwErrTab2,MaxPeriod2-MinPeriod2+1)+MinPeriod2;
			 // Test for harmonics
			 j=2;
			 P0=PF;
			 temp2=ForwErrTab2[PF-MinPeriod2];
			 while (round(P0/j)>=MinPeriod2)
			 {
				 k=round(P0/j);
				  temp1=ForwErrTab2[k-MinPeriod2];
				  temp2=temp1/(ForwErrTab2[(int)round(P0-MinPeriod2)]);
				  if ((temp1<=0.85)&&(temp2<=1.7))
					 { 
						PF=k;
					 }
				  else 
					 { 
						if ((temp1<0.4)&&(temp2<3.5)) 
                    { 
							 PF=k;
                    }
						else 
						  { 
							 if (temp1<=0.05)
                        {
								  PF=k;
                        }
                    }
                }
				  j++;
				}
			 ForwErr=ForwErrTab2[PF-MinPeriod2];
			 if (BackErr<0.48)                            
				{ 
				  ChosenPitch=PB;
				}
			 else 
				{ 
				  if (BackErr<=ForwErr)
                { 
						ChosenPitch=PB;
                }
				  else 
					 { 
						ChosenPitch=PF;
                }
            }
			 P_1=round(ChosenPitch);      
		  }
		P_2=P_1;
		Err_2=Err_1;
		Err_1=ErrorFunctions[0][P_1-MinPeriod2];   
		if (ChosenPitch!=(double)0)
		{ 
			PitchValue[0]=(double)2/(double)ChosenPitch*
				(double)SampleFrequency;
		}
		else 
		{ 
			PitchValue[0]=0;
		}
		// Update the temporary variables
		TempPtr=ErrorFunctions[0];
		for(j=0;j<2;j++)
		{ 
			ErrorFunctions[j]=ErrorFunctions[j+1];
		}
		ErrorFunctions[2]=TempPtr;     
		// end of the analysis process
		i++;
	 }
  catch (FMError& Error1)
  {
	  throw FMError("\nFrameMBEPitch Process\n",ObjectName,Error1);
  }
  catch (...)
  {
	  throw FMError("\nFrameMBEPitch Process => Error\n",ObjectName);
  }
  return (0); // Process teminated correctly
};

// ----------------------------------------------------------------------------

// Reset the parameters
void FrameMBEPitch_A::Reset()
{ 
  // Free the memory
  try
	 {
		if (ForwErrTab2)
		  { 
			 delete [] ForwErrTab2;
		  }	
		if (ForwErrTab) 
		  { 
			 delete [] ForwErrTab;
		  }
		for(i=0;i<3;i++)
		  { 
			 if (ErrorFunctions[i])
				{ 
				  delete [] ErrorFunctions[i]; 
				}
		  }
		if (AutoCorr)
		  { 
			 delete [] AutoCorr;
		  }
		if (Hamming) 
		  { 
			 delete [] Hamming;
		  }
		SigDoub=NULL;
		PitchValue=NULL;
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nFrameMBEPitch Reset\n",ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nFrameMBEPitch Reset => Error\n",ObjectName);
	 }
};
	
// ============================================================================
// Class MBEPitchAnalyserNoVoicing_A
// ============================================================================

// Constructor
MBEPitchAnalyserNoVoicing_A::MBEPitchAnalyserNoVoicing_A(
																			FramePitch_E *FramePitch)
{	
  FMObject::SetName("No Name");
  SoundObject=NULL;
  PitchCurve=NULL;
  PitchByFrame=FramePitch;
  BeginAnalyse=0;
  EndAnalyse=0;
  BeginAnalyse=EndAnalyse=0; 
};

// ----------------------------------------------------------------------------

// Constructor with name
MBEPitchAnalyserNoVoicing_A::MBEPitchAnalyserNoVoicing_A(const char *Name,
																			FramePitch_E *FramePitch)
{	
  FMObject::SetName(Name);
  SoundObject=NULL;
  PitchCurve=NULL;
  PitchByFrame=FramePitch;
  BeginAnalyse=0;
  EndAnalyse=0;
  BeginAnalyse=EndAnalyse=0; 
};

// ----------------------------------------------------------------------------

// Destructor: Free the memory
MBEPitchAnalyserNoVoicing_A::~MBEPitchAnalyserNoVoicing_A()
{ 
  try
	 {
		if (SigDoub)     
		  { 
			 delete SigDoub;
		  }
		if (Sig)
		  { 
			 delete Sig;     
		  }
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nMBEPitchAnalyserNoVoicing_A destructor\n",
						  ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBEPitchAnalyserNoVoicing_A destructor => Error\n",
						  ObjectName);
	 }
};

// ----------------------------------------------------------------------------

// IO_Def: Define the input/output of the block
void MBEPitchAnalyserNoVoicing_A::IO_Def(SoundFile *sound,long int BeginPos, 
													  long int EndPos, PitchObject *Curve)
{ 
  try
	 {
		int LPOrder;
		SoundObject=sound;
		PitchCurve=Curve;
		BeginAnalyse=BeginPos;
		EndAnalyse=EndPos; 
		WindowWidth=PitchCurve->GetWidth();
		WindowShift=PitchCurve->GetShift();
		SampleFrequency=SoundObject->Frequency();   
		if (SampleFrequency==8000)
		  { 
			 LPOrder=11;
		  }
		else 
		  { 
			 if (SampleFrequency==16000)
				{ 
				  LPOrder=23;
				}
			 else 
				{ 
				  throw 
					 FMError("\nMBEPitchAnalyserNoVoicing_A",
								" IO_Def => Fe not allowed\n",
								ObjectName);
            }
		  }
		LocLength=(WindowWidth)+2*(LPOrder-1);
		Sig=new short int[LocLength];
		SigDoub=new Vectord(LocLength);
		// Transient elimination possibilities
		OffSet=(WindowWidth-WindowShift)/2 + LPOrder -1;
		if ((((BeginAnalyse-2*WindowShift)-OffSet)>0)&&
			 ((EndAnalyse+2*WindowShift)<SoundObject->Length()))
		  { 
			 Elim_Trans=TRUE;
		  }
		else 
		  { 
			 Elim_Trans=FALSE;
        }                   	
		// Samples reading
		NFrame=round((EndAnalyse-BeginAnalyse+1)/
						 (double)(WindowShift));
		if (Elim_Trans)
		  { 
			 FirstFrameNumber=-2;
			 LastFrameNumber=NFrame+4;
		  }
		else 
		  { 
			 FirstFrameNumber=0;
			 LastFrameNumber=NFrame+1;  // avant + 2
        }
		PitchByFrame->IO_Def(SigDoub,&value);
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nMBEPitchAnalyserNoVoicing_A IO_Def\n",
						  ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBEPitchAnalyserNoVoicing_A IO_Def => Error\n",
						  ObjectName);
	 }
};

// ----------------------------------------------------------------------------

// Process: Analyse the file frame by frame
int MBEPitchAnalyserNoVoicing_A::Process()
{ 
  if ((SoundObject==NULL)||(PitchCurve==NULL))
	 {
		throw FMError("\nMBEPitchAnalyserNoVoicing_A Process ",
						  "=> IO_Def not called\n",
						  ObjectName);
	 }
  try
	 {
		// Beginning of the process
		for(a=FirstFrameNumber;a<LastFrameNumber;a++)
		  { 
			 BeginWin=BeginAnalyse+a*WindowShift-OffSet;
			 if (BeginWin<0)
				{ 
				  fprintf(stderr,"READ BEFORE BEGINNING OF FILE %i\n",BeginWin);
				  exit(2);
				  BeginWin=0;
				}
			 if (BeginWin+LocLength>SoundObject->Length()-1)
				{ 
				  fprintf(stderr,"READ AFTER END OF FILE %i\n",BeginWin);
				  exit(2);
				  BeginWin=SoundObject->Length()-LocLength;
				}
			 SoundObject->LoadSamples(LocLength,BeginWin,Sig);
			 for(b=0;b<LocLength;b++)
				{ 
				  (*SigDoub)[b]=Sig[b];
				}
			 value=0;
			 PitchByFrame->Process();

			 if (Elim_Trans)
				{ 			 // There is a delay of two frames
				  if ( (a>=2) &&
						 (a<=NFrame+1))
					 PitchCurve->SetPitch(a-1,value);
				}
			 else if (a>=FirstFrameNumber+2)	  // No delay
				PitchCurve->SetPitch(a-2,value);
		  }
		// Hack for extract left and right pitch point removing
		if (Elim_Trans)
		  PitchCurve->Trim();
	 }

  catch (FMError& Error1)
	 {
		throw FMError("\nMBEPitchAnalyserNoVoicing_A Process\n",
						  ObjectName,Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBEPitchAnalyserNoVoicing_A Process => Error\n",
						  ObjectName);
	 }
  return 1; // Process teminated correctly
};

// ----------------------------------------------------------------------------

// Reset the block
void MBEPitchAnalyserNoVoicing_A::Reset()
{ 
  try
	 {
		// Free the memory
		if (SigDoub)     
		  { 
			 delete SigDoub;
		  }
		if (Sig)
		  { 
			 delete Sig;    
		  }                                     
		PitchByFrame->Reset(); 
	 }
  catch (FMError& Error1)
	 {
		throw FMError("\nMBEPitchAnalyserNoVoicing_A Reset\n",ObjectName,
						  Error1);
	 }
  catch (...)
	 {
		throw FMError("\nMBEPitchAnalyserNoVoicing_A Reset => Error\n",
						  ObjectName);
	 }
};
