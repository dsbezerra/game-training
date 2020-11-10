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

inline real32
length_v2(v2 a) {
    real32 result = sqrtf(a.x * a.x + a.y * a.y);
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

inline v2
normalize(v2 a) {
    return mul_v2(a, 1.f / length_v2(a));
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

inline real32
length_v3(v3 a) {
    real32 result = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
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
    return make_color(r, g, b, a);
}

internal inline v4 
lerp_color(u32 a, real32 t, u32 b) {
    real32 a_a = (0xff & (a >> 24)) / 255.f;
    real32 a_r = (0xff & (a >> 16)) / 255.f;
    real32 a_g = (0xff & (a >>  8)) / 255.f;
    real32 a_b = (0xff & (a >>  0)) / 255.f;
    
    real32 b_a = (0xff & (b >> 24)) / 255.f;
    real32 b_r = (0xff & (b >> 16)) / 255.f;
    real32 b_g = (0xff & (b >>  8)) / 255.f;
    real32 b_b = (0xff & (b >>  0)) / 255.f;
    
    
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

internal inline v2
lerp_v2(v2 a, real32 t, v2 b) {
    v2 result = {};
    
    result.x = lerp(a.x, t, b.x);
    result.y = lerp(a.y, t, b.y);
    
    return result;
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
inline
v2 rotate_around(v2 a, v2 center, real32 angle) {
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

inline
v2 rotate(v2 a, real32 angle) {
    v2 result = {};
    
    real32 rad = angle_to_radians(angle);
    real32 cint = cosf(rad);
    real32 sint = sinf(rad);
    
    result.x = cint * a.x - sint * a.y;
    result.y = sint * a.x + cint * a.y;
    
    return result;
}

internal mat4
operator*(mat4 a, mat4 b) {
    // NOTE(casey): This is written to be instructive, not optimal!
    mat4 result = {};
    
    // NOTE(casey): Rows (of A) 
    for(int r = 0; r <= 3; ++r) {
        // NOTE(casey): Column (of B)
        for(int c = 0; c <= 3; ++c) {
            // NOTE(casey): Columns of A, rows of B!
            for(int i = 0; i <= 3; ++i) {
                result.rc[r][c] += a.rc[r][i] * b.rc[i][c];
            }
        }
    }
    
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
translate(v3 pos) {
    mat4 result = identity();
    
    result.e[0 + 3 * 4] = pos.x;
    result.e[1 + 3 * 4] = pos.y;
    result.e[2 + 3 * 4] = pos.z;
    
    return result;
}

inline mat4
scale(v2 scale) {
    mat4 result = identity();
    
    result.e[0 + 0 * 4] = scale.x;
    result.e[1 + 1 * 4] = scale.y;
    
    return result;
}

// NOTE(diego): Not tested.
inline mat4
x_rotation(real32 angle)
{
    real32 c = cosf(angle);
    real32 s = sinf(angle);
    
    mat4 result = identity();
    
    result.rc[1][1] = c;
    result.rc[1][2] = -s;
    result.rc[2][1] = s;
    result.rc[2][2] = c;
    
    return result;
}

// NOTE(diego): Not tested.
inline mat4
y_rotation(real32 angle) {
    real32 c = cosf(angle);
    real32 s = sinf(angle);
    
    mat4 result = identity();
    
    result.rc[0][0] = c;
    result.rc[0][2] = s;
    result.rc[2][0] = -s;
    result.rc[2][2] = c;
    
    return result;
}

// NOTE(diego): Not tested.
inline mat4
z_rotation(real32 angle) {
    real32 c = cosf(angle);
    real32 s = sinf(angle);
    
    mat4 result = identity();
    
    result.rc[0][0] = c;
    result.rc[0][1] = -s;
    result.rc[1][0] = s;
    result.rc[1][1] = c;
    
    return result;
}

// NOTE(diego): Not tested.
// Angle should be in radians.
inline mat4
rotation(real32 angle, v3 axis) {
    mat4 result = identity();
    
    real32 x = axis.x;
    real32 y = axis.y;
    real32 z = axis.z;
    
    real32 length = length_v3(axis);
    if ((length != 1.f) && (length != 0.f)) {
        length = 1.f / length;
        x *= length;
        y *= length;
        z *= length;
    }
    
    real32 s = sinf(angle);
    real32 c = cosf(angle);
    real32 t = 1.f - c; // 1-cos0
    
    result.e[0]  = x*x*t + c;   // cosθ+Rx2(1−cosθ)
    result.e[1]  = y*x*t + z*s; // RyRx(1−cosθ)+Rzsinθ
    result.e[2]  = z*x*t - y*s; // RzRx(1−cosθ)−Rysinθ
    
    result.e[4]  = x*y*t - z*s; // RxRy(1−cosθ)−Rzsinθ
    result.e[5]  = y*y*t + c;   // cosθ+Ry2(1−cosθ)
    result.e[6]  = z*y*t + x*s; // RzRy(1−cosθ)+Rxsinθ
    
    result.e[8]  = x*z*t + y*s; // RxRz(1−cosθ)+Rysinθ
    result.e[9]  = y*z*t - x*s; // RyRz(1−cosθ)−Rxsinθ
    result.e[10] = z*z*t + c;   // cosθ+Rz2(1−cosθ)
    
    return result;
}
