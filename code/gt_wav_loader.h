/* date = December 30th 2020 10:24 pm */

#define RIFF_CODE(a, b, c, d) (((u32) (a) << 0) | ((u32) (b) << 8) | ((u32) (c) << 16) | ((u32) (d) << 24))

enum {
    WAVE_CHUNK_ID_FMT = RIFF_CODE('f', 'm', 't', ' '),
    WAVE_CHUNK_ID_RIFF = RIFF_CODE('R', 'I', 'F', 'F'), 
    WAVE_CHUNK_ID_WAVE = RIFF_CODE('W', 'A', 'V', 'E'), 
    WAVE_CHUNK_ID_LIST = RIFF_CODE('L', 'I', 'S', 'T'),
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
    // NOTE(diego): This is missing some fields.
};
#pragma pack(pop)

struct Riff_Iterator {
    u8* at;
    u8* stop;
};

inline Riff_Iterator
parse_chunk_at(void *at, void* stop) {
    Riff_Iterator iter;
    iter.at = (u8*) at;
    iter.stop = (u8*) stop;
    return iter;
}

inline b32
is_riff_iterator_valid(Riff_Iterator iter) {
    return iter.at < iter.stop;
}

inline Riff_Iterator
next_chunk(Riff_Iterator iter) {
    Wav_Chunk *chunk = (Wav_Chunk*) iter.at;
#if 0
    u32 size = (chunk->size+1) & ~1;
    iter.at += sizeof(Wav_Chunk) + size;
#else
    iter.at += sizeof(Wav_Chunk) + chunk->size;
#endif
    return iter;
}

inline void *
get_chunk_data(Riff_Iterator iter) {
    return iter.at + sizeof(Wav_Chunk);
}

inline u32
get_type(Riff_Iterator iter) {
    Wav_Chunk *chunk = (Wav_Chunk*) iter.at;
    return chunk->id;
}

inline u32
get_chunk_data_size(Riff_Iterator iter) {
    Wav_Chunk *chunk = (Wav_Chunk*) iter.at;
    return chunk->size;
}

// NOTE(diego): This load_wav_from_memory routine gets a Loaded_Sound from
// memory structured like a 16-bit PCM WAV files with 44.1KHz samples.
internal Loaded_Sound
load_wav_from_memory(u8 *data) {
    Loaded_Sound result = {};
    
    // NOTE(diego): Real production code need to handle error instead of assertions.
    
    Wav_Header *header = (Wav_Header *) data;
    assert(header->id == WAVE_CHUNK_ID_RIFF);
    assert(header->format == WAVE_CHUNK_ID_WAVE);
    assert(header->size > 0);
    
    u32 sample_data_size = 0;
    s16* sample_data = 0;
    
    for (Riff_Iterator iter = parse_chunk_at(header + 1, (u8*) (header + 1) + header->size - 4); is_riff_iterator_valid(iter); iter = next_chunk(iter)) {
        
        switch (get_type(iter)) {
            case WAVE_CHUNK_ID_FMT: {
                // Move to fmt chunk
                Wav_Fmt *fmt = (Wav_Fmt*) get_chunk_data(iter);
                
                assert(fmt->format == WAVE_FORMAT_PCM);
                assert(fmt->num_channels == 1 || fmt->num_channels == 2);
                assert(fmt->samples_per_second == 44100);
                assert(fmt->bits_per_sample == 16);
                
                result.samples_per_second = fmt->samples_per_second;
                result.num_channels = fmt->num_channels;
            } break;
            
            case WAVE_CHUNK_ID_DATA: {
                sample_data = (s16*) get_chunk_data(iter);
                sample_data_size = get_chunk_data_size(iter);
            } break;
            
            case WAVE_CHUNK_ID_LIST: {
                // NOTE(diego): Ignore
            } break;
            
            invalid_default_case;
        }
    }
    
    assert(result.num_channels && sample_data && sample_data_size);
    
    u32 num_samples = sample_data_size / result.num_channels;
    if (result.num_channels == 1 || result.num_channels == 2) {
        result.samples = (s16 *) platform_alloc(sample_data_size);
        memcpy(result.samples, sample_data, sample_data_size);
    } else {
        assert(!"Invalid num channels in WAV file");
    }
    
    result.num_samples = num_samples;
    
    return result;
}

// NOTE(diego): This load_wav_file routine loads a WAV file and return
// a Loaded_Sound. Only 16-bit PCM 44.1KHz files are supported.
internal Loaded_Sound
load_wav_file(char *filepath) {
    Loaded_Sound result = {};
    
    File_Contents wav = platform_read_entire_file(filepath);
    assert(wav.file_size > 0);
    
    result = load_wav_from_memory(wav.contents);
    platform_free(wav.contents);
    
    return result;
}


