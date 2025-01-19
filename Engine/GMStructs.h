//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMStructs.h
/// @brief		Galaxy-Music Engine -Structs
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.09
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMPrerequisites.h"
#include "GMEnums.h"

namespace GM
{
    /*************************************************************************
     Macro Defines
    *************************************************************************/

    #define LIGHTYEAR					9.461e15	// 1光年

    /*************************************************************************
     Enums
    *************************************************************************/

    /*************************************************************************
     Classes
    *************************************************************************/

    /*************************************************************************
     Structs
    *************************************************************************/
    struct SGMVector2;
    struct SGMVector2i;
    struct SGMVector2f;

    struct SGMVector3;
    struct SGMVector3i;
    struct SGMVector3f;

    struct SGMVector4;
    struct SGMVector4i;
    struct SGMVector4f;

    /*!
    *  @struct SGMVector2f
    *  @brief GM Vector2f
    */
    struct SGMVector2f
    {
        /** @brief 构造 */
        SGMVector2f(float _x = 0, float _y = 0);
        /** @brief == 重载 */
        bool operator == (const SGMVector2f& _sVec) const;
        /** @brief != 重载 */
        bool operator != (const SGMVector2f& _sVec) const;
        /** @brief < 重载 */
        bool operator < (const SGMVector2f& _sVec) const;
        /** @brief > 重载 */
        bool operator > (const SGMVector2f& _sVec) const;
        /** @brief <= 重载 */
        bool operator <= (const SGMVector2f& _sVec) const;
        /** @brief >= 重载 */
        bool operator >= (const SGMVector2f& _sVec) const;
        /** @brief 指针 */
        float * ptr();
        /** @brief 指针 */
        const float * ptr() const;
        /** @brief [] 重载 */
        float& operator [] (int i);
        /** @brief [] 重载 */
        float operator [] (int i) const;
        /** @brief * 重载 */
        float operator * (const SGMVector2f& _sVec) const;
        /** @brief * 重载 */
        const SGMVector2f operator * (float _fValue) const;
        /** @brief *= 重载 */
        SGMVector2f& operator *= (float _fValue);
        /** @brief / 重载 */
        const SGMVector2f operator / (float _fValue) const;
        /** @brief /= 重载 */
        SGMVector2f& operator /= (float _fValue);
        /** @brief + 重载 */
        const SGMVector2f operator + (const SGMVector2f& _sVec) const;
        /** @brief += 重载 */
        SGMVector2f& operator += (const SGMVector2f& _sVec);
        /** @brief - 重载 */
        const SGMVector2f operator - (const SGMVector2f& _sVec) const;
        /** @brief -= 重载 */
        SGMVector2f& operator -= (const SGMVector2f& _sVec);
        /** @brief 取反 重载 */
        const SGMVector2f operator - () const;
        /** @brief 长度 */
        float Length() const;
        /** @brief 长度平方 */
        float SquaredLength() const;
        /** @brief 直线距离 */
        float Distance(const SGMVector2f& _sVec) const;
        /** @brief 直线距离平方 */
        float SquaredDistance(const SGMVector2f& _sVec) const;
        /** @brief 规范化 */
        float Normalize();
        /** @brief 相乘 */
        SGMVector2f ComponentMultiply(const SGMVector2f& _sVecL, const SGMVector2f& _sVecR);
        /** @brief 相除 */
        SGMVector2f ComponentDivide(const SGMVector2f& _sVecL, const SGMVector2f& _sVecR);
        /** @brief 转Int */
        SGMVector2i ToInt();
        /** @brief 转Double */
        SGMVector2 ToDouble();

        // 变量
        union
        {
            struct
            {
                float				x;
                float				y;
            };
            struct
            {
                float				w;
                float				h;
            };
            struct
            {
                float				lon;		//!< 经度
                float				lat;		//!< 纬度
            };
            struct
            {
                float				_v[2];
            };
        };
    };

