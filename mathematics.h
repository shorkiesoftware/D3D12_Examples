#pragma once

#include "utilities.h"
#include <immintrin.h>
#include <math.h>

#define HALF_PI     1.570796326794896619231321691639
#define PI          3.141592653589793238462643383279
#define TAU         6.283185307179586476925286766559
#define TAU_INV     0.159154943091895335768883763372
#define E           2.718281828459045235360287471352
#define LOG_2_E     1.442695040888963407359924681001

union Vector2 {
    float v[2];
    struct {
        f32 x;
        f32 y;
    };
    Vector2(){}
    Vector2(f32 a): x(a), y(a){}
    Vector2(f32 x, f32 y): x(x), y(y){}
    
    void operator=(Vector2 a){
        x = a.x;
        y = a.y;
    }

    void operator*=(Vector2 a){
        x *= a.x;
        y *= a.y;
    }

    void operator+=(Vector2 a){
        x += a.x;
        y += a.y;
    }

    void operator-=(Vector2 a){
        x -= a.x;
        y -= a.y;
    }
};

union Vector3 {
    __m128 v;
    f32 va[3];  

    struct{
        f32 x;
        f32 y;
        f32 z;
    };

    Vector3(){}
    Vector3(__m128 a): v(a){}
    Vector3(f32 a){
        v = _mm_set_ps(0, a, a, a);
    }
    Vector3(f32 x, f32 y, f32 z){
        v = _mm_set_ps(0, z, y, x);
    }

    void operator=(Vector3 a){
        v = a.v;
    }

    void operator*=(Vector3& a){
        v = _mm_mul_ps(v, a.v);
    }

    void operator*=(f32 a){
        __m128 t = _mm_set_ps(a, a, a, a);
        v = _mm_mul_ps(v, t);
    }

    void operator/=(f32 a){
        __m128 t = _mm_set_ps(a, a, a, a);
        v = _mm_div_ps(v, t);
    }

    void operator+=(Vector3& a){
        v = _mm_add_ps(v, a.v);
    }

    void operator-=(Vector3& a){
        v = _mm_sub_ps(v, a.v);
    }
};

union Vector4 {
    __m128 v;
    f32 va[4];  
    struct{
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };

    struct{
        Vector2 xy;
        Vector2 zw;
    };

    Vector4(){}
    Vector4(__m128 a): v(a){}
    Vector4(f32 a){
        v = _mm_set_ps1(a);
    }
    Vector4(f32 x, f32 y, f32 z, f32 w){
        v = _mm_set_ps(w, z, y, x);
    }
    Vector4(Vector2 xy, f32 z, f32 w){
        v = _mm_set_ps(w, z, xy.y, xy.x);
    }
    Vector4(Vector2 xy, Vector2 zw){
        v = _mm_set_ps(zw.y, zw.x, xy.y, xy.x);
    }
    Vector4(Vector3 xyz, f32 w){
        v = _mm_set_ps(w, xyz.z, xyz.y, xyz.x);
    }

    void operator=(Vector4 a){
        v = a.v;
    }

    void operator+=(Vector4& a){
        v = _mm_add_ps(v, a.v);
    }

    void operator-=(Vector4& a){
        v = _mm_sub_ps(v, a.v);
    }

    void operator*=(Vector4& a){
        v = _mm_mul_ps(v, a.v);
    }
};

union Quaternion {
    __m128 v;
    f32 va[4];  
    struct{
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };

    Quaternion(){
         v = _mm_set_ps(1, 0, 0, 0);
    }
    Quaternion(__m128 a): v(a){}
    Quaternion(f32 x, f32 y, f32 z, f32 w){
        v = _mm_set_ps(w, z, y, x);
    }
    void operator=(Quaternion a){
        v = a.v;
    }

    void operator+=(Quaternion& a){
        v = _mm_add_ps(v, a.v);
    }

    void operator-=(Quaternion& a){
        v = _mm_sub_ps(v, a.v);
    }
};

union Matrix4 {
    f32 m[16];
    struct{
        f32 m2[4][4];
    };
    struct{
        Vector4 v[4];
    };

    Matrix4(){}
    Matrix4(f32 a){
        v[0] = Vector4(a, 0, 0, 0);
        v[1] = Vector4(0, a, 0, 0);
        v[2] = Vector4(0, 0, a, 0);
        v[3] = Vector4(0, 0, 0, a);
    }

