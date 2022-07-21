#pragma once

#include <math.h>
#include <cmath>
#include <stdlib.h> // for rand()

#define PI  3.14159265359
#define TAU 6.28318530718

static inline float PowerF32(float, float);

struct v2 {
    float x,y;

    v2(float, float);
    v2() = default;

    inline float Dot(v2 a)     const;
    inline v2  Normalize()     const;
    inline float Length()      const;
    inline float SqrLength()   const;
    
    inline v2 operator+(float) const;
    inline v2 operator-(float) const;
    inline v2 operator*(float) const;
    inline v2 operator/(float) const;
    inline void operator+=(float);
    inline void operator-=(float);
    inline void operator*=(float);
    inline void operator/=(float);
    inline v2 operator+(v2) const;
    inline v2 operator-(v2) const;
    inline v2 operator*(v2) const;
    inline v2 operator/(v2) const;
    inline void operator+=(v2);
    inline void operator-=(v2);
    inline void operator*=(v2);
    inline void operator/=(v2);
    inline void operator= (v2);

    inline friend v2 operator*(float, v2);

    void Print();
};

v2::v2(float ix, float iy) {
    x = ix;
    y = iy;
}

inline float v2::Dot(v2 a) const {
    return x * a.x + y * a.y;
}

inline v2 v2::Normalize() const {
    v2 res = {0,0};
    float length = sqrtf(x*x + y*y);
    if(length != 0) {
        res = *this * (1.0f / length);
    }
    return res;
}

inline float v2::Length() const {
    return sqrtf(x*x + y*y);
}

inline float v2::SqrLength() const {
    return x*x + y*y;
}

inline v2 v2::operator+(float a) const { return { x + a, y + a}; }
inline v2 v2::operator-(float a) const { return { x - a, y - a}; }
inline v2 v2::operator*(float a) const { return { x * a, y * a}; }
inline v2 v2::operator/(float a) const { return { x / a, y / a}; }
inline void v2::operator+=(float a) { x += a; y += a; }
inline void v2::operator-=(float a) { x -= a; y -= a; }
inline void v2::operator*=(float a) { x *= a; y *= a; }
inline void v2::operator/=(float a) { x /= a; y /= a; }
inline v2 v2::operator+(v2 a) const { return { x + a.x, y + a.y}; }
inline v2 v2::operator-(v2 a) const { return { x - a.x, y - a.y}; }
inline v2 v2::operator*(v2 a) const { return { x * a.x, y * a.y}; }
inline v2 v2::operator/(v2 a) const { return { x / a.x, y / a.y}; }
inline void v2::operator+=(v2 a) { x += a.x; y += a.y; }
inline void v2::operator-=(v2 a) { x -= a.x; y -= a.y; }
inline void v2::operator*=(v2 a) { x *= a.x; y *= a.y; }
inline void v2::operator/=(v2 a) { x /= a.x; y /= a.y; }
inline void v2::operator= (v2 a) { x = a.x; y = a.y; }

inline v2 operator*(float a, v2 v) { return {v.x * a, v.y * a}; }

void v2::Print() {
#ifdef DebugLog
    DebugLog("Vec2(%f, %f)\n", x, y);
#endif
}

struct v3 {
    float x,y,z;

    v3(float, float, float);
    v3() = default;

    inline float Dot(v3 a)     const;
    inline v3  Cross(v3 a)     const;
    inline v3  Normalize()     const;
    inline float Length()      const;
    inline float SqrLength()   const;
    
    inline v3 operator+(float) const;
    inline v3 operator-(float) const;
    inline v3 operator*(float) const;
    inline v3 operator/(float) const;
    inline void operator+=(float);
    inline void operator-=(float);
    inline void operator*=(float);
    inline void operator/=(float);
    inline v3 operator+(v3) const;
    inline v3 operator-(v3) const;
    inline v3 operator*(v3) const;
    inline v3 operator/(v3) const;
    inline void operator+=(v3);
    inline void operator-=(v3);
    inline void operator*=(v3);
    inline void operator/=(v3);
    inline void operator= (v3);

    inline friend v3 operator*(float, v3);

    void Print();
};

v3::v3(float ix, float iy, float iz) {
    x = ix;
    y = iy;
    z = iz;
}

inline float v3::Dot(v3 a) const {
    return x * a.x + y * a.y + z * a.z;
}

