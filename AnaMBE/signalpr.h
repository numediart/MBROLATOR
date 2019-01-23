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
// signalpr.h                                                                                                      signalpr.cpp
//
// functions:
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             21.03.1996: Creation from mathlib.c and dsplib.c (T.Dutoit)
//                              25.03.1996: Modification of Norm2_CV (vectord + first + last)
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_SIGNALPR

#ifndef _INC_DATATYPE
#include "datatype.h"
#endif

#ifndef _INC_COMPLEX
#include "complex.h"
#endif

// ============================================================================

// Compute the product element dy element of two vector
void eltmult_V(Vectord v1,Vectord v2, Vectord* Result);

// ============================================================================

// Divide a complex vector by a real vector
void div_CVV(VectorComplex v1,Vectord v2,CComplex* Result);

// ============================================================================

// Norm of a vector (double)
double norm2_V(Vectord v1);

// ============================================================================

// Norm of a complex vector
double norm2_CV(VectorComplex *v1,unsigned short First=0,
                                unsigned short Last=-1);

// ============================================================================

// Innerproduct of a double vector with a complex vector
CComplex innerproduct_VCV(Vectord v1,VectorComplex v2);

// ============================================================================

// Abs of a vector (double)
void abs_V(Vectord v1,Vectord* result);

// ============================================================================

// Abs of a complex vector
void abs_CV(VectorComplex v1,Vectord* result);

// ============================================================================

// Sum of a vector (double)
double sum_V(Vectord v1);

// ============================================================================

//
void freqz_FIR(Vectord ZCoef,short first,short last,Vectord Omega,
                           VectorComplex* FreqResp);

// ============================================================================

// Compute the complex FFT of a real signal
void fft_R(Vectord Signal,VectorComplex* ComplexFFT);
void initialisation_RFFT(int N,Vectord* cossin);

// ============================================================================

// FFT with a split radix method of length (N/2)
void rfft(VectorComplex* v1,Vectord* cossin,int N);     // Reorganisation
void fft(VectorComplex* v1,Vectord* cossin,int N);