    /*!
     *  @struct SGMVector2
     *  @brief GM Vector2
     */
    struct SGMVector2
    {
        /** @brief 构造 */
        SGMVector2(double _x = 0, double _y = 0);
        /** @brief 构造 */
        SGMVector2(const SGMVector2f vec);
        /** @brief 返回Vector2f */
        operator SGMVector2f() const;
        /** @brief == 重载 */
        bool operator == (const SGMVector2& _sVec) const;
        /** @brief != 重载 */
        bool operator != (const SGMVector2& _sVec) const;
        /** @brief < 重载 */
        bool operator < (const SGMVector2& _sVec) const;
        /** @brief > 重载 */
        bool operator > (const SGMVector2& _sVec) const;
        /** @brief <= 重载 */
        bool operator <= (const SGMVector2& _sVec) const;
        /** @brief >= 重载 */
        bool operator >= (const SGMVector2& _sVec) const;
        /** @brief 指针 */
        double * ptr();
        /** @brief 指针 */
        const double * ptr() const;
        /** @brief [] 重载 */
        double& operator [] (int i);
        /** @brief [] 重载 */
        double operator [] (int i) const;
        /** @brief * 重载 */
        double operator * (const SGMVector2& _sVec) const;
        /** @brief * 重载 */
        const SGMVector2 operator * (double _fValue) const;
        /** @brief *= 重载 */
        SGMVector2& operator *= (double _fValue);
        /** @brief / 重载 */
        const SGMVector2 operator / (double _fValue) const;
        /** @brief /= 重载 */
        SGMVector2& operator /= (double _fValue);
        /** @brief + 重载 */
        const SGMVector2 operator + (const SGMVector2& _sVec) const;
        /** @brief += 重载 */
        SGMVector2& operator += (const SGMVector2& _sVec);
        /** @brief - 重载 */
        const SGMVector2 operator - (const SGMVector2& _sVec) const;
        /** @brief -= 重载 */
        SGMVector2& operator -= (const SGMVector2& _sVec);
        /** @brief 取反 重载 */
        const SGMVector2 operator - () const;
        /** @brief 长度 */
        double Length() const;
        /** @brief 长度平方 */
        double SquaredLength() const;
        /** @brief 直线距离 */
        double Distance(const SGMVector2& _sVec) const;
        /** @brief 直线距离平方 */
        double SquaredDistance(const SGMVector2& _sVec) const;
        /** @brief 规范化 */
        double Normalize();
        /** @brief 相乘 */
        SGMVector2 ComponentMultiply(const SGMVector2& _sVecL, const SGMVector2& _sVecR);
        /** @brief 相除 */
        SGMVector2 ComponentDivide(const SGMVector2& _sVecL, const SGMVector2& _sVecR);
        /** @brief 转Int */
        SGMVector2i ToInt();
        /** @brief 转Float */
        SGMVector2f ToFloat();

        // 变量
        union
        {
            struct
            {
                double				x;
                double				y;
            };
            struct
            {
                double				w;
                double				h;
            };
            struct
            {
                double				lon;		//!< 经度
                double				lat;		//!< 纬度
            };
            struct
            {
                double				_v[2];
            };
        };
    };