inline v3 v3::Cross(v3 a) const {
    v3 r;
    r.x = y * a.z - z * a.y;
    r.y = z * a.x - x * a.z;
    r.z = x * a.y - y * a.x;
    return r;
}

inline v3 v3::Normalize() const {
    v3 res = {0,0,0};
    float length = sqrtf(x*x + y*y + z*z);
    if(length != 0) {
        res = *this * (1.0f / length);
    }
    return res;
}

inline float v3::Length() const {
    return sqrtf(x*x + y*y + z*z);
}

inline float v3::SqrLength() const {
    return x*x + y*y + z*z;
}

inline v3 v3::operator+(float a) const { return { x + a, y + a, z + a }; }
inline v3 v3::operator-(float a) const { return { x - a, y - a, z - a }; }
inline v3 v3::operator*(float a) const { return { x * a, y * a, z * a }; }
inline v3 v3::operator/(float a) const { return { x / a, y / a, z / a }; }
inline void v3::operator+=(float a) { x += a; y += a; z += a; }
inline void v3::operator-=(float a) { x -= a; y -= a; z -= a; }
inline void v3::operator*=(float a) { x *= a; y *= a; z *= a; }
inline void v3::operator/=(float a) { x /= a; y /= a; z /= a; }
inline v3 v3::operator+(v3 a) const { return { x + a.x, y + a.y, z + a.z }; }
inline v3 v3::operator-(v3 a) const { return { x - a.x, y - a.y, z - a.z }; }
inline v3 v3::operator*(v3 a) const { return { x * a.x, y * a.y, z * a.z }; }
inline v3 v3::operator/(v3 a) const { return { x / a.x, y / a.y, z / a.z }; }
inline void v3::operator+=(v3 a) { x += a.x; y += a.y; z += a.z; }
inline void v3::operator-=(v3 a) { x -= a.x; y -= a.y; z -= a.z; }
inline void v3::operator*=(v3 a) { x *= a.x; y *= a.y; z *= a.z; }
inline void v3::operator/=(v3 a) { x /= a.x; y /= a.y; z /= a.z; }
inline void v3::operator= (v3 a) { x = a.x; y = a.y; z = a.z; };

inline v3 operator*(float a, v3 v) { return {v.x * a, v.y * a, v.z * a}; }

void v3::Print() {
#ifdef DebugLog
    DebugLog("Vec3(%f, %f, %f)\n", x, y, z);
#endif
}

struct v4 {
    float x,y,z,w;
    
    v4(float, float, float, float);
    v4(v3, float);
    v4() = default;

    inline float Dot(v4 a)     const;
    inline v4  Normalize()     const;
    inline float Length()      const;
    inline float SqrLength()   const;
    
    inline v4 operator+(float) const;
    inline v4 operator-(float) const;
    inline v4 operator*(float) const;
    inline v4 operator/(float) const;
    inline void operator+=(float);
    inline void operator-=(float);
    inline void operator*=(float);
    inline void operator/=(float);
    inline v4 operator+(v4) const;
    inline v4 operator-(v4) const;
    inline v4 operator*(v4) const;
    inline v4 operator/(v4) const;
    inline void operator+=(v4);
    inline void operator-=(v4);
    inline void operator*=(v4);
    inline void operator/=(v4);
    inline void operator= (v4);

    inline friend v4 operator*(float, v4);

    void Print();
};

v4::v4(float ix, float iy, float iz, float iw) {
    x = ix;
    y = iy;
    z = iz;
    w = iw;
}

v4::v4(v3 v, float iw) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = iw;
}


inline float v4::Dot(v4 a) const {
    return x * a.x + y * a.y + z * a.z + w * a.w;
}

inline v4 v4::Normalize() const {
    v4 res = {0,0,0,0};
    float length = sqrtf(x*x + y*y + z*z + w*w);
    if(length != 0) {
        res = *this * (1.0f / length);
    }
    return res;
}

inline float v4::Length() const {
    return sqrtf(x*x + y*y + z*z + w*w);
}

inline float v4::SqrLength() const {
    return x*x + y*y + z*z + w*w;
}

