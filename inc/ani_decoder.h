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

#ifndef ANI_DECODER_H
#define ANI_DECODER_H

#include    "common.h"
#include    "ani.h"
#include    "goertzel.h"

typedef enum ANI_DECODER_STATE_ENUM
{
      ADS_WAIT_FOR_SIGNAL
    , ADS_WAIT_FOR_PEAK
    , ADS_PEAK_SYNC
    , ADS_WAIT_FOR_REQUEST_END
    , ADS_WAIT_FOR_ANI_END
} ANI_DECODER_STATE;

EXIT_STATUS bb_ani_decode( const char* );
EXIT_STATUS bb_ani_detect_key( const GOERTZEL_DATA*, const int, const int, char* );

#endif
