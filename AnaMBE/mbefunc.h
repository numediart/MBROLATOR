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
// mbefunc.h                                                                                                                    mbe.cpp
//
// functions: InnerProduct
//                        Interpola
//                        GetErrorValue
//                        PosMin
//                        LowPassFilter
//                        HammingWindow
//                        Windowing
//                        AutoCorrelation
//
// Author: Fabrice Malfrère     - TCTS Labs - FRIA Researcher
//
// History:             15.03.1996: Creation from pitchmbe.h
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_MBEFUNC

// ----------------------------------------------------------------------------
// Compute the inner product of two vectors. The dimension of the vectors is
// given by Dim
double InnerProduct(double *Signal1, double *Signal2,
                    short int Dim); // Excepttion

// ----------------------------------------------------------------------------

// Determine by interpolation the signal's value in NewIndex position
double Interpola(double *Signal,short int Dim,double NewIndex); // Exception

// ----------------------------------------------------------------------------

// Evaluate the error function for a value specified of the double of the
// period
double GetErrorValue(int Period2,short int Width,
                     double Energy,double Sommw4,double *Autocorr);// Exception

// ----------------------------------------------------------------------------

// Determine the position of the signal's minimum
short int PosMin(double *Signal,short int Dim);  // Exception

// ----------------------------------------------------------------------------

// Determine the position of the signal's maximum
short int PosMax(double *Signal,short int Dim);

// ----------------------------------------------------------------------------

// Low-Pass filtering
double *LowPassFilter(double *Coef,short int Order,
                      double *Signal,long int Length);  // Exception

// ----------------------------------------------------------------------------

// Create Hamming window
void HammingWindow(double *wind,short int Width); // Exception

// ----------------------------------------------------------------------------

// Compute the product Signal1*Signal2
void Windowing(double *Signal1,double *Signal2,short int Dim); // Exception

// ----------------------------------------------------------------------------

// Compute the autocorrelation of Signal
void Autocorrelation(double *Signal,double *Autocorr,
                     short int Length,short int Order); // Exception
