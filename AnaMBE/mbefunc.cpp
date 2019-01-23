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

// ============================================================================
// Functions
// ============================================================================

// Compute the inner product of two vectors. The dimension of the vectors is
// given by Dim
double InnerProduct(double *Signal1, double *Signal2, short int Dim)
{ 
  double temp=0; 
  try
	 {
		short int i;
		for(i=0;i<Dim;i++)
		  { 
			 temp=temp+Signal1[i]*Signal2[i]; 
		  }
	 }
  catch (...)
	 {
		throw FMError("\nFunction InnerProduct => Error \n");
	 }
  return temp;
};

// ============================================================================

// Determine by interpolation the signal's value in NewIndex position 
double Interpola(double *Signal,short int Dim,double NewIndex)
{ 
  try
	 {
		//printf("\n %f\n",NewIndex);
		double prev,next;
		if ((NewIndex>Dim-1)||(NewIndex<0)) 
		  { 
			 throw FMError("\nFunction Interpola => New index not allowed\n");
		  }
		prev=Signal[(int)floor(NewIndex)];
		next=Signal[(int)floor(NewIndex+1)];
		return (prev+(NewIndex-floor(NewIndex))*(next-prev));
	 }
  catch (FMError& Error1)
	 {
		throw Error1;
	 }
  catch (...)
	 { 
		throw FMError("\nFunction Interpola => Error\n");
	 }
};

// ============================================================================

// Evaluate the error function for a value specified of the double of the 
// period 
double GetErrorValue(int Period2,short int WindowWidth,double Energy,
							double Sommw4,double *Autocorr)
{ 
  double temp1=0,temp2;
  try
	 {
		int i;
		temp2=(double)(WindowWidth-1)*2/(double)Period2;// attention: 
		// oversampling by 2
		for(i=1;i<=int(temp2);i++)
		  { 
			 temp1+=Autocorr[i*Period2];
		  }
		temp2=(double)Period2/2;
		temp1=(2*temp1+Autocorr[0])*temp2;
		if (Energy==double(0))
		  { 
			 return 1E98;
		  }
	 }
  catch (...)
	 {
		throw FMError("\nFunction GetErrorValue => Error\n");
	 }
  return ((Energy-temp1)/Energy/(1-temp2*Sommw4));
};

// ============================================================================

// Determine the position of the signal's minimum
short int PosMin(double *Signal,short int Dim)
{ 
  short int PosTemp=0;
  try
	 {
		short int i;
		double temp;
		temp=1E99;
		for(i=0;i<Dim;i++)
		  { 
			 if (Signal[i]<temp)
				{ 
				  temp=Signal[i];
				  PosTemp=i;
				}
		  }
	 }
  catch (...)
	 {
		throw FMError("\nFunction PosMin => Error\n");
	 }
  return (PosTemp); 
};

// ============================================================================

// Determine the position of the signal's maximum
short int PosMax(double *Signal,short int Dim)
{
  short int PosTemp=0;
  try
	 {
		short int i;
		double temp;
		temp=-1E99;
		for(i=0;i<Dim;i++)
		  { 
			 if (Signal[i]>temp)
				{ 
				  temp=Signal[i];
				  PosTemp=i;
				}
		  }
	 }
  catch (...)
	 {
		throw FMError("\nFunction PosMax => Error\n");
	 }
  return (PosTemp); 
};

// ============================================================================

// This function must become a block in the futur
// Low-Pass filtering
double *LowPassFilter(double *Coef,short int Order,double *Signal,long int Length)
{ 
  try
	 {
		long int j,k;
		for(j=Order-1;j<(Length+Order-1);j ++)
		  { 
			 Signal[j]=Coef[0]*Signal[j];
			 for(k=0;k<(Order-1);k++)
				{ 
				  Signal[j]=Signal[j]+Coef[k+1]*(Signal[j+k+1]+Signal[j-k-1]);
				}
		  }  
	 }
  catch (...)
	 {
		throw FMError("\nFunction LowPassFilter => Error\n");
	 }
  return (Signal);   
};

// ============================================================================

// Create Hamming window
void HammingWindow(double *wind,short int Width)
{ 
  try
	 {
		short int i;
		for(i=0;i<Width;i++)
		  { 
			 wind[i]=0.54-0.46*cos(i*6.2831853/(double)Width);
		  }  
	 }
  catch (...)
	 {
		throw FMError("\nFunction HammingWindow => Error\n");
	 }
  
};

// ============================================================================

// Compute the product Signal1*Signal2. Result in Signal1
void Windowing(double *Signal1,double *Signal2,short int Dim)
{ 
  try
	 {
		short int i;  
		for(i=0;i<Dim;i++)
		  { 
			 Signal1[i]=Signal1[i]*Signal2[i];
		  }
	 }
  catch (...)
	 {
		throw FMError("\nFunction Windowing => Error\n");
	 }
};

// ============================================================================

// Compute the autocorrelation of Signal
void Autocorrelation(double *Signal,double *Autocorr,
                     short int Length, short int Order)
{ 
  try
	 {
		double vr;
		short int i,j,k;  
		for(i=0;i<Order;i++)
		  { 
			 vr=0;
			 k=Length-i;
			 for(j=0;j<k;j++)
				{ 
				  vr=vr+Signal[j] * Signal[j+i];
				}
			 Autocorr[i]=vr;
		  }
	 }
  catch (...)
	 {
		throw FMError("\nFunction AutoCorrelation => Error\n");
	 }
};
