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
//============================================================================
// complex.cpp 
//
// classes:	CComplex
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:		21.03.1996: Creation
//	   
//----------------------------------------------------------------------------
// Documentation:
//
//============================================================================

#ifndef _INC_COMPLEX
#include "complex.h"
#endif

#ifndef _INC_MATH 
#include <math.h>
#endif

// ============================================================================
// Class CComplex
// ============================================================================

CComplex::CComplex() // Default Constructor
{	
	Real=0;
	Imaginary=0;
	//Module=0;
	//Phase=0;  
};

// ----------------------------------------------------------------------------

CComplex::CComplex(double a,double b,int type)
                   // if type=0 then Real=a
{ 
	if (type==0)     // and Imaginary=b
	{ 
		Real=a;       // if type=1 then Module=a
		Imaginary=b; // and Phase=b	
		RIToMP();
	}
	else 
	{ 
		if (type==1)
        { 
			//Module=a;
			//Phase=b;
			MPToRI();
		}
		else 
		{ 
			Real=0;
			Imaginary=0;
			//Module=0;
			//Phase=0; 
	    }
    }
};

// ----------------------------------------------------------------------------

CComplex::~CComplex()
{
};

// ----------
// Operators
// ----------

/*CComplex operator+(CComplex c1,CComplex c2)
{ 
	CComplex a(c1.Real+c2.Real,c1.Imaginary+c2.Imaginary,0);
	return a;
};

// ----------------------------------------------------------------------------

CComplex operator+(CComplex c1,double c2)
{ 
	CComplex a(c1.Real+c2,c1.Imaginary,0);
	return a;
};

// ----------------------------------------------------------------------------

CComplex operator+(double c1,CComplex c2)
{ 
	CComplex a(c1+c2.Real,c2.Imaginary,0);
	return a;
};

// ----------------------------------------------------------------------------

CComplex operator-(CComplex c1,CComplex c2)
{ 
	CComplex a(c1.Real-c2.Real,c1.Imaginary-c2.Imaginary,0);
	return a;
};

// ----------------------------------------------------------------------------

CComplex operator-(CComplex c1,double c2)
{ 
	CComplex a(c1.Real-c2,c1.Imaginary,0);
	return a;
};

// ----------------------------------------------------------------------------

CComplex operator-(double c1,CComplex c2)
{ 
	CComplex a(c1+c2.Real,c2.Imaginary,0);
	return a;
};

// ----------------------------------------------------------------------------

CComplex operator*(CComplex c1,CComplex c2)
{ 
	CComplex a(c1.Module*c2.Module,c1.Phase+c2.Phase,1);
	return a;
};

// ----------------------------------------------------------------------------

CComplex operator*(CComplex c1,double c2)
{ 
	CComplex a(c1.Module*c2,c1.Phase,1); 
	return a;
};

// ----------------------------------------------------------------------------

CComplex operator*(double c1,CComplex c2)
{ 
	CComplex a(c1*c2.Module,c2.Phase,1);
	return a;
};

// ----------------------------------------------------------------------------

CComplex operator/(CComplex c1,CComplex c2)
{ 
	if (c2.Module==0.0) 
	{ 
		CComplex a(1E308,c1.Phase-c2.Phase,1);
		return a;
	}
	else
	{
		CComplex a(c1.Module/c2.Module,c1.Phase-c2.Phase,1);
		return a;
	}
};

// ----------------------------------------------------------------------------

CComplex operator/(CComplex c1,double c2)
{ 
	if (c2==0.0)
	{
		CComplex a(1E308,c1.Phase,1);
		return a;
	}
  	else 
	{
		CComplex a(c1.Module/c2,c1.Phase,1);
		return a;
	}
};

// ----------------------------------------------------------------------------

CComplex operator/(double c1,CComplex c2)
{ 
	if (c2.Module==0.0)
	{  
		CComplex a(1E308,-c2.Phase,1);
		return a;
	}
	else
	{
		CComplex a(c1/c2.Module,-c2.Phase,1);
		return a;
	}
};

// ----------------------------------------------------------------------------

CComplex operator^(CComplex c1,double c2)
{ 
	CComplex a;
	a.Module=pow(c1.Module,c2);
	a.Phase=c1.Phase*c2;
	a.MPToRI();
	return a;
};
*/
// ----------------------------------------------------------------------------

CComplex CComplex::operator=(CComplex c1)
{ 
	Real=c1.Real;
	Imaginary=c1.Imaginary;
//	Module=c1.Module;
//	Phase=c1.Phase;
	return *this;
};

// ----------------------------------------------------------------------------

CComplex CComplex::operator=(double c1)
{ 
	Real=c1;
	Imaginary=0;
//	Module=c1;
//	Phase=0;
	return *this;
};

// ----------------------------------------------------------------------------

int operator==(CComplex c1,CComplex c2)
{ 
  if ((c1.Real==c2.Real)&&(c1.Imaginary==c2.Imaginary))
	 { 
		return 1;
	 }
  else 
	 { 
		return 0;
	 }
};

// ----------
// Functions 
// ----------

CComplex Conjugate(CComplex c1)
{ 
	CComplex a(c1.Real,-c1.Imaginary,0);
	return a;
};

// ----------------------------------------------------------------------------

double CComplex::GetModule()
{
	//RIToMP();
	double Module;
	Module=sqrt(Real*Real+Imaginary*Imaginary);
	return Module;
};

// ----------------------------------------------------------------------------

double CComplex::GetPhase()
{
	//RIToMP();
	double Phase;
	Phase=atan2(Imaginary,Real);
	return Phase;
};

// ----------------------------------------------------------------------------

void CComplex::RIToMP() // Convert a complex (real,imaginary) number
{                       // into a complex (Module,Phase) number
  //  	Module=sqrt(Real*Real+Imaginary*Imaginary);
  //	Phase=atan2(Imaginary,Real);
};

// ----------------------------------------------------------------------------

void CComplex::MPToRI() // Convert a complex (Module,Phase) number
{ 
  //	Real=Module*cos(Phase); // into a complex (Real,Imaginary) number
  //	Imaginary=Module*sin(Phase);
};
