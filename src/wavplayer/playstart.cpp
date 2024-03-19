#include <playstart.h>
#include <SDL/SDL_mixer.h>

using namespace albaos;


void playstart::singasong(){
    //mixer stuff
    //init
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_Chunk *sound = NULL;
    sound = Mix_LoadWAV("start.wav");
    //play sounds
    Mix_PlayChannel(-1, sound, 0);
    Mix_FreeChunk(sound);
    sound = NULL;
    Mix_CloseAudio();

}
