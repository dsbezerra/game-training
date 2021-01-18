#include "math.h"

#define PI 3.14159265359f
#define TAU 2 * PI

inline real32
angle_to_radians(real32 angle) {
    real32 result = 0.f;
    
    result = angle * PI / 180.f;
    
    return result;
}

union Vector2 {
    struct {
        real32 x, y;
    };
    struct {
        real32 u, v;
    };
    struct {
        real32 width, height;
    };
    real32 e[2];
};

union Vector2i {
    struct {
        int x, y;
    };
    struct {
        int width, height;
    };
    int e[2];
};

union Vector3 {
    struct {
        real32 x, y, z;
    };
    struct {
        real32 u, v, __;
    };
    struct {
        real32 r, g, b;
    };
    real32 e[3];
};

union Vector4 {
    struct {
        union {
            Vector3 xyz;
            struct {
                real32 x, y, z;
            };
        };
        real32 w;
    };
    struct {
        union {
            Vector3 rgb;
            struct {
                real32 r, g, b;
            };
        };
        real32 a;
    };
    real32 e[4];
};

union Quaternion {
    struct {
        union {
            Vector3 xyz;
            struct {
                real32 x, y, z;
            };
        };
        real32 w;
    };
    real32 e[4];
};

struct Mat4 {
    union {
        struct {
            real32 _11;
            real32 _21;
            real32 _31;
            real32 _41;
            
            real32 _12;
            real32 _22;
            real32 _32;
            real32 _42;
            
            real32 _13;
            real32 _23;
            real32 _33;
            real32 _43;
            
            real32 _14;
            real32 _24;
            real32 _34;
            real32 _44;
        };
        real32 e[4*4];
        real32 rc[4][4];
        Vector4 rows[4];
    };
};

//
// V2
//

