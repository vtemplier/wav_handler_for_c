#include "wav.h"

int main()
{
    const char* filename = "./sound_files/mozart.wav";
    int16_t* data = NULL;
    WavHeader header;

    // Read wav file and get wav information & data raw
    wav_read(filename, &header, &data);

    // Extract channel 0 information and data from original signal
    int16_t* s0 = NULL;
    WavHeader headerS0;
    wav_extract_channel_data(&s0, &data, &headerS0, &header, 1, -1);

    // Save channel signal to wav file
    wav_write("./sound_files/mozart_s1.wav", &headerS0, &s0);

    // Don't forget to delete data after usage
    free(s0);
    free(data);

    return 0;
}
