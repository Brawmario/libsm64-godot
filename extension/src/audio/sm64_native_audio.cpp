#include <audio/sm64_native_audio.hpp>

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

extern "C" {
#include <libsm64.h>
}

#include <audio/audio_null.hpp>

#ifdef VERSION_EU
#define SAMPLES_HIGH 656
#define SAMPLES_LOW 640
#else
#define SAMPLES_HIGH 544
#define SAMPLES_LOW 528
#endif

static AudioAPI *s_audio_api = &g_audio_null;
static pthread_t s_audio_thread;
static bool s_keep_alive = false;

static long long time_in_milliseconds(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

static void *sm64_native_audio_thread(void *arg)
{
    // from https://github.com/ckosmic/libsm64/blob/audio/src/libsm64.c#L535-L555

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    long long current_time = time_in_milliseconds();

    while(1)
    {
        if (!s_keep_alive) return NULL;

        int16_t audio_buffer[544 * 2 * 2];
        const int buffered_samples = s_audio_api->buffered();
        const int desired_samples = s_audio_api->get_desired_buffered();

        const uint32_t num_audio_samples = sm64_audio_tick(buffered_samples, desired_samples, audio_buffer);

        s_audio_api->play((uint8_t *) audio_buffer, 2 * num_audio_samples * 4);

        const long long target_time = current_time + 33LL;
        while (time_in_milliseconds() < target_time)
        {
            usleep(100);
            if (!s_keep_alive) return NULL;
        }
        current_time = time_in_milliseconds();
    }
}

bool sm64_native_audio_register(AudioAPI *api)
{
    if (api == nullptr) {
        s_audio_api = &g_audio_null;
        return true;
    }

    bool init = api->init();
    if (init)
        s_audio_api = api;
    return init;
}

void sm64_native_audio_start()
{
    s_keep_alive = true;
    pthread_create(&s_audio_thread, NULL, sm64_native_audio_thread, NULL);
}

void sm64_native_audio_stop()
{
    s_keep_alive = false;
}
