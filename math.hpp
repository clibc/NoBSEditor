#pragma once

#include <math.h>
#include <cmath>
#include <stdlib.h> // for rand()

#pragma warning(push)
#pragma warning(disable : 4201)

#define PI  3.14159265359
#define TAU 6.28318530718

static inline float PowerF32(float, float);

union v2
{
    struct
    {
        float x,y;
    };

    struct
    {
        float u,v;
    };
    
    struct
    {
        float Width, Height;
    };

    float Elements[2];

    v2() = default;
    inline
    v2(float ix, float iy)
    {
        x = ix;
        y = iy;
    }
    inline
    v2(float a)
    {
        x = a;
        y = a;
    }
};

union v3
{
    struct
    {
        float x,y,z;
    };

    struct
    {
        float r,g,b;
    };

    struct
    {
        union
        {
            v2 xy;
            v2 rg;
        };
        float Ignored;
    };

    struct
    {
        float Ignored;
        union
        {
            v2 yz;
            v2 gb;
        };
    };
    
    float Elements[3];

    v3() = default;
    inline
    v3(float ix, float iy, float iz)
    {
        x = ix;
        y = iy;
        z = iz;
    }
    inline
    v3(float a)
    {
        x = a;
        y = a;
        z = a;
    }
};

union v4
{
    struct
    {
        float x,y,z,w;
    };

    struct
    {
        float r,g,b,a;
    };

    struct
    {
        union
        {
            v3 xyz;
            v3 rgb;
        };
        float Ignored;
    };
    
    struct
    {
        union
        {
            v2 xy;
            v2 rg;
        };
        float Ignored0;
        float Ignored1;
    };

    float Elements[4];

    v4() = default;
    inline
    v4(float ix, float iy, float iz, float iw)
    {
        x = ix;
        y = iy;
        z = iz;
        w = iw;
    }
    inline
    v4(v3 i, float iw)
    {
        x = i.x;
        y = i.y;
        z = i.z;
        w = iw;
    }
    inline
    v4(float a)
    {
        x = a;
        y = a;
        z = a;
        w = a;
    }
};

// TODO : Fix this m4
// m0 m4 m8  m12
// m1 m5 m9  m13
// m2 m6 m10 m14
// m3 m7 m11 m15
// matrix4x4
struct m4
{ 
    float Elements[16];
    inline float& operator[](const int Index) {return Elements[Index];}

    inline void
    operator=(m4 const& R)
    {
        Elements[0]  = R.Elements[0];
        Elements[1]  = R.Elements[1];
        Elements[2]  = R.Elements[2];
        Elements[3]  = R.Elements[3];
        Elements[4]  = R.Elements[4];
        Elements[5]  = R.Elements[5];
        Elements[6]  = R.Elements[6];
        Elements[7]  = R.Elements[7];
        Elements[8]  = R.Elements[8];
        Elements[9]  = R.Elements[9];
        Elements[10] = R.Elements[10];
        Elements[11] = R.Elements[11];
        Elements[12] = R.Elements[12];
        Elements[13] = R.Elements[13];
        Elements[14] = R.Elements[14];
        Elements[15] = R.Elements[15];
    }

    inline m4 Identity(float a = 1.0f)
    {
        m4 ret = {0};
        ret[0 * 4 + 0] = a;
        ret[1 * 4 + 1] = a;
        ret[2 * 4 + 2] = a;
        ret[3 * 4 + 3] = a;
        return ret;
    }

    inline void
    SetRow(int row, float a1, float a2, float a3, float a4)
    {
        Elements[row + 0 * 4] = a1;
        Elements[row + 1 * 4] = a2;
        Elements[row + 2 * 4] = a3;
        Elements[row + 3 * 4] = a4;
    }

    inline void
    SetColumn(int column, float a1, float a2, float a3, float a4)
    {
        Elements[column * 4 + 0] = a1;
        Elements[column * 4 + 1] = a2;
        Elements[column * 4 + 2] = a3;
        Elements[column * 4 + 3] = a4;
    }
};

