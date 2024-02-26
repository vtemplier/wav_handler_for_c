/**
 ******************************************************************************
 * @file     wav.h
 * @brief    Provide functions to read and write wav files in C
 * 
 ******************************************************************************
 * @attention
 * 
 *  Licensed under the MIT License
 *  Contributor(s): Vincent TEMPLIER
 * 
 ******************************************************************************
 */

#ifndef __WAV_H__
#define __WAV_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Structure to store the wavfile header */
typedef struct WavHeader {
    /* RIFF Chunk */
    char        FileTypeChunkID[4]; // "RIFF" Constant
    uint32_t    FileSize;           // Size of the file minus 8 bytes
    char        FileFormatID[4];    // "WAVE" Constant

    /* FORMAT (fmt) Chunk*/
    char        FormatChunkID[4];   // "fmt " Constant
    uint32_t    FmtChunkSize;       // Size of the bloc minus 16 bytes
    uint16_t    AudioFormat;        // Storage format (1: integer PCM, 3: Floating PCM)
    uint16_t    NbChannels;         // Number of channels (1 to 6)
    uint32_t    SampleRate;         // Sample rate in Hz (standards: 11025, 22050, 44100)
    uint32_t    BytePerSec;         // Number of bytes to read per second 
    uint16_t    BytePerChunk;       // Number of bytes per sample chunk
    uint16_t    BitsPerSample;      // Number of bits used for coding each sample (8, 16)

    /* DATA Chunk */
    char        DataChunkID[4];     // "data" Constant
    uint32_t    DataSize;           // Size of the data chunk

} WavHeader;

/**
 * @details Additional information about the WavHeader 
 * 
 * NbChannels:
 *  1: for mono
 *  2: for stereo
 *  3: for left, right and center
 *  4: for left front, right front, left rear, right rear
 *  5: for left, center, right, surround (ambient)
 *  6: for center left, left, center, center right, right, surround (ambient)
 * 
 * BytePerSec:
 *  BytePerSec = SampleRate * BytePerChunk
 * 
 * BytePerChunk:
 *  BytePerChunk = NbChannels * BitsPerSample / 8
 * 
 * data[]: [Sample 1 bytes from Channel 1] [Sample 1 bytes from Channel 2] 
 *         [Sample 2 bytes from Channel 1] [Sample 2 bytes from Channel 2]
 * 
 */

/**
 * @brief   Read wav information from a wavfile
 * 
 * @param[in]   filename  String of the filename to read
 * @param[out]  header    Pointer to the wavfile header
 * @param[out]  data      Pointer to the data vector
 * @returns               None
 * 
 */ 
void wav_read (const char* filename, 
               WavHeader* header, 
               int16_t** data)
{
    FILE* stream = fopen(filename, "rb");

    if (stream == NULL) {
        fprintf(stderr, "Cannot open file\n");
        exit(1);
    }

    // Read wav header from stream
    if (!fread(header, 1, sizeof(WavHeader), stream)) {
        fprintf(stderr, "Cannot read wav header from stream\n");
        exit(1);
    }

    // Verify if filename is a wavfile
    if (strncmp(header->FileTypeChunkID, "RIFF", 4) ||
        strncmp(header->FileFormatID, "WAVE", 4)) 
    {
        fprintf(stderr, "%s is not a wav file\n", filename);
        exit(1);
    }

    // Verify that the Pulse-code modulation encoding is used 
    // to sample the data
    if (header->AudioFormat != 1) {
        fprintf(stderr, "Only PCM encoding supported\n");
        exit(1);
    }

    // Reset data values if not NULL
    if (*data) 
        free(*data);
 
    *data = (int16_t*)malloc(header->DataSize);

    if (!*data) {
        fprintf(stderr, "Cannot allocate memory for data buffer\n");
        exit(1);
    }

    // Read data values from stream
    if (!fread(*data, 1,header->DataSize, stream)) {
        fprintf(stderr, "Cannot read data from stream\n");
        exit(1);
    }

    if (fclose(stream) == EOF) {
        fprintf(stderr, "Cannot close file\n");
        exit(1);
    }
}


