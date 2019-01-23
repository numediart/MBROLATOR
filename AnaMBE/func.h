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
// func.h                                                                                                                          func.cpp
//
// functions: minab(int,int)
//                        maxab(int,int)
//                        round(double)
//                        Convert16to8(SoundFile*,const char*)
//                        Convert16to4(SoundFile*,const char*)
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             03.10.1995: Creation
//                              23.11.1995: Formalisation
//                              11.03.1996: #ifndef et define _INC_FUNC
//                              30.07.1996: Convert16to8 and Convert16to4
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_FUNC

#ifndef _INC_SOUNDFIL
#include "soundfil.h"
#endif

// Give the minimum of a and b
int minab(int a,int b);

// Give the maximum of a and b
int maxab(int a,int b);

// Give the rounded value of a
//int round(double a);

// Convert a sound file sampled a 16kHz to one sampled at 8kHz
void Convert16to8(SoundFile* Son,const char* Name);

// Convert a sound file sampled a 16kHz to one sampled at 4kHz
void Convert16to4(SoundFile* Son,const char* Name);