inline v4 v4::operator+(float a) const { return { x + a, y + a, z + a, w + a }; }
inline v4 v4::operator-(float a) const { return { x - a, y - a, z - a, w - a }; }
inline v4 v4::operator*(float a) const { return { x * a, y * a, z * a, w * a }; }
inline v4 v4::operator/(float a) const { return { x / a, y / a, z / a, w / a }; }
inline void v4::operator+=(float a) { x += a; y += a; z += a; w += a; }
inline void v4::operator-=(float a) { x -= a; y -= a; z -= a; w -= a; }
inline void v4::operator*=(float a) { x *= a; y *= a; z *= a; w *= a; }
inline void v4::operator/=(float a) { x /= a; y /= a; z /= a; w /= a; }
inline v4 v4::operator+(v4 a) const { return { x + a.x, y + a.y, z + a.z, w + a.w }; }
inline v4 v4::operator-(v4 a) const { return { x - a.x, y - a.y, z - a.z, w - a.w }; }
inline v4 v4::operator*(v4 a) const { return { x * a.x, y * a.y, z * a.z, w * a.w }; }
inline v4 v4::operator/(v4 a) const { return { x / a.x, y / a.y, z / a.z, w / a.w }; }
inline void v4::operator+=(v4 a) { x += a.x; y += a.y; z += a.z, w += a.w; }
inline void v4::operator-=(v4 a) { x -= a.x; y -= a.y; z -= a.z, w -= a.w; }
inline void v4::operator*=(v4 a) { x *= a.x; y *= a.y; z *= a.z, w *= a.w; }
inline void v4::operator/=(v4 a) { x /= a.x; y /= a.y; z /= a.z, w /= a.w; }
inline void v4::operator= (v4 a) { x = a.x; y = a.y; z = a.z; w = a.w; };

inline v4 operator*(float a, v4 v) { return {v.x * a, v.y * a, v.z * a, v.w * a}; }

void v4::Print() {
#ifdef DebugLog
    DebugLog("Vec4(%f, %f, %f, %f)\n", x, y, z, w);
#endif
}

// m0 m4 m8  m12
// m1 m5 m9  m13
// m2 m6 m10 m14
// m3 m7 m11 m15
struct m4 { // matrix4x4
    float values[16];

    static m4 Identity(float);
    inline void   operator=(m4 const&);
    inline float& operator[](int);
    inline m4     operator*(m4 const&);
    inline v4     operator*(v4 const&) const;

    inline void SetRow(int, float, float, float, float);
    inline void SetColumn(int, float, float, float, float);
    inline m4 Inverse();
    
    inline void Print();
};

inline m4 m4::Identity(float a = 1.0f) {
    m4 ret = {0};
    ret[0 * 4 + 0] = a;
    ret[1 * 4 + 1] = a;
    ret[2 * 4 + 2] = a;
    ret[3 * 4 + 3] = a;
    return ret;
}

inline float& m4::operator[](int index) { return values[index]; }

inline void m4::operator=(m4 const& a) {
    (*this)[0]  = a.values[0];
    (*this)[1]  = a.values[1];
    (*this)[2]  = a.values[2];
    (*this)[3]  = a.values[3];
    (*this)[4]  = a.values[4];
    (*this)[5]  = a.values[5];
    (*this)[6]  = a.values[6];
    (*this)[7]  = a.values[7];
    (*this)[8]  = a.values[8];
    (*this)[9]  = a.values[9];
    (*this)[10] = a.values[10];
    (*this)[11] = a.values[11];
    (*this)[12] = a.values[12];
    (*this)[13] = a.values[13];
    (*this)[14] = a.values[14];
    (*this)[15] = a.values[15];
}

