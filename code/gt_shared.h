internal void
copy_string(char *dest, char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest++ = 0;
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

internal int
string_length(char *str) {
	int count = 0;
	while (str[count])
		count++;
	return count;
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

internal char *
consume_next_line(u8 **buffer) {
    if (!buffer) return 0;
    
    u8 *at = *buffer;
    if (*at == '\n' || *at == '\0') {
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

struct Break_String_Result {
    char *lhs;
    char *rhs;
};

internal Break_String_Result
break_by_spaces(char *a) {
    Break_String_Result result = {};
    
    if (!a) {
        result.lhs = 0;
        result.rhs = 0;
    } else {
        char *at = a;
        
        int count = 0;
        while (*at) {
            if (*at == ' ') {
                char *lhs = a; lhs[count++] = '\0';
                result.lhs = lhs;
                result.rhs = a + count;
                break;
            }
            count++;
            at++;
        }
    }
    
    return result;
}