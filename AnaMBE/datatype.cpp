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
// datatype.cpp
// 
// classes: PitchObject
//			PitchList
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:		12.03.1996: Creation from data.cpp (PitchObject)
//							PitchList
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#ifndef _INC_STRING
#include <string.h>
#endif

#ifndef _INC_CONSTANT
#include "constant.h"
#endif

#ifndef _INC_ERROREXC
#include "errorexc.h"
#endif

#ifndef _INC_DATATYPE
#include "datatype.h"
#endif

// ============================================================================
// Class PitchObject
// ============================================================================

// Constructor: Initialise the object and allocate memory
PitchObject::PitchObject(int NbPitchParameters,
                         short int Wwidth,
								 short int Wshift,
								 long int first,
								 long int last)
{ 
	try
	{
		FMObject::SetName("No Name");
		Curve=new double[NbPitchParameters];
		Voicing=new short int[NbPitchParameters];
		Nb=NbPitchParameters;
		Width=Wwidth;
		Shift=Wshift;
		LastSample=last;
		FirstSample=first;
	}
	catch (...)
	  {
		throw FMError("\nPitchObject Constructor\n",ObjectName);
	}
};

// ============================================================================

// Constructor with naming capabilities
PitchObject::PitchObject(const char *Name,int NbPitchParameters,
						 short int Wwidth,short int Wshift,long int first,
						 long int last)
{
	try
	{
		FMObject::SetName(Name);
		Curve=new double[NbPitchParameters];
		Voicing=new short int[NbPitchParameters];
		Nb=NbPitchParameters;
		Width=Wwidth;
		Shift=Wshift;
		FirstSample=first;
		LastSample=last;
	}
	catch (...)
	{
		throw FMError("\nPitchObject Constructor\n",ObjectName);
	}
};
 
// ============================================================================

// Remove 2 pitch points
void PitchObject::Trim()
{ 
  try
	 {
		double *CurveBis;
		
		// Clone the old Curve
		CurveBis= new double[Nb-2];
		memcpy(CurveBis, &Curve[1], (Nb-2)*sizeof(double));
		
		// Remove 2 elements and delete old curve
		Nb-=2;
		delete [] Curve;
		Curve= CurveBis;
	 }
  catch (...)
	 {
		throw FMError("\nPitchObject Trim\n",ObjectName);
	 }
}

// Destructor: Free the memory
PitchObject::~PitchObject()
{ 
	try
	{
		delete [] Curve;
		delete [] Voicing;
	}
	catch (...)
	{
		throw FMError("\nPitchObject destructor\n",ObjectName);
	}
};

// ============================================================================

// Save the pitch parameters in a specified file
int PitchObject::SaveParameters(const char *FileName)
{ 
	FILE *foutput;
	int i;
	foutput=fopen(FileName,"w");
	if (!foutput)
	{ // Error while opening the file
		throw FMError("\nPitchObject SaveParameters => opening file\n",
				      FileName,ObjectName);
	}
	fprintf(foutput,"%hd ",Width); // Save the width and the shift of the 
	fprintf(foutput,"%hd ",Shift); // frame used to perform the analysis
	fprintf(foutput,"%ld ",FirstSample); // first sample of the analysis
	fprintf(foutput,"%ld\n ",LastSample); // last sample of the analysis
	for(i=0;i<Nb;i++)
	  { 
		 fprintf(foutput,"%6.2f\n",Curve[i]);	// Save the pitch value
		 //fprintf(foutput,"%hi \n",Voicing[i]); // Save the voicing character
	  }
	fclose(foutput);
	return (0); 
};

// ============================================================================

// Load the pitch parameters from a specified file
int PitchObject::LoadParameters(const char *FileName)
{ 
	FILE *foutput;
	int i;
	foutput=fopen(FileName,"rt");
	if (!foutput)
	{ // Error while opening the file
		throw FMError("\nPitchObject LoadParameters => opening file\n",
				      FileName,ObjectName);
	}
	//	fscanf(foutput,"%d", &Nb);		// Retrieve the number of parameters
	fscanf(foutput," %hd ",&Width);  // Retrieve the width and the shift of
	fscanf(foutput," %hd ",&Shift);	// the frame used for the analsyis
	fscanf(foutput," %ld ",&FirstSample); // first sample of the analysis
	fscanf(foutput," %ld \n",&LastSample); // last sample of the analysis

	Nb= (LastSample-FirstSample+1)/Shift;
		
	if (Curve!=NULL)
	{
		delete [] Curve;
	}
	if (Voicing!=NULL)
	{
		delete [] Voicing;
	}
    Curve=new double[Nb];
	Voicing=new short int[Nb]; 
	for(i=0;i<Nb;i++)
	  { 
		 fscanf(foutput,"%lf",&Curve[i]);	// Retrieve the pitch value
		 //fscanf(foutput,"%hi \n",&Voicing[i]);
		 // Retrieve the voicing character
	  }
	fclose(foutput);
	return (0); 
};

// ============================================================================

