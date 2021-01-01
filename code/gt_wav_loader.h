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

//
// NOTE(diego): This load_wav_file routine loads a WAV file and return
// a pointer to the sampled data. Only 16-bit PCM files are supported.
//
internal Loaded_Sound
load_wav_file(char *filepath) {
    Loaded_Sound result = {};
    
    File_Contents wav = platform_read_entire_file(filepath);
    assert(wav.file_size > 0);
    
    Wav_Header *header = (Wav_Header *) wav.contents;
    assert(header->id == WAVE_CHUNK_ID_RIFF);
    assert(header->format == WAVE_CHUNK_ID_WAVE);
    assert(header->size > 0);
    
    // NOTE(diego): Real production code need to handle error instead of assertions.
    u8 *at = (u8*) (wav.contents + sizeof(Wav_Header));
    Wav_Fmt *fmt = 0;
    while (*at) {
        Wav_Chunk *chunk = (Wav_Chunk *) at;
        switch (chunk->id) {
            case WAVE_CHUNK_ID_FMT: {
                // Move to fmt chunk
                at += sizeof(Wav_Chunk); 
                fmt = (Wav_Fmt *) at;
                assert(fmt->format == WAVE_FORMAT_PCM);
                assert(fmt->num_channels == 1 || fmt->num_channels == 2);
                assert(fmt->samples_per_second == 44100);
                assert(fmt->bits_per_sample == 16);
                // Move by the chunk size
                at += chunk->size; 
            } break;
            
            case WAVE_CHUNK_ID_DATA: {
                assert(fmt && fmt->block_align == 4);
                result.num_channels = fmt->num_channels;
                result.num_samples = chunk->size / fmt->num_channels;
                // Move to data
                at += sizeof(Wav_Chunk);
                result.samples = (s16 *) platform_alloc(result.num_samples * sizeof(s16));
                memcpy(result.samples, at, result.num_samples * sizeof(s16));
                break;
                
            } break;
            
            default: {
                invalid_code_path;
            } break;
        }
    }
    
    platform_free(wav.contents);
    
    return result;
}


