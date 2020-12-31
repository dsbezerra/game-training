/* date = December 30th 2020 10:24 pm */

#define RIFF_CODE(a, b, c, d) (((u32) (a) << 0) | ((u32) (b) << 8) | ((u32) (c) << 16) | ((u32) (d) << 24))

enum {
    WAVE_CHUNK_ID_FMT = RIFF_CODE('f', 'm', 't', ' '),
    WAVE_CHUNK_ID_RIFF = RIFF_CODE('R', 'I', 'F', 'F'), 
    WAVE_CHUNK_ID_WAVE = RIFF_CODE('W', 'A', 'V', 'E'),
    WAVE_CHUNK_ID_DATA = RIFF_CODE('d', 'a', 't', 'a'),
};

#pragma pack(push, 1)
struct Wav_Header {
    u32 id;
    u32 size;
    u32 format;
};

struct Wav_Chunk {
    u32 id;
    u32 size;
};

struct Wav_Fmt {
    u16 format;
    u16 num_channels;
    u32 samples_per_second;
    u32 bytes_per_second;
    u16 block_align;
    u16 bits_per_sample;
};
#pragma pack(pop)

internal s16 * load_wav(char *filepath);