    /*!
    *  @struct SGMVector2i
    *  @brief GM Vector2i
    */
    struct SGMVector2i
    {
        /** @brief 构造 */
        SGMVector2i(int _x = 0, int _y = 0);
        /** @brief == 重载 */
        bool operator == (const SGMVector2i& _sVec) const;
        /** @brief != 重载 */
        bool operator != (const SGMVector2i& _sVec) const;
        /** @brief < 重载 */
        bool operator < (const SGMVector2i& _sVec) const;
        /** @brief > 重载 */
        bool operator > (const SGMVector2i& _sVec) const;
        /** @brief <= 重载 */
        bool operator <= (const SGMVector2i& _sVec) const;
        /** @brief >= 重载 */
        bool operator >= (const SGMVector2i& _sVec) const;
        /** @brief 指针 */
        int * ptr();
        /** @brief 指针 */
        const int * ptr() const;
        /** @brief [] 重载 */
        int& operator [] (int i);
        /** @brief [] 重载 */
        int operator [] (int i) const;
        /** @brief * 重载 */
        int operator * (const SGMVector2i& _sVec) const;
        /** @brief * 重载 */
        const SGMVector2i operator * (int _fValue) const;
        /** @brief *= 重载 */
        SGMVector2i& operator *= (int _fValue);
        /** @brief / 重载 */
        const SGMVector2i operator / (int _fValue) const;
        /** @brief /= 重载 */
        SGMVector2i& operator /= (int _fValue);
        /** @brief + 重载 */
        const SGMVector2i operator + (const SGMVector2i& _sVec) const;
        /** @brief += 重载 */
        SGMVector2i& operator += (const SGMVector2i& _sVec);
        /** @brief - 重载 */
        const SGMVector2i operator - (const SGMVector2i& _sVec) const;
        /** @brief -= 重载 */
        SGMVector2i& operator -= (const SGMVector2i& _sVec);
        /** @brief 取反 重载 */
        const SGMVector2i operator - () const;
        /** @brief 长度 */
        double Length() const;
        /** @brief 长度平方 */
        int SquaredLength() const;
        /** @brief 直线距离 */
        double Distance(const SGMVector2i& _sVec) const;
        /** @brief 直线距离平方 */
        int SquaredDistance(const SGMVector2i& _sVec) const;
        /** @brief 相乘 */
        SGMVector2i ComponentMultiply(const SGMVector2i& _sVecL, const SGMVector2i& _sVecR);
        /** @brief 相除 */
        SGMVector2i ComponentDivide(const SGMVector2i& _sVecL, const SGMVector2i& _sVecR);
        /** @brief 转Float */
        SGMVector2f ToFloat();
        /** @brief 转Double */
        SGMVector2 ToDouble();

        // 变量
        union
        {
            struct
            {
                int				x;
                int				y;
            };
            struct
            {
                int				w;
                int				h;
            };
            struct
            {
                int				_v[2];
            };
        };
    };

    /*!
    *  @struct SGMVector3f
    *  @brief GM Vector3f
    */
    struct SGMVector3f
    {
        /** @brief 构造 */
        SGMVector3f(float _x = 0, float _y = 0, float _z = 0);
        /** @brief 构造 */
        SGMVector3f(SGMVector2f _sVec2, float _z = 0);
        /** @brief ==重载 */
        bool operator == (const SGMVector3f& _sVec) const;
        /** @brief !=重载 */
        bool operator != (const SGMVector3f& _sVec) const;
        /** @brief < 重载 */
        bool operator < (const SGMVector3f& _sVec) const;
        /** @brief > 重载 */
        bool operator > (const SGMVector3f& _sVec) const;
        /** @brief <= 重载 */
        bool operator <= (const SGMVector3f& _sVec) const;
        /** @brief >= 重载 */
        bool operator >= (const SGMVector3f& _sVec) const;
        /** @brief 指针 */
        float * ptr();
        /** @brief 指针 */
        const float * ptr() const;
        /** @brief [] 重载 */
        float& operator [] (int i);
        /** @brief [] 重载 */
        float operator [] (int i) const;
        /** @brief * 重载 */
        float operator * (const SGMVector3f& _sVec) const;
        /** @brief ^ 重载 */
        const SGMVector3f operator ^ (const SGMVector3f& _sVec) const;
        /** @brief * 重载 */
        const SGMVector3f operator * (float _fValue) const;
        /** @brief *= 重载 */
        SGMVector3f& operator *= (float _fValue);
        /** @brief / 重载 */
        const SGMVector3f operator / (float _fValue) const;
        /** @brief /= 重载 */
        SGMVector3f& operator /= (float _fValue);
        /** @brief + 重载 */
        const SGMVector3f operator + (const SGMVector3f& _sVec) const;
        /** @brief += 重载 */
        SGMVector3f& operator += (const SGMVector3f& _sVec);
        /** @brief - 重载 */
        const SGMVector3f operator - (const SGMVector3f& _sVec) const;
        /** @brief -= 重载 */
        SGMVector3f& operator -= (const SGMVector3f& _sVec);
        /** @brief 取反 重载 */
        const SGMVector3f operator - () const;
        /** @brief 长度 */
        float Length() const;
        /** @brief 长度平方 */
        float SquaredLength() const;
        /** @brief 直线距离 */
        float Distance(const SGMVector3f& _sVec) const;
        /** @brief 直线距离平方 */
        float SquaredDistance(const SGMVector3f& _sVec) const;
        /** @brief 规范化 */
        float Normalize();
        /** @brief 相乘 */
        SGMVector3f ComponentMultiply(const SGMVector3f& _sVecL, const SGMVector3f& _sVecR);
        /** @brief 相除 */
        SGMVector3f ComponentDivide(const SGMVector3f& _sVecL, const SGMVector3f& _sVecR);
        /** @brief 转Int */
        SGMVector3i ToInt();
        /** @brief 转Double */
        SGMVector3 ToDouble();

