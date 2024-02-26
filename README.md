# C/C++ WAV file handler

Tiny WAV handler contained in a single C header file `wav.h` 
and its core depends only on few standard C library functions.

Feel free to import `wav.h` in your projects !

## Usage

The following code is an example on how to use the wav library.

```c
#include "wav.h"
...

const char* filename = "./sound_files/mozart.wav";
int16_t* data = NULL;
WavHeader header;

// Read wav file and get wav information & data raw
wav_read(filename, &header, &data);

// Extract channel 0 information and data from original signal
int16_t* s0 = NULL;
WavHeader headerS0;
wav_extract_channel_data(&s0, &data, &headerS0, &header, 0, -1);

// Save channel signal to wav file
wav_write("./sound_files/mozart_s0.wav", &headerS0, &s0);

// Don't forget to delete data after usage
free(s0);
free(data);
```

## Example

To run the example, clone this repository and run in it
```
make
```

Then run the binary that has been created
```
./bin/main
```

A new file `mozart_s0.wav` will be created in the *sound_files* directory.

## License

This repository has a MIT license, as found in the [LICENSE](LICENSE) file.
