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
// signalpr.cpp
//
// functions: eltmult_V (signalpr.h)
//                        div_CVV (signalpr.h)
//                        norm2_V (signalpr.h)
//                        norm2_CV (signalpr.h)
//                        innerproduct_VCV (signalpr.h)
//                        abs_V (signalpr.h)
//                        abs_CV (signalpr.h)
//                        sum_V (signalpr.h)
//                        freqz_FIR (signalpr.h)
//                        rfft (signalpr.h)
//                        fft (signalpr.h)
//                        fft_R (signalpr.h)
//                        initialisation_RFFT (signalpr.h)
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             21.03.1996: Creation from mathlib.c and dsplib.c (T.Dutoit)
//                              27.03.1996: Implementation of fft

//                              06.05.1996: New implementation of fft
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#ifndef _INC_SIGNALPR
#include "signalpr.h"
#endif

#ifndef _INC_CONSTANT
#include "constant.h"
#endif

#ifndef _INC_DATATYPE
#include "datatype.h"
#endif

#ifndef _INC_COMPLEX
#include "complex.h"
#endif

#ifndef _INC_MATH
#include "math.h"
#endif


#ifndef _INC_FUNC

#include "func.h"

#endif


// ============================================================================

// Compute the product element dy element of two vector
// v1,v2 and v3 must have the same length
void eltmult_V(Vectord v1,Vectord v2, Vectord* Result) // Exception
{
        try
        {
                /*if ((v1.GetLength()!=v2.GetLength())||
                        (v1.GetLength()!=Result->GetLength())||
                        (v2.GetLength()!=Result->GetLength()))
                {
                        throw
                           FMError("\neltmult_V function => Vector of different length\n");
                }*/
                int i,l;
                l=v1.GetLength();
                for(i=0;i<l;i++)
                {
                        (*Result)[i]=v1[i]*v2[i];
                }
        }
        catch (FMError& Error1)
        {
                throw FMError("\neltmult_V function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\neltmult_V function => Error\n");
        }
};

// ============================================================================

// Divide a complex vector by a real vector
void div_CVV(VectorComplex v1,Vectord v2,CComplex* Result)
{
        try
        {
                double temp2;
                CComplex temp;
                temp=innerproduct_VCV(v2,v1);
                temp2=norm2_V(v2);
                Result->Real=temp.Real/temp2;
                Result->Imaginary=temp.Imaginary/temp2;
        }
        catch (FMError& Error1)
        {
                throw FMError("\ndiv_CVV function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\ndiv_CVV function => Error\n");
        }
};

// ============================================================================

// Norm^2 of a vector (double)
double norm2_V(Vectord v1)
{
        try
        {
                int i,l;
                double temp=0.0;
                l=v1.GetLength();
                for(i=0;i<l;i++)
                {
                        temp+=v1[i]*v1[i];
                }
                return temp;
        }
        catch (FMError& Error1)
        {
                throw FMError("\nnorm2_V function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\nnorm2_V function => Error\n");
        }
};

// ============================================================================

// Norm^2 of a complex vector
double norm2_CV(VectorComplex* v1,unsigned short First,unsigned short Last)
{
        try
        {
                int i;
                double temp=0.0;
                if (Last==(unsigned short)-1)
                {
                        Last=v1->GetLength();
                }
                for(i=First;i<Last;i++)
                {
                        temp+=(*v1)[i].Real*(*v1)[i].Real+(*v1)[i].Imaginary*(*v1)[i].Imaginary;
                }
                return temp;
        }
        catch (FMError& Error1)
        {
                throw FMError("\nnorm2_CV function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\nnorm2_CV function => Error\n");
        }
};

// ============================================================================

// Innerproduct of a double vector with a complex vector
CComplex innerproduct_VCV(Vectord v1,VectorComplex v2)
{
        try
        {
                if (v1.GetLength()!=v2.GetLength())
                {
                        throw
                                FMError("\ninnerproduct_VCV function => Length difference\n");
                }
                int i,l;
                CComplex a(0.0,0.0);
                l=v1.GetLength();
                for(i=0;i<l;i++)
                {
                        a.Real+=v2[i].Real*v1[i];
                        a.Imaginary+=v2[i].Imaginary*v1[i];
                }
                return a;
        }
        catch (FMError& Error1)
        {
                throw FMError("\ninnerproduct_VCV function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\ninnerproduct_VCV function => Error\n");
        }
};

