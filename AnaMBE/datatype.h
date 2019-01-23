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
// datatype.h                                                                                                      datatype.cpp
//
// structures: PitchMarker
//                         Pointd
//                         Pointi
//                         Label
//
// classes:     AnalyseParameters
//                      PitchObject
//                      PitchList
//
// templates: Vector
//
// predefined templates: Vectord (double)
//                                               Vectorsi (short int)
//                                               Vectoruc (unsigned char)
//                                               Vectorus (unsigned short)
//                                               Vectorf (float)
//                                               VectorComplex (CComplex)
//                                               VectorPointi
//                                               VectorPointd
//                       VectorLabel
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             11.03.1996: Created from data.h
//                              12.03.1996: PitchObject reviewed
//                              21.03.1996: CComplex Vector template
//                              27.03.1996: PitchObject::GetPitch_Step function
//                              07.06.1996: Pointi,Pointd,Label and vectors associated
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_DATATYPE

#ifndef _INC_STDIO
#include <stdio.h>
#endif

#ifndef _INC_STRING
#include <string.h>
#endif

#ifndef _INC_COMPLEX
#include "complex.h"
#endif

#ifndef _INC_CONSTANT
#include "constant.h"
#endif

#ifndef _INC_ERROREXC
#include "errorexc.h"
#endif

#ifndef _INC_FMOBJECT
#include "fmobject.h"
#endif

// ============================================================================
// struct Pointd: define the coordinates of a point (double)
// ============================================================================
typedef struct
{
  double X,Y;
} Pointd;

// ============================================================================
// struct Pointi: define the coordinates of a point (int)
// ============================================================================
typedef struct
{
  int X,Y;
} Pointi;

// ============================================================================
// struct Label: define a label
// ============================================================================
typedef struct
{
  double X;
  char label[10];
} Label;

// ============================================================================

template<class T> class Vector
{
public:
  T *Value; // Vector values

public:
  long int Length; // Length of the vector
  char Name[50]; // Name of the vector

public:
  Vector(long int Size) // Constructor with the size of the
  {
         Value=new T[Size];  // vector. Default name: 'No Name'
         Length=Size;           // Exception
         strcpy(Name,"No Name");
         if (Value==NULL)
                {
                  throw FMError("\nVector constructor => Memory Error\n",Name);
                }
  };
  Vector(long int Size,const char *name) // Constructor with
  {
         Value=new T[Size];                  // size and name
         Length=Size;                                   // Exception
         if (strlen(name)+1<50)
                {
                  strcpy(Name,name);
                }
         else
                {
                  strcpy(Name,"Name too long");
                }
         if (Value==NULL)
                {
                  throw FMError("\nVector constructor => Memory Error\n",Name);
                }
  };
  // Copy constructor
  Vector(const Vector& v)
  {
         Value=new T[v.Length];
         Length=v.Length;
         for(int i=0;i<Length;i++)
                {
                  Value[i]=v.Value[i];
                }
         strcpy(Name,v.Name);
  };
  ~Vector()             // Destructor
  {
         try
                {
                  if (Value!=NULL)
                         {
                                delete [] Value;
                         }
                }
         catch (...)
                {
                  throw FMError("\nVector destructor => Memory Error\n",Name);
                }
  };

  int GetLength()               // Get the length of the vector
  {
         return Length;
  };
  char *GetName()       // Get the name of the vector
  {
         return Name;
  };
  T & operator[](long int index)   // Operator []
  {                                                               // Exception
         if ((index>=Length)||(index<0))
                {
                  char Message[200];
                  sprintf(Message,"\nIndex %hi not allowed\n%s\n",index,Name);
                  throw FMError(Message);
                }
         return Value[index];
  };
};

// Predefined templates

typedef Vector<double> Vectord; // double
typedef Vector<short int> Vectorsi; // short int
typedef Vector<unsigned char> Vectoruc; // unsigned char
typedef Vector<unsigned short> Vectorus; // unsigned short
typedef Vector<float> Vectorf; // float
typedef Vector<CComplex> VectorComplex; // Complex number
typedef Vector<Pointd> VectorPointd; // Point (double)
typedef Vector<Pointi> VectorPointi; // Point (int)
typedef Vector<Label> VectorLabel; // Label

// ============================================================================
// This class represent the standard analyse parameters
// ============================================================================

class AnalyseParameters
{
public:
  // Default constructor
  AnalyseParameters()
  {
         Frequency=0;
         Width=0;
         Shift=0;
         FirstSample=0;
         LastSample=0;
  };
  // Constructor with parameters
  AnalyseParameters(unsigned short Freq,unsigned short Wi,unsigned short Sh,
                                                  unsigned long First,unsigned long Last)
  {
         Frequency=Freq;
         Width=Wi;
         Shift=Sh;
         FirstSample=First;
         LastSample=Last;
  };
  // Destructor
  ~AnalyseParameters() {};

public:
  unsigned short Frequency; // Sample frequency
  unsigned short Width; // Length of the windows
  unsigned short Shift; // Shift of the windows
  unsigned long FirstSample; // First sample used
  unsigned long LastSample; // Last sample used
};

