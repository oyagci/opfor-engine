#include <opfor/core/base.hpp>
#include "Mat4.hpp"
#include <opfor/core/types/Vec3.hpp>
#include <opfor/core/Math.hpp>
#include <opfor/core/types/Mat3.hpp>
#include <opfor/core/types/Quat.hpp>

namespace opfor
{

Mat4::Mat4(Mat3 const &m) : _values{m[0], m[1], m[2], Vec4(0.f, 0.f, 0.f, 1.f)}
{
}

Vec4 &Mat4::operator[](size_t idx)
{
    OP4_CORE_ASSERT(idx < sizeof(_values) / sizeof(*_values), "");
    return _values[idx];
}

Vec4 const &Mat4::operator[](size_t idx) const
{
    OP4_CORE_ASSERT(idx < sizeof(_values) / sizeof(*_values), "");
    return _values[idx];
}

Vec4 Mat4::operator*(Vec4 const &v) const
{
    Vec4 const mov0(v[0]);
    Vec4 const mov1(v[1]);
    Vec4 const mul0 = operator[](0) * mov0;
    Vec4 const mul1 = operator[](1) * mov1;
    Vec4 const add0 = mul0 + mul1;
    Vec4 const mov2(v[2]);
    Vec4 const mov3(v[3]);
    Vec4 const mul2 = operator[](2) * mov2;
    Vec4 const mul3 = operator[](3) * mov3;
    Vec4 const add1 = mul2 + mul3;
    Vec4 const add2 = add0 + add1;
    return add2;
}

Mat4 Mat4::Identity()
{
    return Mat4(1.0f);
}

Mat4 Mat4::Translate(Vec3 const &v)
{
    Mat4 m(1.f);
    Mat4 Result(1.f);
    Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
    return Result;
}

Mat4 Mat4::Scale(Vec3 const &v)
{
    Mat4 m(1.0f);
    Mat4 result(1.f);

    result[0] = m[0] * v[0];
    result[1] = m[1] * v[1];
    result[2] = m[2] * v[2];
    result[3] = m[3];

    return result;
}

Mat4 Mat4::Rotate(float angle, Vec3 const &v)
{
    float const a = angle;
    float const c = cos(a);
    float const s = sin(a);

    Vec3 axis(Vec3::Normalize(v));
    Vec3 temp((1.f - c) * axis);

    Mat4 rotate(1.f);
    rotate[0][0] = c + temp[0] * axis[0];
    rotate[0][1] = temp[0] * axis[1] + s * axis[2];
    rotate[0][2] = temp[0] * axis[2] - s * axis[1];

    rotate[1][0] = temp[1] * axis[0] - s * axis[2];
    rotate[1][1] = c + temp[1] * axis[1];
    rotate[1][2] = temp[1] * axis[2] + s * axis[0];

    rotate[2][0] = temp[2] * axis[0] + s * axis[1];
    rotate[2][1] = temp[2] * axis[1] - s * axis[0];
    rotate[2][2] = c + temp[2] * axis[2];

    return rotate;
}

Mat4 Mat4::Perspective(float fov, float aspect, float zNear, float zFar)
{
    const float tanHalfFov = math::Tan(fov / 2.f);

    return Mat4{
        {1.f / (aspect * tanHalfFov), 0.f, 0.f, 0.f},
        {0.f, 1.f / tanHalfFov, 0.f, 0.f},
        {0.f, 0.f, -((zFar + zNear) / (zFar - zNear)), -1.f},
        {0.f, 0.f, -((2.f * zFar * zNear) / (zFar - zNear)), 0.f},
    };
}

Mat4 Mat4::LookAt(Vec3 eyeWorldPos, Vec3 targetWorldPos, Vec3 up)
{
    Vec3 const f(Vec3::Normalize(targetWorldPos - eyeWorldPos));
    Vec3 const s(Vec3::Normalize(Vec3::Cross(f, up)));
    Vec3 const u(Vec3::Cross(s, f));

    Mat4 result(1.f);
    result[0][0] = s.x;
    result[1][0] = s.y;
    result[2][0] = s.z;
    result[0][1] = u.x;
    result[1][1] = u.y;
    result[2][1] = u.z;
    result[0][2] =-f.x;
    result[1][2] =-f.y;
    result[2][2] =-f.z;
    result[3][0] =-Vec3::Dot(s, eyeWorldPos);
    result[3][1] =-Vec3::Dot(u, eyeWorldPos);
    result[3][2] = Vec3::Dot(f, eyeWorldPos);

    return result;
}

Mat4 Mat4::Inverse(Mat4 const &m)
{
    float coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
    float coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
    float coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
    
    float coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
    float coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
    float coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
    
    float coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    float coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
    float coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
    
    float coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
    float coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
    float coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
    
    float coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    float coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
    float coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
    
    float coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    float coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    float coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

    Vec4 fac0(coef00, coef00, coef02, coef03);
    Vec4 fac1(coef04, coef04, coef06, coef07);
    Vec4 fac2(coef08, coef08, coef10, coef11);
    Vec4 fac3(coef12, coef12, coef14, coef15);
    Vec4 fac4(coef16, coef16, coef18, coef19);
    Vec4 fac5(coef20, coef20, coef22, coef23);

    Vec4 vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
    Vec4 vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
    Vec4 vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
    Vec4 vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

    Vec4 inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
    Vec4 inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
    Vec4 inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
    Vec4 inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

    Vec4 signA(+1, -1, +1, -1);
    Vec4 signB(-1, +1, -1, +1);
    Mat4 inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

    Vec4 row0(inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]);

