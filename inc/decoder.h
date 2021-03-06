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

#ifndef DECODER_H
#define DECODER_H

#include    "common.h"
#include    "goertzel.h"
#include    "dtmf.h"

typedef enum DECODER_STATE_ENUM
{
      DS_WAIT_FOR_SIGNAL
    , DS_WAIT_FOR_SIGNAL_END
    , DS_WAIT_FOR_PAUSE
    , DS_WAIT_FOR_PAUSE_END
} DECODER_STATE;

EXIT_STATUS bb_decode( const char* );
EXIT_STATUS bbd_detect_key( const GOERTZEL_DATA*, const int, const int, char* );

#endif