        // 变量
        union
        {
            struct
            {
                float				x;
                float				y;
                float				z;
            };
            struct
            {
                float				lon;		//!< 经度
                float				lat;		//!< 纬度
                float				alt;		//!< 高度
            };
            struct
            {
                float				_v[3];
            };
        };
    };

    /*!
     *  @struct SGMVector3
     *  @brief GM Vector3
     */
    struct SGMVector3
    {
        /** @brief 构造 */
        SGMVector3(double _x = 0, double _y = 0, double _z = 0);
        /** @brief 构造 */
        SGMVector3(SGMVector2 _sVec2, double _z = 0);
        /** @brief 构造 */
        SGMVector3(const SGMVector3f vec);
        /** @brief 返回Vector3f */
        operator SGMVector3f() const;
        /** @brief ==重载 */
        bool operator == (const SGMVector3& _sVec) const;
        /** @brief !=重载 */
        bool operator != (const SGMVector3& _sVec) const;
        /** @brief < 重载 */
        bool operator < (const SGMVector3& _sVec) const;
        /** @brief > 重载 */
        bool operator > (const SGMVector3& _sVec) const;
        /** @brief <= 重载 */
        bool operator <= (const SGMVector3& _sVec) const;
        /** @brief >= 重载 */
        bool operator >= (const SGMVector3& _sVec) const;
        /** @brief 指针 */
        double * ptr();
        /** @brief 指针 */
        const double * ptr() const;
        /** @brief [] 重载 */
        double& operator [] (int i);
        /** @brief [] 重载 */
        double operator [] (int i) const;
        /** @brief * 重载 */
        double operator * (const SGMVector3& _sVec) const;
        /** @brief ^ 重载 */
        const SGMVector3 operator ^ (const SGMVector3& _sVec) const;
        /** @brief * 重载 */
        const SGMVector3 operator * (double _fValue) const;
        /** @brief *= 重载 */
        SGMVector3& operator *= (double _fValue);
        /** @brief / 重载 */
        const SGMVector3 operator / (double _fValue) const;
        /** @brief /= 重载 */
        SGMVector3& operator /= (double _fValue);
        /** @brief + 重载 */
        const SGMVector3 operator + (const SGMVector3& _sVec) const;
        /** @brief += 重载 */
        SGMVector3& operator += (const SGMVector3& _sVec);
        /** @brief - 重载 */
        const SGMVector3 operator - (const SGMVector3& _sVec) const;
        /** @brief -= 重载 */
        SGMVector3& operator -= (const SGMVector3& _sVec);
        /** @brief 取反 重载 */
        const SGMVector3 operator - () const;
        /** @brief 长度 */
        double Length() const;
        /** @brief 长度平方 */
        double SquaredLength() const;
        /** @brief 直线距离 */
        double Distance(const SGMVector3& _sVec) const;
        /** @brief 直线距离平方 */
        double SquaredDistance(const SGMVector3& _sVec) const;
        /** @brief 规范化 */
        double Normalize();
        /** @brief 相乘 */
        SGMVector3 ComponentMultiply(const SGMVector3& _sVecL, const SGMVector3& _sVecR);
        /** @brief 相除 */
        SGMVector3 ComponentDivide(const SGMVector3& _sVecL, const SGMVector3& _sVecR);
        /** @brief 转Int */
        SGMVector3i ToInt();
        /** @brief 转Float */
        SGMVector3f ToFloat();

