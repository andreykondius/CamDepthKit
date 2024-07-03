#include "quat.h"

namespace kit {

TQuat::TQuat()
    : x(0.0), y(0.0), z(0.0), w(0.0)
{

}

TQuat::TQuat(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w)
{

}

TQuat &TQuat::operator =(const TQuat &q)
{
    if (this == &q)
        return *this;
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
    return *this;
}

TQuat &TQuat::operator +=(const TQuat &q)
{
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return *this;
}

TQuat &TQuat::operator -=(const TQuat &q)
{
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
    return *this;
}

TQuat &TQuat::operator *=(const TQuat &q)
{
    // Multiply so that rotations are applied in a left to right order.
    TQuat tmp;
    tmp.w = (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z);
    tmp.x = (w * q.x) + (x * q.w) - (y * q.z) + (z * q.y);
    tmp.y = (w * q.y) + (x * q.z) + (y * q.w) - (z * q.x);
    tmp.z = (w * q.z) - (x * q.y) + (y * q.x) + (z * q.w);

    // Multiply so that rotations are applied in a right to left order.
    //		TQuat tmp;
    //	    tmp.w = (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z);
    //		tmp.x = (w * q.x) + (x * q.w) + (y * q.z) - (z * q.y);
    //		tmp.y = (w * q.y) - (x * q.z) + (y * q.w) + (z * q.x);
    //		tmp.z = (w * q.z) + (x * q.y) - (y * q.x) + (z * q.w);

    *this = tmp;
    return *this;
}

TQuat &TQuat::operator /=(const float &scalar)
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
}

TQuat TQuat::operator +(const TQuat &q)
{
    TQuat tmp(*this);
    tmp += q;
    return tmp;
}

TQuat TQuat::operator -(const TQuat &q)
{
    TQuat tmp(*this);
    tmp -= q;
    return tmp;
}

TQuat TQuat::operator *(const TQuat &q)
{
    TQuat tmp(*this);
    tmp *= q;
    return tmp;
}

TQuat TQuat::operator /(const float &scalar)
{
    TQuat tmp(*this);
    tmp /= scalar;
    return tmp;
}

float TQuat::magnitude() const		// длина кватерниона
{
    return sqrtf(x*x + y*y + z*z + w*w);
}

TQuat &TQuat::angle_to_quat(const std::array<float, 3> &rot)
{
    float cosH = cosf(rot[0]);
    float cosP = cosf(rot[1]);
    float cosR = cosf(rot[2]);
    float sinH = sinf(rot[0]);
    float sinP = sinf(rot[1]);
    float sinR = sinf(rot[2]);
    std::array<float, 16> m;
    m[0] = cosR * cosH - sinR * sinP * sinH;
    m[1] = sinR * cosH + cosR * sinP * sinH;
    m[2] = -cosP * sinH;
    m[3] = 0.0f;

    m[4] = -sinR * cosP;
    m[5] = cosR * cosP;
    m[6] = sinP;
    m[7] = 0.0f;

    m[8] = cosR * sinH + sinR * sinP * cosH;
    m[9] = sinR * sinH - cosR * sinP * cosH;
    m[10] = cosP * cosH;
    m[11] = 0.0f;

    m[12] = m[13] = m[14] = 0.0;
    m[15] = 1.0f;


    float s = 0.0f;
    float q[4] = {0.0f};
    float trace = m[0] + m[5] + m[10];

    if (trace > 0.0f)
    {
        s = sqrtf(trace + 1.0f);
        q[3] = s * 0.5f;
        s = 0.5f / s;
        q[0] = (m[6] - m[9]) * s;
        q[1] = (m[8] - m[2]) * s;
        q[2] = (m[1] - m[4]) * s;
    }
    else
    {
        int nxt[3] = {1, 2, 0};
        int i = 0, j = 0, k = 0;

        if (m[1*4+1] > m[0])
            i = 1;

        if (m[2*4+2] > m[i*4+i])
            i = 2;

        j = nxt[i];
        k = nxt[j];
        s = sqrtf((m[i*4+i] - (m[j*4+j] + m[k*4+k])) + 1.0f);

        q[i] = s * 0.5f;
        s = 0.5f / s;
        q[3] = (m[j*4+k] - m[k*4+j]) * s;
        q[j] = (m[i*4+j] + m[j*4+i]) * s;
        q[k] = (m[i*4+k] + m[k*4+i]) * s;
    }

    x = q[0], y = q[1], z = q[2], w = q[3];

    return *this;
}

