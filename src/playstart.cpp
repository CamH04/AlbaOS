#include <playstart.h>
#include <drivers/audio.h>
//#include <SDL/SDL_mixer.h>


using namespace albaos;


void playstart::singasong(){

    //mixer stuff :(
    /*
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
    */

    // notes
    common::uint32_t C2 = 18356;
    common::uint32_t Cs2 = 17292;
    common::uint32_t Db2 = Cs2;
    common::uint32_t D2 = 16344;
    common::uint32_t Ds2 = 15297;
    common::uint32_t Eb2 = Ds2;
    common::uint32_t E2 = 14551;
    common::uint32_t F2 = 13714;
    common::uint32_t Fs2 = 12829;
    common::uint32_t Gb2 = Fs2;
    common::uint32_t G2 = 12175;
    common::uint32_t Gs2 = 11472;
    common::uint32_t Ab2 = Gs2;
    common::uint32_t A2 = 10847;
    common::uint32_t As2 = 10198;
    common::uint32_t Bb2 = As2;
    common::uint32_t B2 = 9700;
    common::uint32_t C3 = 9121;
    common::uint32_t Cs3 = 8609;
    common::uint32_t Db3 = Cs3;
    common::uint32_t D3 = 8126;
    common::uint32_t Ds3 = 7670;
    common::uint32_t Eb3 = Ds3;
    common::uint32_t E3 = 7239;
    common::uint32_t F3 = 6833;
    common::uint32_t Fs3 = 6449;
    common::uint32_t Gb3 = Fs3;
    common::uint32_t G3 = 6087;
    common::uint32_t Gs3 = 5746;
    common::uint32_t Ab3 = Gs3;
    common::uint32_t A3 = 5423;
    common::uint32_t As3 = 5119;
    common::uint32_t Bb3 = As3;
    common::uint32_t B3 = 4831;
    common::uint32_t C4 = 4560;
    common::uint32_t Cs4 = 4304;
    common::uint32_t Db4 = Cs4;
    common::uint32_t D4 = 4063;
    common::uint32_t Ds4 = 3834;
    common::uint32_t Eb4 = Ds4;
    common::uint32_t E4 = 3619;
    common::uint32_t F4 = 3416;
    common::uint32_t Fs4 = 3224;
    common::uint32_t Gb4 = Fs4;
    common::uint32_t G4 = 3043;
    common::uint32_t Gs4 = 2873;
    common::uint32_t Ab4 = Gs4;
    common::uint32_t A4 = 2711;
    common::uint32_t As4 = 2559;
    common::uint32_t Bb4 = As4;
    common::uint32_t B4 = 2415;
    common::uint32_t C5 = 2280;
    common::uint32_t Cs5 = 2152;
    common::uint32_t Db5 = Cs5;
    common::uint32_t D5 = 2031;
    common::uint32_t Ds5 = 1918;
    common::uint32_t Eb5 = Ds5;
    common::uint32_t E5 = 1810;
    common::uint32_t F5 = 1708;
    common::uint32_t Fs5 = 1612;
    common::uint32_t Gb5 = Fs5;
    common::uint32_t G5 = 1522;
    common::uint32_t Gs5 = 1437;
    common::uint32_t Ab5 = Gs5;
    common::uint32_t A5 = 1356;
    common::uint32_t As5 = 1280;
    common::uint32_t Bb5 = As5;
    common::uint32_t B5 = 1208;
    common::uint32_t C6 = 1140;
    //play startup beeps using notes and the PIT



    drivers::Speaker PCSPEAKER;
    PCSPEAKER.PlaySound(C5);
    PCSPEAKER.PlaySound(E3);
	PCSPEAKER.NoSound();
}