// ============================================================================

// Module of the elements of a vector (double)
void abs_V(Vectord v1,Vectord* result)
{
        try
        {
                if (v1.GetLength()!=result->GetLength())
                {
                        throw FMError("\nabs_V function => different length\n");
                }
                int i,l;
                l=v1.GetLength();
                for(i=0;i<l;i++)
                {
                        (*result)[i]=fabs(v1[i]);
                }
        }
        catch (FMError& Error1)
        {
                throw FMError("\nabs_V function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\nabs_V function => Error\n");
        }
};

// ============================================================================

// Abs of a complex vector
void abs_CV(VectorComplex v1,Vectord* result)
{
        try
        {
                if (v1.GetLength()!=result->GetLength())
                {
                        throw FMError("\nabs_CV function => different length\n");
                }
                int i,l;
                l=v1.GetLength();
                for(i=0;i<l;i++)
                {
                        (*result)[i]=sqrt(v1[i].Real*v1[i].Real+

                                              v1[i].Imaginary*v1[i].Imaginary);
                }
        }
        catch (FMError& Error1)
        {
                throw FMError("\nabs_CV function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\nabs_CV function => Error\n");
        }
};

// ============================================================================

// Sum of a vector (double)
double sum_V(Vectord v1)
{
        try
        {
                int i,l;
                double temp=0.0;
                l=v1.GetLength();
                for(i=0;i<l;i++)
                {
                        temp+=v1[i];
                }
                return temp;
        }
        catch (FMError& Error1)
        {
                throw FMError("\nsum_V function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\nsum_V function => Error\n");
        }
};

// ============================================================================

void freqz_FIR(Vectord ZCoef,short first,short last,Vectord Omega,
                           VectorComplex* FreqResp)
{
        try
        {
           int i,j;
           int NOmega;
           double Omegak;
           NOmega=Omega.GetLength();
           for(i=0;i<NOmega;i++)
           {
                   (*FreqResp)[i].Real=ZCoef[first];
                   (*FreqResp)[i].Imaginary=0.0;
                   for(j=first+1;j<last;j++)
                   {
                           Omegak=Omega[i]*(j-first);
                           (*FreqResp)[i].Real+=ZCoef[j]*cos(Omegak);
                           (*FreqResp)[i].Imaginary-=ZCoef[j]*sin(Omegak);
                   }
           }
        }
        catch (FMError& Error1)
        {
                throw FMError("\nfreqz_FIR function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\nfreqz_FIR function => Error\n");
        }
};

// ============================================================================

// Compute the complex FFT of a real signal
void fft_R(Vectord Signal,VectorComplex* ComplexFFT)
{
        try
        {
                int NPoints,i,NFFT;
                NFFT=ComplexFFT->GetLength()*2-2;
                NPoints=Signal.GetLength();
                Vectord cs(NFFT+1,"fft_R cs");
                initialisation_RFFT(NFFT,&cs);
                if (NPoints>NFFT)
                {
                        NPoints=NFFT;
                }
                int i2=0;
                for(i=0;i<NPoints;i++)
                {
                        (*ComplexFFT)[i2].Real=Signal[i++];
                        (*ComplexFFT)[i2].Imaginary=Signal[i];
                        i2++;
                }
                for(i=i2;i<NFFT/2;i++)
                {
                        (*ComplexFFT)[i].Real=0.0;
                        (*ComplexFFT)[i].Imaginary=0.0;
                }
                rfft(ComplexFFT,&cs,NFFT);
                (*ComplexFFT)[NFFT/2].Real=(*ComplexFFT)[0].Imaginary;
                (*ComplexFFT)[0].Imaginary=0.0;
                (*ComplexFFT)[NFFT/2].Imaginary=0.0;
        }
        catch (FMError& Error1)
        {
                throw FMError("\nfft_R function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\nfft_R function => Error\n");
        }
};

