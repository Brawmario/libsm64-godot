#ifndef LIBSM64GD_SM64NATIVEAUDIO_H
#define LIBSM64GD_SM64NATIVEAUDIO_H

#include <audio/audio_api.hpp>

bool sm64_native_audio_register(AudioAPI *api);

void sm64_native_audio_start();
void sm64_native_audio_stop();

#endif // LIBSM64GD_SM64NATIVEAUDIO_H