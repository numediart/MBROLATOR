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
// soundfil.cpp
//
// classes: SoundAU (soundfil.h)
//                      SoundRAW (soundfil.h)

//                      SoundWav (soundfil.h)
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             12.09.1995: Creation
//                              21.09.1995: Updating
//                              22.11.1995: Formalisation
//                              29.02.1996: Error File
//                              11.03.1996: Exception handling and new LoadSamples function
//                              12.06.1996: SoundWav

//                              14.06.1996: Name Of the file

//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================


#ifndef _INC_STDIO
#include <stdio.h>
#endif

#ifndef _INC_STRING
#include <string.h>
#endif

#ifndef _INC_STAT
#include <sys/stat.h>
#endif

#include <stdlib.h>

#ifndef _INC_CONSTANT
#include "constant.h"
#endif

#ifndef _INC_SOUNDFIL
#include "soundfil.h"
#endif

// ============================================================================
// Class SoundAu
// ============================================================================

// Constructor: Open the sound file and extract the sample frequency
SoundAu::SoundAu(char *FileName,const char *Name)
{
        short int *Buffer;      // Buffer used to read the header of the file
        struct stat FileState;  // Properties of the file
        int File_Handle;                  // File Handle
        Buffer=new short int[10]; // Initialize
        FMObject::SetName(Name);
        BUFFER=NULL;
        WIDTH=-1;
        SHIFT=0;
        FrameNummer=0;
        Samples=NULL;
        soundFile=fopen(FileName,"rb"); // Open the sound file
        if(soundFile==NULL)             // Exception thrown if troubles with
        {                                                               // file opening
                throw FMError("\nSoundAu constructor =>File Error",
                                          FileName,ObjectName);
        }
        else
        {
                fread(Buffer,sizeof(short int),9,soundFile); // decodes the header
        fread(&Freq,sizeof(short int),1,soundFile); // of the file to extract
        fread(Buffer,sizeof(short int),2,soundFile); // the sample frequency
        File_Handle=fileno(soundFile);
        fstat(File_Handle,&FileState);
        Nb=(FileState.st_size-24)/2;
    }
        delete [] Buffer;

        int i=0;

        while (FileName[i]!='\0')

        {

                i++;

        }

        FILENAME=new char[i+1];

        strcpy(FILENAME,FileName);
};

// ============================================================================

// Destructor: Free the memory and close the file
SoundAu::~SoundAu()
{
        try
        {
                if (Samples!=NULL)
                {
                        delete [] Samples;
                }
                if (soundFile!=NULL)
                {
                        fclose(soundFile);
                }
                if (BUFFER!=NULL)
                {
                        delete [] BUFFER;
                }

                delete FILENAME;
        }
        catch(FMError& Error1)
        {
                throw FMError("\nSoundAu destructor\n",ObjectName,Error1);
        }
        catch (...)
        {
                throw FMError("\nSoundAu destructor\n",ObjectName);
        }
};

// ============================================================================

// Load N samples from the file. Start at position Pos
void SoundAu::LoadSamples(long int N, long int Pos, short int *Sampl)
{
        long int RealPosition;
        int a;
        RealPosition=(Pos*2+24);      // Real position in the file Pos + length of
                                                                // the  header that's 24 for a [au] file
                                                                // *2 to have the position in byte and
                                                                // not in short int
    try
        {
                fseek(soundFile,RealPosition,SEEK_SET); // Move the pointer to
                                                            // the beginning position
                a=fread(Sampl,sizeof(short int),N,soundFile);
                if (a!=N)
                {
                        for(;a<N;a++)
                        {
                                Sampl[a]=0;
                        }
                }
        }
        catch (...)
        {
                throw FMError("\nSoundAu LoadSamples(...)\n",ObjectName);
        }
};

// ============================================================================

void SoundAu::SetFrame(short int Width,short int Shift)
{
        WIDTH=Width;
        SHIFT=Shift;
        try
        {
                if (BUFFER !=NULL)
                {
                        delete [] BUFFER;
                }
                BUFFER=new short int[WIDTH-SHIFT];
                FrameNummer=0;
        }
        catch (...)
        {
                throw FMError("\nSoundAu SetFrame\n",ObjectName);
        }
};

// ============================================================================

