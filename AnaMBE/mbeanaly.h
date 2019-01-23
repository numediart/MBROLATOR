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
// mbeanalsy.h                                                                                                                  mbe.cpp
//
// classes:     MBE_Analyser_A
//
// Author:      Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             21.03.1996: Creation from T.Dutoit pascal program
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_MBEANALY

#ifndef _INC_FMOBJECT
#include "fmobject.h"
#endif

#ifndef _INC_DATATYPE
#include "datatype.h"
#endif

#ifndef _INC_COMPLEX
#include "complex.h"
#endif

// ============================================================================
// Class MBE_Analyser
// ============================================================================

#define NptsError 9

// -------------------------
// Architecture (No entity)
// -------------------------

class MBE_Analyser_A : public FMObject
{
public:
        // Constructor
        MBE_Analyser_A(const char *Name,unsigned short WindowLength,
                                   unsigned short WindowShift,unsigned short NpointsFFT,
                                   unsigned short NMax_Raie,unsigned short NMaxRaiesAnal);
        // Destructor
        ~MBE_Analyser_A();

        // Define the I/O
        virtual void IO_Def(SoundFile *Son,PitchObject *Pitch,MBE_Parameters *MBE);
        // Process
        virtual int Process();
        // Reset the block
        virtual void Reset();
protected:
        SoundFile *son; // Input sound
        PitchObject *pitch; // Input first estimation of the pitch
        MBE_Parameters *mbe; // Output MBE parameters

        // Internal Variables
        Vectord *SigDoub;
        VectorComplex *SigSpectrum;
        VectorComplex *SigSpectrum2;
        long int DebutFen;
        short int OffSet;
        double SearchFreq;
        double Norm2Sig;
        double Sommw;
        double Sommw2;
        double Sommw4;
        unsigned short FirstRaieSpect,LastRaieSpect;
        unsigned short WinLength;
        unsigned short WinShift;

        unsigned short NMaxRaies;
        unsigned short NRaiesAnal;
        unsigned short NFFT;
        Vectord *HammingW;
        Vectord *RealHamSpect;
        Vectord *ErrorTab;

private:
        // Estimation of the magnitude of the sinusoidal wave
        void GetSinAmpPhase(double SinFreq,unsigned short int FirstRaie,
                                            unsigned short int LastRaie,int FreqRelease,
                                                double *SinAmplitude,double *SinPhase);
        // Estimation of the standard deviation of the noise
        double GetNoiseSigma(unsigned short FirstRaie,unsigned short LastRaie);
        // Estimation of the standard deviation of the noise in contiguous
        // frequency bands (bandwith 150 Hz)
        void GetNoiseBandSigma(MBE_Parameters* AnalysisParam,short frame);
        // Estimation of the minimal relative error of modelisation
        double GetErrorVal(double SinFreq,int FreqRelease,
                                           MBE_Parameters* AnalysisParam,short frame);
};

