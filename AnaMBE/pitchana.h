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
// pitchana.h                                                                                                                   mbe.cpp
//                                                                                                                                              pda.cpp
// classes:     FramePitch_E
//                      FrameMBEPitch_A
//                      Voicing_E
//                      Voicing_A
//                      PitchAnalyser_E
//                      MBEPitchAnalyser_A
//                      MBEPitchAnalyserNoVoicing_A
//                      FrameAutoCorrPDA_A (pda.cpp)
//                      AutoCorrPitchAnalyser_A (pda.cpp)
//
// Author: Fabrice Malrère - TCTS Labs - FRIA Researcher
//
// History:             15.03.1996: Creation from pitchmbe.h
//                                                      New class for pitch analysis but no voicing
//                                                      characteristics
//                                                      Exception handling
//                              16.06.1996: Creation of FrameAutoCorrPDA_A
//                                                      A pitch detector using autocorrelation
//                                                      and AutoCorrPitchAnalyser_A
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_PITCHANA

#ifndef _INC_FMOBJECT
#include "fmobject.h"
#endif

#ifndef _INC_DATATYPE
#include "datatype.h"
#endif

#ifndef _INC_SOUNDFIL
#include "soundfil.h"
#endif

#ifndef _INC_FUNC
#include "func.h"
#endif

#ifndef _INC_AUTOCORR
#include "autocorr.h"
#endif

// ============================================================================
// This class is used to determine the pitch of a frame by the MBE
// method.
// The result is delayed from two frame. The pitch now is the pitch
// from two frame before
// ============================================================================

// -------
// Entity
// -------

// Frame pitch analysis
class FramePitch_E : public FMObject
{
public:
        // Default constructor
        FramePitch_E() { };
        // Destructor
    ~FramePitch_E() { };

        // Define the I/O
    virtual void IO_Def(Vectord *frame,double *pitch)=0;
    // Process
        virtual int Process()=0;
    // Reset the block
        virtual void Reset()=0;

protected:
        Vectord *SigDoub;    // Input
    double *PitchValue; // Output
};

// -------------
// Architecture
// -------------

// Frame MBE pitch analysis
class FrameMBEPitch_A : public FramePitch_E
{
protected:
        // Internal variables
    // Low Pass Filter coefficients
    double LP_8000[11];
    double LP_16000[23];
    // Others
    short int i,j,k; // counters
    double *Hamming,*CoefLP,*TempPtr,*AutoCorr;
    short int LPOrder; // filter's order
    short int MinPeriod2,MaxPeriod2,LocMax,LocMin,LocMax2,LocMin2;
    double Norm,Sommw4;
    double *ErrorFunctions[3]; // error's functions
    short int P_1,P_2,PB,PF,LocLength;
    double P0,Err_1,Err_2,BackErr,ForwErr,ChosenPitch;
    double temp1,temp2;
    double *ForwErrTab,*ForwErrTab2;
    short int WindowWidth; // width of the analyse's windows
    short int SampleFrequency;

public:
        // Constructor
        FrameMBEPitch_A(short int width,int freq);
        // Constructor with name
        FrameMBEPitch_A(const char *Name,short int width,int freq);
        // Destructor
        ~FrameMBEPitch_A();

        // Define the I/O
        void IO_Def(Vectord *SoundFrame,double *pitch);
        // Process
        int Process();
        // Reset the block
        void Reset();
};


// Autocorrelation pitch detector
class FrameAutoCorrPDA_A : public FramePitch_E
{
public:
        // Constructor
        FrameAutoCorrPDA_A(int freq);
        // Constructor with name
        FrameAutoCorrPDA_A(const char *Name,int freq);
        // Destructor
        ~FrameAutoCorrPDA_A();

        // Define the I/O
        void IO_Def(Vectord *SoundFrame,double *pitch);
        // Process
        int Process();
        // Reset the block
        void Reset();
protected:
        // Internal variables
        short int i1a,i1b,i2a,i2b,i3a,i3b;
        short int lengthautocorr;
        Vectord* R1;
        Vectord* R2;
        Vectord* R3;
        double rp1,rp2,rp3;
        short int t1,t2,t3;
        short int Topt;
        int Frequency;
        Vectord* UsedFrame;
        int FIRST;
        short int T0p;
};

// ============================================================================
// This class is used to determine if a frame is voiced or not
// ============================================================================

// -------
// Entity
// -------

class Voicing_E : public FMObject
{
public:
        // Default constructor
        Voicing_E() {};
        // Destructor
        ~Voicing_E() {};

