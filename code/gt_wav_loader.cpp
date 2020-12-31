
internal Loaded_Sound
load_wav_file(char *filepath) {
    Loaded_Sound result = {};
    
    File_Contents wav = platform_read_entire_file(filepath);
    assert(wav.file_size > 0);
    
    Wav_Header *header = (Wav_Header *) wav.contents;
    assert(header->id == WAVE_CHUNK_ID_RIFF);
    assert(header->format == WAVE_CHUNK_ID_WAVE);
    assert(header->size > 0);
    
    //
    // NOTE(diego): Real production code need to handle error instead of assertions.
    //
    u8 *at = (u8*) (wav.contents + sizeof(Wav_Header));
    Wav_Fmt *fmt = 0;
    while (*at) {
        Wav_Chunk *chunk = (Wav_Chunk *) at;
        switch (chunk->id) {
            case WAVE_CHUNK_ID_FMT: {
                at += sizeof(Wav_Chunk); // Move to fmt chunk
                fmt = (Wav_Fmt *) at;
                assert(fmt->format == WAVE_FORMAT_PCM);
                assert(fmt->num_channels == 1 || fmt->num_channels == 2);
                assert(fmt->samples_per_second == 44100);
                assert(fmt->bits_per_sample == 16);
                at += chunk->size; // Move by the chunk size
            } break;
            
            case WAVE_CHUNK_ID_DATA: {
                assert(fmt && fmt->block_align == 4);
                result.num_channels = fmt->num_channels;
                result.num_samples = chunk->size / fmt->num_channels; // num_channels * bits_per_sample / 8
                at += sizeof(Wav_Chunk); // Move to data
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