        // 变量
        union
        {
            struct
            {
                double				x;
                double				y;
                double				z;
            };
            struct
            {
                double				lon;		//!< 经度
                double				lat;		//!< 纬度
                double				alt;		//!< 高度
            };
            struct
            {
                double				_v[3];
            };
        };
    };

    /*!
    *  @struct SGMVector3i
    *  @brief GM Vector3i
    */
    struct SGMVector3i
    {
        /** @brief 构造 */
        SGMVector3i(int _x = 0, int _y = 0, int _z = 0);
        /** @brief 构造 */
        SGMVector3i(SGMVector2i _sVec2, int _z = 0);
        /** @brief ==重载 */
        bool operator == (const SGMVector3i& _sVec) const;
        /** @brief !=重载 */
        bool operator != (const SGMVector3i& _sVec) const;
        /** @brief < 重载 */
        bool operator < (const SGMVector3i& _sVec) const;
        /** @brief > 重载 */
        bool operator > (const SGMVector3i& _sVec) const;
        /** @brief <= 重载 */
        bool operator <= (const SGMVector3i& _sVec) const;
        /** @brief >= 重载 */
        bool operator >= (const SGMVector3i& _sVec) const;
        /** @brief 指针 */
        int * ptr();
        /** @brief 指针 */
        const int * ptr() const;
        /** @brief [] 重载 */
        int& operator [] (int i);
        /** @brief [] 重载 */
        int operator [] (int i) const;
        /** @brief * 重载 */
        int operator * (const SGMVector3i& _sVec) const;
        /** @brief ^ 重载 */
        const SGMVector3i operator ^ (const SGMVector3i& _sVec) const;
        /** @brief * 重载 */
        const SGMVector3i operator * (int _fValue) const;
        /** @brief *= 重载 */
        SGMVector3i& operator *= (int _fValue);
        /** @brief / 重载 */
        const SGMVector3i operator / (int _fValue) const;
        /** @brief /= 重载 */
        SGMVector3i& operator /= (int _fValue);
        /** @brief + 重载 */
        const SGMVector3i operator + (const SGMVector3i& _sVec) const;
        /** @brief += 重载 */
        SGMVector3i& operator += (const SGMVector3i& _sVec);
        /** @brief - 重载 */
        const SGMVector3i operator - (const SGMVector3i& _sVec) const;
        /** @brief -= 重载 */
        SGMVector3i& operator -= (const SGMVector3i& _sVec);
        /** @brief 取反 重载 */
        const SGMVector3i operator - () const;
        /** @brief 长度 */
        double Length() const;
        /** @brief 长度平方 */
        int SquaredLength() const;
        /** @brief 直线距离 */
        double Distance(const SGMVector3i& _sVec) const;
        /** @brief 直线距离平方 */
        int SquaredDistance(const SGMVector3i& _sVec) const;
        /** @brief 相乘 */
        SGMVector3i ComponentMultiply(const SGMVector3i& _sVecL, const SGMVector3i& _sVecR);
        /** @brief 相除 */
        SGMVector3i ComponentDivide(const SGMVector3i& _sVecL, const SGMVector3i& _sVecR);
        /** @brief 转Float */
        SGMVector3f ToFloat();
        /** @brief 转Double */
        SGMVector3 ToDouble();

