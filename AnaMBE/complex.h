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
// complex.h                                                        complex.cpp
//
// classes:     CComplex
//
// Author: Fabrice Malfrere - TCTS Labs - FRIA Researcher
//
// History:             21.03.1996: Creation
//
// ----------------------------------------------------------------------------
// Documentation:
//
// ============================================================================

#define _INC_COMPLEX

// ============================================================================
// This class is useful for working with complex number.
// ============================================================================


class CComplex
{
public:
        // Public Data Members
    double Real,Imaginary;
    // double Module,Phase;
        // Public Function Members
    CComplex(); // Default Constructor
    CComplex(double a,double b,int type=0); // if type=0 then Real=a
                                                                            // and Imaginary=b
                                                                        // if type=1 then Module=a
                                                                                // and Phase=b
    ~CComplex();
    /*friend CComplex operator+(CComplex c1,CComplex c2);
    friend CComplex operator+(CComplex c1,double c2);
    friend CComplex operator+(double c1,CComplex c2);
    friend CComplex operator-(CComplex c1,CComplex c2);
    friend CComplex operator-(CComplex c1,double c2);
    friend CComplex operator-(double c1,CComplex c2);
    friend CComplex operator*(CComplex c1,CComplex c2);
    friend CComplex operator*(CComplex c1,double c2);
    friend CComplex operator*(double c1,CComplex c2);
    friend CComplex operator/(CComplex c1,CComplex c2);
    friend CComplex operator/(CComplex c1,double c2);
    friend CComplex operator/(double c1,CComplex c2);
    friend CComplex operator^(CComplex c1,double c2);*/
    friend int operator==(CComplex c1,CComplex c2);
    CComplex operator=(CComplex c1);
    CComplex operator=(double c1);
    friend CComplex Conjugate(CComplex c1);
        double GetModule();
        double GetPhase();

public:
        void RIToMP(); // Convert a complex (real,imaginary) number
                           // into a complex (Module,Phase) number
    void MPToRI(); // Convert a complex (Module,Phase) number
                                   // into a complex (Real,Imaginary) number
};
