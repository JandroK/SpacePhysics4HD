#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define MAX_FX 200

struct _Mix_Music;
struct Mix_Chunk;

class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fadeTime = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);
	bool Unload1Fx(int index);
	bool UnloadFxs();

	// Play a previously loaded WAV
	bool PlayFx(int channel, unsigned int fx, int volume = 100);

private:

	_Mix_Music* music;
	Mix_Chunk* fx[MAX_FX] = { nullptr };

	uint volumeMusic = 100;
	uint volumeFx = 100;
};

#endif // __AUDIO_H__