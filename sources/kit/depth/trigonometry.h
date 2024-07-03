#pragma once
#include <array>

namespace kit {

std::array<std::array<float,3>,3> inverseMatr(const std::array<std::array<float,3>,3> &inp);
std::array<std::array<float,3>,3> identityMatr3x3();
std::array<std::array<float,4>,4> identityMatr4x4();
float angleDirPlane(const std::array<float, 3> &dir, const std::array<float, 3> &plane);
std::array<float, 3> crossProduct(const std::array<float, 3> &vec1, const std::array<float, 3> &vec2);
std::array<float, 3> norm(const std::array<float, 3> &vec);
std::array<float, 3> operator - (const std::array<float, 3> &a);
std::array<float, 3> operator - (const std::array<float, 3> &a, const std::array<float, 3> &b);
std::array<float, 3> operator + (const std::array<float, 3> &a);
std::array<float, 3> operator + (const std::array<float, 3> &a, const std::array<float, 3> &b);
std::array<float, 3> operator / (const std::array<float, 3> &a, const float b);
std::array<float, 3> operator * (const std::array<float, 3> &a, const float b);
std::array<float, 3> operator * (const std::array<std::array<float,3>,3> &m, const std::array<float,3> &v);
std::array<float, 4> operator * (const std::array<std::array<float, 4>, 4> &a, const std::array<float, 4> &b);
std::array<std::array<float,3>,3> operator *(const std::array<std::array<float,3>,3> &a, const std::array<std::array<float,3>,3> &b);
std::array<std::array<float,4>,4> operator *(const std::array<std::array<float,4>,4> &a, const std::array<std::array<float,4>,4> &b);
}
