#include <iostream>
#include <fstream>
#include <cmath>


//riff chunk
char* chunk_id = "RIFF";
char* chunk_size = "----";
char* format = "WAVE";
//FMT sub-chunk
char* subchunk1_id = "fmt "; //DO NOT REMOVE SPACE
int32_t subchunk1_size = 16;
int32_t audio_format = 1;
int32_t num_channels = 2;
int32_t sample_rate = 44100;
int32_t byte_rate = sample_rate * num_channels * (subchunk1_size / 8);
int32_t block_align = num_channels * (subchunk1_size / 8);
int32_t bits_per_sample = 16;
//data subchunk
char* subchunk2_id = "data";
char* subchunk2_size = "----";

int32_t duration = 2;
int32_t max_amplitude = 32760;
int64_t freq = 250;

//making sure all parts are the correct size
void write_as_bytes(std::ofstream &file, int32_t value, int32_t byte_size){
    file.write(reinterpret_cast<char*>(&value), byte_size);
}

int main(){

    std::ofstream wav;
    wav.open("test.wav", std::ios::binary);
    if(wav.is_open()){
        //header
        wav << chunk_id;
        wav << chunk_size;
        wav << format;

        wav << subchunk1_id;
        write_as_bytes(wav, subchunk1_size, 4);
        write_as_bytes(wav, audio_format, 2);
        write_as_bytes(wav, num_channels, 2);
        write_as_bytes(wav, sample_rate, 4);
        write_as_bytes(wav, byte_rate, 4);
        write_as_bytes(wav, block_align, 2);
        write_as_bytes(wav, bits_per_sample, 2);

        wav << subchunk2_id;
        wav << subchunk2_size;

        int32_t start_audio = wav.tellp();

        //writing data
        //making sure we dont go above max amp then make wave
        for(int32_t i = 0; i < sample_rate * duration; i++){
            int64_t amp = (int64_t)i / sample_rate * max_amplitude;
            int64_t value = std::sin((2 * 3.14 * i * freq) / sample_rate);

            int64_t channel1 = amp * value / 2;
            int64_t channel2 = (max_amplitude - amp) * value;

            write_as_bytes(wav, channel1, 2);
            write_as_bytes(wav, channel2, 2);
        }

        int32_t end_audio = wav.tellp();

        wav.seekp(start_audio - 4);
        write_as_bytes(wav, end_audio - start_audio, 4);

        wav.seekp(4, std::ios::beg);
        write_as_bytes(wav, end_audio -8, 4);

    }

    wav.close();

    return 0;
}
