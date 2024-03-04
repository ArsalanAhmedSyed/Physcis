#pragma once
#define _USE_MATH_DEFINES

#include <math.h>
//------------------------------------------------------------------------
// Vector3 Class and Vector3 functions
//------------------------------------------------------------------------

//Minimum tolerance before treating the value as 0
float const tol = 0.0001f;

class Vector3 {
public:
    float x;
    float y;
    float z;

    Vector3(void);
    Vector3(float xi, float yi, float zi);

    float Magnitude(void);
    void  Normalize(void);
    void  Reverse(void);

    Vector3& operator+=(Vector3 u);
    Vector3& operator-=(Vector3 u);
    Vector3& operator*=(float s);
    Vector3& operator/=(float s);
    Vector3 operator-(void);
};

// Default Constructor
inline Vector3::Vector3(void)
{
    x = 0;
    y = 0;
    z = 0;
}

// Constructor
inline Vector3::Vector3(float xi, float yi, float zi)
{
    x = xi;
    y = yi;
    z = zi;
}

inline float Vector3::Magnitude(void)
{
    return (float)sqrt(x * x + y * y + z * z);
}

inline void Vector3::Normalize(void)
{
    float m = (float)sqrt(x * x + y * y + z * z);
    if (m <= tol) m = 1;
    x /= m;
    y /= m;
    z /= m;

    if (fabs(x) < tol) x = 0.0f;
    if (fabs(y) < tol) y = 0.0f;
    if (fabs(z) < tol) z = 0.0f;
}


inline void Vector3::Reverse(void)
{
    x = -x;
    y = -y;
    z = -z;
}

inline Vector3& Vector3::operator+=(Vector3 u)
{
    x += u.x;
    y += u.y;
    z += u.z;
    return *this;
}

inline Vector3& Vector3::operator-=(Vector3 u)
{
    x -= u.x;
    y -= u.y;
    z -= u.z;
    return *this;
}

inline Vector3& Vector3::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

inline Vector3& Vector3::operator/=(float s)
{
    x /= s;
    y /= s;
    z /= s;
    return *this;
}

inline Vector3 Vector3::operator-(void)
{
    return Vector3(-x, -y, -z);
}

inline Vector3 operator+(Vector3 u, Vector3 v)
{
    return Vector3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline Vector3 operator-(Vector3 u, Vector3 v)
{
    return Vector3(u.x - v.x, u.y - v.y, u.z - v.z);
}

//Vector3 cross product
inline Vector3 operator^(Vector3 u, Vector3 v)
{
    return Vector3(u.y * v.z - u.z * v.y,
        -u.x * v.z + u.z * v.x,
        u.x * v.y - u.y * v.x);
}

// Vector3 dot product
inline float operator*(Vector3 u, Vector3 v)
{
    return (u.x * v.x + u.y * v.y + u.z * v.z);
}

inline Vector3 operator*(float s, Vector3 u)
{
    return Vector3(u.x * s, u.y * s, u.z * s);
}


inline Vector3 operator*(Vector3 u, float s)
{
    return Vector3(u.x * s, u.y * s, u.z * s);
}

inline Vector3 operator/(Vector3 u, float s)
{
    return Vector3(u.x / s, u.y / s, u.z / s);
}

inline float TripleScalarProduct(Vector3 u, Vector3 v, Vector3 w)
{
    return float((u.x * (v.y * w.z - v.z * w.y)) +
        (u.y * (-v.x * w.z + v.z * w.x)) +
        (u.z * (v.x * w.y - v.y * w.x)));
}