        // 变量
        union
        {
            struct
            {
                int				x;
                int				y;
                int				z;
            };
            struct
            {
                int				lon;		//!< 经度
                int				lat;		//!< 纬度
                int				alt;		//!< 高度
            };
            struct
            {
                int				_v[3];
            };
        };
    };

    /*!
    *  @struct SGMVector4f
    *  @brief GM Vector4
    */
    struct SGMVector4f
    {
        /** @brief 构造 */
        SGMVector4f(float _x = 0, float _y = 0, float _z = 0, float _w = 0);
        /** @brief 构造 */
        SGMVector4f(unsigned nColorValue);
        /** @brief 构造 */
        SGMVector4f(SGMVector3f _sVec3, float _w = 0);
        /** @brief ==重载 */
        bool operator == (const SGMVector4f& _sVec) const;
        /** @brief !=重载 */
        bool operator != (const SGMVector4f& _sVec) const;
        /** @brief < 重载 */
        bool operator < (const SGMVector4f& _sVec) const;
        /** @brief > 重载 */
        bool operator > (const SGMVector4f& _sVec) const;
        /** @brief <= 重载 */
        bool operator <= (const SGMVector4f& _sVec) const;
        /** @brief >= 重载 */
        bool operator >= (const SGMVector4f& _sVec) const;
        /** @brief 指针 */
        float * ptr();
        /** @brief 指针 */
        const float * ptr() const;
        /** @brief [] 重载 */
        float& operator [] (int i);
        /** @brief [] 重载 */
        float operator [] (int i) const;
        /** @brief * 重载 */
        float operator * (const SGMVector4f& _sVec) const;
        /** @brief * 重载 */
        const SGMVector4f operator * (float _fValue) const;
        /** @brief *= 重载 */
        SGMVector4f& operator *= (float _fValue);
        /** @brief / 重载 */
        const SGMVector4f operator / (float _fValue) const;
        /** @brief /= 重载 */
        SGMVector4f& operator /= (float _fValue);
        /** @brief + 重载 */
        const SGMVector4f operator + (const SGMVector4f& _sVec) const;
        /** @brief += 重载 */
        SGMVector4f& operator += (const SGMVector4f& _sVec);
        /** @brief - 重载 */
        const SGMVector4f operator - (const SGMVector4f& _sVec) const;
        /** @brief -= 重载 */
        SGMVector4f& operator -= (const SGMVector4f& _sVec);
        /** @brief 取反 重载 */
        const SGMVector4f operator - () const;
        /** @brief 长度 */
        float Length() const;
        /** @brief 长度平方 */
        float SquaredLength() const;
        /** @brief 直线距离 */
        float Distance(const SGMVector4f& _sVec) const;
        /** @brief 直线距离平方 */
        float SquaredDistance(const SGMVector4f& _sVec) const;
        /** @brief 规范化 */
        float Normalize();
        /** @brief 相乘 */
        SGMVector4f ComponentMultiply(const SGMVector4f& _sVecL, const SGMVector4f& _sVecR);
        /** @brief 相除 */
        SGMVector4f ComponentDivide(const SGMVector4f& _sVecL, const SGMVector4f& _sVecR);
        /** @brief 转Int */
        SGMVector4i ToInt();
        /** @brief 转Double */
        SGMVector4 ToDouble();

        // 变量
        union
        {
            struct
            {
                float				x;
                float				y;
                float				z;
                float				w;
            };
            struct
            {
                float				r;
                float				g;
                float				b;
                float				a;
            };
            struct
            {
                float				_v[4];
            };
        };
        
    };

