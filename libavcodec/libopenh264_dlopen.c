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

#include <dlfcn.h>

#include "libopenh264_dlopen.h"

/*
 * The symbol binding makes sure we do not run into strict aliasing issues which
 * can lead into segfaults.
 */
typedef int (*__oh264_WelsCreateSVCEncoder)(ISVCEncoder **);
typedef void (*__oh264_WelsDestroySVCEncoder)(ISVCEncoder *);
typedef int (*__oh264_WelsGetDecoderCapability)(SDecoderCapability *);
typedef long (*__oh264_WelsCreateDecoder)(ISVCDecoder **);
typedef void (*__oh264_WelsDestroyDecoder)(ISVCDecoder *);
typedef OpenH264Version (*__oh264_WelsGetCodecVersion)(void);
typedef void (*__oh264_WelsGetCodecVersionEx)(OpenH264Version *);

#define OH264_SYMBOL_ENTRY(i)                                                  \
  union {                                                                      \
    __oh264_##i f;                                                             \
    void *obj;                                                                 \
  } _oh264_##i

struct oh264_symbols {
  OH264_SYMBOL_ENTRY(WelsCreateSVCEncoder);
  OH264_SYMBOL_ENTRY(WelsDestroySVCEncoder);
  OH264_SYMBOL_ENTRY(WelsGetDecoderCapability);
  OH264_SYMBOL_ENTRY(WelsCreateDecoder);
  OH264_SYMBOL_ENTRY(WelsDestroyDecoder);
  OH264_SYMBOL_ENTRY(WelsGetCodecVersion);
  OH264_SYMBOL_ENTRY(WelsGetCodecVersionEx);
};

/* Symbols are bound by loadLibOpenH264() */
static struct oh264_symbols openh264_symbols;

int oh264_WelsCreateSVCEncoder(ISVCEncoder **ppEncoder) {
  return openh264_symbols._oh264_WelsCreateSVCEncoder.f(ppEncoder);
}

void oh264_WelsDestroySVCEncoder(ISVCEncoder *pEncoder) {
  return openh264_symbols._oh264_WelsDestroySVCEncoder.f(pEncoder);
}

int oh264_WelsGetDecoderCapability(SDecoderCapability *pDecCapability) {
  return openh264_symbols._oh264_WelsGetDecoderCapability.f(pDecCapability);
}

long oh264_WelsCreateDecoder(ISVCDecoder **ppDecoder) {
  return openh264_symbols._oh264_WelsCreateDecoder.f(ppDecoder);
}

void oh264_WelsDestroyDecoder(ISVCDecoder *pDecoder) {
  return openh264_symbols._oh264_WelsDestroyDecoder.f(pDecoder);
}

OpenH264Version oh264_WelsGetCodecVersion(void) {
  return openh264_symbols._oh264_WelsGetCodecVersion.f();
}

void oh264_WelsGetCodecVersionEx(OpenH264Version *pVersion) {
  openh264_symbols._oh264_WelsGetCodecVersionEx.f(pVersion);
}

static void *_oh264_bind_symbol(AVCodecContext *avctx,
                                void *handle,
                                const char *sym_name) {
    void *sym = NULL;

    sym = dlsym(handle, sym_name);
    if (sym == NULL) {
        const char *err = dlerror();
        av_log(avctx,
               AV_LOG_WARNING,
               "%s: Failed to bind %s\n",
               err,
               sym_name);
        return NULL;
    }

    return sym;
}

#define oh264_bind_symbol(avctx, handle, sym_name)                           \
  if (openh264_symbols._oh264_##sym_name.obj == NULL) {                      \
    openh264_symbols._oh264_##sym_name.obj = _oh264_bind_symbol(avctx, handle, #sym_name); \
    if (openh264_symbols._oh264_##sym_name.obj == NULL) {                    \
      return 1;                                                              \
    }                                                                        \
  }

int loadLibOpenH264(AVCodecContext *avctx) {
  static bool initialized = false;
  void *libopenh264 = NULL;
  const char *err = NULL;

  if (initialized) {
      return 0;
  }

#define OPENH264_LIB "libopenh264.so.6"
  libopenh264 = dlopen(OPENH264_LIB, RTLD_LAZY);
  err = dlerror();
  if (err != NULL) {
    av_log(avctx, AV_LOG_WARNING,
           "%s: %s is missing, openh264 support will be disabled\n", err,
           OPENH264_LIB);

    if (libopenh264 != NULL) {
      dlclose(libopenh264);
    }
    return 1;
  }

  oh264_bind_symbol(avctx, libopenh264, WelsCreateSVCEncoder);
  oh264_bind_symbol(avctx, libopenh264, WelsDestroySVCEncoder);
  oh264_bind_symbol(avctx, libopenh264, WelsGetDecoderCapability);
  oh264_bind_symbol(avctx, libopenh264, WelsCreateDecoder);
  oh264_bind_symbol(avctx, libopenh264, WelsDestroyDecoder);
  oh264_bind_symbol(avctx, libopenh264, WelsGetCodecVersion);
  oh264_bind_symbol(avctx, libopenh264, WelsGetCodecVersionEx);

  initialized = true;

  return 0;
}
