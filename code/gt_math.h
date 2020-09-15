#include "math.h"

#define PI 3.14159265359f
#define TAU 2 * PI

union v2 {
    struct {
        float x, y;
    };
    struct {
        float u, v;
    };
    struct {
        float width, height;
    };
    float e[2];
};

union v2i {
    struct {
        int x, y;
    };
    struct {
        int width, height;
    };
    int e[2];
};

union v3 {
    struct {
        float x, y, z;
    };
    struct {
        float u, v, __;
    };
    struct {
        float r, g, b;
    };
    float e[3];
};

union v4 {
    struct {
        union {
            v3 xyz;
            struct {
                float x, y, z;
            };
        };
        float w;
    };
    struct {
        union {
            v3 rgb;
            struct {
                float r, g, b;
            };
        };
        float a;
    };
    float e[4];
};

struct mat4 {
    union {
        real32 e[4*4];
        real32 rc[4][4];
        v4 rows[4];
    };
};

internal inline mat4
identity() {
    mat4 result = {};
    
    result.e[0 + 0 * 4] = 1.0f;
    result.e[1 + 1 * 4] = 1.0f;
    result.e[2 + 2 * 4] = 1.0f;
    result.e[3 + 3 * 4] = 1.0f;
    
    return result;
}

internal inline v2
make_v2(real32 x, real32 y) {
    v2 result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal inline v2i
make_v2i(int x, int y) {
    v2i result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal inline v3
make_v3(real32 x, real32 y, real32 z) {
    v3 result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    
    return result;
}

internal inline v4
make_v4(float x, float y, float z, float w) {
    v4 result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    
    return result;
}

inline v2
add_v2(v2 a, v2 b) {
    v2 result = {};
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    
    return result;
}

inline v2i
add_v2i(v2i a, v2i b) {
    v2i result = {};
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    
    return result;
}

inline v2
sub_v2(v2 a, v2 b) {
    v2 result = {};
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    
    return result;
}

inline v2i
sub_v2i(v2i a, v2i b) {
    v2i result = {};
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    
    return result;
}

inline v2
mul_v2(v2 a, real32 scalar) {
    v2 result = {};
    
    result.x = a.x * scalar;
    result.y = a.y * scalar;
    
    return result;
}

inline b32
equal_v4(v4 a, v4 b) {
    b32 result = false;
    
    result = (a.x == b.x && 
              a.y == b.y && 
              a.z == b.z && 
              a.w == b.w);
    
    return result;
}

internal int
clamp(int min, int val, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

internal real32
clampf(real32 min, real32 val, real32 max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

internal v2
clamp_v2(v2 min, v2 val, v2 max) {
    v2 result = {};
    
    result.x = clampf(min.x, val.x, max.x);
    result.y = clampf(min.y, val.y, max.y);
    
    return result;
}

internal inline real32
safe_divide_1(real32 a, real32 b) {
    if (b == 0.f) b = 1.f;
    return a / b;
}


internal inline real32
max_real32(real32 a, real32 b) {
    if (a > b) return a; 
    return b;
}

internal inline real32
min_real32(real32 a, real32 b) {
    if (a < b) return a;
    return b;
}

inline real32
lerp(real32 a, real32 t, real32 b) {
    return (1.f - t) * a + t * b;
}

internal inline v4
make_color(real32 r, real32 g, real32 b) {
    return make_v4(r, g, b, 1.0f);
}

internal inline v4
make_color(real32 r, real32 g, real32 b, real32 a) {
    return make_v4(r, g, b, a);
}

internal inline v4 
make_color(u32 color) {
    real32 a = (0xff & (color >> 24)) / 255.f;
    real32 r = (0xff & (color >> 16)) / 255.f;
    real32 g = (0xff & (color >>  8)) / 255.f;
    real32 b = (0xff & (color >>  0)) / 255.f;
    if (a == 0.f) {
        a = 255.f;
    }
    return make_color(r, g, b, a);
}

internal inline v4 
lerp_color(u32 a, real32 t, u32 b) {
    real32 a_a = (0xff & (a >> 24)) / 255.f;
    real32 a_r = (0xff & (a >> 16)) / 255.f;
    real32 a_g = (0xff & (a >>  8)) / 255.f;
    real32 a_b = (0xff & (a >>  0)) / 255.f;
    if (a_a == 0.f) {
        a_a = 255.f;
    }
    
    real32 b_a = (0xff & (b >> 24)) / 255.f;
    real32 b_r = (0xff & (b >> 16)) / 255.f;
    real32 b_g = (0xff & (b >>  8)) / 255.f;
    real32 b_b = (0xff & (b >>  0)) / 255.f;
    if (b_a == 0.f) {
        b_a = 255.f;
    }
    
    return make_color(lerp(a_r, t, b_r),
                      lerp(a_g, t, b_g),
                      lerp(a_b, t, b_b),
                      lerp(a_a, t, b_a));
}

internal inline v4 
lerp_color(v4 a, real32 t, v4 b) {
    return make_color(lerp(a.r, t, b.r),
                      lerp(a.g, t, b.g),
                      lerp(a.b, t, b.b),
                      lerp(a.a, t, b.a));
}
//
// Random
//

u32 random_state = 1234;

inline u32
random_u32() {
    u32 result = random_state;
    result ^= result << 13;
    result ^= result >> 17;
    result ^= result << 5;
    random_state = result;
    return result;
}

inline b32
random_choice(int chance) {
    return random_u32 () % chance == 0;
}

inline int
random_int_in_range(int min, int max) { //inclusive
    int range = max - min + 1;
    int result = random_u32() % range;
    result += min;
    return result;
}

inline real32
random_unilateral() {
    return (real32) random_u32() / (real32) MAX_U32;
}

inline real32
random_bilateral() {
    return random_unilateral() * 2.f - 1.f;
}

inline real32
random_real32_in_range(real32 min, real32 max) {
    return random_unilateral() * (max - min) + min;
}

inline real32
angle_to_radians(real32 angle) {
    real32 result = 0.f;
    
    result = angle * PI / 180.f;
    
    return result;
}

//
// Matrices
//
inline v2 rotate_v2_around(v2 a, v2 center, real32 angle) {
    v2 result = {};
    
    real32 rad = angle_to_radians(angle);
    real32 c = cosf(rad);
    real32 s = sinf(rad);
    
    result.x = (a.x - center.x) * c - (a.y - center.y) * s;
    result.y = (a.x - center.x) * s + (a.y - center.y) * c;
    
    result.x += center.x;
    result.y += center.y;
    
    return result;
}
inline v2 rotate_v2(v2 a, real32 angle) {
    v2 result = {};
    
    real32 rad = angle_to_radians(angle);
    real32 cint = cosf(rad);
    real32 sint = sinf(rad);
    
    result.x = cint * a.x - sint * a.y;
    result.y = sint * a.x + cint * a.y;
    
    return result;
}

inline mat4
ortho(real32 left, real32 right, real32 top, real32 bottom, real32 f, real32 n) {
    mat4 result = identity();
    
	result.e[0 + 0 * 4] = 2.f / (right - left);
	result.e[1 + 1 * 4] = 2.f / (top - bottom);
	result.e[2 + 2 * 4] = -2.f / (f - n);
    
	result.e[0 + 3 * 4] = -((right + left) / (right - left));
	result.e[1 + 3 * 4] = -((top + bottom) / (top - bottom));
    result.e[2 + 3 * 4] = -((f + n) / (f - n));
    
	return result;
}

inline mat4
ortho(real32 size, real32 aspect_ratio, real32 f, real32 n) {
    real32 right = size * aspect_ratio;
    real32 left = -right;
    
    real32 top = size;
    real32 bottom = -top;
    
	return ortho(left, right, top, bottom, f, n);
}

inline mat4
perspective(real32 fov, real32 aspect_ratio, real32 f, real32 n) {
    mat4 result = identity();
    
    real32 q = 1.f / tanf(angle_to_radians(fov) * 0.5f);
    real32 a = q / aspect_ratio;
    real32 b = (n + f) / (n - f);
    real32 c = (2.f * n * f) / (n - f);
    
    result.e[0 + 0 * 4] = a;
    result.e[1 + 1 * 4] = q;
    result.e[2 + 2 * 4] = b;
    result.e[3 + 2 * 4] = -1.f;
    result.e[2 + 3 * 4] = c;
    
    return result;
}

inline mat4
translate(v2 pos) {
    mat4 result = identity();
    
    result.e[0 + 3 * 4] = pos.x;
    result.e[1 + 3 * 4] = pos.y;
    
    return result;
}

inline mat4
scale(v2 scale) {
    mat4 result = identity();
    
    result.e[0 * 0 * 4] = scale.x;
    result.e[1 * 1 * 4] = scale.y;
    
    return result;
}