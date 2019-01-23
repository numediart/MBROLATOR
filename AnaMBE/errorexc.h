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
// errorexc.h
//
// classes: FMError
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             08.03.1996: Creation
//                              10.03.1996: Identification number of the error
//                              13.03.1996: New constructors
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_ERROREXC

#ifndef _INC_STRING
#include <string.h>
#endif

#ifndef _INC_CONSTANT
#include "constant.h"
#endif

// ============================================================================

class FMError
{
public:
        // Constructor: Error Message
    FMError(const char *ErrorMessage)
    {
        if (strlen(ErrorMessage)+1<1000)
                {
                        strcpy(Message,ErrorMessage);
                }
                else
                {
                        strcpy(Message,"Message too long");
                }
        IDError=-1; // IDError not used
    };
        // Construct a new FMError with a completed message
        FMError(const char *Mes1,const char *Mes2,FMError Error1)
        {
                if ((strlen(Mes1)+strlen(Mes2)+strlen(Error1.GetErrorMessage())+3)
                        <1000)
                {
                        strcpy(Message,Mes1);
                        strcat(Message,Mes2);
                        strcat(Message,Error1.GetErrorMessage());
                }
                else
                {
                        strcpy(Message,"Message too long");
                }
                IDError=-1;
        };
        // Construct a new FMError with a new message
        FMError(const char *Mes1,const char *Mes2)
        {
                if ((strlen(Mes1)+strlen(Mes2)+2)<1000)
                {
                        strcpy(Message,Mes1);
                        strcat(Message,Mes2);
                }
                else
                {
                        strcpy(Message,"Message too long");
                }
                IDError=-1;
        };
        // Construct a new FMError with a new message
        FMError(const char *Mes1,const char *Mes2,const char *Mes3)
        {
                if ((strlen(Mes1)+strlen(Mes2)+strlen(Mes3)+3)<1000)
                {
                        strcpy(Message,Mes1);
                        strcat(Message,Mes2);
                        strcat(Message,Mes3);
                }
                else
                {
                        strcpy(Message,"Message too long");
                }
                IDError=-1;
        };
    // Constructor: Error ID
    FMError(short int Number)
    {
            IDError=Number;
            strcpy(Message,"NU"); // Message not used
    };
    // Constructor: Error ID and Error Message
        FMError(const char *ErrorMessage,short int Number)
        {
                IDError=Number;
                if (strlen(ErrorMessage)+1<1000)
                {
                        strcpy(Message,ErrorMessage);
                }
                else
                {
                        strcpy(Message,"Message too long");
                }
        };
    // Default constructor: 'NE',0
    FMError()
    {
        strcpy(Message,"N0 ERROR MESSAGE DEFINED");
            IDError=0; // No error ==> IDError=0;
    };
    // Destructor
    ~FMError()
    {
    };

    // Return a pointer to the Message
    virtual const char *GetErrorMessage()
    {
                return Message;
    };
    // Set the Message to the value of ErrorMessage
    virtual void SetErrorMessage(const char *ErrorMessage)
    {
        if (strlen(ErrorMessage)+1<1000)
                {
                        strcpy(Message,ErrorMessage);
                }
    };

protected:
        char Message[1000];
    short int IDError;
};





