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
// func.cpp
//
// functions: minab(int,int)  (func.h)
//			  maxab(int,int)  (func.h)
//			  round(double) (func.h)
//			  Convert16to8(SoundFile*,const char*)
//			  Convert16to4(SoundFile*,const char*)	
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
// 
// History:		03.10.1995: Creation
//				23.11.1995: Formalisation
//				11.03.1996: #define #ifndef
//				30.07.1996: Convert16to8 and Convert16to8
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#ifndef _INC_STDLIB
#include <stdio.h>
#endif

#ifndef _INC_FUNC
#include "func.h"
#endif

// ============================================================================

// Give the minimum of a and b
int minab(int a,int b)
{ 
	if (a>b)			// If a=b return b
	{
		return b;
	}
	else
	{
		return a; 
	}
};

// ============================================================================

// Give the maximum of a and b
int maxab(int a,int b)
{ 
	if (a>b)			// If a=b return b
	{
		return a;
	}
	else 
	{
		return b;
	}
};

// ============================================================================

// Give the rounded value of a  
int round(double a)
{ 
	int value;
	double diff;
	value=int(a);
	diff=a-value;
	if (diff>=0)
	{ 
		if (diff>=0.5)
		{ 
			value++;
		}
	}
	else 
	{ 
		if (diff<=-0.5)
        {
			value--;
        }
    }
	return value ;
};

// ============================================================================

// Convert a sound file from 16kHz to 4kHz
void Convert16to4(SoundFile* Son,const char* Name)
{
	long int i;
	short* SAMPLE=new short[Son->Length()];
	Son->LoadSamples(Son->Length(),0,SAMPLE);
	FILE* ficconv;
	ficconv=fopen(Name,"wb");
	for(i=0;i<Son->Length();i=i+4)
	{
		fwrite(SAMPLE+i,sizeof(short),1,ficconv);
	}
	fclose(ficconv);
	delete SAMPLE;
};

// ============================================================================

// Convert a sound file from 16kHz to 8kHz
void Convert16to8(SoundFile* Son,const char* Name)
{
	long int i;
	short* SAMPLE=new short[Son->Length()];
	Son->LoadSamples(Son->Length(),0,SAMPLE);
	FILE* ficconv;
	ficconv=fopen(Name,"wb");
	for(i=0;i<Son->Length();i=i+2)
	{
		fwrite(SAMPLE+i,sizeof(short),1,ficconv);
	}
	fclose(ficconv);
	delete SAMPLE;
};