    Vec4 dot0(m[0] * row0);
    float dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

    float oneOverDeterminant = 1.f / dot1;

    return inverse * oneOverDeterminant;
}

bool Mat4::Decompose(Mat4 const &model, Vec3 &scale, Quat &orientation, Vec3 &translation, Vec3 &skew, Vec4 &perspective)
{
    Mat4 localMatrix(model);

    if (math::Abs(localMatrix[3][3]) <= std::numeric_limits<float>::epsilon())
    {
        return false;
    }

    Mat4 perspectiveMatrix(localMatrix);

    for (size_t i = 0; i < 3; i++)
    {
        perspectiveMatrix[i][3] = 0.f;
    }
    perspectiveMatrix[3][3] = 1.f;

    if (localMatrix[0][3] >= std::numeric_limits<float>::epsilon() ||
        localMatrix[1][3] >= std::numeric_limits<float>::epsilon() ||
        localMatrix[2][3] >= std::numeric_limits<float>::epsilon())
    {
        // rightHandSide is the right hand side of the equation.
        Vec4 rightHandSide;
        rightHandSide[0] = localMatrix[0][3];
        rightHandSide[1] = localMatrix[1][3];
        rightHandSide[2] = localMatrix[2][3];
        rightHandSide[3] = localMatrix[3][3];

        // Solve the equation by inverting PerspectiveMatrix and multiplying
        // rightHandSide by the inverse.  (This is the easiest way, not
        // necessarily the best.)
        Mat4 inversePerspectiveMatrix = Inverse(perspectiveMatrix);//   inverse(PerspectiveMatrix, inversePerspectiveMatrix);
        Mat4 transposedInversePerspectiveMatrix = Transpose(inversePerspectiveMatrix);//   transposeMatrix4(inversePerspectiveMatrix, transposedInversePerspectiveMatrix);

        perspective = transposedInversePerspectiveMatrix * rightHandSide;
        //  v4MulPointByMatrix(rightHandSide, transposedInversePerspectiveMatrix, perspectivePoint);

        // Clear the perspective partition
        localMatrix[0][3] = 0.f;
        localMatrix[1][3] = 0.f;
        localMatrix[2][3] = 0.f;
        localMatrix[3][3] = 1.f;
    }
    else
    {
        // No perspective.
        perspective = Vec4(0.f, 0.f, 0.f, 1.f);
    }

    translation = Vec3(localMatrix[3].x, localMatrix[3].y, localMatrix[3].z);
    localMatrix[3] = Vec4(0, 0, 0, localMatrix[3].w);

    Vec3 row[3];
    Vec3 Pdum3;

    // Now get scale and shear.
    for(size_t i = 0; i < 3; ++i)
    for(size_t j = 0; j < 3; ++j)
        row[i][j] = localMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = row[0].Magnitude();// v3Length(Row[0]);
    row[0] = row[0].Scale(1.f);

    // Compute XY shear factor and make 2nd row orthogonal to 1st.
    skew.z = Vec3::Dot(row[0], row[1]);
    row[1] = row[1] * 1.f + row[0] * -skew.z;

    // Now, compute Y scale and normalize 2nd row.
    scale.y = row[1].Magnitude();
    row[1] = row[1].Scale(1.f);
    skew.z /= scale.y;

    // Compute XZ and YZ shears, orthogonalize 3rd row.
    skew.y = Vec3::Dot(row[0], row[2]);
    row[2] = row[2] * 1.f + row[0] * -skew.y;
    skew.x = Vec3::Dot(row[1], row[2]);
    row[2] = row[2] * 1.f + row[1] * -skew.x;

    // Next, get Z scale and normalize 3rd row.
    scale.z = row[2].Magnitude();
    row[2] = row[2].Scale(1.f);
    skew.y /= scale.z;
    skew.x /= scale.z;

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
    Pdum3 = Vec3::Cross(row[1], row[2]); // v3Cross(row[1], row[2], Pdum3);
    if(Vec3::Dot(row[0], Pdum3) < 0)
    {
        for(size_t i = 0; i < 3; i++)
        {
            scale[i] *= -1.f;
            row[i] *= -1.f;
        }
    }

    int i = 0;
    int j;
    int k = 0;

    float root = 0.f;
    float trace = row[0].x + row[1].y + row[2].z;
    if (trace > 0.f)
    {
        root = sqrt(trace + 1.0f);
        orientation.w = 0.5f * root;
        root = 0.5f / root;
        orientation.x = root * (row[1].z - row[2].y);
        orientation.y = root * (row[2].x - row[0].z);
        orientation.z = root * (row[0].y - row[1].x);
    } // End if > 0
    else
    {
        static int Next[3] = {1, 2, 0};
        i = 0;
        if(row[1].y > row[0].x) i = 1;
        if(row[2].z > row[i][i]) i = 2;
        j = Next[i];
        k = Next[j];

        root = sqrt(row[i][i] - row[j][j] - row[k][k] + 1.f);

        orientation[i] = 0.5f * root;
        root = 0.5f / root;
        orientation[j] = root * (row[i][j] + row[j][i]);
        orientation[k] = root * (row[i][k] + row[k][i]);
        orientation.w = root * (row[j][k] - row[k][j]);
    } // End if <= 0

    return true;
}

