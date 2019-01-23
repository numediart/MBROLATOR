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
// constant.h
//
// No classes
//
// Author : Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             08.03.1996: Creation
//                              10.03.1996: Error messages
//                              12.03.1996: VOICED and UNVOICED constants
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_CONSTANT // Include or not this file

#ifndef TRUE             // TRUE constant
#define TRUE 1
#endif

#ifndef FALSE            // FALSE constant
#define FALSE 0
#endif

#ifndef NULL             // NULL constant
#define NULL 0
#endif

#ifndef VOICED           // VOICED constant
#define VOICED 1
#endif

#ifndef UNVOICED         // UNVOICED constant
#define UNVOICED 0
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

// ============================================================================

// Error Messages

#ifndef ERROR_NAME
#define ERROR_NAME "Memory error object name: "
#endif

#ifndef ERROR_INDEX
#define ERROR_INDEX "Index not allowed: "
#endif

#ifndef ERROR_MEMORY
#define ERROR_MEMORY "Memory error: "
#endif

#ifndef ERROR_FILE
#define ERROR_FILE "File Error: "
#endif

#ifndef ERROR_DIVISION
#define ERROR_DIVISION "Division by zero: "
#endif