    struct SGMVector4
    {
        /** @brief 构造 */
        SGMVector4(double _x = 0, double _y = 0, double _z = 0, double _w = 0);
        /** @brief 构造 */
        SGMVector4(unsigned nColorValue);
        /** @brief 构造 */
        SGMVector4(SGMVector3 _sVec3, double _w = 0);
        /** @brief 构造 */
        SGMVector4(const SGMVector4f vec);
        /** @brief 返回Vector4f */
        operator SGMVector4f() const;
        /** @brief ==重载 */
        bool operator == (const SGMVector4& _sVec) const;
        /** @brief !=重载 */
        bool operator != (const SGMVector4& _sVec) const;
        /** @brief < 重载 */
        bool operator < (const SGMVector4& _sVec) const;
        /** @brief > 重载 */
        bool operator > (const SGMVector4& _sVec) const;
        /** @brief <= 重载 */
        bool operator <= (const SGMVector4& _sVec) const;
        /** @brief >= 重载 */
        bool operator >= (const SGMVector4& _sVec) const;
        /** @brief 指针 */
        double * ptr();
        /** @brief 指针 */
        const double * ptr() const;
        /** @brief [] 重载 */
        double& operator [] (int i);
        /** @brief [] 重载 */
        double operator [] (int i) const;
        /** @brief * 重载 */
        double operator * (const SGMVector4& _sVec) const;
        /** @brief * 重载 */
        const SGMVector4 operator * (double _fValue) const;
        /** @brief *= 重载 */
        SGMVector4& operator *= (double _fValue);
        /** @brief / 重载 */
        const SGMVector4 operator / (double _fValue) const;
        /** @brief /= 重载 */
        SGMVector4& operator /= (double _fValue);
        /** @brief + 重载 */
        const SGMVector4 operator + (const SGMVector4& _sVec) const;
        /** @brief += 重载 */
        SGMVector4& operator += (const SGMVector4& _sVec);
        /** @brief - 重载 */
        const SGMVector4 operator - (const SGMVector4& _sVec) const;
        /** @brief -= 重载 */
        SGMVector4& operator -= (const SGMVector4& _sVec);
        /** @brief 取反 重载 */
        const SGMVector4 operator - () const;
        /** @brief 长度 */
        double Length() const;
        /** @brief 长度平方 */
        double SquaredLength() const;
        /** @brief 直线距离 */
        double Distance(const SGMVector4& _sVec) const;
        /** @brief 直线距离平方 */
        double SquaredDistance(const SGMVector4& _sVec) const;
        /** @brief 规范化 */
        double Normalize();
        /** @brief 相乘 */
        SGMVector4 ComponentMultiply(const SGMVector4& _sVecL, const SGMVector4& _sVecR);
        /** @brief 相除 */
        SGMVector4 ComponentDivide(const SGMVector4& _sVecL, const SGMVector4& _sVecR);
        /** @brief 转Int */
        SGMVector4i ToInt();
        /** @brief 转Float */
        SGMVector4f ToFloat();

        // 变量
        union
        {
            struct
            {
                double				x;
                double				y;
                double				z;
                double				w;
            };
            struct
            {
                double				r;
                double				g;
                double				b;
                double				a;
            };
            struct
            {
                double				_v[4];
            };
        };
    };