void SoundAu::LoadSamples(short int *Sampl)
{
        if (WIDTH==-1)
        {
                throw FMError("\nSoundAu LoadSamples : SetFrame not called\n",
                                          ObjectName);
        }
        try
        {
                short int a;
                if (FrameNummer==0)
                {
                        a=fread(Sampl,sizeof(short int),WIDTH,soundFile);
                }
                else
                {
                        memcpy(Sampl,BUFFER,(WIDTH-SHIFT)*sizeof(short int));
                        a=fread(&Sampl[WIDTH-SHIFT],sizeof(short int),SHIFT,soundFile);
                        if (a!=SHIFT)
                {
                                for(;a<SHIFT;a++)
                        {
                                        Sampl[(WIDTH-SHIFT+a)]=0;
                        }
                }
                }
        memcpy(BUFFER,&Sampl[SHIFT],(WIDTH-SHIFT)*sizeof(short int));
        FrameNummer++;
        }
        catch (...)
        {
                throw FMError("\nSoundAu LoadSamples\n",ObjectName);
        }
};

// ============================================================================
// Class SoundRAW
// ============================================================================

// Constructor: Initialise the object and open the sound file
SoundRAW::SoundRAW(char *FileName,const char *Name,int freq)
{
        Samples=NULL;
        Freq=freq;
        FMObject::SetName(Name);
        soundFile=fopen(FileName,"rb"); // Open the sound file
        if (!soundFile)                 // Exception thrown if troubles with
        {                                                               // file opening
                throw FMError("\nSoundRAW constructor =>File Error",
                                          FileName,ObjectName);
        }
        BUFFER=NULL;
        WIDTH=-1;
        SHIFT=0;
        FrameNummer=0;
        struct stat FileState;
        int File_Handle;
        File_Handle=fileno(soundFile);
        fstat(File_Handle,&FileState);
        Nb=(FileState.st_size)/2;

        int i=0;

        while (FileName[i]!='\0')

        {

                i++;

        }

        FILENAME=new char[i+1];

        strcpy(FILENAME,FileName);
};

// ============================================================================

// Destructor: Free the memory and close the file
SoundRAW::~SoundRAW()
{
        try
        {
                if (Samples!=NULL)
                {
                        delete Samples;
                }
                if (soundFile!=NULL)
                {
                        fclose(soundFile);
                }
                if (BUFFER!=NULL)
                {
                        delete [] BUFFER;
                }

                delete FILENAME;
        }
        catch(FMError& Error1)
        {
                throw FMError("\nSoundRAW destructor\n",ObjectName,Error1);
        }
        catch (...)
        {
                throw FMError("\nSoundRAW destructor\n",ObjectName);
        }
};

// ============================================================================

// Load N samples from the sound file starting at position Pos
void SoundRAW::LoadSamples(long int N, long int Pos,
                           short int *Sampl)
{
        long int RealPosition;
        long int a;
        RealPosition=(Pos*2); // Real position in the file Pos + length
                          // *2 to have the position in byte
                          // and not in short int
    try
        {
                fseek(soundFile,RealPosition,SEEK_SET); // Move the pointer to
                                                // the beginning position
                a=fread(Sampl,sizeof(short),N,soundFile);

#ifndef LITTLE_ENDIAN
                swab((char *)Sampl,(char *)Sampl,a*2);
#endif

                if (a!=N)
                {
                        for(;a<N;a++)
                        {
                                Sampl[a]=0;
                        }
                }
        }
        catch (...)
        {
                throw FMError("\nSoundRAW LoadSamples(...)\n",ObjectName);
        }
};

// ============================================================================

void SoundRAW::SetFrame(short int Width,short int Shift)
{
        WIDTH=Width;
        SHIFT=Shift;
        try
        {
                if (BUFFER !=NULL)
                {
                        delete [] BUFFER;
                }
                BUFFER=new short int[WIDTH-SHIFT];
                FrameNummer=0;
        }
        catch (...)
        {
                throw FMError("\nSoundRAW SetFrame\n",ObjectName);
        }
};

// ============================================================================

void SoundRAW::LoadSamples(short int *Sampl)
{
        if (WIDTH==-1)
        {
                throw FMError("\nSoundRAW LoadSamples : SetFrame not called\n",
                                          ObjectName);
        }
        try
        {
                short int a;
                if (FrameNummer==0)
                {
                        a=fread(Sampl,sizeof(short int),WIDTH,soundFile);
                }
                else
                {
                        memcpy(Sampl,BUFFER,(WIDTH-SHIFT)*sizeof(short int));
                        a=fread(&Sampl[WIDTH-SHIFT],sizeof(short int),SHIFT,soundFile);
                        if (a!=SHIFT)
                {
                                for(;a<SHIFT;a++)
                        {
                                        Sampl[(WIDTH-SHIFT+a)]=0;
                        }
                }
                }
        memcpy(BUFFER,&Sampl[SHIFT],(WIDTH-SHIFT)*sizeof(short int));
        FrameNummer++;
        }
        catch (...)
        {
                throw FMError("\nSoundRAW LoadSamples\n",ObjectName);
        }
};

// ============================================================================

