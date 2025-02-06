#ifndef LIBSM64GD_LIBSM64AUDIOSTREAMPLAYER_H
#define LIBSM64GD_LIBSM64AUDIOSTREAMPLAYER_H

#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/ref.hpp>

class LibSM64AudioStreamPlayer : public godot::AudioStreamPlayer {
	GDCLASS(LibSM64AudioStreamPlayer, godot::AudioStreamPlayer);

private:
	void audio_tick();

public:
	LibSM64AudioStreamPlayer() = default;

	virtual void _ready() override;
	virtual void _process(double delta) override;

protected:
	static void _bind_methods() {}

private:
	double time_since_last_tick = 0.0;
	int32_t playback_frames_buffer_length = 0;
};

#endif // LIBSM64GD_LIBSM64AUDIOSTREAMPLAYER_H
