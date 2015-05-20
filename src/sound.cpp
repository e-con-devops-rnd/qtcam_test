/*******************************************************************************#
#           guvcview              http://guvcview.sourceforge.net               #
#                                                                               #
#           Paulo Assis <pj.assis@gmail.com>                                    #
#                                                                               #
# This program is free software; you can redistribute it and/or modify          #
# it under the terms of the GNU General Public License as published by          #
# the Free Software Foundation; either version 2 of the License, or             #
# (at your option) any later version.                                           #
#                                                                               #
# This program is distributed in the hope that it will be useful,               #
# but WITHOUT ANY WARRANTY; without even the implied warranty of                #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 #
# GNU General Public License for more details.                                  #
#                                                                               #
# You should have received a copy of the GNU General Public License             #
# along with this program; if not, write to the Free Software                   #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA     #
#                                                                               #
********************************************************************************/


#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "sound.h"



void
set_sound (struct paRecordData* pdata)
{

	pdata->audio_buff[0] = NULL;
	pdata->recordedSamples = NULL;

    pdata->samprate = 48000;
    pdata->channels = 2;
//	__LOCK_MUTEX( __AMUTEX );
//		pdata->skip_n = global->skip_n; /*initial video frames to skip*/
//	__UNLOCK_MUTEX( __AMUTEX );


    //outbuffer size in bytes (max value is for pcm 2 bytes per sample)
    pdata->outbuf_size = MPG_NUM_SAMP * pdata->channels * 2; //a good value is 240000;

    /*initialize lavc data*/
    if(!(pdata->lavc_data))
    {
        pdata->lavc_data = init_lavc_audio(pdata);
    }
    /*use lavc audio codec frame size to determine samples*/
    pdata->aud_numSamples = (pdata->lavc_data)->codec_context->frame_size * pdata->channels;
    if(pdata->aud_numSamples <= 0)
    {
        pdata->aud_numSamples = MPG_NUM_SAMP * pdata->channels;
    }

	pdata->aud_numBytes = pdata->aud_numSamples * sizeof(SAMPLE);
	pdata->input_type = PA_SAMPLE_TYPE;
	pdata->mp2Buff = NULL;

	pdata->sampleIndex = 0;

    printf("AUDIO: samples(%d)\n", pdata->aud_numSamples);
	pdata->flush = 0;
	pdata->a_ts= -1;
	pdata->ts_ref = 0;

	pdata->stream = NULL;


    pdata->delay += 0; /*add predefined delay - def = 0*/

	/*reset the indexes*/
	pdata->r_ind     = 0;
	pdata->w_ind     = 0;
	pdata->bw_ind    = 0;
	pdata->br_ind    = 0;
	pdata->blast_ind = 0;
	pdata->last_ind  = 0;
	/*buffer for video PCM 16 bits*/
    pdata->pcm_sndBuff=NULL;

}


int
init_sound(struct paRecordData* pdata)
{
    int err = paNoError;
    int i = 0;
    int j = 0;

    /*alloc audio ring buffers*/
    if(!(pdata->audio_buff[0]))
    {
        for(j=0; j< AUDBUFF_NUM; j++)
        {
            pdata->audio_buff[j] = (AudBuff *) malloc (sizeof(AudBuff *) * AUDBUFF_SIZE);
            for(i=0; i<AUDBUFF_SIZE; i++)
            {
                pdata->audio_buff[j][i].frame = (SAMPLE *) malloc(sizeof(SAMPLE *) * pdata->aud_numSamples);
                pdata->audio_buff[j][i].used = false;
                pdata->audio_buff[j][i].time_stamp = 0;
            }
            pdata->audio_buff_flag[j] = AUD_READY;
        }
    }

    /*alloc the callback buffer*/
    pdata->recordedSamples = (SAMPLE *) malloc(sizeof(SAMPLE *) * pdata->aud_numSamples);

    err = port_init_audio(pdata);
    if(err)
        goto error;


    /*sound start time - used to sync with video*/
    pdata->snd_begintime = ns_time_monotonic();

    return (0);

error:
    pdata->streaming=false;
    pdata->flush=0;
    pdata->delay=0;

    if(pdata->recordedSamples) free (pdata->recordedSamples);
    pdata->recordedSamples=NULL;
    if(pdata->audio_buff)
    {

        for(j=0; j< AUDBUFF_NUM; j++)
        {
            for(i=0; i<AUDBUFF_SIZE; i++)
            {
                free(pdata->audio_buff[j][i].frame);
            }
            free(pdata->audio_buff[j]);
            pdata->audio_buff[j] = NULL;
        }
    }
    /*lavc is allways checked and cleaned when finishing worker thread*/
    return(-1);
}