// ============================================================================

void initialisation_RFFT(int N,Vectord* cossin)
{
        try
        {
                double theta,arg;
                int i;
                theta=2*PI/N;
                arg=0;

                for(i=0;i<(N/4);i++)
                {
                        cossin->Value[2*i]=cos(arg);
                        cossin->Value[2*i+1]=sin(arg);
                        arg+=theta;

                }
                int N2;
                N2=N/2;
                theta=2*PI/N2;
                arg=0;
                for(i=0;i<(N2/4-1);i++)
                {
                        arg+=theta;
                        (*cossin)[N2+i*4]=cos(arg);
                        (*cossin)[N2+i*4+1]=sin(arg);
                        (*cossin)[N2+i*4+2]=cos(3*arg);
                        (*cossin)[N2+i*4+3]=sin(3*arg);
                }
        }
        catch (FMError& Error1)
        {
                throw FMError("\ninitialisation_RFFT function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\ninitialisation_RFFT function => Error\n");
        }
};

// ============================================================================

// FFT with a split radix method of length (N/2)
void rfft(VectorComplex* v1,Vectord* cossin,int N)      // Reorganisation
{
        try
        {
                int i,j,N2,is;
                int tmp0,tmp1;
                double r,t1,t2,s1,s2,c,s;
                N2=N/2;
                Vectord COSSIN(N/2,"rfft COSSIN");
                for (i=0;i<N/2;i++)
                {
                        COSSIN[i]=(*cossin)[i+N/2];
                }
                fft(v1,&COSSIN,N2);
                r=(*v1)[0].Real;
                (*v1)[0].Real=r+(*v1)[0].Imaginary;
                (*v1)[0].Imaginary=r-(*v1)[0].Imaginary;
                (*v1)[N2].Imaginary=-(*v1)[N2].Imaginary;
                tmp0=1;
                tmp1=N2-1;
                is=2;
                for(j=1;j<(int)(N/4);j++)
                {
                        t1=(*v1)[tmp0].Real+(*v1)[tmp1].Real;
                        t2=(*v1)[tmp0].Imaginary-(*v1)[tmp1].Imaginary;
                        r=(*v1)[tmp1].Real-(*v1)[tmp0].Real;
                        s2=(*v1)[tmp1].Imaginary+(*v1)[tmp0].Imaginary;
                        c=(*cossin)[is++];
                        s=(*cossin)[is++];
                        s1=c*r-s*s2;
                        s2=s*r+c*s2;
                        (*v1)[tmp0].Real=(t1+s2)/2;
                        (*v1)[tmp0].Imaginary=(t2+s1)/2;
                        (*v1)[tmp1].Real=(t1-s2)/2;
                        (*v1)[tmp1].Imaginary=(s1-t2)/2;
                        tmp0++;
                        tmp1--;
                }
        }
        catch (FMError& Error1)
        {
                throw FMError("\nrfft function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\nrfft function => Error\n");
        }
};

// ============================================================================