    Matrix4(Vector4 a){
        v[0] = Vector4(a.x, 0, 0, 0);
        v[1] = Vector4(0, a.y, 0, 0);
        v[2] = Vector4(0, 0, a.z, 0);
        v[3] = Vector4(0, 0, 0, a.w);
    }

    void operator=(Matrix4 a){
        v[0] = a.v[0];
        v[1] = a.v[1];
        v[2] = a.v[2];
        v[3] = a.v[3];
    }
};

static f32 map(f32 v, f32 minA, f32 maxA, f32 minB, f32 maxB){
    f32 pct = (v - minA) / (maxA - minA);
    return ((maxB - minB) * pct) + minB;
}

static s32 wrap(s32 v, s32 min, s32 max){
    s32 rng = max - min;
    v = ((v - min) % rng);
    if(v < 0){
        return max + v;
    }else{
        return min + v;
    }
}

static u32 xorshift(u32 x){
    x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
    return x;
}

static f32 sine(f32 v){
    f32 t = v * TAU_INV;
    t = t - (s32)t;
    if(t < 0.5){
        return (-16.0f * t * t) + (8.0f * t);
    }else{
        return (16.0f * t * t) - (16.0f * t) - (8.0f * t) + 8.0f;
    }
}

static f32 cosine(f32 v){
    return sine(v + HALF_PI);
}

static Vector2 operator+(Vector2 v1, Vector2 v2){
    return Vector2(v1.x + v2.x, v1.y + v2.y);
}

static Vector3 operator+(Vector3 v1, Vector3 v2){
    return Vector3(_mm_add_ps(v1.v, v2.v));
}

static Vector4 operator+(Vector4 v1, Vector4 v2){
    return Vector4(_mm_add_ps(v1.v, v2.v));
}

static Quaternion operator+(Quaternion q1, Quaternion q2){
    return Quaternion(_mm_add_ps(q1.v, q2.v));
}

static Vector2 operator-(Vector2 v1, Vector2 v2){
    return Vector2(v1.x - v2.x, v1.y - v2.y);
}

static Vector3 operator-(Vector3 v1, Vector3 v2){
    return Vector3(_mm_sub_ps(v1.v, v2.v));
}

static Vector4 operator-(Vector4 v1, Vector4 v2){
    return Vector4(_mm_sub_ps(v1.v, v2.v));
}

static Quaternion operator-(Quaternion q1, Quaternion q2){
    return Quaternion(_mm_sub_ps(q1.v, q2.v));
}

static Vector2 operator*(Vector2 v1, Vector2 v2){
    return Vector2(v1.x * v2.x, v1.y * v2.y);
}

static Vector3 operator*(Vector3 v1, Vector3 v2){
    return Vector3(_mm_mul_ps(v1.v, v2.v));
}

static Vector4 operator*(Vector4 v1, Vector4 v2){
    return Vector4(_mm_mul_ps(v1.v, v2.v));
}

static Quaternion operator*(Quaternion q1, Quaternion q2){
    Quaternion q;
    q.x = q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
    q.y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y; 
    q.z = q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    q.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
    return q;
}

static Vector2 operator*(Vector2 v1, f32 amt){
    return Vector2(v1.x * amt, v1.y * amt);
}

static Vector3 operator*(Vector3 v1, f32 amt){
    return Vector3(_mm_mul_ps(v1.v, _mm_set_ps1(amt)));
}

static Vector4 operator*(Vector4 v1, f32 amt){
    return Vector4(_mm_mul_ps(v1.v, _mm_set_ps1(amt)));
}

static Quaternion operator*(Quaternion q1, f32 amt){
    return Quaternion(_mm_mul_ps(q1.v, _mm_set_ps1(amt)));
}

static Vector2 operator/(Vector2 v1, Vector2 v2){
    return Vector2(v1.x / v2.x, v1.y / v2.y);
}

static Vector3 operator/(Vector3 v1, Vector3 v2){
    return Vector3(_mm_div_ps(v1.v, v2.v));
}

static Vector4 operator/(Vector4 v1, Vector4 v2){
    return Vector4(_mm_div_ps(v1.v, v2.v));
}

