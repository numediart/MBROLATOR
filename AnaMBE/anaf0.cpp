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

int main(int argc, char **argv)
{
  char *f0_file_name,*s16_file_name, output_s16[255],output_pm[255];
  int Freq=16000,FrameLength=512, FrameShift=80, FirstSample=0, LastSample=0 ;

  MBEPitchAnalyserNoVoicing_A *a_PitchAnalyser;
  FrameMBEPitch_A *a_FramePitch;
  SoundRAW *a_SoundFile;
  PitchObject *a_PitchCurve;

  if ( (argc!=8) &&
		 (argc!=6))
	 {
		fprintf(stderr,"USAGE: %s input_s16_file f0_ouput_filename FirstSample=%i LastSample=%i "
				  "[ Freq=%i FrameLength=%i FrameShift=%f ]\n",
				  argv[0], FirstSample, LastSample, Freq, FrameLength, FrameShift);
		exit(1);
	 }

  s16_file_name=argv[1];
  f0_file_name=argv[2];
  FirstSample = atoi(argv[3]);
  LastSample = atoi(argv[4]);

  if (argc==8)
	 {
		Freq=atoi(argv[5]);
		FrameLength=atoi(argv[6]);
		FrameShift = atoi(argv[7]);
	 }
  else
	 {
		FILE *init_file;

		init_file=fopen(argv[5],"rt");
		if ( (fscanf(init_file," Freq= %i ",&Freq)!=1) ||
			  (fscanf(init_file," FrameLength= %i ",&FrameLength)!=1) ||
			  (fscanf(init_file," FrameShift= %i ",&FrameShift)!=1))
		  {
			 fprintf(stderr,"Param: Freq, FrameLength, FrameShift\n");
			 exit(1);
		  }

		fclose(init_file);
	 }

  int nb_tranche= (LastSample-FirstSample+1)/FrameShift;
  a_PitchCurve= new PitchObject(nb_tranche+2,FrameLength,FrameShift,FirstSample,LastSample);

  a_SoundFile= new SoundRAW(s16_file_name, s16_file_name, Freq);
  a_SoundFile->SetFrame(FrameLength, FrameShift);

  a_FramePitch= new FrameMBEPitch_A(FrameLength,Freq);
  a_PitchAnalyser= new MBEPitchAnalyserNoVoicing_A(a_FramePitch);

  a_PitchAnalyser->IO_Def(a_SoundFile,FirstSample,LastSample,a_PitchCurve);
  a_PitchAnalyser->Process();
  a_PitchCurve->SaveParameters(f0_file_name);
  return(0);
}