inline v2 operator+(v2 L, v2 R)    {return {L.x + R.x, L.y + R.y};}
inline v2 operator-(v2 L, v2 R)    {return {L.x - R.x, L.y - R.y};}
inline v2 operator+(v2 L, float R) {return {L.x + R, L.y + R};}
inline v2 operator-(v2 L, float R) {return {L.x - R, L.y - R};}
inline v2 operator*(float L, v2 R) {return {L * R.x, L * R.y};}
inline v2 operator*(v2 L, float R) {return {L.x * R, L.y * R};}
inline v2 operator/(float L, v2 R) {return {L / R.x, L / R.y};}
inline v2 operator/(v2 L, float R) {return {L.x / R, L.y / R};}
inline v2 operator*(v2 L, v2 R)    {return {L.x * R.x, L.y * R.y};}
inline v2& operator+=(v2& L, float R) {return (L = L + R);}
inline v2& operator+=(v2& L, v2 R)    {return (L = L + R);}
inline v2& operator-=(v2& L, float R) {return (L = L - R);}
inline v2& operator-=(v2& L, v2 R)    {return (L = L - R);}
inline v2& operator*=(v2& L, float R) {return (L = L * R);}
inline v2& operator/=(v2& L, float R) {return (L = L / R);}

inline v3 operator+(v3 L, v3 R)    {return {L.x + R.x, L.y + R.y, L.z + R.z};}
inline v3 operator-(v3 L, v3 R)    {return {L.x - R.x, L.y - R.y, L.z - R.z};}
inline v3 operator+(v3 L, float R) {return {L.x + R, L.y + R, L.z + R};}
inline v3 operator-(v3 L, float R) {return {L.x - R, L.y - R, L.z - R};}
inline v3 operator*(float L, v3 R) {return {L * R.x, L * R.y, L * R.z};}
inline v3 operator*(v3 L, float R) {return {L.x * R, L.y * R, L.z * R};}
inline v3 operator/(float L, v3 R) {return {L / R.x, L / R.y, L / R.z};}
inline v3 operator/(v3 L, float R) {return {L.x / R, L.y / R, L.z / R};}
inline v3 operator*(v3 L, v3 R)    {return {L.x * R.x, L.y * R.y, L.z * R.z};}
inline v3& operator+=(v3& L, float R) {return (L = L + R);}
inline v3& operator+=(v3& L, v3 R)    {return (L = L + R);}
inline v3& operator-=(v3& L, float R) {return (L = L - R);}
inline v3& operator-=(v3& L, v3 R)    {return (L = L - R);}
inline v3& operator*=(v3& L, float R) {return (L = L * R);}
inline v3& operator/=(v3& L, float R) {return (L = L / R);}

inline v4 operator+(v4 L, v4 R)    {return {L.x + R.x, L.y + R.y, L.z + R.z, L.w + R.w};}
inline v4 operator-(v4 L, v4 R)    {return {L.x - R.x, L.y - R.y, L.z - R.z, L.w - R.w};}
inline v4 operator+(v4 L, float R) {return {L.x + R, L.y + R, L.z + R, L.w + R};}
inline v4 operator-(v4 L, float R) {return {L.x - R, L.y - R, L.z - R, L.w - R};}
inline v4 operator*(float L, v4 R) {return {L * R.x, L * R.y, L * R.z, L * R.w};}
inline v4 operator*(v4 L, float R) {return {L.x * R, L.y * R, L.z * R, L.w * R};}
inline v4 operator/(float L, v4 R) {return {L / R.x, L / R.y, L / R.z, L / R.w};}
inline v4 operator/(v4 L, float R) {return {L.x / R, L.y / R, L.z / R, L.w / R};}
inline v4 operator*(v4 L, v4 R)    {return {L.x * R.x, L.y * R.y, L.z * R.z, L.w * R.w};}
inline v4& operator+=(v4& L, float R) {return (L = L + R);}
inline v4& operator+=(v4& L, v4 R)    {return (L = L + R);}
inline v4& operator-=(v4& L, float R) {return (L = L - R);}
inline v4& operator-=(v4& L, v4 R)    {return (L = L - R);}
inline v4& operator*=(v4& L, float R) {return (L = L * R);}
inline v4& operator/=(v4& L, float R) {return (L = L / R);}