        // Define the I/O
        virtual void IO_Def(Vectord *frame,
                        PitchObject *pitchcurve)=0;
        // Process
        virtual int Process()=0;
        // Reset the block
        virtual void Reset()=0;

protected:
        int FrameNumber; // Internal variable
        Vectord *Frame; // Input
        double *moyenne; // Internal variable
        double moyglob;  // Internal varialble
        PitchObject *PitchCurve; // Input/Output
};

// -------------
// Architecture
// -------------

// Simple implementation for voicing determination
// (Energy level and pitch variations)
class Voicing_A : public Voicing_E
{
public:
        // Default constructor
        Voicing_A();
        // Constructor with name
        Voicing_A(const char *Name);
        // Destructor
        ~Voicing_A();

        // Define the I/O
        virtual void IO_Def(Vectord *frame,PitchObject *pitchcurve);
        // Process
        virtual int Process();
        // Reset the block
        virtual void Reset();
};

// ============================================================================
// This class is used to perform a pitch analysis of an entire
// sound file.
// ============================================================================

// -------
// Entity
// -------

class PitchAnalyser_E : public FMObject
{
public:
        // Default constructor
        PitchAnalyser_E() { };
        // Destructor
        ~PitchAnalyser_E() { };

        // Define the I/O
        virtual void IO_Def(SoundFile *sound,
                            long int BeginPos,
                        long int EndPos,
                        PitchObject *Curve)=0;
        // Process
        virtual int Process()=0;
    // Reset the block
        virtual void Reset()=0;

protected:
        SoundFile *SoundObject; // Input
    PitchObject *PitchCurve; // Output
    long int BeginAnalyse,EndAnalyse; // Begin and end sample for the analysis
                                                                          // in the sound file
};

// -------------
// Architecture
// -------------

class MBEPitchAnalyser_A : public PitchAnalyser_E
{
protected:
        // Sub blocks
    FramePitch_E *PitchByFrame; // Pitch frame analyser
    Voicing_E *VoicingDet; // Voiced/UnVoiced Determination
        // Internal variables
    double value;
    int a,b;
    long int BeginWin;
    short int *Sig;
    Vectord *SigDoub;
    short int LocLength;
    short int OffSet,Elim_Trans,FirstFrameNumber,LastFrameNumber;
    short int NFrame;
    short int WindowWidth;
    short int WindowShift;
    short int SampleFrequency;

public:
        // Constructor
        MBEPitchAnalyser_A(FramePitch_E *FramePitch,
                       Voicing_E *voi);
        // Constructor with name
        MBEPitchAnalyser_A(const char *Name,FramePitch_E *FramePitch,
                       Voicing_E *voi);
        // Destructor
        ~MBEPitchAnalyser_A();

        // Define the I/O
        void IO_Def(SoundFile *sound,long int BeginPos,
                long int EndPos, PitchObject *Curve);
        // Process
        int Process();
        // Reset the block
        void Reset();
};

// class AutoCorrPitchAnalyser_A
class AutoCorrPitchAnalyser_A : public PitchAnalyser_E
{
public:
        // Constructor
        AutoCorrPitchAnalyser_A(FramePitch_E *FramePitch,
                            Voicing_E *voi);
        // Constructor with name
        AutoCorrPitchAnalyser_A(const char *Name,FramePitch_E *FramePitch,
                            Voicing_E *voi);
        // Define the I/O
        virtual void IO_Def(SoundFile *sound,
                            long int BeginPos,
                        long int EndPos,
                        PitchObject *Curve);
        // Process
        virtual int Process();
    // Reset the block
        virtual void Reset();
protected:
        // Sub Blocks
        FramePitch_E* PitchByFrame;
        Voicing_E* VoicingDet;
};

// ============================================================================

class MBEPitchAnalyserNoVoicing_A : public PitchAnalyser_E
{
protected:
        // Sub blocks
    FramePitch_E *PitchByFrame; // Pitch frame analyser
        // Internal variables
    double value;
    int a,b;
    long int BeginWin;
    short int *Sig;
    Vectord *SigDoub;
    short int LocLength;
    short int OffSet,Elim_Trans,FirstFrameNumber,LastFrameNumber;
    short int NFrame;
    short int WindowWidth;
    short int WindowShift;
    short int SampleFrequency;

public:
        // Constructor
        MBEPitchAnalyserNoVoicing_A(FramePitch_E *FramePitch);
        // Constructor with name
        MBEPitchAnalyserNoVoicing_A(const char *Name,FramePitch_E *FramePitch);
        // Destructor
        ~MBEPitchAnalyserNoVoicing_A();

        // Define the I/O
        void IO_Def(SoundFile *sound,long int BeginPos,
                long int EndPos, PitchObject *Curve);
        // Process
        int Process();
        // Reset the block
        void Reset();
};
