#include "trigonometry.h"
#include <math.h>

namespace kit {

std::array<std::array<float, 3>, 3> inverseMatr(const std::array<std::array<float, 3>, 3> &inp)
{
    std::array<std::array<float,3>,3> res;
    // calc of determinant for inp
    float d = inp[0][0]*inp[1][1]*inp[2][2] + inp[0][1]*inp[1][2]*inp[2][0] + inp[0][2]*inp[1][0]*inp[2][1] -
              inp[0][2]*inp[1][1]*inp[2][0] - inp[0][0]*inp[1][2]*inp[2][1] - inp[0][1]*inp[1][0]*inp[2][2];
    if (d == 0)
        return res;
    // calc of inverse matrix for inp
    float m11 = inp[1][1]*inp[2][2] - inp[1][2]*inp[2][1];    // minor 11
    float a11 = m11;    // algebraic complement
    float m12 = inp[1][0]*inp[2][2] - inp[1][2]*inp[2][0];    // minor 12
    float a12 = -m12;    // algebraic complement 12
    float m13 = inp[1][0]*inp[2][1] - inp[1][1]*inp[2][0];    // minor 13
    float a13 = m13;    // algebraic complement 12
    float m21 = inp[0][1]*inp[2][2] - inp[0][2]*inp[2][1];    // minor 21
    float a21 = -m21;    // algebraic complement 21
    float m22 = inp[0][0]*inp[2][2] - inp[0][2]*inp[2][0];    // minor 22
    float a22 = m22;    // algebraic complement 22
    float m23 = inp[0][0]*inp[2][1] - inp[0][1]*inp[2][0];    // minor 23
    float a23 = -m23;    // algebraic complement 23
    float m31 = inp[0][1]*inp[1][2] - inp[0][2]*inp[1][1];    // minor 31
    float a31 = m31;    // algebraic complement 31
    float m32 = inp[0][0]*inp[1][2] - inp[0][2]*inp[1][0];    // minor 32
    float a32 = -m32;    // algebraic complement 32
    float m33 = inp[0][0]*inp[1][1] - inp[0][1]*inp[1][0];    // minor 33
    float a33 = m33;    // algebraic complement 33
    res[0][0] = a11; res[0][1] = a21; res[0][2] = a31;
    res[1][0] = a12; res[1][1] = a22; res[1][2] = a32;
    res[2][0] = a13; res[2][1] = a23; res[2][2] = a33;
    for (auto i=0; i < 3; ++i)
        for (auto j=0; j < 3; ++j)
            res[i][j] /= d;
    return res;
}

float angleDirPlane(const std::array<float, 3> &dir, const std::array<float, 3> &plane)
{
    return asin(fabs(dir[0]*plane[0]+dir[1]*plane[1]+dir[2]*plane[2])/
            (sqrtf(dir[0]*dir[0]+dir[1]*dir[1]+dir[2]*dir[2])*(sqrtf(plane[0]*plane[0]+plane[1]*plane[1]+plane[2]*plane[2]))));
}

std::array<float, 3> crossProduct(const std::array<float, 3> &vec1, const std::array<float, 3> &vec2)
{
    return {vec1[1]*vec2[2] - vec1[2]*vec2[1], vec1[2]*vec2[0] - vec1[0]*vec2[2], vec1[0]*vec2[1] - vec1[1]*vec2[0]};
}

std::array<float, 3> norm(const std::array<float, 3> &vec)
{
    float len = sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
    return {vec[0]/len, vec[1]/len, vec[2]/len};
}

std::array<float, 3> operator * (const std::array<std::array<float, 3>, 3> &m, const std::array<float, 3> &v)
{
    std::array<float, 3> res;
    for (auto i = 0; i < 3; ++i) {
        float t = 0.0;
        for (auto j = 0; j < 3; ++j)
            t += m[i][j]*v[j];
        res[i] = t;
    }
    return res;
}

std::array<std::array<float, 3>, 3> operator *(const std::array<std::array<float, 3>, 3> &a, const std::array<std::array<float, 3>, 3> &b)
{
    std::array<std::array<float,3>,3> res;
    res[0][0] = a[0][0]*b[0][0] + a[0][1]*b[1][0] + a[0][2]*b[2][0];
    res[0][1] = a[0][0]*b[0][1] + a[0][1]*b[1][1] + a[0][2]*b[2][1];
    res[0][2] = a[0][0]*b[0][2] + a[0][1]*b[1][2] + a[0][2]*b[2][2];
    res[1][0] = a[1][0]*b[0][0] + a[1][1]*b[1][0] + a[1][2]*b[2][0];
    res[1][1] = a[1][0]*b[0][1] + a[1][1]*b[1][1] + a[1][2]*b[2][1];
    res[1][2] = a[1][0]*b[0][2] + a[1][1]*b[1][2] + a[1][2]*b[2][2];
    res[2][0] = a[2][0]*b[0][0] + a[2][1]*b[1][0] + a[2][2]*b[2][0];
    res[2][1] = a[2][0]*b[0][1] + a[2][1]*b[1][1] + a[2][2]*b[2][1];
    res[2][2] = a[2][0]*b[0][2] + a[2][1]*b[1][2] + a[2][2]*b[2][2];
    return res;
}

std::array<std::array<float, 4>, 4> operator *(const std::array<std::array<float, 4>, 4> &a, const std::array<std::array<float, 4>, 4> &b)
{
    std::array<std::array<float,4>,4> res;
    for (auto i=0; i < 4; ++i) {
        for (auto j=0; j < 4; ++j) {
            res[i][j] = 0.0;
            for (auto k=0; k < 4; ++k)
                res[i][j] += a[i][k]*b[k][j];
        }
    }
    return res;
}

std::array<std::array<float, 3>, 3> identityMatr3x3()
{
    std::array<std::array<float,3>,3> res;
    for (auto i = 0; i < 3; ++i) {
        for (auto j = 0; j < 3; ++j) {
            res[i][j] = 0.0;
            if (i == j)
                res[i][j] = 1.0;
        }
    }
    return res;
}

std::array<std::array<float, 4>, 4> identityMatr4x4()
{
    std::array<std::array<float,4>,4> res;
    for (auto i = 0; i < 4; ++i) {
        for (auto j = 0; j < 4; ++j) {
            res[i][j] = 0.0;
            if (i == j)
                res[i][j] = 1.0;
        }
    }
    return res;
}

std::array<float, 3> operator -(const std::array<float, 3> &a)
{
    std::array<float, 3> res;
    for (auto i=0; i<3; i++)
        res[i] = -a[i];
    return res;
}

std::array<float, 3> operator -(const std::array<float, 3> &a, const std::array<float, 3> &b)
{
    std::array<float, 3> res;
    for (auto i=0; i<3; i++)
        res[i] = a[i] - b[i];
    return res;
}

std::array<float, 3> operator +(const std::array<float, 3> &a)
{
    std::array<float, 3> res;
    for (auto i=0; i<3; i++)
        res[i] = +a[i];
    return res;
}

std::array<float, 3> operator +(const std::array<float, 3> &a, const std::array<float, 3> &b)
{
    std::array<float, 3> res;
    for (auto i=0; i<3; i++)
        res[i] = a[i] + b[i];
    return res;
}

std::array<float, 3> operator /(const std::array<float, 3> &a, const float b)
{
    std::array<float, 3> res;
    for (auto i=0; i<3; i++)
        res[i] = a[i] / b;
    return res;
}

std::array<float, 3> operator *(const std::array<float, 3> &a, const float b)
{
    std::array<float, 3> res;
    for (auto i=0; i<3; i++)
        res[i] = a[i] * b;
    return res;
}

std::array<float, 4> operator *(const std::array<std::array<float, 4>, 4> &m, const std::array<float, 4> &v)
{
    std::array<float,4> res = {0, 0, 0, 0};
    for (auto i=0; i < 4; ++i) {
        for (auto j=0; j < 4; ++j)
            res[i] += m[i][j]*v[j];
    }
    return res;
}


}
