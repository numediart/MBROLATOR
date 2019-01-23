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

#include <stdio.h>
#include <stdlib.h>

#ifndef _INC_PITCHANA
#include "pitchana.h"
#endif

#ifndef _INC_MBEPARAM
#include "mbeparam.h"
#endif

#ifndef _INC_MBEANALY
#include "mbeanaly.h"
#endif

#ifndef _INC_MATH
#include "math.h"
#endif

int main(int argc, char **argv)
{
  char *mbe_file_name,*f0_file_name,*s16_file_name, output_s16[255],output_pm[255];
  int Freq=16000,FrameLength=512, FrameShift=80,  Nfft=512;
  long int FirstSample=0, LastSample=0;

  MBEPitchAnalyserNoVoicing_A *a_PitchAnalyser;
  FrameMBEPitch_A *a_FramePitch;
  SoundRAW *a_SoundFile;
  PitchObject *a_PitchCurve;

  if ( (argc!=10) &&
                 (argc!=7))
         {
                fprintf(stderr,"USAGE: %s s16_data f0_data mbe_res "
                                  "FirstSample=%li LastSample=%li "
                                  "[FFT=%i Freq=%i FrameLength=%i FrameShift=%i ]\n",
                                  argv[0],FirstSample,LastSample,Nfft,Freq,FrameLength,FrameShift);
                exit(1);
         }

  s16_file_name=argv[1];
  f0_file_name=argv[2];
  mbe_file_name=argv[3];
  FirstSample=atoi(argv[4]);
  LastSample=atoi(argv[5]);

  if (argc==10)
         {
                Nfft=atoi(argv[6]);
                Freq=atoi(argv[7]);
                FrameLength=atoi(argv[8]);
                FrameShift=atoi(argv[9]);
         }
  else
         {
                FILE *init_file;

                init_file=fopen(argv[6],"rt");
                if ((fscanf(init_file," Nfft= %i ",&Nfft)!=1) ||
                         (fscanf(init_file," Freq= %i ",&Freq)!=1) ||
                         (fscanf(init_file," FrameLength= %i ",&FrameLength)!=1) ||
                         (fscanf(init_file," FrameShift= %i ",&FrameShift)!=1))
                  {
                         fprintf(stderr,"Parameters: Nfft, Freq, FrameLength, FrameShift\n");
                         exit(1);
                  }
                fclose(init_file);
         }


  a_SoundFile= new SoundRAW(s16_file_name, s16_file_name, Freq);

  int nb_tranche= ((FrameLength-FrameShift)/2 +LastSample-FirstSample)/ FrameShift;
  int MaxRaie=80;
  int Comp=1;

  a_PitchCurve= new PitchObject(nb_tranche, FrameLength, FrameShift,FirstSample,LastSample);
  a_PitchCurve->LoadParameters(f0_file_name);

  AnalyseParameters par(Freq,FrameLength,FrameShift,FirstSample,LastSample);
  MBE_Parameters mbepar("MBE Parameters",0,nb_tranche,MaxRaie,Comp,par);
  MBE_Analyser_A mbeanalyser("MBE Analyser",FrameLength,FrameShift,Nfft,MaxRaie,MaxRaie);
  mbeanalyser.IO_Def(a_SoundFile, a_PitchCurve, &mbepar);
  mbeanalyser.Process();
  mbepar.SaveParameters(mbe_file_name);

  return(0);
}
