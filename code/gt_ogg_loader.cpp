#include "stb/stb_vorbis.c"

internal Loaded_Sound
load_ogg_file(char *filepath) {
    Loaded_Sound result = {};
    
    result.num_samples = stb_vorbis_decode_filename(filepath, (int *) &result.num_channels, (int *) &result.samples_per_second, &result.samples);
    assert(result.num_samples);
    assert(result.num_channels == 1 || result.num_channels == 2);
    assert(result.samples_per_second == 44100);
    assert(result.samples);
    
    return result;
}

internal Loaded_Sound
load_ogg_from_memory(char *filepath) {
    Loaded_Sound result = {};
    
    result.num_samples = stb_vorbis_decode_memory((u8 *) filepath, string_length(filepath), (int *) &result.num_channels, (int *) &result.samples_per_second, &result.samples);
    assert(result.num_samples);
    assert(result.num_channels == 1 || result.num_channels == 2);
    assert(result.samples_per_second == 44100);
    assert(result.samples);
    
    return result;
}