internal inline Vector2
make_vector2(real32 x, real32 y) {
    Vector2 result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal inline Vector2i
make_vector2i(int x, int y) {
    Vector2i result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

inline real32
inner(Vector2 a, Vector2 b) {
    real32 result = a.x * b.x + a.y * b.y;
    return result;
}

inline s32
inner(Vector2i a, Vector2i b) {
    s32 result = a.x * b.x + a.y * b.y;
    return result;
}

inline Vector2
operator* (real32 a, Vector2 b) {
    Vector2 result = {};
    
    result.x = a * b.x;
    result.y = a * b.y;
    
    return result;
}

inline Vector2
operator* (Vector2 b, real32 a) {
    Vector2 result = a * b;
    return result;
}

inline Vector2 &
operator*= (Vector2 &b, real32 a) {
    b = a * b;
    return b;
}

inline Vector2
operator- (Vector2 a) {
    Vector2 result;
    
    result.x = -a.x;
    result.y = -a.y;
    
    return result;
}

inline Vector2
operator+ (Vector2 a, Vector2 b) {
    Vector2 result = {};
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    
    return result;
}

inline Vector2 &
operator+= (Vector2 &a, Vector2 b) {
    a = a + b;
    return a;
}

inline Vector2
operator- (Vector2 a, Vector2 b) {
    Vector2 result = {};
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    
    return result;
}

inline Vector2 &
operator-= (Vector2 &a, Vector2 b) {
    a = a - b;
    return a;
}

inline real32
length_sq(Vector2 a) {
    real32 result = inner(a, a);
    return result;
}

inline real32
length(Vector2 a) {
    real32 result = sqrtf(length_sq(a));
    return result;
}

inline Vector2
normalize(Vector2 a) {
    return a * (1.f / length(a));
}

inline Vector2i
operator* (s32 a, Vector2i b) {
    Vector2i result = {};
    
    result.x = a * b.x;
    result.y = a * b.y;
    
    return result;
}

inline Vector2i
operator* (Vector2i b, s32 a) {
    Vector2i result = a * b;
    return result;
}

inline Vector2i &
operator*= (Vector2i &b, s32 a) {
    b = a * b;
    return b;
}

inline Vector2i
operator- (Vector2i a) {
    Vector2i result;
    
    result.x = -a.x;
    result.y = -a.y;
    
    return result;
}

inline Vector2i
operator+ (Vector2i a, Vector2i b) {
    Vector2i result = {};
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    
    return result;
}

inline Vector2i &
operator+= (Vector2i &a, Vector2i b) {
    a = a + b;
    return a;
}

inline Vector2i
operator- (Vector2i a, Vector2i b) {
    Vector2i result = {};
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    
    return result;
}

inline Vector2i &
operator-= (Vector2i &a, Vector2i b) {
    a = a - b;
    return a;
}

//
// V3
//

internal inline Vector3
make_vector3(real32 x, real32 y, real32 z) {
    Vector3 result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    
    return result;
}

inline real32
inner(Vector3 a, Vector3 b) {
    real32 result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

inline real32
length_sq(Vector3 a) {
    real32 result = inner(a, a);
    return result;
}

inline real32
length(Vector3 a) {
    real32 result = sqrtf(length_sq(a));
    return result;
}

inline Vector3
cross(Vector3 a, Vector3 b) {
    Vector3 result = {};
    
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    
    return result;
}

inline Vector3
operator* (real32 a, Vector3 b) {
    Vector3 result = {};
    
    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;
    
    return result;
}

inline Vector3
operator* (Vector3 b, real32 a) {
    Vector3 result = a * b;
    return result;
}

inline Vector3 &
operator*= (Vector3 &b, real32 a) {
    b = a * b;
    return b;
}

inline Vector3
operator- (Vector3 a) {
    Vector3 result;
    
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    
    return result;
}

inline Vector3
operator+ (Vector3 a, Vector3 b) {
    Vector3 result = {};
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    
    return result;
}

inline Vector3 &
operator+= (Vector3 &a, Vector3 b) {
    a = a + b;
    return a;
}

inline Vector3
operator- (Vector3 a, Vector3 b) {
    Vector3 result = {};
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    
    return result;
}

inline Vector3 &
operator-= (Vector3 &a, Vector3 b) {
    a = a - b;
    return a;
}

inline b32
operator==(Vector3 a, Vector3 b) {
    b32 result = false;
    
    result = (a.x == b.x && 
              a.y == b.y && 
              a.z == b.z);
    
    return result;
}

inline b32
operator!=(Vector3 a, Vector3 b) {
    return !(a == b);
}


inline Vector3
normalize(Vector3 a) {
    Vector3 result = a * (1.f / length(a));
    return result;
}

//
// V4
// 

internal inline Vector4
make_vector4(real32 x, real32 y, real32 z, real32 w) {
    Vector4 result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    
    return result;
}

inline real32
inner(Vector4 a, Vector4 b) {
    real32 result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return result;
}

inline real32
length_sq(Vector4 a) {
    real32 result = inner(a, a);
    return result;
}

inline real32
length(Vector4 a) {
    real32 result = sqrtf(length_sq(a));
    return result;
}

inline Vector4
operator* (Vector4 a, Vector4 b) {
    Vector4 result = {};
    
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    result.w = a.w * b.w;
    
    return result;
}

inline Vector4
operator* (real32 a, Vector4 b) {
    Vector4 result = {};
    
    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;
    result.w = a * b.w;
    
    return result;
}

inline Vector4
operator* (Vector4 b, real32 a) {
    Vector4 result = a * b;
    return result;
}

inline Vector4 &
operator*= (Vector4 &b, real32 a) {
    b = a * b;
    return b;
}

inline Vector4
operator- (Vector4 a) {
    Vector4 result;
    
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;
    
    return result;
}

inline Vector4
operator+ (Vector4 a, Vector4 b) {
    Vector4 result = {};
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    
    return result;
}

inline Vector4 &
operator+= (Vector4 &a, Vector4 b) {
    a = a + b;
    return a;
}

inline Vector4
operator- (Vector4 a, Vector4 b) {
    Vector4 result = {};
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;
    
    return result;
}

inline Vector4 &
operator-= (Vector4 &a, Vector4 b) {
    a = a - b;
    return a;
}

inline b32
operator==(Vector4 a, Vector4 b) {
    b32 result = false;
    
    result = (a.x == b.x && 
              a.y == b.y && 
              a.z == b.z && 
              a.w == b.w);
    
    return result;
}

inline Vector4
normalize(Vector4 a) {
    Vector4 result = a * (1.f / length(a));
    return result;
}

//
// Quaternion
//
inline Quaternion
make_quaternion(real32 x, real32 y, real32 z, real32 w) {
    Quaternion result = {};
    
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    
    return result;
}

internal void
set_from_axis_and_angle(Quaternion *a, Vector3 axis, real32 angle) {
    real32 half_rad = angle_to_radians(angle) / 2.f;
    real32 s = sinf(half_rad);
    
    a->x = axis.x*s;
    a->y = axis.y*s;
    a->z = axis.z*s;
    
    a->w = cosf(half_rad);
}

inline Quaternion
make_quaternion(Vector3 axis, real32 angle) {
    Quaternion result = {};
    set_from_axis_and_angle(&result, axis, angle);
    return result;
}

inline Quaternion
quaternion_identity() {
    Quaternion result = {};
    
    result.w = 1.0f;
    
    return result;
}

inline Quaternion
operator* (Quaternion a, Quaternion b) {
    Quaternion result = {};
    
    result.w   = a.w*b.w - inner(a.xyz, b.xyz);
    result.xyz = a.xyz*b.w + b.xyz*a.w + cross(a.xyz, b.xyz);
    
    return result;
}

inline Quaternion &
operator*= (Quaternion &b, Quaternion a) {
    b = a * b;
    return b;
}

inline Vector3
operator* (Quaternion a, Vector3 b) {
    Quaternion q;
    q.w = 0.f;
    q.xyz = b;
    
    Vector3 c = cross(a.xyz, b);
    return b + c*(2.f*a.w) + cross(a.xyz, c)*2.f;
}

inline Vector3 &
operator*= (Vector3 &b, Quaternion a) {
    b = a * b;
    return b;
}

internal void
to_axis_angle(Quaternion a, Vector3 &axis, real32& angle) {
    // Convert the quaternion back into axis/angle
    if (length_sq(a.xyz) < 0.0001f) {
        axis = make_vector3(1.f, 0.f, 0.f);
    } else {
        axis = normalize(a.xyz);
    }
    
    assert(fabs(length_sq(axis) - 1) < 0.000001f);
    
    // This is the opposite procedure as explained in
    // http://youtu.be/SCbpxiCN0U0 w = cos(a/2) and a = acos(w)*2
    angle = acosf(a.w)*2.f;
    
    // Convert to degrees
    angle *= 360.f / (PI * 2.f);
}

internal Quaternion
interpolate(Quaternion a, real32 t) {
    // Convert the quaternion back into axis/angle
    Quaternion result = {};
    
    real32 angle;
    Vector3 axis;
    
    to_axis_angle(a, axis, angle);
    
    // Scale the angle by t
    real32 at = angle * t;
    
    set_from_axis_and_angle(&result, axis, at);
    
    return result;
}

inline Quaternion
operator^ (Quaternion a, float t) {
    return interpolate(a, t);
}

inline Quaternion &
operator^= (Quaternion &a, float t) {
    a = a ^ t;
    return a;
}

// NOTE(diego): Don't seem to work correctly.
internal Quaternion
slerp(Quaternion a, Quaternion b, real32 t) {
    
    real32 cos_omega = a.w * b.w + inner(b.xyz, a.xyz);
    if (cos_omega < 0.f) {
        // Avoid going the long way around.
        b.w = -b.w;
        b.xyz = -b.xyz;
        cos_omega = -cos_omega;
    }
    
    real32 k0;
    real32 k1;
    if (cos_omega > 0.9999f) {
        // Very close, use a lerp.
        k0 = 1.f - t;
        k1 = t;
    } else {
        // Trig identity, sin^2 + cos^2 = 1
        real32 sin_omega = sqrtf(1.f - cos_omega * cos_omega);
        
        // Compute the angle omega
        real32 omega = atan2f(sin_omega, cos_omega);
        real32 one_over_sin_omega = 1.f / sin_omega;
        
        k0 = sinf((1.f - t) * omega) * one_over_sin_omega;
        k1 = sinf(t * omega) * one_over_sin_omega;
    }
    
    // Interpolate
    Quaternion result;
	result.w   = a.w   * k0 + b.w   * k1;
	result.xyz = a.xyz * k0 + b.xyz * k1;
    
    return result;
}

internal Quaternion
from_euler(real32 y, real32 p, real32 r) {
    Quaternion result;
    
    real32 cy = cosf(y * 0.5f);
    real32 sy = sinf(y * 0.5f);
    
    real32 cp = cosf(p * 0.5f);
    real32 sp = sinf(p * 0.5f);
    
    real32 cr = cosf(r * 0.5f);
    real32 sr = sinf(r * 0.5f);
    
    result.w = cr * cp * cy + sr * sp * sy;
    result.x = sr * cp * cy - cr * sp * sy;
    result.y = cr * sp * cy + sr * cp * sy;
    result.z = cr * cp * sy - sr * sp * cy;
    
    return result;
}

internal Vector3
to_euler(Quaternion a) {
    Vector3 result = {};
    
    // roll (x-axis rotation)
    real32 sr_cp = 2.f * (a.w * a.x + a.y * a.z);
    real32 cr_cp = 1.f - 2.f * (a.x * a.x + a.y * a.y);
    result.x = atan2f(sr_cp, cr_cp);
    
    // pitch (y-axis rotation)
    real32 sp = 2.f * (a.w * a.y - a.z * a.x);
    if (fabs(sp) >= 1.f) {
        result.y = copysignf(PI / 2.f, sp); // use 90 degrees if out of range
    } else {
        result.y = asinf(sp);
    }
    
    // yaw (z-axis rotation)
    real32 sy_cp = 2.f * (a.w * a.z + a.x * a.y);
    real32 cy_cp = 1.f - 2.f * (a.y * a.y + a.z * a.z);
    result.z = atan2f(sy_cp, cy_cp);
    
    return result;
}

//
// Clamp
//

internal s32
clamp(s32 min, s32 val, s32 max) {
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

internal Vector2
clamp_Vector2(Vector2 min, Vector2 val, Vector2 max) {
    Vector2 result = {};
    
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

//
// Color
//

internal inline Vector4
make_color(real32 r, real32 g, real32 b) {
    return make_vector4(r, g, b, 1.0f);
}

internal inline Vector4
make_color(real32 r, real32 g, real32 b, real32 a) {
    return make_vector4(r, g, b, a);
}

internal inline Vector4 
make_color(u32 color) {
    real32 a = (0xff & (color >> 24)) / 255.f;
    real32 r = (0xff & (color >> 16)) / 255.f;
    real32 g = (0xff & (color >>  8)) / 255.f;
    real32 b = (0xff & (color >>  0)) / 255.f;
    return make_color(r, g, b, a);
}

internal inline Vector4
invert_color(Vector4 color) {
    Vector4 result;
    
    result.r = 1.f - color.r;
    result.g = 1.f - color.g;
    result.b = 1.f - color.b;
    result.a = color.a;
    
    return result;
}

internal inline Vector4 
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

internal inline Vector4 
lerp_color(Vector4 a, real32 t, Vector4 b) {
    return make_color(lerp(a.r, t, b.r),
                      lerp(a.g, t, b.g),
                      lerp(a.b, t, b.b),
                      lerp(a.a, t, b.a));
}

internal inline Vector2
lerp_vector2(Vector2 a, real32 t, Vector2 b) {
    Vector2 result = {};
    
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

//
// Matrices
//

internal inline Mat4
mat4_identity() {
    Mat4 result = {};
    
    result._11 = 1.0f;
    result._22 = 1.0f;
    result._33 = 1.0f;
    result._44 = 1.0f;
    
    return result;
}

inline
Vector2 rotate_around(Vector2 a, Vector2 center, real32 angle) {
    Vector2 result = {};
    
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
Vector2 rotate(Vector2 a, real32 angle) {
    Vector2 result = {};
    
    real32 rad = angle_to_radians(angle);
    real32 cint = cosf(rad);
    real32 sint = sinf(rad);
    
    result.x = cint * a.x - sint * a.y;
    result.y = sint * a.x + cint * a.y;
    
    return result;
}

inline Mat4
translate(Vector2 pos) {
    Mat4 result = mat4_identity();
    
    result._14 = pos.x;
    result._24 = pos.y;
    
    return result;
}

inline Mat4
translate(Vector3 pos) {
    Mat4 result = mat4_identity();
    
    result._14 = pos.x;
    result._24 = pos.y;
    result._34 = pos.z;
    
    return result;
}

internal Mat4
operator*(Mat4 a, Mat4 b) {
    Mat4 result = {};
    // memory layout:
    //
    //                row no (=vertical)
    //               |  0   1   2   3
    //            ---+----------------
    //            0  | m00 m10 m20 m30
    // column no  1  | m01 m11 m21 m31
    // (=horiz)   2  | m02 m12 m22 m32
    //            3  | m03 m13 m23 m33
    
    result._11 = a._11 * b._11 + a._12 * b._21 + a._13 * b._31 + a._14 * b._41;
    result._12 = a._11 * b._12 + a._12 * b._22 + a._13 * b._32 + a._14 * b._42;
    result._13 = a._11 * b._13 + a._12 * b._23 + a._13 * b._33 + a._14 * b._43;
    result._14 = a._11 * b._14 + a._12 * b._24 + a._13 * b._34 + a._14 * b._44;
    
    result._21 = a._21 * b._11 + a._22 * b._21 + a._23 * b._31 + a._24 * b._41;
    result._22 = a._21 * b._12 + a._22 * b._22 + a._23 * b._32 + a._24 * b._42;
    result._23 = a._21 * b._13 + a._22 * b._23 + a._23 * b._33 + a._24 * b._43;
    result._24 = a._21 * b._14 + a._22 * b._24 + a._23 * b._34 + a._24 * b._44;
    
    result._31 = a._31 * b._11 + a._32 * b._21 + a._33 * b._31 + a._34 * b._41;
    result._32 = a._31 * b._12 + a._32 * b._22 + a._33 * b._32 + a._34 * b._42;
    result._33 = a._31 * b._13 + a._32 * b._23 + a._33 * b._33 + a._34 * b._43;
    result._34 = a._31 * b._14 + a._32 * b._24 + a._33 * b._34 + a._34 * b._44;
    
    result._41 = a._41 * b._11 + a._42 * b._21 + a._43 * b._31 + a._44 * b._41;
    result._42 = a._41 * b._12 + a._42 * b._22 + a._43 * b._32 + a._44 * b._42;
    result._43 = a._41 * b._13 + a._42 * b._23 + a._43 * b._33 + a._44 * b._43;
    result._44 = a._41 * b._14 + a._42 * b._24 + a._43 * b._34 + a._44 * b._44;
    
    return result;
}

inline Mat4
ortho(real32 left, real32 right, real32 bottom, real32 top, real32 n, real32 f) {
    Mat4 result = mat4_identity();
    
    result._11 =  2.f / (right - left);
    result._22 =  2.f / (top - bottom);
    result._33 = -2.f / (f - n);
    
    result._14 = -((right + left) / (right - left));
    result._24 = -((top + bottom) / (top - bottom));
    result._34 = -((f + n) / (f - n));
    
    return result;
}

inline Mat4
ortho(real32 size, real32 aspect_ratio, real32 n, real32 f) {
    real32 right = size * aspect_ratio;
    real32 left = -right;
    
    real32 top = size;
    real32 bottom = -top;
    
    return ortho(left, right, bottom, top, n, f);
}

inline Mat4
perspective(real32 fov, real32 aspect_ratio, real32 n, real32 f) {
    Mat4 m = mat4_identity();
    
    real32 flTanThetaOver2 = tanf(fov * (real32)PI / 360.f);
    
	// X and Y scaling
	m._11 = 1.f / flTanThetaOver2;
	m._22 = aspect_ratio / flTanThetaOver2;
    
	// Z coordinate makes z -1 when we're on the near plane and +1 on the far plane
	m._33 = (n + f) / (n - f);
	m._34 = 2 * n * f / (n - f);
    
	// W = -1 so that we have [x y z -z], a homogenous vector that becomes [-x/z -y/z -1] after division by w.
	m._43 = -1;
    
	// Must zero this out, the identity has it as 1.
	m._44 = 0;
    
    return m;
}

inline Mat4
look_at(Vector3 position, Vector3 target, Vector3 up = make_vector3(0.f, 1.f, .0f)) {
    Mat4 result = mat4_identity();
    
    Vector3 f = normalize(target - position);
    Vector3 s = normalize(cross(f, up));
    Vector3 u = cross(s, f);
    
    result._11 = s.x;
    result._12 = s.y;
    result._13 = s.z;
    
    result._21 = u.x;
    result._22 = u.y;
    result._23 = u.z;
    
    result._31 = -f.x;
    result._32 = -f.y;
    result._33 = -f.z;
    
    result._14 = -inner(s, position);
    result._24 = -inner(u, position);
    result._34 =  inner(f, position);
    
    return result;
}

inline Mat4
scale(Vector2 scale) {
    Mat4 result = mat4_identity();
    
    result._11 = scale.x;
    result._22 = scale.y;
    
    return result;
}

inline Mat4
scale(Vector3 scale) {
    Mat4 result = mat4_identity();
    
    result._11 = scale.x;
    result._22 = scale.y;
    result._33 = scale.z;
    
    return result;
}

inline Mat4
scale(Mat4 matrix, Vector3 scale) {
    Mat4 result = mat4_identity();
    
    result._11 = scale.x;
    result._22 = scale.y;
    result._33 = scale.z;
    
    return matrix * result;
}

// NOTE(diego): Not tested.
inline Mat4
x_rotation(real32 angle)
{
    real32 c = cosf(angle);
    real32 s = sinf(angle);
    
    Mat4 result = mat4_identity();
    
    result.rc[1][1] = c;
    result.rc[1][2] = -s;
    result.rc[2][1] = s;
    result.rc[2][2] = c;
    
    return result;
}

// NOTE(diego): Not tested.
inline Mat4
y_rotation(real32 angle) {
    real32 c = cosf(angle);
    real32 s = sinf(angle);
    
    Mat4 result = mat4_identity();
    
    result.rc[0][0] = c;
    result.rc[0][2] = s;
    result.rc[2][0] = -s;
    result.rc[2][2] = c;
    
    return result;
}

// NOTE(diego): Not tested.
inline Mat4
z_rotation(real32 angle) {
    real32 c = cosf(angle);
    real32 s = sinf(angle);
    
    Mat4 result = mat4_identity();
    
    result.rc[0][0] = c;
    result.rc[0][1] = -s;
    result.rc[1][0] = s;
    result.rc[1][1] = c;
    
    return result;
}

inline void
set_rotation(Mat4 *m, real32 angle, Vector3 axis) {
    // Normalize beforehand
	assert(fabs(length_sq(axis) - 1) < 0.000001f);
    
    // Assume m is identity
    real32 x = axis.x;
    real32 y = axis.y;
    real32 z = axis.z;
    
    real32 s = sinf(angle);
    real32 c = cosf(angle);
    real32 t = 1.f - c;
    
    m->_11 = x*x*t + c;
    m->_21 = x*y*t - z*s;
    m->_31 = z*x*t + y*s;
    
    m->_12 = y*x*t + z*s;
    m->_22 = y*y*t + c;
    m->_32 = y*z*t - x*s;
    
    m->_13 = z*y*t - y*s;
    m->_23 = z*y*t + x*s;
    m->_33 = z*z*t + c;
}

// Angle should be in radians.
inline Mat4
rotation(real32 angle, Vector3 axis) {
    Mat4 result = mat4_identity();
    set_rotation(&result, angle, axis);
    return result;
}

// NOTE(diego): Borrowed from Unity.
internal void
set_rotation(Mat4 *m, Quaternion q) {
    // NOTE(diego): Assumes m is identity and q is unit.
    
    // Precalculate coordinate products
    real32 x = q.x * 2.0f;
    real32 y = q.y * 2.0f;
    real32 z = q.z * 2.0f;
    real32 xx = q.x * x;
    real32 yy = q.y * y;
    real32 zz = q.z * z;
    real32 xy = q.x * y;
    real32 xz = q.x * z;
    real32 yz = q.y * z;
    real32 wx = q.w * x;
    real32 wy = q.w * y;
    real32 wz = q.w * z;
    
    // Calculate 3x3 matrix from orthonormal basis
    m->_11 = 1.0f - (yy + zz);
    m->_21 = xy + wz;
    m->_31 = xz - wy;
    m->_41 = 0.0f;
    
    m->_12 = xy - wz;
    m->_22 = 1.0f - (xx + zz);
    m->_32 = yz + wx;
    m->_42 = 0.0f;
    
    m->_13 = xz + wy;
    m->_23 = yz - wx;
    m->_33 = 1.0f - (xx + yy);
    m->_43 = 0.0f;
    
    m->_14 = 0.0f;
    m->_24 = 0.0f;
    m->_34 = 0.0f;
    m->_44 = 1.0f;
}
