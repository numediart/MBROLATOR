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
// soundfil.h                                                                                                      soundfil.cpp
//
// classes: SoundFile
//                      SoundRAW
//                      SoundAU
//                      SoundWAV
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History: 12.09.1995: Creation
//          21.09.1995: Updating
//          22.11.1995: Formalisation
//              17.01.1996: Frame Loading
//          10.03.1996: Object name
//                      11.03.1996: Use of FMObject     and exception handling
//                      12.06.1996: SoundWav
//
// ----------------------------------------------------------------------------
// Documentation:
//
// SoundAU used for SUN format (.au)
// SoundRAW used for RAW format (.raw,.snd)
//
// ============================================================================

#define _INC_SOUNDFIL

#ifndef _INC_STRING
#include <string.h>
#endif

#ifndef _INC_CONSTANT
#include "constant.h"
#endif

#ifndef _INC_FMOBJECT
#include "fmobject.h"
#endif

#ifndef _INC_ERROREXC
#include "errorexc.h"
#endif

// ============================================================================

class SoundFile : public FMObject
{
protected:
        // Internal variables
    long int Nb; // Sample number
    short int Freq; // Sampling frequency
    short int *Samples; // Sample buffer
    FILE *soundFile; // Pointer to the sound file
        char* FILENAME; // Pointer to the File name of the sound
        short int WIDTH,SHIFT; // Width and Shift of the frame
        short int *BUFFER; // Buffer used to store the overlapped
                           // part of the frame
        short int FrameNummer; // Nummer of the current frame

public:
        // Constructor
    SoundFile(){};
    // Destructor
    ~SoundFile(){};

    // Show the sampling frequency of the sound file
    virtual short int Frequency()=0;
    // Show the sample number
    virtual long int Length()=0;
    // Read in the sound file a number N of samples beginninng at
    // position Pos. Return the number of samples read
    virtual void LoadSamples(long int N, long int Pos,
                             short int *Sampl)=0;
        virtual void LoadSamples(short int *Sampl)=0;
        // Activate the Frame Loading
    virtual void SetFrame(short int Width,short int Shift)=0;
                // Give the name of the sound file associated
        virtual char* GetFileName()=0;
};

// ============================================================================

class SoundAu : public SoundFile
{
public:
        // Constructors
    SoundAu(char *FileName,const char *Name); // File's name and object name
                                                                                          // Exception
    // Destructor
    ~SoundAu();

        // Show the sampling frequency
    virtual short int Frequency()
        {
                return (Freq);
        };
    // Show the sample number
    virtual long int Length()
        {
                return (Nb);
        };
    // Read in the sound file a number N of samples beginninng at
    // position Pos. Return the number of samples read
    virtual void LoadSamples(long int N, long int Pos,
                             short int *Sampl);
    // Activate the Frame Loading
        // The frame loading is used to read frame by frame a sound file with
        // a specified shift
    virtual void SetFrame(short int Width,short int Shift);
        // Frame Loading
        virtual void LoadSamples(short int *Sampl);
        // Give the name of the sound file associated
        virtual char* GetFileName()
        {
                return FILENAME;
        };
};

// ============================================================================

class SoundRAW : public SoundFile
{
public:
        // Constructors
    SoundRAW(char *FileName,const char *Name,int Freq); // Sound file name,
                                                                                                                // object name,
                                                                                                                // sample frequency
                                                                                                                // Exception
    // Destructor
    ~SoundRAW();

    // Show the sampling frequency
    virtual short int Frequency()
        {
                return (Freq);
        };
    // Show the sample number
    virtual long int Length()
        {
                return (Nb);
        };
    // Read in the sound file a number N of samples beginninng at
    // position Pos. Return the number of samples read
    virtual void LoadSamples(long int N, long int Pos,
                             short int *Sampl);
    // Activate the Frame Loading
        // The frame loading is used to read frame by frame a sound file with
        // a specified shift
    virtual void SetFrame(short int Width,short int Shift);
        // Frame Loading
        virtual void LoadSamples(short int *Sampl);
        // Give the name of the sound file associated
        virtual char* GetFileName()
        {
                return FILENAME;
        };
};

// ===========================================================================

class SoundWAV : public SoundFile
{
public:
        // Constructors
    SoundWAV(char *FileName,const char *Name); // Sound file name,
                                                                                          // object name,

    // Destructor
    ~SoundWAV();

    // Show the sampling frequency
    virtual short int Frequency()
        {
                return (Freq);
        };
    // Show the sample number
    virtual long int Length()
        {
                return (Nb);
        };
    // Read in the sound file a number N of samples beginninng at
    // position Pos. Return the number of samples read
    virtual void LoadSamples(long int N, long int Pos,
                             short int *Sampl);
    // Activate the Frame Loading
        // The frame loading is used to read frame by frame a sound file with
        // a specified shift
    virtual void SetFrame(short int Width,short int Shift);
        // Frame Loading
        virtual void LoadSamples(short int *Sampl);
                // Give the name of the sound file associated
        virtual char* GetFileName()
        {
                return FILENAME;
        };
};

