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
// autocorr.h                                                                                                            lpccep.cpp
//
// classes: AutoCorr_E
//                      AutoCorr_A
//                      AutoCorr2_A
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             13.03.1996: Creation from lpccep.h
//                                                      Exception handling
//                              16.06.1996: new block for autocorrelation (used in pda)
//                                                      AutoCorr2_A
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_AUTOCORR

#ifndef _INC_CONSTANT
#include "constant.h"
#endif

#ifndef _INC_ERROREXC
#include "errorexc.h"
#endif

#ifndef _INC_FMOBJECT
#include "fmobject.h"
#endif

#ifndef _INC_DATATYPE
#include "datatype.h"
#endif

// ============================================================================
// Compute an autocorrelation on a frame
// ============================================================================

// -------
// Entity
// -------

class AutoCorr_E : public FMObject
{
public:
        // Default constructor
        AutoCorr_E() {};
        // Destructor
        ~AutoCorr_E() {};

        // Define the I/O
        virtual void IO_Def(Vectord *Frame,Vectord *AutoCorrCoef)=0;
        // Process
        virtual int Process()=0;
        // Reset of the bloc
        virtual void Reset()=0;

        // Order of the autocorrelation analysis
        int Order;
        // Width of the frame
        int Width;

protected:
        Vectord *Sampl; // input (frame samples)
        Vectord *AutoCorr; // output (auto-correlation coefficients)
};

// -------------
// Architecture
// -------------

//        Width-1
//  a[i]= +       [s(j)*s(j+i)]          0<=i<=Order
//        j=0
class AutoCorr_A : public AutoCorr_E
{
public:
        // Default constructor
        AutoCorr_A();
        // Constructor with name
        AutoCorr_A(const char *Name);
        // Destructor
        ~AutoCorr_A();

        // Define the I/O
        virtual void IO_Def(Vectord *Frame,Vectord *AutoCorrCoef);
        // Process
        virtual int Process();
        // Reset the block
        virtual void Reset();
};

//        Width-1
//  a[i]= +       [s(j)*s(j-i)]          first<=i<=last
//        j=0
class AutoCorr2_A : public AutoCorr_E
{
public:
        // Default constructor
        AutoCorr2_A();
        // Constructor with name
        AutoCorr2_A(const char *Name,short int Dec,short int First,short int Last);
        // Destructor
        ~AutoCorr2_A();

        // Define the I/O
        virtual void IO_Def(Vectord *Frame,Vectord *AutoCorrCoef);
        // Process
        virtual int Process();
        // Reset the block
        virtual void Reset();
protected:
        // Internal Variables
        short int dec,first,last;
};

