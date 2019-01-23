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
// fmobject.h
//
// classes: FMObject
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             11.03.1996: Creation
//
// ----------------------------------------------------------------------------
// Documentation:
//
// The FMObject is a base object that defines the naming capabilities of every
// derived object.
//
// ============================================================================

#define _INC_FMOBJECT

#ifndef _INC_STRING
#include <string.h>
#endif

#ifndef _INC_CONSTANT
#include "constant.h"
#endif

#ifndef _INC_ERROREXC
#include "errorexc.h"
#endif

// ============================================================================

class FMObject
{
public:
        // Default constructor
        FMObject()
        {
                strcpy(ObjectName,"No Name");
        };
        // Contructor that gives a name to the object
        FMObject(const char *Name)      // Exception
        {
                if (strlen(Name)+1<50)
                {
                        strcpy(ObjectName,Name);
                }
                else
                {
                        strcpy(ObjectName,"Name too long");
                }
        };
        // Destructor
        ~FMObject()
        {
        };

        // Give a name to the object
        void SetName(const char *Name)
        {
                if (strlen(Name)+1<50)
                {
                        strcpy(ObjectName,Name);
                }
                else
                {
                        strcpy(ObjectName,"Name too long");
                }

        };
        // Give the name of the object
        char *GetName()
        {
                return ObjectName;
        };

protected:
        char ObjectName[50];
};
