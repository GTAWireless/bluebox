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

#include    "ani_decoder.h"

EXIT_STATUS bb_ani_decode( const char* _infname )
{
    if ( !_infname )
    {
        fprintf( stderr, "NULL input file name\n" );

        return ES_BADARG;
    }
    int i = 0;

    printf( "decode '%s' file\n"
            , _infname
          );

    SF_INFO opt = { 0 };
    SNDFILE* ifd = sf_open( _infname
                            , SFM_READ
                            , &opt
                          );

    if ( !ifd )
    {
        fprintf( stderr, "unable to open input file: %s\n"
                 , sf_strerror( ifd )
               );

        return ES_BAD;
    }

    FILE** ofd = (FILE**)calloc( opt.channels, sizeof(FILE*) );
    if ( !ofd )
    {
        fprintf( stderr, "unable to allocate memory for debug goertzel fd array\n" );
    }
    else
    {
        for ( i = 0; i < opt.channels; i++ )
        {
/*FIXME: magic number*/
            char fname[256];
            sprintf( fname, "goertz_ch%d", i );

            ofd[i] = fopen( fname, "w" );
            if ( !(ofd[i]) )
            {
                fprintf( stderr, "unable to open Goertzel debug file for ch %d\n"
                        , i
                        );
            }
        }
    }

#ifdef _DEBUG
    printf( "'%s' -> %d Hz, %d ch\n"
            , _infname
            , opt.samplerate
            , opt.channels
          );
#endif

    EXIT_STATUS es = ES_OK;

    size_t blockcount = ANI_READ_BLOCK_SIZE * opt.channels;
    float* block = (float*)calloc( blockcount, sizeof(float) );

    if ( !block )
    {
        fprintf( stderr, "unable to allocate memory(%zu bytes) for readblock\n"
                , blockcount * opt.channels
               );

        sf_close( ifd );
        return ES_BAD;
    }

    /*allocate memory for each ANI freq on each channel*/
    size_t gdatasz = ANI_FREQ_COUNT * opt.channels;
    GOERTZEL_DATA* gdata = (GOERTZEL_DATA*)calloc( gdatasz, sizeof(GOERTZEL_DATA) );
    if ( !gdata )
    {
        fprintf( stderr, "unable to allocate memory(%zu bytes) for goertzel data\n"
                , gdatasz
               );

        free( block );
        sf_close( ifd );
        return ES_BAD;
    }

    /*allocate memory for wavefrom data on each channel*/
    WAVEFORM_DATA* wfd = (WAVEFORM_DATA*)calloc( opt.channels, sizeof(WAVEFORM_DATA) );
    if ( !wfd )
    {
        fprintf( stderr, "unable to allocate memory(%zu bytes) for waveform data\n"
                , (opt.channels * sizeof(WAVEFORM_DATA))
               );

        free( block );
        free( gdata );
        sf_close( ifd );
        return ES_BAD;
    }

    char* results = (char*)calloc( CHARBUFSZ * opt.channels, sizeof(char) );
    if ( !results )
    {
        fprintf( stderr, "unable to allocate memory(%zu bytes) for results\n"
                , (CHARBUFSZ * opt.channels * sizeof(char))
               );

        return ES_BAD;
    }

    for ( i = 0; i < ANI_FREQ_COUNT; i++ )
    {
        int ch = 0;
        for ( ch = 0; ch < opt.channels; ch++ )
        {
            GOERTZEL_DATA* data = &(gdata[ i*opt.channels + ch ]);
            float tfreq = 0;
            switch ( i )
            {
            case 0:     tfreq = ANI_REQUEST_FREQ; break;
            case 1:     tfreq = ANI_FREQ_1; break;
            case 2:     tfreq = ANI_FREQ_2; break;
            case 3:     tfreq = ANI_FREQ_3; break;
            case 4:     tfreq = ANI_FREQ_4; break;
            case 5:     tfreq = ANI_FREQ_5; break;
            case 6:     tfreq = ANI_FREQ_6; break;
            }

            bbg_initialize_goertzel_data( data, opt.samplerate, tfreq );
        }
    }

    float tm = 0;
    size_t tm_sample = 0;
    size_t samples_for_analyze = (size_t)S2SAMPLES( (1/1000.0) , opt.samplerate );
    sf_count_t readcount = 0;
    int samplen = 0;
    float samplev = 0;
    int ch = 0;
    ANI_DECODER_STATE dstate = ADS_WAIT_FOR_SIGNAL;
    struct PEAK_STRUCT {
        float value;
        float tm;
    };
    struct PEAK_STRUCT peaks[3] = { {0} };
    int peakn = 0;
    const int peaksToMiss = 2;
    const float peaktrs = 0.7;
    size_t peakPeriod = 0;
    while ( (readcount = sf_readf_float( ifd, block, blockcount )) > 0 )
    {
        for ( samplen = 0; samplen < readcount; samplen++ )
        {
            samplev = block[ samplen ];
            for ( ch = 0; ch < opt.channels; ch++ )
            {
                BOOL update_wf = ( tm_sample % samples_for_analyze == 0 );
                for ( i = 0; i < ANI_FREQ_COUNT; i++ )
                {
                    GOERTZEL_DATA* data = &(gdata[ i*opt.channels + ch ]);
                    if ( bbg_goertzel( data, samplev, false ) == ES_OK )
                    {
                        if (    ofd
                             && ofd[ch]
                           )
                        {
                            if ( i == 0 )
                            {
                                fprintf( ofd[ch], "%0.6f ", tm );
                            }

                            fprintf( ofd[ch], "%0.6f%c"
                                    , data->xk
                                    , ' '
                                   );
                        }
                    }
#ifdef _DEBUG
                    else
                    {
                        fprintf( stderr, "unable to run Goertzel for current sample\n" );
                    }
#endif
                }

                if ( update_wf )
                {
#ifdef _DEBUG
                    printf( "%0.6f [ch %d] (%0.6f)\t"
                           , tm
                           , ch
                           , samplev
                         );
#endif

                    wfd[ch].wf3 = wfd[ch].wf2;
                    wfd[ch].wf2 = wfd[ch].wf1;
                    wfd[ch].wf1 = wfd[ch].wf0;
                    wfd[ch].wf0 = samplev;

#ifdef _DEBUG
                    printf( "%0.6f %0.6f %0.6f %0.6f\n"
                           , wfd[ch].wf3
                           , wfd[ch].wf2
                           , wfd[ch].wf1
                           , wfd[ch].wf0
                         );
#endif
                }

/*FIXME: `if` crutch*/
                if ( true )
                {
/*#ifdef _DEBUG*/
/*                    printf( "dstate: %d\n"*/
/*                            , dstate*/
/*                          );*/
/*#endif*/
                    switch ( dstate )
                    {
                    case ADS_WAIT_FOR_SIGNAL:
                        {
                            if ( bbg_is_signal( &(wfd[ch]) ) == ES_OK )
                            {
#ifdef _DEBUG
                                printf( "signal start @ %0.6f\n"
                                        , tm
                                      );
#endif
                               /*for ( i = 0; i < ANI_FREQ_COUNT; i++ )*/
                               /*{*/
                               /*    GOERTZEL_DATA* data = &(gdata[ i*opt.channels + ch ]);*/
                               /*    bbg_save_start_stop( data, true );*/
                               /*}*/

                               dstate = ADS_WAIT_FOR_PEAK;
                               wfd[ch].tm = tm;
                            }
                            break;
                        }
                    case ADS_WAIT_FOR_PEAK:
                        {
                            /*avoid neighbour dots in the same peak*/
                            if ( update_wf )
                            {
                                if ( samplev > peaktrs )
                                {
                                    if ( samplev > peaks[ peakn ].value )
                                    {
#ifdef _DEBUG
                                        printf( "peak #%d @ %0.6f: %0.6f\n"
                                                , peakn
                                                , tm
                                                , samplev
                                              );
#endif

                                        peaks[ peakn ].value = samplev;
                                        peaks[ peakn ].tm = tm;

                                        peakn++;

                                        if ( peakn == 3 )
                                        {
                                            dstate = ADS_WAIT_FOR_SYNC;
                                        }
                                    }
                                }
                            }
                            break;
                        }
                    case ADS_WAIT_FOR_SYNC:
                        {
                            float T = AVG(   ABS(peaks[1].tm - peaks[0].tm)
                                           , ABS(peaks[2].tm - peaks[1].tm)
                                         );
                            peakPeriod = (size_t)S2SAMPLES( T, opt.samplerate );
#ifdef _DEBUG
                            printf( "peak period %0.6f\n"
                                    , T
                                  );
#endif

                            peakn = peaksToMiss;
                            dstate = ADS_WAIT_FOR_REQUEST_END;

                            break;
                        }
                    case ADS_WAIT_FOR_REQUEST_END:
                        {
                            if ( tm_sample % peakPeriod == 0 )
                            {
                                if ( samplev < peaktrs )
                                {
#ifdef _DEBUG
                                    printf( "missing peak @ %0.6f\n"
                                            , tm
                                          );
#endif
                                    peakn--;

                                    if ( !peakn )
                                    {
#ifdef _DEBUG
                                        printf( "request ends @ %0.6f\n"
                                                , tm
                                              );
#endif
                                        dstate = ADS_WAIT_FOR_ANI_END;
                                    }
                                }
                                else
                                {
                                    peakn = peaksToMiss;
                                }
                            }
                            break;
                        }
                    case ADS_WAIT_FOR_ANI_END:
                        {
                            break;
                        }
                    default:
                        {
                            fprintf( stderr, "unknown decoder state: %d\n"
                                    , dstate
                                   );
                        }
                    }

                }
                /*write waveform data*/
                if (    ofd
                     && ofd[ch]
                   )
                {
                    fprintf( ofd[ch], "%0.6f\n"
                            , samplev
                           );
                }
            }

            tm += (1.0 / opt.samplerate);
            tm_sample++;
        }

        memset( block, 0, blockcount * sizeof( float ) );
    }

    /* --------------------------------------------------------------------- */

    free( block );
    free( gdata );
    free( wfd );
    free( results );
    if ( ofd )
    {
        for ( i = 0; i < opt.channels; i++ )
        {
            fclose( ofd[i] );
        }
        free( ofd );
    }
    sf_close( ifd );

    return es;
}
