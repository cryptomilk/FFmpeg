/*
 * OpenH264 dlopen code
 *
 * Copyright (C) 2022 Andreas Schneider <asn@cryptomilk.org>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef HAVE_LIBOPENH264_DLOPEN_H
#define HAVE_LIBOPENH264_DLOPEN_H

#ifdef CONFIG_LIBOPENH264_DLOPEN

#include <wels/codec_api.h>
#include <wels/codec_ver.h>

#include "avcodec.h"

int oh264_WelsCreateSVCEncoder(ISVCEncoder **ppEncoder);
#define WelsCreateSVCEncoder oh264_WelsCreateSVCEncoder

void oh264_WelsDestroySVCEncoder(ISVCEncoder *pEncoder);
#define WelsDestroySVCEncoder oh264_WelsDestroySVCEncoder

int oh264_WelsGetDecoderCapability(SDecoderCapability *pDecCapability);
#define WelsGetDecoderCapability oh264_WelsGetDecoderCapability

long oh264_WelsCreateDecoder(ISVCDecoder **ppDecoder);
#define WelsCreateDecoder oh264_WelsCreateDecoder

void oh264_WelsDestroyDecoder(ISVCDecoder *pDecoder);
#define WelsDestroyDecoder oh264_WelsDestroyDecoder

OpenH264Version oh264_WelsGetCodecVersion(void);
#define WelsGetCodecVersion oh264_WelsGetCodecVersion

void oh264_WelsGetCodecVersionEx(OpenH264Version *pVersion);
#define WelsGetCodecVersionEx oh264_WelsGetCodecVersionEx

int loadLibOpenH264(AVCodecContext *avctx);

#endif /* CONFIG_LIBOPENH264_DLOPEN */

#endif /* HAVE_LIBOPENH264_DLOPEN_H */