/**
 * @brief   Write wav information into a wavfile
 * 
 * @param[in]  filename  String of the filename to write
 * @param[in]  header    Pointer to the wavfile header
 * @param[in]  data      Pointer to the data vector
 * @returns              None
 * 
 */  
void wav_write (const char* filename, 
                WavHeader* header, 
                int16_t** data)
{
    FILE * stream = fopen(filename, "wb");

    if (stream == NULL) {
        fprintf(stderr, "Cannot open file\n");
        exit(1);
    }

    if (!*data) {
        fprintf(stderr, "Data buffer empty\n");
        exit(1);
    }

    // Write wav header into stream
    if (!fwrite(header, sizeof(WavHeader), 1, stream)) {
        fprintf(stderr, "Cannot write wav header into stream\n");
        exit(1);
    }

    // Write data values into stream
    if (!fwrite(*data, header->DataSize, 1, stream)) {
        fprintf(stderr, "Cannot write data into stream\n");
        exit(1);
    }

    if (fclose(stream) == EOF) {
        fprintf(stderr, "Cannot close file\n");
        exit(1);
    }
}


/**
 * @brief   Extract a channel data vector from a multiple channel vector
 * @details Copy a @p sizeMax elements of a @p channel 
 *          from @p srcData to @p dstData .                             
 * 
 * @param[out]  dstData     Pointer to the destination vector
 * @param[in]   srcData     Pointer to the source vector
 * @param[in]   dstheader   Pointer to the wav header of the dest vector
 * @param[in]   srcheader   Pointer to the wav header of the source vector
 * @param[in]   channel     Channel number where to extract data
 * @param[in]   sizeMax     Max number of elements to extract
 * @returns                 None
 * 
 */ 
void wav_extract_channel_data (int16_t** dstData, 
                               int16_t** srcData, 
                               WavHeader* dstHeader,
                               WavHeader* srcHeader,
                               unsigned int channel,
                               int sizeMax)
{     
    if (!*srcData) {
        fprintf(stderr, "Source data buffer empty\n");
        exit(1);
    }

    if (channel > 6) {
        fprintf(stderr, "Only 6 channels available\n");
        exit(1);
    }

    // Reset data values if not NULL
    if (*dstData) 
        free(*dstData);

    // Copy source wav header info to the dest wav header
    memcpy(dstHeader, srcHeader, sizeof(WavHeader));

    /// Number of elements per channel in the srcData vector
    int nbData = srcHeader->DataSize / dstHeader->BytePerChunk;

    /// Number of elements max to copy to dstData
    sizeMax = (sizeMax < 0 || sizeMax > nbData) ? nbData : sizeMax;

    /// Number of elements max (in bytes) to copy to dstData
    int sizeMaxBytes = sizeMax * (srcHeader->BitsPerSample / 8);

    // Update dest wavheader
    dstHeader->NbChannels = 1;
    dstHeader->DataSize = sizeMaxBytes;
    dstHeader->BytePerChunk = dstHeader->NbChannels * dstHeader->BitsPerSample / 8;
    dstHeader->BytePerSec = dstHeader->SampleRate * dstHeader->BytePerChunk;
    dstHeader->FileSize = dstHeader->DataSize + sizeof(WavHeader) - 8;

    *dstData = (int16_t*)malloc(sizeMaxBytes);

    if (!*dstData) {
        fprintf(stderr, "Cannot allocate memory for data buffer\n");
        exit(1);
    }

    // Copy the elements at the channel position
    for (int i = 0; i < sizeMax; ++i) {
        (*dstData)[i] = (*srcData)[i * srcHeader->NbChannels + channel];
    }
    
}


int wav_get_duration(WavHeader* header)
{
    int duration_sec = 0;
    duration_sec = header->DataSize / header->BytePerSec; 
    // duration_sec = header->DataSize / ((header->NbChannels * header->SampleRate * header->BitsPerSample) / 8);
    return duration_sec;
}


#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif  // __WAV_H__