// Save the pitch parameters in a binary file
int PitchObject::Save(const char *FileName)
{ 
	FILE *foutput;
	foutput=fopen(FileName,"wb");
	if (!foutput)
	{ // Error while opening the file
		throw FMError("\nPitchObject Save => opening file\n",
				      FileName,ObjectName);
	}
	fwrite(&Nb,sizeof(int),1,foutput); // Save the number of parameters
	fwrite(&Width,sizeof(short int),1,foutput); // Save the characteristics of
	fwrite(&Shift,sizeof(short int),1,foutput);	// the frame of the analysis
	fwrite(&FirstSample,sizeof(long int),1,foutput); // the first and the last
	fwrite(&LastSample,sizeof(long int),1,foutput); // sample of the analysis 														
	fwrite(&Curve,sizeof(double),Nb,foutput); // Save the pitch value
	fwrite(&Voicing,sizeof(short int),Nb,foutput); // Save the voicing 
												   // character
	fclose(foutput);
	return (0);
};

// ============================================================================

// Load the parameters from a binary file
int PitchObject::Load(const char *FileName)
{ 
	FILE *foutput;
	foutput=fopen(FileName,"rb");
	if (!foutput)
	{ // Error while opening the file
		throw FMError("\nPitchObject Save => opening file\n",
				      FileName,ObjectName);
	}
	fread(&Nb,sizeof(int),1,foutput);	// Retrieve the number of parameters
	fread(&Width,sizeof(short int),1,foutput); // Retrieve the frame parameters
	fread(&Shift,sizeof(short int),1,foutput);
	fread(&FirstSample,sizeof(long int),1,foutput); // the first and the last
	fread(&LastSample,sizeof(long int),1,foutput); // sample of the analysis 														
	fread(&Curve,sizeof(double),Nb,foutput); // Retrieve the pitch curve
	fread(&Voicing,sizeof(short int),Nb,foutput); // Retrieve the voicing curve
	fclose(foutput);
	return (0);
};

// ============================================================================

// Reset the parameters
void PitchObject::ResetParameters(int NbPitchParameters,
                                  short int Wwidth,
                                  short int Wshift,
								  long int first,
								  long int last)
{ 
	try
	{
	delete [] Curve;
	delete [] Voicing;
	Curve=new double[NbPitchParameters];
	Voicing=new short int[NbPitchParameters];
	Nb=NbPitchParameters;
	Width=Wwidth;
	Shift=Wshift;
	FirstSample=first;
	LastSample=last;
	}
	catch (...)
	{
		throw FMError("\nPitchObject ResetParameters\n",ObjectName);
	}
};

// ============================================================================

// Send an index error exception
void PitchObject::SendErrorMessage(int index,const char *func)
{
	char *Message;
	int l;
	l=strlen("PitchObject ");
	l+=strlen(func);
	l+=strlen("\nIndex ######### not allowed\n");
	l+=strlen(ObjectName);
	Message=new char[l];
	sprintf(Message,"PitchObject %s\n Index %i not allowed\n%s\n",
			func,index,ObjectName);
	throw FMError(Message);
};

// ============================================================================

// Get the pitch value at sample position pos
double PitchObject::GetPitch_Step(long int pos)
{
	short CurrentFrame;
	CurrentFrame=((pos-FirstSample)/Shift)+1;
	if (CurrentFrame<(Nb-1))
	{
		return Curve[CurrentFrame];
	}
	else
	{
		return Curve[Nb-1];
	}
};

// ============================================================================
// Class PitchList
// ============================================================================

// Default constructor
PitchList::PitchList()
{ 
	FMObject::SetName("No Name"); // Default object name
	F0S=NULL; // Current marker
	First=NULL;  // First marker
	Number=-1; // Number of markers
};

// ============================================================================

// Constructor with Name
PitchList::PitchList(const char *Name)
{
   	FMObject::SetName(Name); // Set the object name
	F0S=NULL; // Current marker
	First=0; // First marker
	Number=-1; // Number of markers
};

// ============================================================================

// Destructor
PitchList::~PitchList()
{ 
	try
	{
		if (Number!=-1)
		{
			while (First->Prev!=First->Next)
			{ 
				F0S=First->Prev;
				First->Prev->Next=First->Next;
				First->Next->Prev=First->Prev;
				delete First;
				First=F0S->Prev;
			}
			if (First!=0)
			{
				delete First;
			}
		}
	}
	catch (...)
	{ 
		throw FMError("PitchList destructor Error\n",ObjectName);
	}
};

// ============================================================================

// Add a new marker to the list
void PitchList::AddNew(short int a,long int pos)
{ 
	try
	{
		if (Number==-1)			// If there is no elements into the list
		{ 
			F0S=new PitchMarker;
			F0S->Value=a;
			F0S->Position=pos;
			F0S->Next=NULL;
			F0S->Prev=NULL;  
			First=F0S;
			Number=1;
		}
		else 
		{	F0S->Next=new PitchMarker;
			F0S->Next->Prev=F0S;
			F0S->Next->Next=First;
			First->Prev=F0S->Next;
			F0S=F0S->Next;
			F0S->Value=a;
			F0S->Position=pos;
			Number++;
		}
	}
	catch (...)
	{
		throw FMError("PitchList AddNew Error\n",ObjectName);
	};
};

// ============================================================================

// Go to the next marker
void PitchList::Next()
{ 
	F0S=F0S->Next;
};

// ============================================================================

// Go to the previous marker
void PitchList::Prev()
{ 
	F0S=F0S->Prev;
};