// ============================================================================
// This class allow the user to create a abject who manage the different
// parameters of the pitch in a sound like pitch values evolution and voicing
// characteristics.
// ============================================================================

class PitchObject : public FMObject
{
protected:
  double *Curve;  // The pitch value
  short int *Voicing; // The pitch value is related to a voiced
  // (1) or unvoiced (0) frame.
  // In this case the pitch value is
  // trivial
  int Nb;  // The number of pitch value
  short int Width; // The window's width of the analyse windows
  short int Shift; // The window's shift of the analyse windows
  long int FirstSample; // First sample of the analyse
  long int LastSample; // Last sample of the analyse

public:
  // Constructor: Number of pitch parameters, width and shift of the frames
  //                            used to perform the analyse
  PitchObject(int NbPitchParameters,short int Wwidth,
                        short int Wshift,long int first=0,long int last=0);
  // Same constructor but with a specific name to the object
  PitchObject(const char *Name,int NbPitchParameters,short int Wwidth,
                        short int Wshift,long int first=0,long int last=0);
  // Destructor
  ~PitchObject();

  // Save the pitch parameters as a text file
  int SaveParameters(const char *FileName); // Text format
  // Load the pitch parameters from a text file
  int LoadParameters(const char *FileName); // Text format
  // Save the pitch parameters as a binary file
  int Save(const char *FileName); // Binary format
  // Load the pitch parameters from a binary file
  int Load(const char *FileName); // Binary format
  //Suppress left and right pitch point
  void Trim();
  // Reset the pitch object
  void ResetParameters(int NbPitchParameters,short int Wwidth,
                                                          short int Wshift,long int first,long int last);
  // Get the pitch value at position
  double GetPitch(int position)          // Exception
  {
         if ((position>=Nb)||(position<0))
                {
                  SendErrorMessage(position,"GetPitch");
                }
         return Curve[position];
  };
  // Set the pitch value at position
  void SetPitch(int position,double value) // Exception
  {
         if ((position>=Nb)||(position<0))
                {
                  SendErrorMessage(position,"SetPitch");
                }
         Curve[position]=value;
  };
  // Get the width of the frame used for analysis
  short int GetWidth()
  {
         return (Width);
  };
  // Get the shift of the frame used for analysis
  short int GetShift()
  {
         return (Shift);
  };
  // Get a pointer to the entire pitch curve
  double *GetCurve()
  {
         return (Curve);
  };
  // Get the number of pitch parameters
  int GetNb()
  {
         return (Nb);
  };
  // Get a pointer to the entire voicing 'curve'
  short int *GetVoicing()
  {
         return (Voicing);
  };
  // Set the index parameters to VOICED
  void SetVoiced(int index)
  {
         if ((index>=Nb)||(index<0))
                {
                  SendErrorMessage(index,"SetVoiced");
                }
         Voicing[index]=VOICED;
  };
  // Set the index parameters to UNVOICED
  void SetUnVoiced(int index)
  {
         if ((index>=Nb)||(index<0))
                {
                  SendErrorMessage(index,"SetUnVoiced");
                }
         Voicing[index]=UNVOICED;
  };
  // Get the voicing characteristic of the index parameters
  short int GetVoicing(int index)
  {
         if ((index>=Nb)||(index<0))
                {
                  SendErrorMessage(index,"GetVoicing");
                }
         return (Voicing[index]);
  };
  // Get the pitch value at sample position pos
  double GetPitch_Step(long int pos);
  // Get the number of the first sample
  long int GetFirst()
  {
         return FirstSample;
  };
  // Get the number of the last sample
  long int GetLast()
  {
         return LastSample;
  };
private:
  void SendErrorMessage(int index,const char *func);
};

// ============================================================================
// Structure used to store a list (PitchList classes)
// ============================================================================

struct PitchMarker       // Element of a list structure
{ short int Value;       // Marker pitch value
  long int Position; // Position of the marker
  PitchMarker *Next; // Pointer to the next marker
  PitchMarker *Prev; // Pointer to the previous marker
};

// ============================================================================
// Class used to store a set of pitch marker (dynamically)
// ============================================================================

class PitchList : public FMObject
{
protected:
  struct PitchMarker *F0S;   // List
  int Number;                 // Number of pitch marker
  struct PitchMarker *First; // First marker
  struct PitchMarker *Last;  // Last marker

public:
  // Default constructor
  PitchList();
  // Constructor with name
  PitchList(const char *Name);
  // Destructor
  ~PitchList();

  // Return the current pitch value
  short int GetPitch()
  {
         return F0S->Value;
  }
  // Return the position of the current pitch marker
  long int GetPosition()
  {
         return F0S->Position;
  };
  // Set the value of the current pitch marker
  void SetPitch(short int a)
  {
         F0S->Value=a;
  };
  // Set the position of the current pitch marker
  void SetPosition(long int pos)
  {
         F0S->Position=pos;
  };
  // Get the number of pitch marker in the list
  int GetNumber()
  {
         return Number;
  };
  // Set the list to her beginning
  void Begin()
  {
         F0S=First;
  };
  // Add a new pitch marker
  void AddNew(short int a,long int pos);
  // Go to the next marker
  void Next();
  // Go to the previous marker
  void Prev();
};