TQuat TQuat::normal()					// нормаль кватерниона
{
    float m = 1/magnitude();
    if (m == 0.0)
        m = 1.0;
    x *= m;
    y *= m;
    z *= m;
    w *= m;
    return *this;
}

TQuat TQuat::conjugate() const
{
    TQuat tmp(w, -x, -y, -z);
    return tmp;
}

TQuat TQuat::inverse() const
{
    float length = (1.0f / magnitude());
    return conjugate() * length;
}

void TQuat::quat_rot(float &angle, std::array<float, 3> &vec) // перевод кватерниона в ось вращения и угол
{
    TQuat Q;
    Q.x = x; Q.y = y; Q.z = z; Q.w = w;
    Q.normal();
    float cos_angle  = Q.w;
    angle      = acos( cos_angle );// * 2 * RAD;
    float sin_angle  = sqrt( 1.0 - cos_angle * cos_angle );

    if ( fabs( sin_angle ) < 0.0005 )
        sin_angle = 1;

    vec[0] = Q.x / sin_angle;
    vec[1] = Q.y / sin_angle;
    vec[2] = Q.z / sin_angle;
}

void TQuat::rot_quat(const float &angle, const std::array<float, 3> &vec)	// перевод оси вращения и угла в кватернион
{
    float sinus = (float)sinf(angle);
    float cosin = (float)cosf(angle);

    x += vec[0]*sinus;
    y += vec[1]*sinus;
    z += vec[2]*sinus;
    w += cosin;
}

std::array<float, 16> TQuat::quatTo16Mat()
{
    // Converts this quaternion to a rotation matrix.
    //
    //  | 1 - 2(y^2 + z^2)	2(xy + wz)			2(xz - wy)			0  |
    //  | 2(xy - wz)		1 - 2(x^2 + z^2)	2(yz + wx)			0  |
    //  | 2(xz + wy)		2(yz - wx)			1 - 2(x^2 + y^2)	0  |
    //  | 0					0					0					1  |

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    float xx = x * x2;
    float xy = x * y2;
    float xz = x * z2;
    float yy = y * y2;
    float yz = y * z2;
    float zz = z * z2;
    float wx = w * x2;
    float wy = w * y2;
    float wz = w * z2;

    std::array<float, 16> m;

    m[0] = 1.0f - (yy + zz);
    m[1] = xy + wz;
    m[2] = xz - wy;
    m[3] = 0.0f;

    m[4] = xy - wz;
    m[5] = 1.0f - (xx + zz);
    m[6] = yz + wx;
    m[7] = 0.0f;

    m[8] = xz + wy;
    m[9] = yz - wx;
    m[10] = 1.0f - (xx + yy);
    m[11] = 0.0f;

    m[12] = m[13] = m[14] = 0.0f;
    m[15] = 1.0f;

    return m;
}

std::array<std::array<float, 3>, 3> TQuat::quatTo3x3Mat()
{
    // Converts this quaternion to a rotation matrix.
    //
    //  | 1 - 2(y^2 + z^2)	2(xy + wz)			2(xz - wy)			0  |
    //  | 2(xy - wz)		1 - 2(x^2 + z^2)	2(yz + wx)			0  |
    //  | 2(xz + wy)		2(yz - wx)			1 - 2(x^2 + y^2)	0  |
    //  | 0					0					0					1  |

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    float xx = x * x2;
    float xy = x * y2;
    float xz = x * z2;
    float yy = y * y2;
    float yz = y * z2;
    float zz = z * z2;
    float wx = w * x2;
    float wy = w * y2;
    float wz = w * z2;

    std::array<std::array<float, 3>, 3> res;

    res[0][0] = 1.0f - (yy + zz);
    res[0][1] = xy + wz;
    res[0][2] = xz - wy;

    res[1][0] = xy - wz;
    res[1][1] = 1.0f - (xx + zz);
    res[1][2] = yz + wx;

    res[2][0] = xz + wy;
    res[2][1] = yz - wx;
    res[2][2] = 1.0f - (xx + yy);

    return res;
}

TQuat TQuat::operator *(const float &scalar)
{
    TQuat tmp(*this);
    tmp *= scalar;
    return tmp;
}

TQuat &TQuat::operator *=(const float &scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

}