static Quaternion operator/(Quaternion q1, Quaternion q2){
    return Quaternion(_mm_div_ps(q1.v, q2.v));
}

static Vector2 operator+(Vector2 v1, f32 amt){
    return Vector2(v1.x + amt, v1.y + amt);
}

static Vector3 operator/(Vector3 v1, f32 amt){
    return Vector3(_mm_div_ps(v1.v, _mm_set_ps1(amt)));
}

static Vector4 operator/(Vector4 v1, f32 amt){
    return Vector4(_mm_div_ps(v1.v, _mm_set_ps1(amt)));
}

static Vector2 operator-(Vector2 v){
    return Vector2(-v.x, -v.y);
}

static Vector3 operator-(Vector3 v){
    return Vector3(_mm_sub_ps(_mm_set1_ps(0.0), v.v));
}

static Vector4 operator-(Vector4 v){
    return Vector4(_mm_sub_ps(_mm_set1_ps(0.0), v.v));
}

static f32 length(Vector2 v){
    return (f32)sqrt(v.x * v.x + v.y * v.y);
}

static f32 length(Vector3 v){
    v *= v;
    v.v = _mm_hadd_ps(v.v, v.v);
    v.v = _mm_hadd_ps(v.v, v.v);
    return (f32)sqrt(v.x);
    // return (f32)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static f32 length(Vector4 v){
    v *= v;
    v.v = _mm_hadd_ps(v.v, v.v);
    v.v = _mm_hadd_ps(v.v, v.v);
    return (f32)sqrt(v.x);
}

static f32 length(Quaternion v){
    v.v = _mm_mul_ps(v.v, v.v);
    v.v = _mm_hadd_ps(v.v, v.v);
    v.v = _mm_hadd_ps(v.v, v.v);
    return (f32)sqrt(v.x);
}

static void normalize(Vector2* v){
    f32 len = length(*v);
    if(len != 0){
        v->x /= len;
        v->y /= len;
    }else{
        v->x = 0;
        v->y = 0;
    }
}

static void normalize(Vector3* v){
    f32 len = length(*v);
    if(len != 0){
        v->x /= len;
        v->y /= len;
        v->z /= len;
    }else{
        v->x = 0;
        v->y = 0;
        v->z = 0;
    }
}

static void normalize(Vector4* v){
    f32 len = length(*v);
    if(len != 0){
        __m128 t = _mm_set_ps1(len);
        v->v = _mm_div_ps(v->v, t);
    }else{
        v->v = _mm_set_ps1(0);
    }
}

static void normalize(Quaternion* v){
    f32 len = length(*v);
    if(len != 0){
        __m128 t = _mm_set_ps1(len);
        v->v = _mm_div_ps(v->v, t);
    }else{
        v->v = _mm_set_ps(1, 0, 0, 0);
    }
}

static Vector2 normalOf(Vector2 v){
    f32 len = length(v);
    if(len != 0){
        return Vector2(v.x /= len, v.y /= len);
    }else{
        return Vector2(0);
    }
}

static Vector3 normalOf(Vector3 v){
    f32 len = length(v);
    if(len != 0){
        return Vector3(v.x /= len, v.y /= len, v.z /= len);
    }else{
        return Vector3(0);
    }
}

static Vector4 normalOf(Vector4 v){
    f32 len = length(v);
    if(len != 0){
        __m128 t = _mm_set_ps1(len);
        t = _mm_div_ps(v.v, t);
        return Vector4(t);
    }else{
        return Vector4(0);
    }
}

static Quaternion normalOf(Quaternion v){
    f32 len = length(v);
    if(len != 0){
        __m128 t = _mm_set_ps1(len);
        t = _mm_div_ps(v.v, t);
        return Quaternion(t);
    }else{
        return Quaternion();
    }
}

static f32 dot(Vector2 v1, Vector2 v2){
    return v1.x * v2.x + v1.y * v2.y;
}

