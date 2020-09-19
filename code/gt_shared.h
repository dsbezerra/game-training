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