inline m4 operator*(m4& L, m4 const& a)
{
    m4 res = {};
    float* o1 = &(L.Elements[0]);
    const float* o2 = &(a.Elements[0]);

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

inline v4
operator*(m4& o, v4 const& a)
{
    v4 res = {};
    
    res.x = o[0] * a.x + o[1] * a.y + o[2]  * a.z + o[3]  * a.w;
    res.y = o[4] * a.x + o[5] * a.y + o[6]  * a.z + o[7]  * a.w;
    res.z = o[8] * a.x + o[9] * a.y + o[10] * a.z + o[11] * a.w;
    res.w = o[12] * a.x + o[13] * a.y + o[14] * a.z + o[15] * a.w;

    return res;
}

// https://semath.info/src/inverse-cofactor-ex4.html
inline m4
Inverse(m4& A)
{
    float* m = (float*)&(A[0]);
    // calculate co-factors and determinant
    float determinant = 0.0f;
    float cofactors[16];
    for(int i = 0; i < 16; ++i)
    {
        float v[9];
        int vcounter = 0;
        int col = i % 4;
        int row = i / 4;
        for(int j = 0; j < 16; ++j)
        {
            int scol = j % 4;
            int srow = j / 4;
            if(scol != row && srow != col)
            { // we flip row and column
                v[vcounter++] = m[j];
            };
        }
        float det = v[0] * v[4] * v[8] + v[3] * v[7] * v[2] + v[1] * v[5] * v[6] - v[6] * v[4] * v[2] - v[1] * v[3] * v[8] - v[7] * v[5] * v[0];
        cofactors[i] = PowerF32(-1.0f, (float)(col + row + 2)) * det;

        // so this is not same with the article I read
        // https://semath.info/src/determinant-four-by-four.html
        if(i < 4)
        {
            if(i == 0) determinant = A.Elements[i * 4] * det;
            else
            {
                if((i % 2) == 0) determinant += A.Elements[i * 4] * det;
                else determinant -= A.Elements[i * 4] * det;
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
    for(int i = 0; i < 16; ++i)
    {
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

inline void
Print(v2& A)
{
#ifdef DebugLog
    DebugLog("Vec2(%f, %f)\n", A.x, A.y);
#endif
}

inline void
Print(v3& A)
{
#ifdef DebugLog
    DebugLog("Vec3(%f, %f, %f)\n", A.x, A.y, A.z);
#endif
}

inline void
Print(v4& A)
{
#ifdef DebugLog
    DebugLog("Vec4(%f, %f, %f, %f)\n", A.x, A.y, A.z, A.w);
#endif
}

inline void
Print(m4& A)
{
#ifdef DebugLog
    DebugLog("\nRow major:\n");
    DebugLog("(%f, %f, %f, %f)\n", A.Elements[0], A.Elements[4], A.Elements[8],  A.Elements[12]); 
    DebugLog("(%f, %f, %f, %f)\n", A.Elements[1], A.Elements[5], A.Elements[9],  A.Elements[13]); 
    DebugLog("(%f, %f, %f, %f)\n", A.Elements[2], A.Elements[6], A.Elements[10], A.Elements[14]); 
    DebugLog("(%f, %f, %f, %f)\n", A.Elements[3], A.Elements[7], A.Elements[11], A.Elements[15]); 
    DebugLog("Column major:\n");
    DebugLog("(%f, %f, %f, %f)\n", A.Elements[0], A.Elements[1], A.Elements[2],  A.Elements[3]); 
    DebugLog("(%f, %f, %f, %f)\n", A.Elements[4], A.Elements[5], A.Elements[6],  A.Elements[7]); 
    DebugLog("(%f, %f, %f, %f)\n", A.Elements[8], A.Elements[9], A.Elements[10], A.Elements[11]); 
    DebugLog("(%f, %f, %f, %f)\n", A.Elements[12], A.Elements[13], A.Elements[14], A.Elements[15]); 
    DebugLog("\n");
#endif
}

inline float
Dot(v2 a, v2 b)
{
    return a.x * b.x + a.y * b.y;
}

inline float
Dot(v3 a, v3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float
Dot(v4 a, v4 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w + b.w;
}

inline float
Length(v2 a)
{
    return sqrtf(Dot(a,a));
}

inline float
Length(v3 a)
{
    return sqrtf(Dot(a,a));
}

inline float
Length(v4 a)
{
    return sqrtf(Dot(a,a));
}

inline float
SqrLength(v2 a)
{
    return Dot(a,a);
}

inline float
SqrLength(v3 a)
{
    return Dot(a,a);
}

inline float
SqrLength(v4 a)
{
    return Dot(a,a);
}

inline v3
Cross(v3 a, v3 b)
{
    v3 r;
    r.x = a.y * b.z - a.z * b.y;
    r.y = a.z * b.x - a.x * b.z;
    r.z = a.x * b.y - a.y * b.x;
    return r;
}

inline v2
Normalize(v2 a)
{
    return a / Length(a);
}

inline v3
Normalize(v3 a)
{
    return a / Length(a);
}

inline v4
Normalize(v4 a)
{
    return a / Length(a);
}

inline float
Sqrt(float a)
{
    return sqrtf(a);
}

inline float
Rand01()
{
    return (float)rand() / (float)RAND_MAX; 
}

inline v3
RandomInUnitSphere()
{
    for(;;)
    {
        v3 p = v3(Rand01() * 2 - 1, Rand01() * 2 - 1, Rand01() * 2 - 1);
        if(SqrLength(p) >= 1) continue;
        return p;
    }
}

inline v3
RandomInHemiSphere(v3 normal)
{
    v3 rand = RandomInUnitSphere();
    return (Dot(normal, rand) > 0) ? rand : rand * -1;
}

inline v3
RandomInUnitDisk()
{
    for(;;)
    {
        v3 p = v3(Rand01()*2-1, Rand01()*2-1, 0);
        if (SqrLength(p) >= 1) continue;
        return p;
    }
}

inline float
Abs(float a)
{
    return fabs(a);
}

inline float
Min(float a, float b)
{
    return a < b ? a : b;
}

inline float
Max(float a, float b)
{
    return a > b ? a : b;
}

inline int
Max(int a, int b)
{
    return a > b ? a : b;
}

inline int
Min(int a, int b)
{
    return a < b ? a : b;
}

inline float
PowerF32(float x, float p)
{
    return pow(x, p);
}

inline v3
Reflect(v3 d, v3 n)
{
    return d - 2*Dot(d, n)*n;
}

inline v3
Refract(v3 uv, v3 n, float etai_over_etat)
{
    float cos_theta = Min(Dot(uv * -1, n), 1.0);
    v3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    v3 r_out_parallel = -Sqrt(Abs(1.0f - SqrLength(r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}

inline float
DegToRad(float deg)
{
    return deg * (float)(PI/180.0f);
}

inline float
Sin(float a)
{
    return sin(a);
}

inline float
Cos(float a)
{
    return cos(a);
}

inline float
Tan(float a)
{
    return tan(a);
}

inline float
SmoothStep(float edge0, float edge1, float x)
{
   if (x < edge0)
      return 0;
   if (x >= edge1)
      return 1;
   x = (x - edge0) / (edge1 - edge0);
   return x * x * (3 - 2 * x);
}

inline int
TruncateF32ToS32(float Value)
{
    return (int)Value;
}

inline int
RoundF32ToS32(float Value)
{
    return (int)(Value + 0.5f);
}

inline float
Clamp(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline int
Clamp(int x, int min, int max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

static inline float
Lerp(float A, float B, float T)
{
    return A + T*(B-A);
}

#pragma warning(pop)
