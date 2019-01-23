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
// mbeparam.h                                                                                               mbe.cpp
//
// classes: MBE_Parameters
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             08.03.1996: Creation (MatchProsody not implemented)
//                              10.03.1996: Adding object name
//                              11.03.1996: Creating base object containing the name
//                                                      Frame parameters object
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_MBEPARAM   // Include or not this file

#ifndef _INC_FMOBJECT
#include "fmobject.h"
#endif

#ifndef _INC_DATATYPE
#include "datatype.h"
#endif

// ============================================================================

class MBE_Parameters : public FMObject
{
public:
  AnalyseParameters Param; // Sample frequency, width, shift, ...
  unsigned short NMaxTrchs; // Max number of synthesis frames
  unsigned char NMaxRaies; // Max number of harmonics
  unsigned char NRaiesNV; // Number of noise bands
  // (Freq div 2 div 150)
  unsigned char PhaseComp; // TRUE if phases are to be stored
  unsigned short NTranches; // Number of frames

  Vectoruc *NbreRaies; // Number of harmonics f(F0)
  // The number of noise bands
  // is constant
  Vectorus *NPoints; // Number of analysis synthesis points
  Vectord *F0; // Synthesis pitch
  Vectord *Energy; // Energy mesured on the FFT
  Vectord *LSError; // Harmonics modelisation Error
  // Normalized by complex least square

  Vectord **Amp; // Magnitudes vector of the spectrum lines
  Vectord **Phases; // Phases vector of the spectrum lines
  Vectord **Sigma; // V/NV ratio of the spectrum lines

public:
  // Default constructor
  MBE_Parameters();
  // Constructor: Name of the object
  //              Number of frames
  //                        Max number of frames
  //                        Max number of spectrum lines
  //                        Phase computation or not
  //                            Sample frequency
  MBE_Parameters(const char *Name,short int NumberOfFrames,
                           unsigned char NMaxFr,unsigned char NMaxRa,
                                          unsigned char Comp,AnalyseParameters Fr); // Exception
  // Constructor: Number of frames
  //                        Max number of frames
  //                        Max number of spectrum lines
  //                        Phase computation or not
  //                            Sample frequency
  MBE_Parameters(short int NumberOfFrames,unsigned char NMaxFr,
                                          unsigned char NMaxRa,unsigned char Comp,
                                          AnalyseParameters Fr); // Exception
  ~MBE_Parameters(); // Destructor

  // Load the parameters from the file FileName
  // Return 1 if no troubles
  //       -1 if troubles
  virtual int LoadParameters(const char *FileName); // Exception
  // Save the parameters to the file FileName
  // Return 1 if no troubles
  //             -1 if troubles
  virtual int SaveParameters(const char *FileName); // Exception
  // Copy the parameters of a frame to another
  // Return 1 if no troubles
  //       -1 if troubles
  virtual int CopyFrameParam(short int source,
                                       short int destination); // Exception
  // Adapt the parameters to the desired prosody
  // Return 1 if no troubles
  //       -1 if troubles
  virtual int MatchProsody(Vectord *Pitch,Vectord *Timing); // Exception
};
