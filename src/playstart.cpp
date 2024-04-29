#include <playstart.h>
#include <drivers/audio.h>
//#include <SDL/SDL_mixer.h>


using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;

void printf(char*);
void sleep(common::uint32_t ms);
void PlayNote(char ch, uint8_t octave, common::uint16_t time);
uint16_t FrequencyMatch(char ch, uint8_t octave);


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
    drivers::Speaker PCSPEAKER;
	PCSPEAKER.PlaySound(FrequencyMatch('C',5));
    PCSPEAKER.NoSound();
}
void playstart::song1() {

	//at 135bpm, each beat is 444 ms
	char notes1[] = {
		'C', 'd', 'C', 'C', 'E', 'G', 'A', 'C', 'C', 'B'
	};

	common::uint8_t octave1[] = {
		3, 3, 3, 4, 4, 4, 4, 4, 3, 4
	};

	common::uint16_t time1[] = {
		3552, 1776, 3552, 1776, 1776, 1776, 1776, 1776, 1776, 1776
	};
    uint16_t songnotelength = 10;
    printf("Starting song: Beyond The Breeze\n");
	//TODO print note
    for (common::uint8_t i = 0; i <= songnotelength - 1; i++) {

        PlayNote(notes1[i], octave1[i], time1[i]);
    }
	printf("hope you liked my song!\n");
}


//stuff to play the songs
void playstart::PlayNote(char ch, common::uint8_t octave, common::uint16_t time) {
    drivers::Speaker PCSPEAKER;
	PCSPEAKER.PlaySound(FrequencyMatch(ch, octave));
	sleep(time);
}

uint16_t FrequencyMatch(char ch, uint8_t octave) {

	uint16_t noteFreq = 0;

	switch (octave) {

		case 3:
			switch (ch) {
			case 'C': noteFreq = 131; break;
			case 'c': noteFreq = 139; break;
			case 'D': noteFreq = 147; break;
			case 'd': noteFreq = 156; break;
			case 'E': noteFreq = 165; break;
			case 'F': noteFreq = 175; break;
			case 'f': noteFreq = 185; break;
			case 'G': noteFreq = 196; break;
			case 'g': noteFreq = 208; break;
			case 'A': noteFreq = 220; break;
			case 'a': noteFreq = 233; break;
			case 'B': noteFreq = 247; break;
			default: break;
			}
			break;
		case 4:
			switch (ch) {
			case 'C': noteFreq = 262; break;
			case 'c': noteFreq = 277; break;
			case 'D': noteFreq = 294; break;
			case 'd': noteFreq = 311; break;
			case 'E': noteFreq = 330; break;
			case 'F': noteFreq = 349; break;
			case 'f': noteFreq = 370; break;
			case 'G': noteFreq = 392; break;
			case 'g': noteFreq = 415; break;
			case 'A': noteFreq = 440; break;
			case 'a': noteFreq = 466; break;
			case 'B': noteFreq = 494; break;
			default: break;
			}
			break;
		case 5:
			switch (ch) {
			case 'C': noteFreq = 523; break;
			case 'c': noteFreq = 554; break;
			case 'D': noteFreq = 587; break;
			case 'd': noteFreq = 622; break;
			case 'E': noteFreq = 659; break;
			case 'F': noteFreq = 698; break;
			case 'f': noteFreq = 740; break;
			case 'G': noteFreq = 784; break;
			case 'g': noteFreq = 831; break;
			case 'A': noteFreq = 880; break;
			case 'a': noteFreq = 932; break;
			case 'B': noteFreq = 988; break;
			default: break;
			}
			break;
		case 6:
			switch (ch) {
			case 'C': noteFreq = 1047; break;
			case 'c': noteFreq = 1109; break;
			case 'D': noteFreq = 1175; break;
			case 'd': noteFreq = 1245; break;
			case 'E': noteFreq = 1319; break;
			case 'F': noteFreq = 1397; break;
			case 'f': noteFreq = 1480; break;
			case 'G': noteFreq = 1568; break;
			case 'g': noteFreq = 1661; break;
			case 'A': noteFreq = 1760; break;
			case 'a': noteFreq = 1865; break;
			case 'B': noteFreq = 1976; break;
			default: break;
			}
			break;
		default:
			break;
	}


	return noteFreq;
}