void fft(VectorComplex* v1,Vectord* cossin,int N)
{
        try
        {

                int M;

                int n2,n4;

                int pas;

                int j,k; // Counters

                int i0,i1,i2,i3,i,id; // Indices

                double r1,r2,s1,s2,s3,co,so,ct,st; // Temporary

                M=round(log(N)/log(2));

                n2=N;

                pas=1;

                for(k=1;k<M;k++)

                {

                        n4=n2/4;

                        i0=0;

                        id=2*n2;

                        do

                        {

                                do

                                {

                                        i1=i0+n4;

                                        i2=i1+n4;

                                        i3=i2+n4;

                                        r1=(*v1)[i0].Real-(*v1)[i2].Real;

                                        (*v1)[i0].Real=(*v1)[i0].Real+(*v1)[i2].Real;

                                        r2=(*v1)[i1].Real-(*v1)[i3].Real;

                                        (*v1)[i1].Real=(*v1)[i1].Real+(*v1)[i3].Real;

                                        s1=(*v1)[i0].Imaginary-(*v1)[i2].Imaginary;

                                        (*v1)[i0].Imaginary=(*v1)[i0].Imaginary+(*v1)[i2].Imaginary;

                                        s2=(*v1)[i1].Imaginary-(*v1)[i3].Imaginary;

                                        (*v1)[i1].Imaginary=(*v1)[i1].Imaginary+(*v1)[i3].Imaginary;

                                        (*v1)[i2].Real=r1+s2;

                                        (*v1)[i2].Imaginary=s1-r2;

                                        (*v1)[i3].Real=r1-s2;

                                        (*v1)[i3].Imaginary=r2+s1;

                                        i0=i0+id;

                                }

                                while (i0<N-1);

                                i0=2*id-n2;

                                id=4*id;

                        }

                        while (i0<N-1);

                        // Butterfly  +++

                        i=-1;

                        for(j=1;j<n4;j++)

                        {

                                i=i+pas;

                                co=(*cossin)[4*i];

                                so=(*cossin)[4*i+1];

                                ct=(*cossin)[4*i+2];

                                st=(*cossin)[4*i+3];

                                i0=j;

                                id=2*n2;

                                do

                                {

                                        do

                                        {

                                                i1=i0+n4;

                                                i2=i1+n4;

                                                i3=i2+n4;

                                                r1=(*v1)[i0].Real-(*v1)[i2].Real;

                                                (*v1)[i0].Real=(*v1)[i0].Real+(*v1)[i2].Real;

                                                r2=(*v1)[i1].Real-(*v1)[i3].Real;

                                                (*v1)[i1].Real=(*v1)[i1].Real+(*v1)[i3].Real;

                                                s1=(*v1)[i0].Imaginary-(*v1)[i2].Imaginary;

                                                (*v1)[i0].Imaginary=(*v1)[i0].Imaginary+

                                                                                        (*v1)[i2].Imaginary;

                                                s2=(*v1)[i1].Imaginary-(*v1)[i3].Imaginary;

                                                (*v1)[i1].Imaginary=(*v1)[i1].Imaginary+

                                                                                        (*v1)[i3].Imaginary;

                                                s3=r1-s2;

                                                r1=r1+s2;

                                                s2=s1-r2;

                                                r2=r2+s1;

                                                (*v1)[i2].Real=r1*co+s2*so;

                                                (*v1)[i2].Imaginary=s2*co-r1*so;

                                                (*v1)[i3].Real=s3*ct+r2*st;

                                                (*v1)[i3].Imaginary=r2*ct-s3*st;

                                                i0=i0+id;

                                        }

                                        while (i0<N-1);

                                        i0=2*id-n2+j;

                                        id=4*id;

                                }

                                while (i0<N-1);

                        }

                        n2=n2/2;

                        pas=pas*2;

                }

                i0=0;

                id=4;

                do

                {

                        do

                        {

                                i1=i0+1;

                                r1=(*v1)[i0].Real;

                                (*v1)[i0].Real=r1+(*v1)[i1].Real;

                                (*v1)[i1].Real=r1-(*v1)[i1].Real;

                                r1=(*v1)[i0].Imaginary;

                                (*v1)[i0].Imaginary=r1+(*v1)[i1].Imaginary;

                                (*v1)[i1].Imaginary=r1-(*v1)[i1].Imaginary;

                                i0=i0+id;

                        }

                        while (i0<N-1);

                        i0=2*id-2;

                        id=4*id;

                }

                while (i0<N-1);

                j=0;

                for(i=0;i<N-1;i++)

                {

                        if (i<j)

                        {

                                r1=(*v1)[i].Real;

                                (*v1)[i].Real=(*v1)[j].Real;

                                (*v1)[j].Real=r1;

                                s1=(*v1)[i].Imaginary;

                                (*v1)[i].Imaginary=(*v1)[j].Imaginary;

                                (*v1)[j].Imaginary=s1;

                        }

                        k=N/2;

                        while (k<=j)

                        {

                                j=j-k;

                                k=k/2;

                        }

                        j=j+k;

                }
        }
        catch (FMError& Error1)
        {
                throw FMError("\nfft function\n","",Error1);
        }
        catch (...)
        {
                throw FMError("\nfft function => Error\n");
        }
};