inline m4 m4::operator*(m4 const& a) {
    m4 res = {};
    float* o1 = &(this->values[0]);
    const float* o2 = &(a.values[0]);

    res[0]  = o1[0] * o2[0] + o1[4] * o2[1] + o1[8] * o2[2] + o1[12] * o2[3]; 
    res[4]  = o1[0] * o2[4] + o1[4] * o2[5] + o1[8] * o2[6] + o1[12] * o2[7]; 
    res[8]  = o1[0] * o2[8] + o1[4] * o2[9] + o1[8] * o2[10] + o1[12] * o2[11]; 
    res[12] = o1[0] * o2[12] + o1[4] * o2[13] + o1[8] * o2[14] + o1[12] * o2[15]; 

    res[1]  = o1[1] * o2[0] + o1[5] * o2[1] + o1[9] * o2[2] + o1[13] * o2[3];
    res[5]  = o1[1] * o2[4] + o1[5] * o2[5] + o1[9] * o2[6] + o1[13] * o2[7]; 
    res[9]  = o1[1] * o2[8] + o1[5] * o2[9] + o1[9] * o2[10] + o1[13] * o2[11]; 
    res[13] = o1[1] * o2[12] + o1[5] * o2[13] + o1[9] * o2[14] + o1[13] * o2[15]; 

    res[2]  = o1[2] * o2[0] + o1[6] * o2[1] + o1[10] * o2[2] + o1[14] * o2[3];
    res[6]  = o1[2] * o2[4] + o1[6] * o2[5] + o1[10] * o2[6] + o1[14] * o2[7]; 
    res[10] = o1[2] * o2[8] + o1[6] * o2[9] + o1[10] * o2[10] + o1[14] * o2[11]; 
    res[14] = o1[2] * o2[12] + o1[6] * o2[13] + o1[10] * o2[14] + o1[14] * o2[15];

    res[3]  = o1[3] * o2[0] + o1[7] * o2[1] + o1[11] * o2[2] + o1[15] * o2[3];
    res[7]  = o1[3] * o2[4] + o1[7] * o2[5] + o1[11] * o2[6] + o1[15] * o2[7];
    res[11] = o1[3] * o2[8] + o1[7] * o2[9] + o1[11] * o2[10] + o1[15] * o2[11];
    res[15] = o1[3] * o2[12] + o1[7] * o2[13] + o1[11] * o2[14] + o1[15] * o2[15];
    
    return res;
}

inline v4 m4::operator*(v4 const& a) const {
    v4 res = {};
    const float* o = &(this->values[0]);
    
    res.x = o[0] * a.x + o[1] * a.y + o[2]  * a.z + o[3]  * a.w;
    res.y = o[4] * a.x + o[5] * a.y + o[6]  * a.z + o[7]  * a.w;
    res.z = o[8] * a.x + o[9] * a.y + o[10] * a.z + o[11] * a.w;
    res.w = o[12] * a.x + o[13] * a.y + o[14] * a.z + o[15] * a.w;

    return res;
}

inline void m4::SetRow(int row, float a1, float a2, float a3, float a4) {
    values[row + 0 * 4] = a1;
    values[row + 1 * 4] = a2;
    values[row + 2 * 4] = a3;
    values[row + 3 * 4] = a4;
}

inline void m4::SetColumn(int column, float a1, float a2, float a3, float a4) {
    values[column * 4 + 0] = a1;
    values[column * 4 + 1] = a2;
    values[column * 4 + 2] = a3;
    values[column * 4 + 3] = a4;
}

// https://semath.info/src/inverse-cofactor-ex4.html
inline m4 m4::Inverse() {
    float* m = (float*)&(this->values[0]);
    // calculate co-factors and determinant
    float determinant = 0.0f;
    float cofactors[16];
    for(int i = 0; i < 16; ++i) {
        float v[9];
        int vcounter = 0;
        int col = i % 4;
        int row = i / 4;
        for(int j = 0; j < 16; ++j) {
            int scol = j % 4;
            int srow = j / 4;
            if(scol != row && srow != col) { // we flip row and column
                v[vcounter++] = m[j];
            };
        }
        float det = v[0] * v[4] * v[8] + v[3] * v[7] * v[2] + v[1] * v[5] * v[6] - v[6] * v[4] * v[2] - v[1] * v[3] * v[8] - v[7] * v[5] * v[0];
        cofactors[i] = PowerF32(-1.0f, (float)(col + row + 2)) * det;

        // so this is not same with the article I read
        // https://semath.info/src/determinant-four-by-four.html
        if(i < 4) {
            if(i == 0) determinant = values[i * 4] * det;
            else {
                if((i % 2) == 0) determinant += values[i * 4] * det;
                else determinant -= values[i * 4] * det;
            }
        }
    }
    // transpose matrix
    // float adjoint[16];
    // for(int i = 0; i < 4; ++i) {
    //     for(int j = 0; j < 4; ++j) {
    //         int row = i + j * 4;
    //         int col = i * 4 + j;
    //         adjoint[row] = cofactors[col];
    //     }
    // }
    // if determinant is zero there is no inverse matrix
    if(determinant == 0) return {0};

    m4 inverse = {};
    // 1.0f / determinant * adjoint
    float OneOverDet = 1.0f / determinant;
    for(int i = 0; i < 16; ++i) {
        inverse[i] = cofactors[i] * OneOverDet;
    }
    return inverse;
}