Mat4 Mat4::Transpose(Mat4 const &m)
{
    Mat4 result;
    result[0][0] = m[0][0];
    result[0][1] = m[1][0];
    result[0][2] = m[2][0];
    result[0][3] = m[3][0];

    result[1][0] = m[0][1];
    result[1][1] = m[1][1];
    result[1][2] = m[2][1];
    result[1][3] = m[3][1];

    result[2][0] = m[0][2];
    result[2][1] = m[1][2];
    result[2][2] = m[2][2];
    result[2][3] = m[3][2];

    result[3][0] = m[0][3];
    result[3][1] = m[1][3];
    result[3][2] = m[2][3];
    result[3][3] = m[3][3];
    return result;
}

Quat Mat4::AsQuaternion() const
{
    Mat4 const &m = *this;

    float fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
    float fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
    float fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
    float fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

    int biggestIndex = 0;
    float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
    {
        fourBiggestSquaredMinus1 = fourXSquaredMinus1;
        biggestIndex = 1;
    }
    if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
    {
        fourBiggestSquaredMinus1 = fourYSquaredMinus1;
        biggestIndex = 2;
    }
    if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
    {
        fourBiggestSquaredMinus1 = fourZSquaredMinus1;
        biggestIndex = 3;
    }

    float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.f) * 0.5f;
    float mult = 0.25f / biggestVal;

    switch (biggestIndex)
    {
    case 0:
        return Quat(biggestVal, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult,
                         (m[0][1] - m[1][0]) * mult);
    case 1:
        return Quat((m[1][2] - m[2][1]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult,
                         (m[2][0] + m[0][2]) * mult);
    case 2:
        return Quat((m[2][0] - m[0][2]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal,
                         (m[1][2] + m[2][1]) * mult);
    case 3:
        return Quat((m[0][1] - m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult,
                         biggestVal);
    default: // Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
        assert(false);
        return Quat(1, 0, 0, 0);
    }
}


}