internal void
advance(char **str, int n) {
    int i = 0;
    while (i < n) {
        (*str)++;
        i++;
    }
}

internal int
string_length(char *str) {
	int count = 0;
	while (str && str[count])
		count++;
	return count;
}

internal void
put_string_at(char *a, char *b, int pos) {
    char *at = a + pos;
    while (*b) {
        *at = *b++; at++;
    }
}
internal void
put_string_at(char *a, char *b, int pos, int len_a, int len_b) {
    if (len_a == 0 || len_b == 0) return;
    
    if (pos + len_b > len_a) return;
    
    put_string_at(a, b, pos);
}

internal void
copy_string(char *dest, char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest++ = 0;
}

internal char *
copy_string(char *src) {
    int len = string_length(src);
    if (len == 0) return 0;
    
    char *result = (char *) platform_alloc(sizeof(char) * len);
    copy_string(result, src);
    return result;
}

internal void
copy_string_n(char *dest, char *src, int count) {
	while (*src && count--) {
		*dest++ = *src++;
	}
	if (count) {
		*dest++ = 0;
    }
}

internal char *
concat(char *a, char *b, int len_a, int len_b) {
    if (len_a == 0) return b;
    if (len_b == 0) return a;
    
    char *result = (char *) platform_alloc((len_a + len_b + 1) * sizeof(char));
    put_string_at(result, a, 0);
    put_string_at(result, b, len_a);
    return result;
}

internal b32
strings_are_equal(char *a, char *b) {
    while (1) {
        if (*a != *b) {
            return false;
        }
        if (!*a) {
            return true;
        }
        a++;
        b++;
    }
    return false;
}

internal int
find_character_from_left(char *a, char c) {
    int result = -1;
    int i = 0;
    while (*a) {
        if (*a == c) {
            result = i;
            break;
        }
        *a++; i++;
    }
    return result;
}

internal int
find_character_from_right(char *a, char c, u32 length) {
    int result = -1;
    int i = length - 1;
    
    char *at = a + i;
    while (*at) {
        if (*at == c) {
            result = i;
            break;
        }
        *at--; i--;
    }
    return result;
}

internal int
find_character_from_right(char *a, char c) {
    return find_character_from_right(a, c, string_length(a));
}

internal b32
string_starts_with(char *a, char *b) {
    b32 result = true;
    
    if (b) {
        char *at = b;
        
        int len = string_length(a);
        for (size_t index = 0; index < len; ++index, ++at) {
            if ((*at == 0) || (a[index] != *at)) {
                result = false;
                break;
            }
        }
    } else {
        result = false;
    }
    
    return result;
}

internal b32
string_ends_with(char *a, char *b) {
    b32 result = true;
    
    if (b) {
        char *at = a;
        int a_len = string_length(a);
        int b_len = string_length(b);
        
        at += a_len - b_len;
        for (int i = 0; i < b_len; ++i) {
            if (*at++ != b[i]) {
                result = false;
                break;
            }
        }
    } else {
        result = false;
    }
    
    return result;
}

internal u32
count_lines(u8 *buffer) {
    u32 result = 0;
    
    u8 *at = buffer;
    
    while (*at) {
        if (*at == '\n') result++;
        at++;
    }
    
    return result;
}

inline bool32
is_eof(char c) {
    bool32 result = ((c == '\n') ||
                     (c == '\r'));
    
    return result;
}

inline bool32
is_whitespace(char c)
{
    bool32 result = ((c == ' ') ||
                     (c == '\t') ||
                     (c == '\v') ||
                     (c == '\f') ||
                     is_eof(c));
    
    return result;
}

internal void
eat_spaces(u8 **a) {
    if (!a) return;
    
    char *at = (char *) *a;
    while (is_whitespace(at[0])) {
        (*a)++; at++;
    }
}

struct Text_File_Handler {
    u8 *data;
    u32 current_line;
    u32 num_lines;
};

internal void
init_handler(Text_File_Handler *handler, u8 *buffer) {
    handler->data = buffer;
    handler->current_line = 0;
    handler->num_lines = count_lines(buffer);
}

internal char *
consume_next_line(u8 **buffer) {
    if (!buffer) return 0;
    
    eat_spaces(buffer);
    
    u8 *at = *buffer;
    if (*at == '\0') {
        return 0;
    }
    
    char *line = (char *) at;
    
    int count = 0;
    while (*at++) {
        if (*at == '\n') {
            line = (char*) *buffer;
            line[++count] = '\0';
            break;
        }
        ++count;
    }
    
    *buffer += count + 1;
    
    return line;
}