// Class SoundWAV

// ============================================================================



// Constructor: Initialise the object and open the sound file

SoundWAV::SoundWAV(char *FileName,const char *Name)

{

        Samples=NULL;

        FMObject::SetName(Name);

        soundFile=fopen(FileName,"rb"); // Open the sound file

        if (!soundFile)                 // Exception thrown if troubles with

        {                                                               // file opening

                throw FMError("\nSoundWAV constructor =>File Error",

                                          FileName,ObjectName);

        }

        BUFFER=NULL;

        WIDTH=-1;

        SHIFT=0;

        FrameNummer=0;

        struct stat FileState;

        int File_Handle;

        File_Handle=fileno(soundFile);

        fstat(File_Handle,&FileState);

        Nb=(FileState.st_size-126)/2;

        long int temporaire;

        long int fr;

        fread(&temporaire,sizeof(long int),1,soundFile);

        fread(&temporaire,sizeof(long int),1,soundFile);

        fread(&temporaire,sizeof(long int),1,soundFile);

        fread(&temporaire,sizeof(long int),1,soundFile);

        fread(&temporaire,sizeof(long int),1,soundFile);

        fread(&temporaire,sizeof(long int),1,soundFile);

        fread(&fr,sizeof(long int),1,soundFile);

        fread(&temporaire,sizeof(long int),1,soundFile);

        fread(&temporaire,sizeof(long int),1,soundFile);

        fread(&temporaire,sizeof(long int),1,soundFile);

        fread(&temporaire,sizeof(long int),1,soundFile);

        Freq=(short int)fr;

        int i=0;

        while (FileName[i]!='\0')

        {

                i++;

        }

        FILENAME=new char[i+1];

        strcpy(FILENAME,FileName);

};



// ============================================================================



// Destructor: Free the memory and close the file

SoundWAV::~SoundWAV()

{

        try

        {

                if (Samples!=NULL)

                {

                        delete Samples;

                }

                if (soundFile!=NULL)

                {

                        fclose(soundFile);

                }

                if (BUFFER!=NULL)

                {

                        delete [] BUFFER;

                }

                delete FILENAME;

        }

        catch(FMError& Error1)

        {

                throw FMError("\nSoundRAW destructor\n",ObjectName,Error1);

        }

        catch (...)

        {

                throw FMError("\nSoundWAV destructor\n",ObjectName);

        }

};



// ============================================================================



// Load N samples from the sound file starting at position Pos

void SoundWAV::LoadSamples(long int N, long int Pos,

                           short int *Sampl)

{

        long int RealPosition;

        long int a;

        RealPosition=(Pos*2+44); // Real position in the file Pos + length

                          // *2 to have the position in byte

                          // and not in short int

    try

        {

                fseek(soundFile,RealPosition,SEEK_SET); // Move the pointer to

                                                // the beginning position

                a=fread(Sampl,sizeof(short),N,soundFile);

                if (a!=N)

                {

                        for(;a<N;a++)

                        {

                                Sampl[a]=0;

                        }

                }

        }

        catch (...)

        {

                throw FMError("\nSoundWAV LoadSamples(...)\n",ObjectName);

        }

};



// ============================================================================



void SoundWAV::SetFrame(short int Width,short int Shift)

{

        WIDTH=Width;

        SHIFT=Shift;

        try

        {

                if (BUFFER !=NULL)

                {

                        delete [] BUFFER;

                }

                BUFFER=new short int[WIDTH-SHIFT];

                FrameNummer=0;

        }

        catch (...)

        {

                throw FMError("\nSoundWAV SetFrame\n",ObjectName);

        }

};



// ============================================================================



void SoundWAV::LoadSamples(short int *Sampl)

{

        if (WIDTH==-1)

        {

                throw FMError("\nSoundWAV LoadSamples : SetFrame not called\n",

                                          ObjectName);

        }

        try

        {

                short int a;

                if (FrameNummer==0)

                {

                        a=fread(Sampl,sizeof(short int),WIDTH,soundFile);

                }

                else

                {

                        memcpy(Sampl,BUFFER,(WIDTH-SHIFT)*sizeof(short int));

                        a=fread(&Sampl[WIDTH-SHIFT],sizeof(short int),SHIFT,soundFile);

                        if (a!=SHIFT)

                {

                                for(;a<SHIFT;a++)

                        {

                                        Sampl[(WIDTH-SHIFT+a)]=0;

                        }

                }

                }

        memcpy(BUFFER,&Sampl[SHIFT],(WIDTH-SHIFT)*sizeof(short int));

        FrameNummer++;

        }

        catch (...)

        {

                throw FMError("\nSoundWAV LoadSamples\n",ObjectName);

        }

};


