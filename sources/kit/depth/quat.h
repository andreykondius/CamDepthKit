#pragma once
#include <array>

namespace kit {
class TQuat
{
public:
    TQuat();
    TQuat(float x, float y, float z, float w);
    TQuat &operator = (const TQuat& q);
    TQuat &operator += (const TQuat& q);
    TQuat &operator -= (const TQuat& q);
    TQuat &operator *= (const TQuat& q);
    TQuat &operator *= (const float& scalar);;
    TQuat &operator /= (const float& scalar);;
    TQuat operator + (const TQuat& q);
    TQuat operator - (const TQuat& q);
    TQuat operator * (const TQuat& q);
    TQuat operator * (const float& scalar);
    TQuat operator / (const float& scalar);
    float magnitude() const;
    TQuat & angle_to_quat(const std::array<float, 3> &rot);
    TQuat normal();
    TQuat conjugate() const;
    TQuat inverse() const;
    void quat_rot( float &angle, std::array<float, 3> &vec);
    void rot_quat( const float &angle,const std::array<float, 3> &vec );
    std::array<float, 16> quatTo16Mat();
    std::array<std::array<float,3>,3> quatTo3x3Mat();

private:
    float x,y,z,w;
};

}
