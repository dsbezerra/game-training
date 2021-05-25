#include "stb/stb_vorbis.c"

internal Loaded_Sound
load_ogg_file(char *filepath) {
    Loaded_Sound result = {};
    
    File_Contents contents = platform_read_entire_file(filepath);
    assert(contents.file_size > 0);
    
    result.num_samples = stb_vorbis_decode_memory(contents.contents, contents.file_size,  (int *) &result.num_channels, (int *) &result.samples_per_second, &result.samples);
    platform_free(contents.contents);
    
    return result;
}
