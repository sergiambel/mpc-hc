/*
 * (C) 2003-2006 Gabest
 * (C) 2006-2013 see Authors.txt
 *
 * This file is part of MPC-HC.
 *
 * MPC-HC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-HC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// For configuring which internal filters are included into the build.

#pragma once

// By default all internal filters are enabled except for lite builds
// where they are all disabled.
#if defined(MPCHC_LITE)
#define INTERNAL_FILTERS_ENABLED 0
#else
#define INTERNAL_FILTERS_ENABLED 1
#endif

// Internal source filters
#define INTERNAL_SOURCEFILTER_AVI       INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_CDDA      INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_CDXA      INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_DSM       INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_DTSAC3    INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_VTS       INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_DVSOURCE  INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_FLIC      INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_FLAC      INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_FLV       INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_MATROSKA  INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_MP4       INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_OGG       INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_MPEGAUDIO INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_MPEG      INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_REALMEDIA INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_SHOUTCAST INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_UDP       INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_AVI2AC3   INTERNAL_FILTERS_ENABLED
#define INTERNAL_SOURCEFILTER_RFS       INTERNAL_FILTERS_ENABLED

// Internal audio decoders
#define INTERNAL_DECODER_LPCM           INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_PS2AUDIO       INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_REALAUDIO      INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_PCM            INTERNAL_FILTERS_ENABLED

// Internal audio decoders (FFmpeg)
#define INTERNAL_DECODER_AC3            INTERNAL_FILTERS_ENABLED /* also E-AC3, TrueHD, MLP */
#define INTERNAL_DECODER_DTS            INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_AAC            INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_ALAC           INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_ALS            INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_MPEGAUDIO      INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_VORBIS         INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_NELLYMOSER     INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_AMR            INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_ADPCM          INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_FLAC           INTERNAL_FILTERS_ENABLED

// Internal video decoders
#define INTERNAL_DECODER_MPEG1          INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_MPEG2          INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_REALVIDEO      INTERNAL_FILTERS_ENABLED

// Internal video decoders (FFmpeg)
#define INTERNAL_DECODER_H264           INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_VC1            INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_FLV            INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_VP356          INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_DIVX           INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_XVID           INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_WMV            INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_MSMPEG4        INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_SVQ            INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_H263           INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_THEORA         INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_AMVV           INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_VP8            INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_MJPEG          INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_INDEO          INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_SCREEN         INTERNAL_FILTERS_ENABLED

// DXVA decoders
#define INTERNAL_DECODER_H264_DXVA      INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_VC1_DXVA       INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_MPEG2_DXVA     INTERNAL_FILTERS_ENABLED
#define INTERNAL_DECODER_WMV3_DXVA      INTERNAL_FILTERS_ENABLED


// Groups
#define HAS_SOURCEFILTERS \
    (INTERNAL_SOURCEFILTER_AVI || INTERNAL_SOURCEFILTER_CDDA || INTERNAL_SOURCEFILTER_CDXA || INTERNAL_SOURCEFILTER_DSM || INTERNAL_SOURCEFILTER_DTSAC3 ||       \
    INTERNAL_SOURCEFILTER_VTS || INTERNAL_SOURCEFILTER_DVSOURCE || INTERNAL_SOURCEFILTER_FLIC || INTERNAL_SOURCEFILTER_FLAC || INTERNAL_SOURCEFILTER_FLV ||      \
    INTERNAL_SOURCEFILTER_MATROSKA || INTERNAL_SOURCEFILTER_MP4 || INTERNAL_SOURCEFILTER_OGG || INTERNAL_SOURCEFILTER_MPEGAUDIO || INTERNAL_SOURCEFILTER_MPEG || \
    INTERNAL_SOURCEFILTER_REALMEDIA || INTERNAL_SOURCEFILTER_SHOUTCAST || INTERNAL_SOURCEFILTER_UDP || INTERNAL_SOURCEFILTER_AVI2AC3 || INTERNAL_SOURCEFILTER_RFS)

#define HAS_OTHER_AUDIO_DECODERS \
    (INTERNAL_DECODER_LPCM || INTERNAL_DECODER_PS2AUDIO || INTERNAL_DECODER_REALAUDIO || INTERNAL_DECODER_PCM)

#define HAS_OTHER_VIDEO_DECODERS \
    (INTERNAL_DECODER_MPEG1 || INTERNAL_DECODER_MPEG2 || INTERNAL_DECODER_REALVIDEO)

#define HAS_OTHER_DECODERS \
    (HAS_OTHER_AUDIO_DECODERS || HAS_OTHER_VIDEO_DECODERS)

#define HAS_FFMPEG_AUDIO_DECODERS \
    (INTERNAL_DECODER_AC3 || INTERNAL_DECODER_AAC || INTERNAL_DECODER_ALAC || INTERNAL_DECODER_ALS || INTERNAL_DECODER_DTS || INTERNAL_DECODER_MPEGAUDIO || \
    INTERNAL_DECODER_VORBIS || INTERNAL_DECODER_NELLYMOSER || INTERNAL_DECODER_AMR || INTERNAL_DECODER_ADPCM || INTERNAL_DECODER_REALAUDIO || INTERNAL_DECODER_FLAC)

#define HAS_FFMPEG_VIDEO_DECODERS \
    (INTERNAL_DECODER_H264 || INTERNAL_DECODER_VC1 || INTERNAL_DECODER_FLV || INTERNAL_DECODER_VP356 || INTERNAL_DECODER_DIVX || INTERNAL_DECODER_XVID ||    \
    INTERNAL_DECODER_WMV || INTERNAL_DECODER_MSMPEG4 || INTERNAL_DECODER_SVQ || INTERNAL_DECODER_H263 || INTERNAL_DECODER_THEORA || INTERNAL_DECODER_AMVV || \
    INTERNAL_DECODER_VP8 || INTERNAL_DECODER_MJPEG || INTERNAL_DECODER_INDEO || INTERNAL_DECODER_SCREEN || INTERNAL_DECODER_REALVIDEO)

#define HAS_FFMPEG_DECODERS \
    (HAS_FFMPEG_AUDIO_DECODERS || HAS_FFMPEG_VIDEO_DECODERS)

#define HAS_DXVA_VIDEO_DECODERS \
    (INTERNAL_DECODER_H264_DXVA || INTERNAL_DECODER_VC1_DXVA || INTERNAL_DECODER_MPEG2_DXVA || INTERNAL_DECODER_WMV3_DXVA)

#define HAS_FFMPEG \
    (HAS_FFMPEG_AUDIO_DECODERS || HAS_FFMPEG_VIDEO_DECODERS || HAS_DXVA_VIDEO_DECODERS)
