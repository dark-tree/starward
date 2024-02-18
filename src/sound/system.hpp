#pragma once

#include <external.hpp>
#include <sound/debug.hpp>
#include <sound/buffer.hpp>
#include <sound/source.hpp>
#include <sound/listener.hpp>

// to learn all the Audio api quirks
// see: https://emscripten.org/docs/porting/Audio.html

class SoundSystem {

	private:

		std::list<std::unique_ptr<SoundSource>> sources;
		SoundVolumes volumes;

	public:

		SoundSystem() {
			ALCdevice* device = alcOpenDevice(nullptr);

			if (device == nullptr) {
				fault("Sound system failed to start, unable to open device!\n");
			}

			ALCcontext* context = alcCreateContext(device, nullptr);

			if (context == nullptr) {
				fault("Sound system failed to start, unable to create context!\n");
			}

			alcMakeContextCurrent(context);
			debug::openal::check_error("alcMakeContextCurrent");

			glm::vec3 origin {0, 0, 0};
			listener().position(origin).velocity(origin).gain(1.0f);

			printf("Sound system engaged!\n");
		}

		void update() {
			std::list<std::unique_ptr<SoundSource>>::iterator iter = sources.begin();

			while (iter != sources.end()) {
				bool drop = (*iter)->should_drop();

				if (drop) {
					printf("Dropped sound '%s', sound count: %d\n", (*iter)->identifier(), (int) sources.size() - 1);
					iter = sources.erase(iter);
				} else {
					(*iter)->update();
					iter ++;
				}
			}
		}

		SoundSource& add(const SoundBuffer& buffer) {
			return add(std::make_unique<SoundSource>(buffer, volumes));
		}

		SoundSource& add(std::unique_ptr<SoundSource>&& source) {
			printf("Added sound '%s', sound count: %d\n", source->identifier(), (int) sources.size() + 1);
			sources.push_back(std::move(source));

			return *sources.back();
		}

		SoundVolumes& volume() {
			return volumes;
		}

		SoundListener listener() {
			return {};
		}

};