    /*!
    *  @struct SGMVector4i
    *  @brief GM Vector4
    */
    struct SGMVector4i
    {
        /** @brief 构造 */
        SGMVector4i(int _x = 0, int _y = 0, int _z = 0, int _w = 0);
        /** @brief 构造 */
        SGMVector4i(unsigned nColorValue);
        /** @brief 构造 */
        SGMVector4i(SGMVector3i _sVec3, int _w = 0);
        /** @brief ==重载 */
        bool operator == (const SGMVector4i& _sVec) const;
        /** @brief !=重载 */
        bool operator != (const SGMVector4i& _sVec) const;
        /** @brief < 重载 */
        bool operator < (const SGMVector4i& _sVec) const;
        /** @brief > 重载 */
        bool operator > (const SGMVector4i& _sVec) const;
        /** @brief <= 重载 */
        bool operator <= (const SGMVector4i& _sVec) const;
        /** @brief >= 重载 */
        bool operator >= (const SGMVector4i& _sVec) const;
        /** @brief 指针 */
        int * ptr();
        /** @brief 指针 */
        const int * ptr() const;
        /** @brief [] 重载 */
        int& operator [] (int i);
        /** @brief [] 重载 */
        int operator [] (int i) const;
        /** @brief * 重载 */
        int operator * (const SGMVector4i& _sVec) const;
        /** @brief * 重载 */
        const SGMVector4i operator * (int _fValue) const;
        /** @brief *= 重载 */
        SGMVector4i& operator *= (int _fValue);
        /** @brief / 重载 */
        const SGMVector4i operator / (int _fValue) const;
        /** @brief /= 重载 */
        SGMVector4i& operator /= (int _fValue);
        /** @brief + 重载 */
        const SGMVector4i operator + (const SGMVector4i& _sVec) const;
        /** @brief += 重载 */
        SGMVector4i& operator += (const SGMVector4i& _sVec);
        /** @brief - 重载 */
        const SGMVector4i operator - (const SGMVector4i& _sVec) const;
        /** @brief -= 重载 */
        SGMVector4i& operator -= (const SGMVector4i& _sVec);
        /** @brief 取反 重载 */
        const SGMVector4i operator - () const;
        /** @brief 长度 */
        double Length() const;
        /** @brief 长度平方 */
        int SquaredLength() const;
        /** @brief 直线距离 */
        double Distance(const SGMVector4i& _sVec) const;
        /** @brief 直线距离平方 */
        int SquaredDistance(const SGMVector4i& _sVec) const;
        /** @brief 相乘 */
        SGMVector4i ComponentMultiply(const SGMVector4i& _sVecL, const SGMVector4i& _sVecR);
        /** @brief 相除 */
        SGMVector4i ComponentDivide(const SGMVector4i& _sVecL, const SGMVector4i& _sVecR);
        /** @brief 转Double */
        SGMVector4 ToDouble();
        /** @brief 转Float */
        SGMVector4f ToFloat();

        // 变量
        union
        {
            struct
            {
                int				x;
                int				y;
                int				z;
                int				w;
            };
            struct
            {
                int				r;
                int				g;
                int				b;
                int				a;
            };
            struct
            {
                int				_v[4];
            };
        };
    };

    /**
    * 体积数据范围
    * @author LiuTao
    * @since 2020.12.13
    * @param fXMin, fXMax			长度
    * @param fYMin, fYMax			宽度
    * @param fZMin, fZMax			高度
    */
    struct SGMVolumeRange
    {
        SGMVolumeRange()
            :fXMin(-5.0f), fXMax(5.0f), fYMin(-5.0f), fYMax(5.0f), fZMin(-0.5f), fZMax(0.5f)
        {
        }

        float					fXMin;
        float					fXMax;
        float					fYMin;
        float					fYMax;
        float					fZMin;
        float					fZMax;
    };

    /*!
     *  @struct SGMModelData
     *  @brief GM Model Data
     */
    struct SGMModelData
    {
        /** @brief 构造 */
        SGMModelData()
            : vPos(0, 0, 0), vOri(0, 0, 0), vScale(1, 1, 1)
        {}

        // 变量
        std::string			strName = "";                   //!< 名称
        std::string			strFilePath = "";               //!< 文件路径
        SGMVector3			vPos;                           //!< 模型位
        SGMVector3			vOri;                           //!< 模型方向
        SGMVector3			vScale;                         //!< 模型缩放
        int					iEntRenderBin = 1;              //!< 模型渲染顺序Renderbin
        EGMMaterial			eMaterial = EGM_MATERIAL_PBR;   //!< 材质
        EGMBlend			eBlend = EGM_BLEND_Opaque;      //!< 半透明混合模式
        bool				bCastShadow = true;             //!< 是否投射阴影
    };
}	// GM