internal char *
consume_next_line(Text_File_Handler *handler) {
    if (!handler) return 0;
    
    while (handler->current_line < handler->num_lines) {
        handler->current_line++;
        char *line = consume_next_line(&handler->data);
        if (!line) {
            continue;
        }
        return line; 
    }
    
    return 0;
}

struct Break_String_Result {
    char *lhs;
    char *rhs;
};

internal Break_String_Result
break_by_tok(char *a, char tok) {
    Break_String_Result result = {};
    
    if (!a) {
        result.lhs = 0;
        result.rhs = 0;
    } else {
        int pos = find_character_from_left(a, tok);
        if (pos >= 0) {
            char *lhs = a; lhs[pos++] = '\0';
            result.lhs = lhs;
            result.rhs = a + pos;
        } else {
            result.lhs = a;
            result.rhs = 0;
        }
    }
    
    return result;
}

internal Break_String_Result
break_by_spaces(char *a) {
    Break_String_Result result = {};
    
    result = break_by_tok(a, ' ');
    
    return result;
}

internal Break_String_Result
break_at_index(char *a, int index) {
    Break_String_Result result = {};
    
    if (!a) {
        result.lhs = 0;
        result.rhs = 0;
    } else {
        if (index > 0) {
            char *lhs = a; lhs[index++] = '\0';
            result.lhs = lhs;
            result.rhs = a + index;
        }
    }
    
    return result;
}

struct StringArray {
    char **data;
    int count;
    int capacity;
};

internal StringArray *
make_string_array(u32 capacity) {
    StringArray *result = (StringArray *) platform_alloc(sizeof(StringArray));
    
    result->capacity = capacity;
    result->count = 0;
    result->data = (char **) platform_alloc(sizeof(char *) * result->capacity);
    
    return result;
}

internal void
clear_string_array(StringArray *array) {
    for (int i = 0; i < array->count; ++i) {
        platform_free(array->data[i]);
    }
    array->count = 0;
}

internal void
release_string_array(StringArray *array) {
    for (int i = 0; i < array->count; ++i) {
        platform_free(array->data[i]);
    }
    platform_free(array);
}

internal int
get_first_empty_space(StringArray *array) {
    for (int i = 0; i < array->capacity; ++i) {
        if (!array->data[i]) return i;
    }
    return -1;
}

internal void
array_add(StringArray *array, char *value) {
    int index = get_first_empty_space(array);
    assert(index >= 0 && index < array->capacity);
    array->data[index] = (char *) platform_alloc(sizeof(char) * string_length(value));
    copy_string(array->data[index], value);
    array->count++;
}

internal b32
array_find(StringArray *array, char *value) {
    b32 result = false;
    for (int i = 0; i < array->count; ++i) {
        if (strings_are_equal(array->data[i], value)) {
            result= true;
            break;
        }
    }
    return result;
}

internal void
array_add_if_unique(StringArray *array, char *value) {
    b32 found = array_find(array, value);
    if (!found) {
        array_add(array, value);
    }
}

internal char *
get_item(StringArray *array, int index) {
    if (index >= 0 && index < array->count) {
        return array->data[index];
    }
    return 0;
}

internal char *
get_filename(char *filepath, b32 with_extension = false) {
    char *filename = copy_string(filepath);
    // Convert any back slashes to forward slashes
    char *at = filename;
    while (*at) {
        if (*at == '\\') {
            *at = '/';
        }
        at++;
    }
    int t = find_character_from_right(filename, '/');
    while (t >= 0) {
        advance(&filename, t + 1);
    }
    if (with_extension) {
        return filename;
    }
    Break_String_Result r = break_by_tok(filename, '.');
    return r.lhs;
}

internal s32
s32_from_c_string(char *at) {
    s32 result = 0;
    
    s32 sign = 1;
    if (*at == '-') {
        sign = -1;
        advance(&at, 1);
    } else if (*at == '+') {
        advance(&at, 1);
    }
    while (*at >= '0' && *at <= '9') {
        result *= 10;
        result += (*at - '0');
        ++at;
    }
    
    return result * sign;
}

internal real32
center_hv(real32 width, real32 height, real32 size) {
    real32 dimension = width == 0 ? height : width;
    if (dimension <= 0) return 0;
    real32 result = dimension * .5f;
    if (size > 0) {
        result -= size * .5f;
    }
    return result;
}

internal real32
center_horizontally(real32 width, real32 size) {
    return center_hv(width, 0, size);
}

internal real32
center_vertically(real32 height, real32 size) {
    return center_hv(0, height, size);
}