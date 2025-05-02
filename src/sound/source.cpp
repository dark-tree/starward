
#include "source.hpp"
#include "system.hpp"

/*
 * SourceSource
 */

void SoundSource::flush() {
	alSourcef(source, AL_GAIN, gain * SoundSystem::getInstance().getMasterVolume());
	debug::openal::check_error("alSourcef");
}