inline m4
MakeOrthoMatrix(float Left, float Right,
            float Top, float Bottom,
            float Near, float Far)
{
    m4 OrthoMatrix;
    OrthoMatrix.SetRow(0, 2.0f/(Right - Left), 0, 0, 0);
    OrthoMatrix.SetRow(1, 0, 2.0f/(Top - Bottom), 0, 0);
    OrthoMatrix.SetRow(2, 0, 0, -2.0f/(Far - Near), 0);
    OrthoMatrix.SetRow(3, -((Right + Left)/(Right - Left)), -((Top + Bottom)/(Top - Bottom)), -((Far+Near)/(Far-Near)), 1);

    return OrthoMatrix;
}

inline void m4::Print() {
#ifdef DebugLog
    DebugLog("\nRow major:\n");
    DebugLog("(%f, %f, %f, %f)\n", values[0], values[4], values[8],  values[12]); 
    DebugLog("(%f, %f, %f, %f)\n", values[1], values[5], values[9],  values[13]); 
    DebugLog("(%f, %f, %f, %f)\n", values[2], values[6], values[10], values[14]); 
    DebugLog("(%f, %f, %f, %f)\n", values[3], values[7], values[11], values[15]); 
    DebugLog("Column major:\n");
    DebugLog("(%f, %f, %f, %f)\n", values[0], values[1], values[2],  values[3]); 
    DebugLog("(%f, %f, %f, %f)\n", values[4], values[5], values[6],  values[7]); 
    DebugLog("(%f, %f, %f, %f)\n", values[8], values[9], values[10], values[11]); 
    DebugLog("(%f, %f, %f, %f)\n", values[12], values[13], values[14], values[15]); 
    DebugLog("\n");
#endif
}

static inline float
Abs(float a) {
    return fabs(a);
}

static inline float
Min(float a, float b) {
    return a < b ? a : b;
}

static inline float
Max(float a, float b) {
    return a > b ? a : b;
}

static inline int
Max(int a, int b) {
    return a > b ? a : b;
}

static inline int
Min(int a, int b) {
    return a < b ? a : b;
}

static inline float
PowerF32(float x, float p) {
    return pow(x, p);
}

static inline float
Rand01() {
    return (float)rand() / (float)RAND_MAX; 
}

static inline float
Clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

static inline int
Clamp(int x, int min, int max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

static inline float
Dot(v3 a, v3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline v3
Cross(v3 a, v3 b) {
    v3 r;
    r.x = a.y * b.z - a.z * b.y;
    r.y = a.z * b.x - a.x * b.z;
    r.z = a.x * b.y - a.y * b.x;
    return r;
}

static inline v3
Normalize(v3 a) {
    return a / a.Length();
}

static inline float
Sqrt(float a) {
    return sqrtf(a);
}

static inline v3
RandomInUnitSphere() {
    for(;;) {
        v3 p = v3(Rand01() * 2 - 1, Rand01() * 2 - 1, Rand01() * 2 - 1);
        if(p.SqrLength() >= 1) continue;
        return p;
    }
}

static inline v3
RandomInHemiSphere(v3 normal) {
    v3 rand = RandomInUnitSphere();
    return (Dot(normal, rand) > 0) ? rand : rand * -1;
}

static inline v3
RandomInUnitDisk() {
    while (true) {
        v3 p = v3(Rand01()*2-1, Rand01()*2-1, 0);
        if (p.SqrLength() >= 1) continue;
        return p;
    }
}

static inline v3
Reflect(v3 d, v3 n) {
    return d - 2*Dot(d, n)*n;
}

static inline v3
Refract(v3 uv, v3 n, float etai_over_etat) {
    float cos_theta = Min(Dot(uv * -1, n), 1.0);
    v3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    v3 r_out_parallel = -Sqrt(Abs(1.0f - r_out_perp.SqrLength())) * n;
    return r_out_perp + r_out_parallel;
}

static inline float
DegToRad(float deg) {
    return deg * (float)(PI/180.0f);
}

static inline float
Sin(float a) {
    return sin(a);
}

static inline float
Cos(float a) {
    return cos(a);
}

static inline float
Tan(float a) {
    return tan(a);
}

static inline float
SmoothStep(float edge0, float edge1, float x) {
   if (x < edge0)
      return 0;
   if (x >= edge1)
      return 1;
   x = (x - edge0) / (edge1 - edge0);
   return x * x * (3 - 2 * x);
}

static inline float
Lerp(float A, float B, float T)
{
    return A + T*(B-A);
}