static f32 dot(Vector3 v1, Vector3 v2){
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

static f32 dot(Vector4 v1, Vector4 v2){
    Vector4 v = v1 * v2;
    v.v = _mm_hadd_ps(v.v, v.v);
    v.v = _mm_hadd_ps(v.v, v.v);
    return v.x; 
}

static f32 dot(Quaternion q1, Quaternion q2){
    Quaternion v(_mm_mul_ps(q1.v, q2.v));
    v.v = _mm_hadd_ps(v.v, v.v);
    v.v = _mm_hadd_ps(v.v, v.v);
    return v.x; 
}

static Vector3 cross(Vector3 v1, Vector3 v2){
    __m128 tmp0 = _mm_shuffle_ps(v1.v, v1.v, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 tmp1 = _mm_shuffle_ps(v2.v, v2.v, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 tmp2 = _mm_mul_ps(tmp0, v2.v);
    __m128 tmp3 = _mm_mul_ps(tmp0, tmp1);
    __m128 tmp4 = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));
    return Vector3(_mm_sub_ps(tmp3, tmp4));
}

static Vector2 rotateAroundPoint(Vector2 v, Vector2 p, f32 angle){
    v -= p;
    f32 s = sin(-angle);
    f32 c = cos(-angle);
    Vector2 n(v.x * c - v.y * s, v.x * s + v.y * c);
    v = n + p;
    return v;
}

static Vector3 linearInterpolation(Vector3 v1, Vector3 v2, f32 t){
    f32 x = v1.x + (v2.x - v1.x) * t;
    f32 y = v1.y + (v2.y - v1.y) * t;
    f32 z = v1.z + (v2.z - v1.z) * t;
    return Vector3(x, y, z);
}

static f32 signedVolume(Vector3 a, Vector3 b, Vector3 c, Vector3 d){
    return (1.0 / 6.0) * dot(cross(b - a, c - a), d - a);
}

static Quaternion rotationToQuaternion(Vector3 axis, f32 angle){
    f32 halfAng = angle * 0.5;
    f32 sinHalfAng = (f32)sin(halfAng);
    f32 cosHalfAng = (f32)cos(halfAng);
    Quaternion q;

    q.x = sinHalfAng * axis.x;
    q.y = sinHalfAng * axis.y;    
    q.z = sinHalfAng * axis.z;
    q.w = cosHalfAng;
    return q;
}

static Quaternion matrix4ToQuaternion(Matrix4* mat){
    Quaternion q;

    f32 sum = mat->m2[0][0] + mat->m2[1][1] + mat->m2[2][2];

    if(sum > 0){
        f32 qw = sqrt(1 + sum) * 0.5;
        f32 qwX4 = 4 * qw;
        q.x = (mat->m2[1][2] - mat->m2[2][1]) / qwX4;
        q.y = (mat->m2[2][0] - mat->m2[0][2]) / qwX4;
        q.z = (mat->m2[0][1] - mat->m2[1][0]) / qwX4;
        q.w = qw;
    }else if(mat->m2[0][0] > mat->m2[1][1] && mat->m2[0][0] > mat->m2[2][2]){
        f32 qq = 2 * sqrt(1 + mat->m2[0][0] - mat->m2[1][1] - mat->m2[2][2]);
        q.x = qq * 0.25;
        q.y = (mat->m2[1][0] + mat->m2[0][1]) / qq;
        q.z = (mat->m2[2][0] + mat->m2[0][2]) / qq;
        q.w = (mat->m2[1][2] - mat->m2[2][1]) / qq;
    }else if(mat->m2[1][1] > mat->m2[2][2]){
        f32 qq = 2 * sqrt(1 + mat->m2[1][1] - mat->m2[0][0] - mat->m2[2][2]);
        q.x = (mat->m2[1][0] + mat->m2[0][1]) / qq;
        q.y = qq * 0.25;
        q.z = (mat->m2[2][1] + mat->m2[1][2]) / qq;
        q.w = (mat->m2[2][0] - mat->m2[0][2]) / qq;
    }else{
        f32 qq = 2 * sqrt(1 + mat->m2[2][2] - mat->m2[0][0] - mat->m2[1][1]);
        q.x = (mat->m2[2][0] + mat->m2[0][2]) / qq;
        q.y = (mat->m2[2][1] + mat->m2[1][2]) / qq;
        q.z = qq * 0.25;
        q.w = (mat->m2[0][1] - mat->m2[1][0]) / qq;
    }
    

    return q;
}

static Quaternion multiply(Quaternion q1, Quaternion q2){
    Quaternion q;
    q.x =   q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
    q.y =  -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
    q.z =   q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    q.w =  -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
    return q;
}

static void rotate(Quaternion* q, Vector3 axis, f32 angle){
    Quaternion rot = rotationToQuaternion(axis, angle);
    *q = multiply(*q, rot);
    normalize(q);
}

static Matrix4 operator*(Matrix4& m1, Matrix4& m2){
    Matrix4 m;
    Vector4 m1c0(m1.m2[0][0], m1.m2[1][0], m1.m2[2][0], m1.m2[3][0]);
    Vector4 m1c1(m1.m2[0][1], m1.m2[1][1], m1.m2[2][1], m1.m2[3][1]);
    Vector4 m1c2(m1.m2[0][2], m1.m2[1][2], m1.m2[2][2], m1.m2[3][2]);
    Vector4 m1c3(m1.m2[0][3], m1.m2[1][3], m1.m2[2][3], m1.m2[3][3]);
     
    m.m[0]  = dot(m1c0, m2.v[0]); 
    m.m[1]  = dot(m1c1, m2.v[0]); 
    m.m[2]  = dot(m1c2, m2.v[0]); 
    m.m[3]  = dot(m1c3, m2.v[0]); 
    m.m[4]  = dot(m1c0, m2.v[1]);  
    m.m[5]  = dot(m1c1, m2.v[1]); 
    m.m[6]  = dot(m1c2, m2.v[1]); 
    m.m[7]  = dot(m1c3, m2.v[1]); 
    m.m[8]  = dot(m1c0, m2.v[2]); 
    m.m[9]  = dot(m1c1, m2.v[2]); 
    m.m[10] = dot(m1c2, m2.v[2]); 
    m.m[11] = dot(m1c3, m2.v[2]); 
    m.m[12] = dot(m1c0, m2.v[3]); 
    m.m[13] = dot(m1c1, m2.v[3]); 
    m.m[14] = dot(m1c2, m2.v[3]); 
    m.m[15] = dot(m1c3, m2.v[3]); 

    return m;
}

static void scaleMatrix(Matrix4* m, Vector3 amt){
    m->m2[0][0] *= amt.x;
    m->m2[1][1] *= amt.y;
    m->m2[2][2] *= amt.z;
}

static void scaleMatrix(Matrix4* m, f32 amt){
    m->m2[0][0] *= amt;
    m->m2[1][1] *= amt;
    m->m2[2][2] *= amt;
}

static Matrix4 createOrthogonalProjection(f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar){
    f32 rminl = right - left;
    f32 tminb = top - bottom;
    Matrix4 m;
    m.v[0] = Vector4(2.0f / rminl, 0, 0, 0);
    m.v[1] = Vector4(0, 2.0f / tminb, 0, 0);
    m.v[2] = Vector4(0, 0, 2.0f / (znear - zfar), 0);
    m.v[3] = Vector4(-(right + left) / rminl, -(top + bottom) / tminb, -(zfar + znear) / (zfar - znear), 1);
    return m;
}

static Matrix4 createPerspectiveProjection(f32 fov, f32 aspect, f32 znear, f32 zfar){
    fov = (TAU * fov) / 360.0;
    Matrix4 m;
    m.m[0] = 1 / (aspect * tan(fov / 2.0));
    m.m[1] = 0;
    m.m[2] = 0;
    m.m[3] = 0;
    m.m[4] = 0;
    m.m[5] = 1 / (tan(fov / 2.0));
    m.m[6] = 0;
    m.m[7] = 0;
    m.m[8] = 0;
    m.m[9] = 0;
    m.m[10] = -(zfar + znear) / (zfar - znear);
    m.m[11] = -1;
    m.m[12] = 0;
    m.m[13] = 0;
    m.m[14] = -(zfar * znear) / (zfar - znear);
    m.m[15] = 0;
    return m;
}

static Matrix4 quaternionToMatrix4(Quaternion q){
    Matrix4 m(0);

    m.m2[0][0] = 1 - (2 * q.y * q.y) - (2 * q.z * q.z);
    m.m2[0][1] = (2 * q.x * q.y) + (2 * q.z * q.w);
    m.m2[0][2] = (2 * q.x * q.z) - (2 * q.y * q.w);
    m.m2[0][3] = 0;
    m.m2[1][0] = (2 * q.x * q.y) - (2 * q.z * q.w);
    m.m2[1][1] = 1 - (2 * q.x * q.x) - (2 * q.z * q.z);
    m.m2[1][2] = (2 * q.y * q.z) + (2 * q.x * q.w);
    m.m2[1][3] = 0;
    m.m2[2][0] = (2 * q.x * q.z) + (2 * q.y * q.w);
    m.m2[2][1] = (2 * q.y * q.z) - (2 * q.x * q.w);
    m.m2[2][2] = 1 - (2 * q.x * q.x) - (2 * q.y * q.y);
    m.m2[2][3] = 0;
    m.m2[3][0] = 0;
    m.m2[3][1] = 0;
    m.m2[3][2] = 0;
    m.m2[3][3] = 1;

    return m;
}

static void translateMatrix(Matrix4* m, Vector3 v){
    m->m2[3][0] += v.x;
    m->m2[3][1] += v.y;
    m->m2[3][2] += v.z;
}

static Matrix4 buildModelMatrix(Vector3 position, Vector3 scale, Quaternion orientation){
    Matrix4 rot = quaternionToMatrix4(orientation);
    translateMatrix(&rot, position);
    return rot * Matrix4(Vector4(scale, 1));
}

static Vector3 position(Matrix4* m){
    return Vector3(m->m2[3][0], m->m2[3][1], m->m2[3][2]);
}

static Matrix4 inverseOf(Matrix4* m){
    Matrix4 a;
    a.m[0] = m->m2[1][1] * m->m2[2][2] * m->m2[3][3] + 
             m->m2[1][2] * m->m2[2][3] * m->m2[3][1] +
             m->m2[1][3] * m->m2[2][1] * m->m2[3][2] -
             m->m2[1][1] * m->m2[2][3] * m->m2[3][2] -
             m->m2[1][2] * m->m2[2][1] * m->m2[3][3] -
             m->m2[1][3] * m->m2[2][2] * m->m2[3][1];
    
    a.m[1] = m->m2[0][1] * m->m2[2][3] * m->m2[3][2] + 
             m->m2[0][2] * m->m2[2][1] * m->m2[3][3] +
             m->m2[0][3] * m->m2[2][2] * m->m2[3][1] -
             m->m2[0][1] * m->m2[2][2] * m->m2[3][3] -
             m->m2[0][2] * m->m2[2][3] * m->m2[3][1] -
             m->m2[0][3] * m->m2[2][1] * m->m2[3][2];

    a.m[2] = m->m2[0][1] * m->m2[1][2] * m->m2[3][3] + 
             m->m2[0][2] * m->m2[1][3] * m->m2[3][1] +
             m->m2[0][3] * m->m2[1][1] * m->m2[3][2] -
             m->m2[0][1] * m->m2[1][3] * m->m2[3][2] -
             m->m2[0][2] * m->m2[1][1] * m->m2[3][3] -
             m->m2[0][3] * m->m2[1][2] * m->m2[3][1];

    a.m[3] = m->m2[0][1] * m->m2[1][3] * m->m2[2][2] + 
             m->m2[0][2] * m->m2[1][1] * m->m2[2][3] +
             m->m2[0][3] * m->m2[1][2] * m->m2[2][1] -
             m->m2[0][1] * m->m2[1][2] * m->m2[2][3] -
             m->m2[0][2] * m->m2[1][3] * m->m2[2][1] -
             m->m2[0][3] * m->m2[1][1] * m->m2[2][2];

    a.m[4] = m->m2[1][0] * m->m2[2][3] * m->m2[3][2] + 
             m->m2[1][2] * m->m2[2][0] * m->m2[3][3] +
             m->m2[1][3] * m->m2[2][2] * m->m2[3][0] -
             m->m2[1][0] * m->m2[2][2] * m->m2[3][3] -
             m->m2[1][2] * m->m2[2][3] * m->m2[3][0] -
             m->m2[1][3] * m->m2[2][0] * m->m2[3][2];

    a.m[5] = m->m2[0][0] * m->m2[2][2] * m->m2[3][3] + 
             m->m2[0][2] * m->m2[2][3] * m->m2[3][0] +
             m->m2[0][3] * m->m2[2][0] * m->m2[3][2] -
             m->m2[0][0] * m->m2[2][3] * m->m2[3][2] -
             m->m2[0][2] * m->m2[2][0] * m->m2[3][3] -
             m->m2[0][3] * m->m2[2][2] * m->m2[3][0];
    
    a.m[6] = m->m2[0][0] * m->m2[1][3] * m->m2[3][2] + 
             m->m2[0][2] * m->m2[1][0] * m->m2[3][3] +
             m->m2[0][3] * m->m2[1][2] * m->m2[3][0] -
             m->m2[0][0] * m->m2[1][2] * m->m2[3][3] -
             m->m2[0][2] * m->m2[1][3] * m->m2[3][0] -
             m->m2[0][3] * m->m2[1][0] * m->m2[3][2];

    a.m[7] = m->m2[0][0] * m->m2[1][2] * m->m2[2][3] + 
             m->m2[0][2] * m->m2[1][3] * m->m2[2][0] +
             m->m2[0][3] * m->m2[1][0] * m->m2[2][2] -
             m->m2[0][0] * m->m2[1][3] * m->m2[2][2] -
             m->m2[0][2] * m->m2[1][0] * m->m2[2][3] -
             m->m2[0][3] * m->m2[1][2] * m->m2[2][0];

    a.m[8] = m->m2[1][0] * m->m2[2][1] * m->m2[3][3] + 
             m->m2[1][1] * m->m2[2][3] * m->m2[3][0] +
             m->m2[1][3] * m->m2[2][0] * m->m2[3][1] -
             m->m2[1][0] * m->m2[2][3] * m->m2[3][1] -
             m->m2[1][1] * m->m2[2][0] * m->m2[3][3] -
             m->m2[1][3] * m->m2[2][1] * m->m2[3][0];

    a.m[9] = m->m2[0][0] * m->m2[2][3] * m->m2[3][1] + 
             m->m2[0][1] * m->m2[2][0] * m->m2[3][3] +
             m->m2[0][3] * m->m2[2][1] * m->m2[3][0] -
             m->m2[0][0] * m->m2[2][1] * m->m2[3][3] -
             m->m2[0][1] * m->m2[2][3] * m->m2[3][0] -
             m->m2[0][3] * m->m2[2][0] * m->m2[3][1];

    a.m[10] = m->m2[0][0] * m->m2[1][1] * m->m2[3][3] + 
              m->m2[0][1] * m->m2[1][3] * m->m2[3][0] +
              m->m2[0][3] * m->m2[1][0] * m->m2[3][1] -
              m->m2[0][0] * m->m2[1][3] * m->m2[3][1] -
              m->m2[0][1] * m->m2[1][0] * m->m2[3][3] -
              m->m2[0][3] * m->m2[1][1] * m->m2[3][0];

    a.m[11] = m->m2[0][0] * m->m2[1][3] * m->m2[2][1] + 
              m->m2[0][1] * m->m2[1][0] * m->m2[2][3] +
              m->m2[0][3] * m->m2[1][1] * m->m2[2][0] -
              m->m2[0][0] * m->m2[1][1] * m->m2[2][3] -
              m->m2[0][1] * m->m2[1][3] * m->m2[2][0] -
              m->m2[0][3] * m->m2[1][0] * m->m2[2][1];

    a.m[12] = m->m2[1][0] * m->m2[2][2] * m->m2[3][1] + 
              m->m2[1][1] * m->m2[2][0] * m->m2[3][2] +
              m->m2[1][2] * m->m2[2][1] * m->m2[3][0] -
              m->m2[1][0] * m->m2[2][1] * m->m2[3][2] -
              m->m2[1][1] * m->m2[2][2] * m->m2[3][0] -
              m->m2[1][2] * m->m2[2][0] * m->m2[3][1];

    a.m[13] = m->m2[0][0] * m->m2[2][1] * m->m2[3][2] + 
              m->m2[0][1] * m->m2[2][2] * m->m2[3][0] +
              m->m2[0][2] * m->m2[2][0] * m->m2[3][1] -
              m->m2[0][0] * m->m2[2][2] * m->m2[3][1] -
              m->m2[0][1] * m->m2[2][0] * m->m2[3][2] -
              m->m2[0][2] * m->m2[2][1] * m->m2[3][0];

    a.m[14] = m->m2[0][0] * m->m2[1][2] * m->m2[3][1] + 
              m->m2[0][1] * m->m2[1][0] * m->m2[3][2] +
              m->m2[0][2] * m->m2[1][1] * m->m2[3][0] -
              m->m2[0][0] * m->m2[1][1] * m->m2[3][2] -
              m->m2[0][1] * m->m2[1][2] * m->m2[3][0] -
              m->m2[0][2] * m->m2[1][0] * m->m2[3][1];

    a.m[15] = m->m2[0][0] * m->m2[1][1] * m->m2[2][2] + 
              m->m2[0][1] * m->m2[1][2] * m->m2[2][0] +
              m->m2[0][2] * m->m2[1][0] * m->m2[2][1] -
              m->m2[0][0] * m->m2[1][2] * m->m2[2][1] -
              m->m2[0][1] * m->m2[1][0] * m->m2[2][2] -
              m->m2[0][2] * m->m2[1][1] * m->m2[2][0];

    

    f32 det = m->m[0] * a.m[0] + m->m[1] * a.m[4] + m->m[2] * a.m[8] + m->m[3] * a.m[12];

    if(det == 0){
        return Matrix4(1);
    } 

    det = 1.0 / det;

    for(u32 i = 0; i < 16; i++){
        a.m[i] *= det;
    }

    return a;
}

Matrix4 lookAt(Vector3 position, Vector3 target, Vector3 up = Vector3(0, 1, 0)){
    Vector3 forward = normalOf(target - position);
    Vector3 right = normalOf(cross(forward, up));
    up = normalOf(cross(right, forward));
    Matrix4 mat;
    mat.m2[0][0] = right.x;
    mat.m2[0][1] = up.x;
    mat.m2[0][2] = -forward.x;
    mat.m2[0][3] = 0;
    mat.m2[1][0] = right.y;
    mat.m2[1][1] = up.y;
    mat.m2[1][2] = -forward.y;
    mat.m2[1][3] = 0;
    mat.m2[2][0] = right.z;
    mat.m2[2][1] = up.z;
    mat.m2[2][2] = forward.z;
    mat.m2[2][3] = 0;
    mat.m2[3][0] = dot(-right, position);
    mat.m2[3][1] = dot(-up, position);
    mat.m2[3][2] = dot(forward, position);
    mat.m2[3][3] = 1;
    return mat;
}

static Quaternion lookAtQuaternion(Vector3 position, Vector3 target, Vector3 faceVector = Vector3(0, 0, 1))
{
    Vector3 forwardVector = normalOf(target - position);

    Vector3 rotAxis = cross(faceVector, forwardVector);
    f32 dt = dot(faceVector, forwardVector);

    Quaternion q;
    q.x = rotAxis.x;
    q.y = rotAxis.y;
    q.z = rotAxis.z;
    q.w = dt + 1;

    return normalOf(q);
}

static Quaternion slerp(Quaternion q1, Quaternion q2, f32 t){
    normalize(&q1);
    normalize(&q2);

    f32 dp = dot(q1, q2);

    if (dp < 0.0f) {
        q1 = Quaternion(-q1.x, -q1.y, -q1.z, -q1.w);
        dp = -dp;
    }

    if (dp > 0.99995) {
        Quaternion result = q1 + (q2 - q1) * t;
        normalize(&result);
        return result;
    }

    f32 theta_0 = acos(dp);        
    f32 theta = theta_0 * t;          
    f32 sin_theta = sin(theta);     
    f32 sin_theta_0 = sin(theta_0); 

    f32 s0 = cos(theta) - dp * sin_theta / sin_theta_0;  
    f32 s1 = sin_theta / sin_theta_0;

    return (q1 * s0) + (q2 * s1);
}

static f32 absoluteValue(f32 f){
    return f < 0 ? -f : f;
}

static f32 cubicInterpolation(f32 p1, f32 p2, f32 p3, f32 p4, f32 t){
    f32 a = (-0.5 * p1 + 1.5 * p2 - 1.5 * p3 + 0.5 * p4) * t * t * t;
    f32 b = (p1 - 2.5 * p2 + 2 * p3 - 0.5 * p4) * t * t;
    f32 c = (-0.5 * p1 + 0.5 * p3) * t;
    return a + b + c + p2; 
}

static f32 clamp(f32 v, f32 min, f32 max){
    if(v < min) return min;
    if(v > max) return max;
    return v;
}