/*
 *Yet another DTMF signals encoder/decoder.
 *Copyright (C) 2012  Ilya Arefiev <arefiev.id@gmail.com>
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMON_H
#define COMMON_H

#include    <stdio.h>

#include    <string.h>
#include    <strings.h>
#include    <memory.h>
#include    <stdlib.h>

#include    <math.h>

#include    <sndfile.h>

typedef enum EXIT_STATUS_ENUM
{
      ES_OK
    , ES_BADARG
    , ES_BAD
} EXIT_STATUS;

typedef enum BOOL_ENUM { false, true } BOOL;

#define CHARBUFSZ 256

#define SQR(x) (x*x)

#define MAX(a,b) (a>b? a: b)
#define MAX3(a,b,c) MAX( MAX(a,b), MAX(b,c) )
#define MAX4(a,b,c,d) MAX( MAX(a,b), MAX(c,d) )

#define MIN(a,b) (a<b? a: b)
#define MIN4(a,b,c,d) MIN( MIN(a,b), MIN(c,d) )

#define ABS(x) (x>=0? x: (-1*x))
#define AVG(a,b) ( (a+b)/2 )

#define S2SAMPLES(s,sr) ( floorf( s / (1.0/sr) ) )

#endif
