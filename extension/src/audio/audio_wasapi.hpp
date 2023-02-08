#ifndef AUDIO_WASAPI_H
#define AUDIO_WASAPI_H

#if defined(_WIN32) || defined(_WIN64)
#include <audio/audio_api.hpp>
extern struct AudioAPI g_audio_wasapi;
#define HAVE_WASAPI 1
#else
#define HAVE_WASAPI 0
#endif // defined(_WIN32) || defined(_WIN64)

#endif // AUDIO_WASAPI_H
