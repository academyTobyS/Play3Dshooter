///////////////////////////////////////////////////////////////////////////
//      Copyright (C) Sumo Digital Ltd. All rights reserved.
///////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __PLAY3D__
#define __PLAY3D__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif



//-----------------------------------------------------------
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <atomic>
#include <windows.h>
#include <wincodec.h>
#include <dxgi1_3.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <hidusage.h>
#include <xaudio2.h>

//-----------------------------------------------------------


//-----------------------------------------------------------
// Play3dImpl\Play3dApi.h





//-----------------------------------------------------------
// Play3dImpl\TypesApi.h






//-----------------------------------------------------------
// Play3dImpl\DebugApi.h



namespace Play3d
{
	namespace Debug
	{
		void Put(const char* pStr);

		void Printf(const char* pFmtStr, ...);

		void Tracef(const char* pStrFilename, unsigned int lineNum, const char* pFmtStr, ...);

		void HLine();
	}
}



#define PLAY_UNUSED(x) (void)x

#ifdef _DEBUG
#define PLAY_ASSERT(condition) if(!(condition)){ Play3d::Debug::Tracef(__FILE__, __LINE__, "PLAY_ASSERT(%s) FAILED : ", #condition); __debugbreak(); }
#define PLAY_ASSERT_MSG(condition, fmt, ...) if(!(condition)){ Play3d::Debug::Tracef(__FILE__, __LINE__, "PLAY_ASSERT(%s) FAILED : "##fmt, #condition, __VA_ARGS__); __debugbreak(); }
#else
#define PLAY_ASSERT(condition) 
#define PLAY_ASSERT_MSG(condition, fmt, ...) 
#endif
#ifndef WIN32_LEAN_AND_MEAN
#error WIN32_LEAN_AND_MEAN must be defined
#endif

#ifndef NOMINMAX
#error NOMINMAX must be defined
#endif

#pragma warning( disable : 4100 ) // unreferenced formal parameter
#pragma warning( disable : 4189 ) // local variable is initialized but not referenced
#pragma warning( disable : 4201 ) // nonstandard extension used: nameless struct/union

#define PLAY_NONCOPYABLE(classname) \
	classname(const classname&) = delete; \
	classname& operator=(const classname&) = delete;

#define PLAY_SINGLETON_INTERFACE(classname) \
		public:\
			static classname& Instance(){ return *ms_pInstance; }\
			static void Initialise(){ if(!ms_pInstance){ ms_pInstance = new classname; } }\
			static void Destroy() { delete ms_pInstance; ms_pInstance = nullptr; }\
		private:\
			static classname* ms_pInstance;

#define PLAY_SINGLETON_IMPL(classname) \
	classname* classname::ms_pInstance = nullptr;

#define PLAY_DEFINE_RESOURCE_PLACEHOLDER(classname)\
class classname;\
using classname##Id = IdKey<classname>;\
struct classname##Desc {};\
class classname{\
public:\
	classname(const classname##Desc& rDesc) {}\
};

#define PLAY_SAFE_RELEASE(ptr) if(ptr){ptr->Release(); ptr = nullptr;}
#define PLAY_SAFE_DELETE(ptr) if(ptr){delete ptr; ptr = nullptr;}
#define PLAY_SAFE_DELETE_ARRAY(ptr) if(ptr){delete [] ptr; ptr = nullptr;}

namespace Play3d
{

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	using s8 = int8_t;
	using s16 = int16_t;
	using s32 = int32_t;
	using s64 = int64_t;

	using f32 = float;
	using f64 = double;


	using result_t = int;
	enum Result : result_t
	{
		RESULT_QUIT = -2,
		RESULT_FAIL = -1,
		RESULT_OK = 0
	};


	template<typename KeyType, typename ValueType = u32, ValueType kInvalidValue = ~0u>
	class IdKey
	{
	public:
		using key_type = KeyType;
		using value_type = ValueType;

		IdKey()
			: m_value(kInvalidValue)
		{}
		explicit IdKey(ValueType value)
			: m_value(value)
		{}
		IdKey(const IdKey& op)
			: m_value(op.m_value)
		{}
		IdKey& operator = (const IdKey& op)
		{
			m_value = op.m_value;
			return *this;
		}
		value_type GetValue() const { return m_value; }
		bool IsInvalid() const { return m_value == kInvalidValue; }
		bool IsValid() const { return m_value != kInvalidValue; }
		void Invalidate() { m_value = kInvalidValue; }
		bool operator == (IdKey rhs) const { return m_value == rhs.m_value; }
		bool operator != (IdKey rhs) const { return m_value != rhs.m_value; }
	private:
		ValueType m_value;
	};

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

};

//-----------------------------------------------------------
// Play3dImpl\MathConstants.h


namespace Play3d {

	constexpr f32 kfHalfPi = 3.141592654f / 2.0f;
	constexpr f32 kfPi = 3.141592654f;
	constexpr f32 kfTwoPi = 6.283185307f;

} // namespace Play3d


//-----------------------------------------------------------
// Play3dImpl\ScalarMath.h


namespace Play3d
{


template<typename T>
inline bool EqualTol(const T lhs, const T rhs, const T tolerance)
{
	return std::abs(lhs - rhs) < tolerance;
}


}


//-----------------------------------------------------------
// Play3dImpl\VectorMath.h




namespace Play3d {


template<int N, typename T>
struct TVector
{
	T v[N];
};


template<typename T>
struct TVector<2, T>
{
	TVector() {}
	explicit TVector(T value) : x(value), y(value) {}
	explicit TVector(T x, T y) : x(x), y(y) {}
	union
	{
		T v[2];
		struct { T x; T y; };
	};
};

template<typename T>
struct TVector<3, T>
{
	TVector() {}
	explicit TVector(T value) : x(value), y(value), z(value) {}
	explicit TVector(T x, T y, T z) : x(x), y(y), z(z) {}
	explicit TVector(const TVector<2, T>& v, T z) : x(v.x), y(v.y), z(z) {}

	TVector<2, T> xy() const { return TVector<2, T>(x, y); }

	union
	{
		T v[3];
		struct { T x; T y; T z; };
	};
};

template<typename T>
struct TVector<4, T>
{
	TVector() {}
	explicit TVector(T value) : x(value), y(value), z(value), w(value) {}
	explicit TVector(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
	explicit TVector(const TVector<3, T>& v, T w) : x(v.x), y(v.y), z(v.z), w(w) {}

	TVector<2, T> xy() const { return TVector<2, T>(x, y); }
	TVector<3, T> xyz() const { return TVector<3, T>(x,y,z); }

	union
	{
		T v[4];
		struct { T x; T y; T z; T w; };
	};
};


using Vector2f = TVector<2, float>;
using Vector3f = TVector<3, float>;
using Vector4f = TVector<4, float>;





template<int N, typename T>
inline TVector<N, T> operator + (const TVector<N, T>& lhs, const TVector<N, T>& rhs)
{
	TVector<N, T> ret;
	for (int i = 0; i < N; ++i)
	{
		ret.v[i] = lhs.v[i] + rhs.v[i];
	}
	return ret;
}

template<int N, typename T>
inline TVector<N, T>& operator += (TVector<N, T>& lhs, const TVector<N, T>& rhs)
{
	for (int i = 0; i < N; ++i)
	{
		lhs.v[i] += rhs.v[i];
	}
	return lhs;
}

template<int N, typename T>
inline TVector<N, T> operator - (const TVector<N, T>& lhs, const TVector<N, T>& rhs)
{
	TVector<N, T> ret;
	for (int i = 0; i < N; ++i)
	{
		ret.v[i] = lhs.v[i] - rhs.v[i];
	}
	return ret;
}

template<int N, typename T>
inline TVector<N, T>& operator -= (TVector<N, T>& lhs, const TVector<N, T>& rhs)
{
	for (int i = 0; i < N; ++i)
	{
		lhs.v[i] -= rhs.v[i];
	}
	return lhs;
}

template<int N, typename T>
inline TVector<N, T> operator - (const TVector<N, T>& op)
{
	TVector<N, T> ret;
	for (int i = 0; i < N; ++i)
	{
		ret.v[i] = -op.v[i];
	}
	return ret;
}

template<int N, typename T>
inline TVector<N, T> operator * (const TVector<N, T>& lhs, const TVector<N, T>& rhs)
{
	TVector<N, T> ret;
	for (int i = 0; i < N; ++i)
	{
		ret.v[i] = lhs.v[i] * rhs.v[i];
	}
	return ret;
}

template<int N, typename T>
inline TVector<N, T> operator / (const TVector<N, T>& lhs, const TVector<N, T>& rhs)
{
	TVector<N, T> ret;
	for (int i = 0; i < N; ++i)
	{
		ret.v[i] = lhs.v[i] / rhs.v[i];
	}
	return ret;
}

template<int N, typename T>
inline TVector<N, T> operator * (const TVector<N, T>& lhs, const T rhs)
{
	TVector<N, T> ret;
	for (int i = 0; i < N; ++i)
	{
		ret.v[i] = lhs.v[i] * rhs;
	}
	return ret;
}

template<int N, typename T>
inline TVector<N, T> operator * (const T lhs, const TVector<N, T>& rhs)
{
	return rhs * lhs;
}

template<int N, typename T>
inline TVector<N, T> operator / (const TVector<N, T>& lhs, const T rhs)
{
	return lhs * (T(1)/rhs);
}

template<int N, typename T>
inline TVector<N, T> operator / (const T lhs, const TVector<N, T>& rhs)
{
	TVector<N, T> ret;
	for (int i = 0; i < N; ++i)
	{
		ret.v[i] = lhs / rhs.v[i];
	}
	return ret;
}

template<int N, typename T>
inline T dot(const TVector<N, T>& lhs, const TVector<N, T>& rhs)
{
	T ret = 0.f;
	for (int i = 0; i < N; ++i)
	{
		ret += lhs.v[i] * rhs.v[i];
	}
	return ret;
}

template<int N, typename T>
inline T lengthSqr(const TVector<N, T>& v)
{
	return dot(v, v);
}

template<int N, typename T>
inline T length(const TVector<N, T>& v)
{
	return sqrt(dot(v,v));
}

template<int N, typename T>
inline TVector<N, T> normalize(const TVector<N, T>& v)
{
	return v / length(v);
}

template<typename T>
inline TVector<3, T> cross(const TVector<3, T>& lhs, const TVector<3, T>& rhs)
{
	return TVector<3, T>(lhs.y*rhs.z - lhs.z*rhs.y, lhs.z*rhs.x - lhs.x*rhs.z, lhs.x*rhs.y - lhs.y*rhs.x);
}


template<int N, typename T>
inline bool operator == (const TVector<N, T>& lhs, const TVector<N, T>& rhs)
{
	TVector<N, T> ret;
	for (int i = 0; i < N; ++i)
	{
		if (lhs.v[i] != rhs.v[i])
			return false;
	}
	return true;
}

template<int N, typename T>
inline bool operator != (const TVector<N, T>& lhs, const TVector<N, T>& rhs)
{
	return !(lhs == rhs);
}



template<int N, typename T>
inline bool EqualTol(const TVector<N, T>& lhs, const TVector<N, T>& rhs, const T tolerance)
{
	TVector<N, T> ret;
	for (int i = 0; i < N; ++i)
	{
		if (std::abs(lhs.v[i] - rhs.v[i]) > tolerance)
		{
			return false;
		}
	}
	return true;
}




} // namespace Play3d




//-----------------------------------------------------------
// Play3dImpl\MatrixMath.h





namespace Play3d
{

	template<int ROWS, int COLUMNS, typename T>
	struct TMatrix
	{
		T m[COLUMNS][ROWS]; // Column major storage matching DX12 HLSL
	};


	template<typename T>
	struct TMatrix<2, 2, T>
	{
		using VectorType = TVector<2, T>;
		TMatrix() {}
		explicit TMatrix(const VectorType& column0, const VectorType& column1)
			: m_column{ column0, column1 }
		{}
		union
		{
			T m[2][2];
			VectorType m_column[2];
		};
	};



	template<typename T>
	struct TMatrix<3, 3, T>
	{
		using VectorType = TVector<3, T>;
		TMatrix() {}
		explicit TMatrix(const VectorType& column0, const VectorType& column1, const VectorType& column2)
			: m_column{ column0, column1, column2 }
		{}
		
		TMatrix<2, 2, T> Upper2x2() const { return TMatrix<2, 2, T>(m_column[0].xy(), m_column[1].xy()); }
		
		union
		{
			T m[3][3];
			VectorType m_column[3];
		};
	};


	template<typename T>
	struct TMatrix<4, 4, T>
	{
		using VectorType = TVector<4, T>;
		TMatrix() {}
		explicit TMatrix(const VectorType& column0, const VectorType& column1, const VectorType& column2, const VectorType& column3)
			: m_column{ column0, column1, column2, column3 }
		{}

		explicit TMatrix(const TMatrix<3, 3, T>& M, const TVector<3, T> v)
			: m_column{ VectorType(M.m_column[0], 0), VectorType(M.m_column[1], 0), VectorType(M.m_column[2], 0), VectorType(v, T(1)) }
		{}

		TMatrix<3, 3, T> Upper3x3() const { return TMatrix<3, 3, T>(m_column[0].xyz(), m_column[1].xyz(), m_column[2].xyz()); }

		union
		{
			T m[4][4];
			VectorType m_column[4];
		};
	};


	using Matrix2x2f = TMatrix<2, 2, f32>;
	using Matrix3x3f = TMatrix<3, 3, f32>;
	using Matrix4x4f = TMatrix<4, 4, f32>;

	template<int ROWS, int COLUMNS, typename T>
	inline TMatrix<ROWS, COLUMNS, T> operator + (const TMatrix<ROWS, COLUMNS, T>& lhs, const TMatrix<ROWS, COLUMNS, T>& rhs)
	{
		TMatrix<ROWS, COLUMNS, T> ret;
		for (int i = 0; i < COLUMNS; ++i)
			for (int j = 0; j < ROWS; ++j)
		{
			ret.v[i][j] = lhs.v[i][j] + rhs.v[i][j];
		}
		return ret;
	}

	template<int ROWS, int COLUMNS, typename T>
	inline TMatrix<ROWS, COLUMNS, T> operator - (const TMatrix<ROWS, COLUMNS, T>& lhs, const TMatrix<ROWS, COLUMNS, T>& rhs)
	{
		TMatrix<ROWS, COLUMNS, T> ret;
		for (int i = 0; i < COLUMNS; ++i)
			for (int j = 0; j < ROWS; ++j)
			{
				ret.v[i][j] = lhs.v[i][j] - rhs.v[i][j];
			}
		return ret;
	}

	template<int M, int N, int P, typename T>
	inline TMatrix<M, P, T> operator * (const TMatrix<M, N, T>& lhs, const TMatrix<N, P, T>& rhs)
	{
		TMatrix<M, P, T> ret;
		for (int i = 0; i < M; ++i)
		{
			for (int j = 0; j < P; ++j)
			{
				T t = 0;
				for (int k = 0; k < N; ++k)
				{
					t += lhs.m[k][i] * rhs.m[j][k];
				}
				ret.m[j][i] = t;
			}
		}
		return ret;
	}

	template<int ROWS, int COLUMNS, typename T>
	inline TVector<ROWS, T> Transform(const TMatrix<ROWS, COLUMNS, T>& m, const TVector<ROWS, T>& v)
	{
		TVector<ROWS, T> ret;

		for (int i = 0; i < ROWS; ++i)
		{
			ret.v[i] = T(0);
			for (int j = 0; j < COLUMNS; ++j)
			{ 
				ret.v[i] += m.m[j][i] * v.v[j];
			}
		}
		return ret;
	}

	template<int ROWS, int COLUMNS, typename T>
	inline TMatrix<COLUMNS, ROWS, T> Transpose(const TMatrix<ROWS, COLUMNS, T>& op)
	{
		TMatrix<COLUMNS, ROWS, T> ret;
		for (int i = 0; i < COLUMNS; ++i)
		{
			for (int j = 0; j < ROWS; ++j)
			{
				ret.m[j][i] = op.m[i][j];
			}
		}
		return ret;
	}

	template<int ROWS, int COLUMNS, typename T>
	inline void MatrixFill(TMatrix<ROWS, COLUMNS, T>& rMatOut, const T value)
	{
		for (int i = 0; i < COLUMNS; ++i)
			for (int j = 0; j < ROWS; ++j)
			{
				rMatOut.m[i][j] = value;
			}
	}

	template<int ROWS, int COLUMNS, typename T>
	inline void MatrixFillIdentity(TMatrix<ROWS, COLUMNS, T>& rMatOut)
	{
		for (int i = 0; i < COLUMNS; ++i)
			for (int j = 0; j < ROWS; ++j)
			{
				rMatOut.m[i][j] = (i == j) ? T(1) : 0;
			}
	}

	template<int DIM, typename T>
	inline TMatrix<DIM, DIM, T> MatrixIdentity()
	{
		TMatrix<DIM, DIM, T> mat;
		MatrixFillIdentity(mat);
		return mat;
	}

	template<typename T>
	TMatrix<4, 4, T> MatrixRotationX(const T theta)
	{
		T c = cos(theta);
		T s = sin(theta);

		return TMatrix<4, 4, T>(
			TVector<4, T>(1, 0, 0, 0),
			TVector<4, T>(0, c, s, 0),
			TVector<4, T>(0, -s, c, 0),
			TVector<4, T>(0, 0, 0, 1)
		);
	}
	template<typename T>
	TMatrix<4, 4, T> MatrixRotationY(const T theta)
	{
		T c = cos(theta);
		T s = sin(theta);

		return TMatrix<4, 4, T>(
			TVector<4, T>(c, 0, -s, 0),
			TVector<4, T>(0, 1, 0, 0),
			TVector<4, T>(s, 0, c, 0),
			TVector<4, T>(0, 0, 0, 1)
			);
	}
	template<typename T>
	TMatrix<4, 4, T> MatrixRotationZ(const T theta)
	{
		T c = cos(theta);
		T s = sin(theta);

		return TMatrix<4, 4, T>(
			TVector<4, T>(c, s, 0, 0),
			TVector<4, T>(-s, c, 0, 0),
			TVector<4, T>(0, 0, 1, 0),
			TVector<4, T>(0, 0, 0, 1)
			);
	}
	template<typename T>
	TMatrix<4, 4, T> MatrixScale(const T x, const T y, const T z)
	{
		return TMatrix<4, 4, T>(
			TVector<4, T>(x, 0, 0, 0),
			TVector<4, T>(0, y, 0, 0),
			TVector<4, T>(0, 0, z, 0),
			TVector<4, T>(0, 0, 0, 1)
			);
	}
	template<typename T>
	TMatrix<4, 4, T> MatrixTranslate(const T x, const T y, const T z)
	{
		return TMatrix<4, 4, T>(
			TVector<4, T>(1, 0, 0, 0),
			TVector<4, T>(0, 1, 0, 0),
			TVector<4, T>(0, 0, 1, 0),
			TVector<4, T>(x, y, z, 1)
			);
	}

	template<typename T>
	TMatrix<4, 4, T> MatrixOrthoProjectLH(const T left, const T right, const T bottom, const T top, const T nearZ, const T farZ)
	{
		T w = right - left;
		T h = top - bottom;

		return TMatrix<4, 4, T>(
			TVector<4, T>(T(2) / w, 0, 0, 0),
			TVector<4, T>(0, T(2) / h, 0, 0),
			TVector<4, T>(0, 0,  T(1) / (farZ-nearZ), 0),
			TVector<4, T>(-(right+left)/w, -(top+bottom)/h, nearZ / (nearZ - farZ), 1)
			);
	}

	template<typename T>
	TMatrix<4, 4, T> MatrixOrthoProjectRH(const T left, const T right, const T bottom, const T top, const T nearZ, const T farZ)
	{
		T w = right - left;
		T h = top - bottom;

		return TMatrix<4, 4, T>(
			TVector<4, T>(T(2) / w, 0, 0, 0),
			TVector<4, T>(0, T(2) / h, 0, 0),
			TVector<4, T>(0, 0, -T(1) / (farZ - nearZ), 0),
			TVector<4, T>(-(right + left) / w, -(top + bottom) / h, nearZ / (nearZ - farZ), 1)
			);
	}

	template<typename T>
	TMatrix<4, 4, T> MatrixPerspectiveProjectLH(const T fovY, const T aspect, const T nearZ, const T farZ)
	{
		T a = T(1) / tan(fovY * T(0.5));
		T b = farZ / (farZ - nearZ);

		return TMatrix<4, 4, T>(
			TVector<4, T>(a / aspect, 0, 0, 0),
			TVector<4, T>(0, a, 0, 0),
			TVector<4, T>(0, 0, b, 1),
			TVector<4, T>(0, 0, -nearZ * b, 0)
			);
	}

	template<typename T>
	TMatrix<4, 4, T> MatrixPerspectiveProjectRH(const T fovY, const T aspect, const T nearZ, const T farZ)
	{
		T a = T(1) / tan(fovY * T(0.5));
		T b = farZ / (farZ - nearZ);

		return TMatrix<4, 4, T>(
			TVector<4, T>(a / aspect, 0, 0, 0),
			TVector<4, T>(0, a, 0, 0),
			TVector<4, T>(0, 0, -b, -1),
			TVector<4, T>(0, 0, -nearZ * b, 0)
			);
	}

	template<typename T>
	TMatrix<4, 4, T> MatrixLookatLH(const TVector<3, T>& eye, const TVector<3, T>& target, const TVector<3, T>& up)
	{
		TVector<3, T> w = normalize(target - eye);
		TVector<3, T> u = normalize(cross(up, w));
		TVector<3, T> v = cross(w,u);

		TVector<3, T> negEye = -eye;

		return TMatrix<4, 4, T>(
			TVector<4, T>(u.x, v.x, w.x, 0),
			TVector<4, T>(u.y, v.y, w.y, 0),
			TVector<4, T>(u.z, v.z, w.z, 0),
			TVector<4, T>(dot(negEye, u), dot(negEye, v), dot(negEye, w), 1)
			);
	}
	template<typename T>
	TMatrix<4, 4, T> MatrixLookatRH(const TVector<3, T>& eye, const TVector<3, T>& target, const TVector<3, T>& up)
	{
		TVector<3, T> w = -normalize(target - eye);
		TVector<3, T> u = normalize(cross(up, w)); // RH
		TVector<3, T> v = cross(w, u);

		TVector<3, T> negEye = -eye;

		return TMatrix<4, 4, T>(
			TVector<4, T>(u.x, v.x, w.x, 0),
			TVector<4, T>(u.y, v.y, w.y, 0),
			TVector<4, T>(u.z, v.z, w.z, 0),
			TVector<4, T>(dot(negEye, u), dot(negEye, v), dot(negEye, w), 1)
			);
	}


	template<int ROWS, int COLUMNS, typename T>
	inline bool EqualTol(const TMatrix<ROWS, COLUMNS, T>& lhs, const TMatrix<ROWS, COLUMNS, T>& rhs, const T tolerance)
	{
		for (int i = 0; i < COLUMNS; ++i)
		{
			for (int j = 0; j < ROWS; ++j)
			{
				if (std::abs(lhs.m[i][j] - rhs.m[i][j]) > tolerance)
				{
					return false;
				}
			}
		}
		return true;
	}

	template<int ROWS, int COLUMNS, typename T>
	inline bool EqualTolPtr(const TMatrix<ROWS, COLUMNS, T>& lhs, const T* pB, const T tolerance)
	{
		const T* pA = static_cast<const T*>(&lhs.m[0][0]);
		for (u32 i = 0; i < (ROWS*COLUMNS); ++i)
		{
			if ( std::abs(pA[i] - pB[i]) > tolerance)
			{
				return false;
			}
		}
		return true;
	}

	template<typename T>
	inline T det2x2(T a, T b, T c, T d)
	{
		return a * d - b * c;
	}

	template<typename T>
	inline T Determinant(const TMatrix<2, 2, T>& m)
	{
		return det2x2<T>(m.m[0][0], m.m[1][0], m.m[0][1], m.m[1][1]);
	}

	template<typename T>
	inline T Determinant(const TMatrix<3, 3, T>& m)
	{
		return m.m[0][0] * m.m[1][1] * m.m[2][2]
			+ m.m[1][0] * m.m[2][1] * m.m[0][2]
			+ m.m[2][0] * m.m[0][1] * m.m[1][2]
			- m.m[0][0] * m.m[2][1] * m.m[1][2]
			- m.m[1][0] * m.m[0][1] * m.m[2][2]
			- m.m[2][0] * m.m[1][1] * m.m[0][2]
			;
	}



	template<typename T>
	inline TMatrix<2, 2, T> Inverse(const TMatrix<2, 2, T>& m)
	{
		T d = Determinant(m);
		PLAY_ASSERT_MSG(d != 0.f, "Zero determinant");

		T f = T(1) / d;

		return TMatrix<2, 2, T>(
			TVector<2, T>(m.m[1][1] * f, -m.m[0][1] * f),
			TVector<2, T>(-m.m[1][0] * f, m.m[0][0] * f)
			);
	}


	template<typename T>
	inline TMatrix<3, 3, T> Inverse(const TMatrix<3, 3, T>& m)
	{
		T d = Determinant(m);
		PLAY_ASSERT_MSG(d != 0.f, "Zero determinant");

		T f = T(1) / d;

		T c00 = det2x2(m.m[1][1], m.m[2][1], m.m[1][2], m.m[2][2]) * f;
		T c10 = det2x2(m.m[0][1], m.m[2][1], m.m[0][2], m.m[2][2]) * -f;
		T c20 = det2x2(m.m[0][1], m.m[1][1], m.m[0][2], m.m[1][2]) * f;

		T c01 = det2x2(m.m[1][0], m.m[2][0], m.m[1][2], m.m[2][2]) * -f;
		T c11 = det2x2(m.m[0][0], m.m[2][0], m.m[0][2], m.m[2][2]) * f;
		T c21 = det2x2(m.m[0][0], m.m[1][0], m.m[0][2], m.m[1][2]) * -f;

		T c02 = det2x2(m.m[1][0], m.m[2][0], m.m[1][1], m.m[2][1]) * f;
		T c12 = det2x2(m.m[0][0], m.m[2][0], m.m[0][1], m.m[2][1]) * -f;
		T c22 = det2x2(m.m[0][0], m.m[1][0], m.m[0][1], m.m[1][1]) * f;

		return TMatrix<3, 3, T>(
			TVector<3, T>(c00, c10, c20),
			TVector<3, T>(c01, c11, c21),
			TVector<3, T>(c02, c12, c22)
			);
	}
}


//-----------------------------------------------------------
// Play3dImpl\ResourcesApi.h



namespace Play3d
{
	namespace Resources
	{

		template <typename T> class ResourceManager
		{
			ResourceManager()
			{
			}
			~ResourceManager()
			{
				ReleaseAll();
			}
		public:
			static ResourceManager& Instance() { static ResourceManager s_instance; return s_instance; }

			template<typename ... ConstructorArgs> IdKey<T> Create(ConstructorArgs ... args)
			{ 

				T* p = new T(args ...);
				PLAY_ASSERT(p);
				if (!p)
				{
					return IdKey<T>();
				}
				if (m_freelist.empty())
				{
					u32 index = (u32)m_objects.size();
					m_objects.push_back(p);

					return IdKey<T>(index);
				}
				else
				{
					u32 index = m_freelist.back();
					m_freelist.pop_back();

					m_objects[index] = p;
					return IdKey<T>(index);
				}
			}

			void Release(IdKey<T> id)
			{
				if(id.IsValid())
				{
					u32 index = id.GetValue();
					T* p = m_objects.at(index);
					PLAY_ASSERT(p);
;					m_objects.at(index) = nullptr;
					m_freelist.push_back(index);
					delete p;
				}
			}

			IdKey<T> Find(std::string_view name) const
			{ 
				return IdKey<T>();
			}

			T* GetPtr(IdKey<T> id) const
			{
				if (id.IsValid())
				{
					u32 index = id.GetValue();
					return m_objects.at(index);
				}
				return nullptr;
			}

			void AddAlias(std::string_view name, IdKey<T> id) {}

			void ReleaseAll()
			{
				for (u32 i = 0; i < (u32)m_objects.size(); ++i)
				{
					if(m_objects[i])
					{
						delete m_objects[i];
						m_objects[i] = nullptr;
						m_freelist.push_back(i);
					}
				}
			}

		private:
			std::vector<T*> m_objects;
			std::vector<uint32_t> m_freelist;
			std::unordered_map<std::string, uint32_t> m_namedObjectLUT;
		};

		template <typename T>
		inline IdKey<T> FindAsset(std::string_view name)
		{
			return ResourceManager<T>::Instance().Find(name);
		}

		template <typename T, typename ... ConstructorArgs>
		inline IdKey<T> CreateAsset(ConstructorArgs ... args)
		{
			return ResourceManager<T>::Instance().Create(args ...);
		}

		class AsyncLoadingTask {};

		IdKey<AsyncLoadingTask> AsyncLoadAssets(std::string_view pathToAssetPack);

		bool AssetsLoaded(IdKey<AsyncLoadingTask> hAsyncLoad);

		result_t LoadAssets(std::string_view pathToAssetPack);
	}
}

//-----------------------------------------------------------
// Play3dImpl\InputApi.h



namespace Play3d
{
	namespace Input
	{
		enum class InputDevice
		{
			GAMEPAD_0,
			GAMEPAD_1,
			GAMEPAD_2,
			GAMEPAD_3,
		};

		enum class ButtonId
		{
			BUTTON_DPAD_UP,
			BUTTON_DPAD_DOWN,
			BUTTON_DPAD_LEFT,
			BUTTON_DPAD_RIGHT,
			BUTTON_X,
			BUTTON_Y,
			BUTTON_A,
			BUTTON_B,
			BUTTON_LEFT_SHOULDER,
			BUTTON_RIGHT_SHOULDER,
			BUTTON_LEFT_STICK,
			BUTTON_RIGHT_STICK,
			BUTTON_START,
			BUTTON_BACK,
			BUTTON_LEFT_TRIGGER,
			BUTTON_RIGHT_TRIGGER,

			BUTTON_COUNT
		};

		enum class AxisId
		{
			AXIS_LEFTSTICK_X,
			AXIS_LEFTSTICK_Y,
			AXIS_RIGHTSTICK_X,
			AXIS_RIGHTSTICK_Y,

			AXIS_LEFT_TRIGGER_PRESSURE,
			AXIS_RIGHT_TRIGGER_PRESSURE,

			AXIS_COUNT
		};

		struct MouseState
		{
			f32 m_x;
			f32 m_y;
			f32 m_deltaX;
			f32 m_deltaY;
			bool m_leftButton;
			bool m_middleButton;
			bool m_rightButton;
		};

		bool IsKeyPressed(u32 keycode);
		bool IsKeyDown(u32 keycode);
		bool IsButtonPressed(InputDevice device, ButtonId buttonId);
		bool IsButtonDown(InputDevice device, ButtonId buttonId);
		float GetAxis(InputDevice device, AxisId axisId);

		const MouseState& GetMouseState();
		void CaptureMouse(bool bEnable);

	}
}

//-----------------------------------------------------------
// Play3dImpl\AudioApi.h




//-----------------------------------------------------------
// Play3dImpl\Sound.h



namespace Play3d
{
	namespace Audio
	{
		class Sound;
		using SoundId = IdKey<Sound>;

		struct SoundDesc
		{
			size_t m_sizeBytes; // Size of data
			const void* m_pData; // RIFF format audio data (PCM .wav file).
		};

		class Sound
		{
		public:
			Sound(const SoundDesc& rDesc);
			~Sound();
		private:
			friend class Audio_Impl;
			WAVEFORMATEX m_format;
			XAUDIO2_BUFFER m_buffer;
			void* m_pData;
			size_t m_sizeBytes;
		};


		struct Voice {};
		using VoiceId = IdKey<Voice>;

	}
}

namespace Play3d
{
	namespace Audio
	{
		SoundId LoadSoundFromFile(const char* filePath);
		VoiceId PlaySound(SoundId soundId, f32 fGain = 1.0f, f32 fPan = 0.5f);
		void StopSound(VoiceId voiceId);
	}
};


//-----------------------------------------------------------
// Play3dImpl\MeshBuilder.h




//-----------------------------------------------------------
// Play3dImpl\Colour.h


namespace Play3d
{
	class ColourValue
	{
	public:
		ColourValue() : m_value(0) {}

		explicit constexpr ColourValue(const u32 value) : m_value(value) {}

		explicit constexpr ColourValue(const u8 r, const u8 g, const u8 b, const u8 a = 255)
			: m_value(r | (g << 8) | (b << 16) | (a << 24))
		{}

		constexpr u32 as_u32() const { return m_value; }

		constexpr u32 with_alpha(u8 alpha) const { return (m_value & 0x00ffFFFF) | (alpha << 24); }

		constexpr void as_float_rgba_linear(f32 f[]) const {
			f[0] = f32((m_value) & 0xFF) / 255.f;
			f[1] = f32((m_value >> 8) & 0xFF) / 255.f;
			f[2] = f32((m_value >> 16) & 0xFF) / 255.f;
			f[3] = f32((m_value >> 24) & 0xFF) / 255.f;
		}

		void as_float_rgba_srgb(f32 f[]) const {
			f[0] = pow(f32((m_value) & 0xFF) / 255.f, 2.2f);
			f[1] = pow(f32((m_value >> 8) & 0xFF) / 255.f, 2.2f);
			f[2] = pow(f32((m_value >> 16) & 0xFF) / 255.f, 2.2f);
			f[3] = pow(f32((m_value >> 24) & 0xFF) / 255.f, 2.2f);
		}

	private:
		u32 m_value;
	};

	namespace Colour
	{
		constexpr ColourValue Aliceblue(240, 248, 255);
		constexpr ColourValue Antiquewhite(250, 235, 215);
		constexpr ColourValue Aqua(0, 255, 255);
		constexpr ColourValue Aquamarine(127, 255, 212);
		constexpr ColourValue Azure(240, 255, 255);
		constexpr ColourValue Beige(245, 245, 220);
		constexpr ColourValue Bisque(255, 228, 196);
		constexpr ColourValue Black(0, 0, 0);
		constexpr ColourValue Blanchedalmond(255, 235, 205);
		constexpr ColourValue Blue(0, 0, 255);
		constexpr ColourValue Blueviolet(138, 43, 226);
		constexpr ColourValue Brown(165, 42, 42);
		constexpr ColourValue Burlywood(222, 184, 135);
		constexpr ColourValue Cadetblue(95, 158, 160);
		constexpr ColourValue Chartreuse(127, 255, 0);
		constexpr ColourValue Chocolate(210, 105, 30);
		constexpr ColourValue Coral(255, 127, 80);
		constexpr ColourValue Cornflowerblue(100, 149, 237);
		constexpr ColourValue Cornsilk(255, 248, 220);
		constexpr ColourValue Crimson(220, 20, 60);
		constexpr ColourValue Cyan(0, 255, 255);
		constexpr ColourValue Darkblue(0, 0, 139);
		constexpr ColourValue Darkcyan(0, 139, 139);
		constexpr ColourValue Darkgoldenrod(184, 134, 11);
		constexpr ColourValue Darkgray(169, 169, 169);
		constexpr ColourValue Darkgreen(0, 100, 0);
		constexpr ColourValue Darkgrey(169, 169, 169);
		constexpr ColourValue Darkkhaki(189, 183, 107);
		constexpr ColourValue Darkmagenta(139, 0, 139);
		constexpr ColourValue Darkolivegreen(85, 107, 47);
		constexpr ColourValue Darkorange(255, 140, 0);
		constexpr ColourValue Darkorchid(153, 50, 204);
		constexpr ColourValue Darkred(139, 0, 0);
		constexpr ColourValue Darksalmon(233, 150, 122);
		constexpr ColourValue Darkseagreen(143, 188, 143);
		constexpr ColourValue Darkslateblue(72, 61, 139);
		constexpr ColourValue Darkslategray(47, 79, 79);
		constexpr ColourValue Darkslategrey(47, 79, 79);
		constexpr ColourValue Darkturquoise(0, 206, 209);
		constexpr ColourValue Darkviolet(148, 0, 211);
		constexpr ColourValue Deeppink(255, 20, 147);
		constexpr ColourValue Deepskyblue(0, 191, 255);
		constexpr ColourValue Dimgray(105, 105, 105);
		constexpr ColourValue Dimgrey(105, 105, 105);
		constexpr ColourValue Dodgerblue(30, 144, 255);
		constexpr ColourValue Firebrick(178, 34, 34);
		constexpr ColourValue Floralwhite(255, 250, 240);
		constexpr ColourValue Forestgreen(34, 139, 34);
		constexpr ColourValue Fuchsia(255, 0, 255);
		constexpr ColourValue Gainsboro(220, 220, 220);
		constexpr ColourValue Ghostwhite(248, 248, 255);
		constexpr ColourValue Gold(255, 215, 0);
		constexpr ColourValue Goldenrod(218, 165, 32);
		constexpr ColourValue Gray(128, 128, 128);
		constexpr ColourValue Green(0, 128, 0);
		constexpr ColourValue Greenyellow(173, 255, 47);
		constexpr ColourValue Grey(128, 128, 128);
		constexpr ColourValue Honeydew(240, 255, 240);
		constexpr ColourValue Hotpink(255, 105, 180);
		constexpr ColourValue Indianred(205, 92, 92);
		constexpr ColourValue Indigo(75, 0, 130);
		constexpr ColourValue Ivory(255, 255, 240);
		constexpr ColourValue Khaki(240, 230, 140);
		constexpr ColourValue Lavender(230, 230, 250);
		constexpr ColourValue Lavenderblush(255, 240, 245);
		constexpr ColourValue Lawngreen(124, 252, 0);
		constexpr ColourValue Lemonchiffon(255, 250, 205);
		constexpr ColourValue Lightblue(173, 216, 230);
		constexpr ColourValue Lightcoral(240, 128, 128);
		constexpr ColourValue Lightcyan(224, 255, 255);
		constexpr ColourValue Lightgoldenrodyellow(250, 250, 210);
		constexpr ColourValue Lightgray(211, 211, 211);
		constexpr ColourValue Lightgreen(144, 238, 144);
		constexpr ColourValue Lightgrey(211, 211, 211);
		constexpr ColourValue Lightpink(255, 182, 193);
		constexpr ColourValue Lightsalmon(255, 160, 122);
		constexpr ColourValue Lightseagreen(32, 178, 170);
		constexpr ColourValue Lightskyblue(135, 206, 250);
		constexpr ColourValue Lightslategray(119, 136, 153);
		constexpr ColourValue Lightslategrey(119, 136, 153);
		constexpr ColourValue Lightsteelblue(176, 196, 222);
		constexpr ColourValue Lightyellow(255, 255, 224);
		constexpr ColourValue Lime(0, 255, 0);
		constexpr ColourValue Limegreen(50, 205, 50);
		constexpr ColourValue Linen(250, 240, 230);
		constexpr ColourValue Magenta(255, 0, 255);
		constexpr ColourValue Maroon(128, 0, 0);
		constexpr ColourValue Mediumaquamarine(102, 205, 170);
		constexpr ColourValue Mediumblue(0, 0, 205);
		constexpr ColourValue Mediumorchid(186, 85, 211);
		constexpr ColourValue Mediumpurple(147, 112, 219);
		constexpr ColourValue Mediumseagreen(60, 179, 113);
		constexpr ColourValue Mediumslateblue(123, 104, 238);
		constexpr ColourValue Mediumspringgreen(0, 250, 154);
		constexpr ColourValue Mediumturquoise(72, 209, 204);
		constexpr ColourValue Mediumvioletred(199, 21, 133);
		constexpr ColourValue Midnightblue(25, 25, 112);
		constexpr ColourValue Mintcream(245, 255, 250);
		constexpr ColourValue Mistyrose(255, 228, 225);
		constexpr ColourValue Moccasin(255, 228, 181);
		constexpr ColourValue Navajowhite(255, 222, 173);
		constexpr ColourValue Navy(0, 0, 128);
		constexpr ColourValue Oldlace(253, 245, 230);
		constexpr ColourValue Olive(128, 128, 0);
		constexpr ColourValue Olivedrab(107, 142, 35);
		constexpr ColourValue Orange(255, 165, 0);
		constexpr ColourValue Orangered(255, 69, 0);
		constexpr ColourValue Orchid(218, 112, 214);
		constexpr ColourValue Palegoldenrod(238, 232, 170);
		constexpr ColourValue Palegreen(152, 251, 152);
		constexpr ColourValue Paleturquoise(175, 238, 238);
		constexpr ColourValue Palevioletred(219, 112, 147);
		constexpr ColourValue Papayawhip(255, 239, 213);
		constexpr ColourValue Peachpuff(255, 218, 185);
		constexpr ColourValue Peru(205, 133, 63);
		constexpr ColourValue Pink(255, 192, 203);
		constexpr ColourValue Plum(221, 160, 221);
		constexpr ColourValue Powderblue(176, 224, 230);
		constexpr ColourValue Purple(128, 0, 128);
		constexpr ColourValue Red(255, 0, 0);
		constexpr ColourValue Rosybrown(188, 143, 143);
		constexpr ColourValue Royalblue(65, 105, 225);
		constexpr ColourValue Saddlebrown(139, 69, 19);
		constexpr ColourValue Salmon(250, 128, 114);
		constexpr ColourValue Sandybrown(244, 164, 96);
		constexpr ColourValue Seagreen(46, 139, 87);
		constexpr ColourValue Seashell(255, 245, 238);
		constexpr ColourValue Sienna(160, 82, 45);
		constexpr ColourValue Silver(192, 192, 192);
		constexpr ColourValue Skyblue(135, 206, 235);
		constexpr ColourValue Slateblue(106, 90, 205);
		constexpr ColourValue Slategray(112, 128, 144);
		constexpr ColourValue Slategrey(112, 128, 144);
		constexpr ColourValue Snow(255, 250, 250);
		constexpr ColourValue Springgreen(0, 255, 127);
		constexpr ColourValue Steelblue(70, 130, 180);
		constexpr ColourValue Tan(210, 180, 140);
		constexpr ColourValue Teal(0, 128, 128);
		constexpr ColourValue Thistle(216, 191, 216);
		constexpr ColourValue Tomato(255, 99, 71);
		constexpr ColourValue Turquoise(64, 224, 208);
		constexpr ColourValue Violet(238, 130, 238);
		constexpr ColourValue Wheat(245, 222, 179);
		constexpr ColourValue White(255, 255, 255);
		constexpr ColourValue Whitesmoke(245, 245, 245);
		constexpr ColourValue Yellow(255, 255, 0);
		constexpr ColourValue Yellowgreen(154, 205, 50);

	}
}




//-----------------------------------------------------------
// Play3dImpl\Mesh.h



namespace Play3d
{
	namespace Graphics
	{
		class Mesh;
		using MeshId = IdKey<Mesh>;

		enum class StreamType
		{
			INDEX, // u32 indices

			POSITION, // float3
			COLOUR, // u32/colour
			NORMAL, // float3
			UV, // float2

			USER0, // custom - 
			USER1,
			USER2,
			USER3,
			USER4,

			COUNT,
		};

		namespace StreamFlags
		{
			enum Type
			{
				NONE = 0,
				DYNAMIC_STREAM,
				INSTANCE_STREAM
			};
		}
		struct StreamInfo
		{
			StreamType m_type = StreamType::POSITION;
			void* m_pData = nullptr;
			size_t m_dataSize = 0;
			u32 m_flags = StreamFlags::NONE; // StreamFlags::Type
			u32 m_stride = 0; // Used with for custom semantics.
			u32 m_offset = 0; // Used with custom semantics.
		};

		struct MeshDesc
		{
			StreamInfo* m_pStreams = nullptr;
			u32 m_streamCount = 0;
			u32 m_indexCount = 0;
			u32 m_vertexCount = 0;
		};

		class Mesh
		{
		public:
			Mesh(const MeshDesc& rDesc);
			~Mesh();
			void AddStream(const StreamInfo& info);
			void Bind(ID3D11DeviceContext* pDC) const;
		private:
			friend class Graphics_Impl;
			std::vector<StreamInfo> m_streamInfos;
			std::vector<ID3D11Buffer*> m_streamBuffers;
			std::vector<UINT> m_strides;
			std::vector<UINT> m_offsets;
			ID3D11Buffer* m_pIndexBuffer;

			u32 m_indexCount;
			u32 m_vertexCount;
		};
	}
}
namespace Play3d
{
	namespace Graphics
	{
		class MeshBuilder
		{
		public:
			u32 AddVertex(const Vector3f& position, const Vector3f& normal, const Vector2f& uv, ColourValue colour);


			void AddFace(u32 vertexCount, const Vector3f* positions, ColourValue colour, bool flip = false);

			void AddFace(u32 vertexCount, const Vector3f* positions, const ColourValue* colours, const Vector3f* normals, const Vector2f* uvs, bool flip = false);

			void AddTriangle(u32 i0, u32 i1, u32 i2);

			void AddTriangle(const Vector3f& P0, const Vector3f& P1, const Vector3f& P2, ColourValue colour, bool flip = false);

			void AddQuad(u32 i0, u32 i1, u32 i2, u32 i3);

			void AddQuad(const Vector3f& P0, const Vector3f& P1, const Vector3f& P2, const Vector3f& P3, ColourValue colour, bool flip = false);

			void AddFlatFace(u32 vertexCount, const Vector3f* positions, ColourValue colour, bool flip = false);

			void AddIndexedFlatFace(u32 indexCount, const u32* indices, const Vector3f* positions, ColourValue colour, bool flip = false);

			void Extrude(u32 vertexCount, const Vector3f* positions, ColourValue colour, f32 fLength);

			result_t ParseObjFormat(std::string_view objString, ColourValue coloure, f32 fScale);

			MeshId CreateMesh();

			void Reset();
		private:

			std::vector<Vector3f> m_positions;
			std::vector<Vector3f> m_normals;
			std::vector<Vector2f> m_uvs;
			std::vector<ColourValue> m_colours;
			std::vector<u32> m_indices;
		};
	}
}

//-----------------------------------------------------------
// Play3dImpl\GraphicsApi.h




//-----------------------------------------------------------
// Play3dImpl\Shader.h




namespace Play3d
{
	namespace Graphics
	{
		class Shader;

		using ShaderId = IdKey<Shader>;

		enum class ShaderType
		{
			VERTEX_SHADER,
			PIXEL_SHADER,

			MAX_SHADER_TYPES,
		};

		struct ShaderDesc
		{
			ShaderType m_type;
			std::string m_name; // optional debug annotation
			void* m_pByteCode;
			size_t m_sizeBytes;
		};

		enum class ShaderCompilationFlags
		{
			DEBUG = 1
		};

		struct ShaderCompilerDesc
		{
			ShaderType m_type;
			std::string m_name; // optional debug annotation
			std::string m_hlslCode;
			std::string m_entryPoint;
			std::vector<D3D_SHADER_MACRO> m_defines;
			u32 m_flags; // from ShaderCompilationFlags
		};

		class Shader
		{
		public:
			Shader(const ShaderDesc& rDesc);
			~Shader();

			ShaderType GetType() const { return m_shaderType; }

			void Bind(ID3D11DeviceContext* pContext);

			const void* GetByteCode() const { return m_pByteCode; }
			size_t GetByteCodeSize() const { return m_sizeBytes; }

			static ShaderId Compile(const ShaderCompilerDesc& rDesc);
		private:
			ShaderType m_shaderType;
			ComPtr<ID3D11DeviceChild> m_pShader; // COM cast to ID3D11VertexShader or ID3D11PixelShader
			void* m_pByteCode;
			size_t m_sizeBytes;
		};
	}
}


//-----------------------------------------------------------
// Play3dImpl\Material.h




//-----------------------------------------------------------
// Play3dImpl\Texture.h


namespace Play3d
{
	namespace Graphics
	{

		class Texture;
		using TextureId = IdKey<Texture>;

		enum class TextureFormat
		{
			GRAYSCALE,
			RGBA
		};

		struct TextureDesc
		{
			u32 m_width = 0;
			u32 m_height = 0;
			TextureFormat m_format = TextureFormat::RGBA;
			void* m_pImageData = nullptr;
			bool m_bGenerateMips = true;
		};

		class Texture
		{
		public:
			Texture(const TextureDesc& rDesc);
		private:
			friend class Graphics_Impl;
			ComPtr<ID3D11Texture2D> m_pTexture;
			ComPtr<ID3D11ShaderResourceView> m_pSRV;
		};
	}
}


//-----------------------------------------------------------
// Play3dImpl\Sampler.h


namespace Play3d
{
	namespace Graphics
	{

		class Sampler;
		using SamplerId = IdKey<Sampler>;

		enum class FilterMode
		{
			POINT,
			BILINEAR
		};

		enum class AddressMode
		{
			CLAMP,
			WRAP
		};

		struct SamplerDesc
		{
			FilterMode m_filter = FilterMode::BILINEAR;
			AddressMode m_addressMode = AddressMode::WRAP;
		};

		class Sampler
		{
		public:
			Sampler(const SamplerDesc& rDesc);
		private:
			friend class Graphics_Impl;
			ComPtr<ID3D11SamplerState> m_pSampler;
		};
	}
}
namespace Play3d
{
	namespace Graphics
	{
		class Material;
		using MaterialId = IdKey<Material>;


		enum class FillMode
		{
			SOLID,
			WIREFRAME,
		};

		enum class CullMode
		{
			BACK,
			FRONT,
			NONE
		};

		constexpr u32 kMaxMaterialTextureSlots = 4;

		struct MaterialStateSettings
		{
			FillMode m_fillMode = FillMode::SOLID;
			CullMode m_cullMode = CullMode::BACK;
		};

		struct MaterialConstantData
		{
			Vector4f diffuseColour;
			Vector4f specularColour;
		};

		struct SimpleMaterialDesc
		{
			MaterialStateSettings m_state;
			MaterialConstantData m_constants;
			TextureId m_texture[kMaxMaterialTextureSlots];
			SamplerId m_sampler[kMaxMaterialTextureSlots];
			
			u32 m_lightCount = 1;
			bool m_bEnableLighting = false;
		};

		struct ComplexMaterialDesc
		{
			MaterialStateSettings m_state;
			ShaderId m_VertexShader;
			ShaderId m_PixelShader;
			TextureId m_texture[kMaxMaterialTextureSlots];
			SamplerId m_sampler[kMaxMaterialTextureSlots];
			const void* m_pConstantData = nullptr;
			size_t m_dataSize = 0;
		};

		class Material
		{
		public:
			Material(const SimpleMaterialDesc& rDesc);
			Material(const ComplexMaterialDesc& rDesc);
			~Material();
		private:
			void SetupState(ID3D11Device* pDevice, const MaterialStateSettings& state);
			void SetupConstantBuffer(ID3D11Device* pDevice, const void* pData, size_t size);
			void SetupTextureBindings(ID3D11Device* pDevice, const TextureId* pTextureId, const SamplerId* pSamplerId);
		private:
			friend class Graphics_Impl;
			ComPtr<ID3D11RasterizerState> m_pRasterState;
			ComPtr<ID3D11Buffer> m_pMaterialConstants;
			ShaderId m_VertexShader;
			ShaderId m_PixelShader;
			TextureId m_texture[kMaxMaterialTextureSlots];
			SamplerId m_sampler[kMaxMaterialTextureSlots];
		};
	}
}
namespace Play3d
{
	namespace Graphics
	{
		enum class SurfaceFormat
		{
			DEPTH,
			RGBA
		};

		TextureId GetTempSurface(u32 width, u32 height, SurfaceFormat format);
		TextureId GetDisplaySurface();

		struct SurfaceSize
		{
			u32 m_width;
			u32 m_height;
		};

		SurfaceSize GetDisplaySurfaceSize();

		void SetDepthTarget(TextureId textureId);
		void SetRenderTarget(TextureId textureId);



		struct Viewport
		{
			Viewport() {};
			Viewport(const SurfaceSize& s) : x(0.f), y(0.f), width((f32)s.m_width), height((f32)s.m_height), minDepth(0.f), maxDepth(1.f) {};

			f32 x;
			f32 y;
			f32 width;
			f32 height;
			f32 minDepth;
			f32 maxDepth;
		};
		void SetViewport(const Viewport& viewport);
		void SetViewMatrix(const Matrix4x4f& m);
		void SetProjectionMatrix(const Matrix4x4f& m);

		void BeginPrimitiveBatch();
		void DrawPoint(const Vector3f& v1, ColourValue colour);
		void DrawLine(const Vector3f& v1, const Vector3f& v2, ColourValue colour);
		void DrawLine(const Vector3f& v1, const Vector3f& v2, ColourValue c1, ColourValue c2);
		void DrawTriangle(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, ColourValue colour);
		void DrawTriangle(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, ColourValue c1, ColourValue c2, ColourValue c3);
		void DrawQuad(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector3f& v4, ColourValue colour);
		void DrawQuad(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector3f& v4, ColourValue c1, ColourValue c2, ColourValue c3, ColourValue c4);
		void EndPrimitiveBatch();

		void DrawMesh(MeshId hMesh, const Matrix4x4f& transform);

		void SetMaterial(MaterialId materialId);

		void SetLightPosition(u32 index, const Vector3f& vPosition);
		void SetLightDirection(u32 index, const Vector3f& vDirection);
		void SetLightColour(u32 index, ColourValue colour);

		MeshId CreatePlane(f32 fWidth, f32 fHeight, ColourValue colour = Colour::White, f32 fUVScale = 1.0f);
		MeshId CreateMeshCube(f32 size, ColourValue colour = Colour::White);
		MeshId CreateMeshBox(f32 sizeX, f32 sizeY, f32 sizeZ, ColourValue colour = Colour::White);
		MeshId CreateCylinder(f32 fLength, f32 fRadius, u32 segments, ColourValue colour = Colour::White);
		MeshId CreateSphere(f32 fRadius, u32 segments, u32 slices, ColourValue colour = Colour::White);

		MeshId CreatePlatonicTetrahedron(ColourValue colour = Colour::White);
		MeshId CreatePlatonicHexahedron(ColourValue colour = Colour::White);
		MeshId CreatePlatonicOctahedron(ColourValue colour = Colour::White);

		MeshId CreateMeshFromObjString(std::string_view objString, ColourValue colour = Colour::White, f32 fScale = 1.0f);
		MeshId CreateMeshFromObjFile(const char* filePath, ColourValue colour = Colour::White, f32 fScale = 1.0f);


		TextureId CreateTextureCheckerboard(u32 width, u32 height, ColourValue a, ColourValue b, u32 checkSize);
		TextureId CreateTextureFromFile(const char* pFilePath);

		SamplerId CreateLinearSampler();
	}
}



//-----------------------------------------------------------
// Play3dImpl\UIApi.h




//-----------------------------------------------------------
// Play3dImpl\SpriteApi.h



namespace Play3d
{
	namespace Sprite
	{
		class SpriteAtlas;
		struct SpriteAtlasDesc {};
		using SpriteAtlasId = IdKey<SpriteAtlas>;
		class SpriteAtlas{};

		void DrawSprite(SpriteAtlasId hAtlas, Graphics::TextureId texture, u32 index);
	}
}


//-----------------------------------------------------------
// Play3dImpl\Font.h



namespace Play3d
{

	namespace UI
	{

		class Font;
		using FontId = IdKey<Font>;

		struct FontDesc
		{
			std::string m_fontName;
			std::string m_charSet;
			u32 m_pointSize;
			u32 m_textureWidth;
			u32 m_textureHeight;
		};
		
		struct FontVertex
		{
			Vector2f position;
			Vector2f uv;
			ColourValue colour;
		};

		class Font
		{
		public:
			Font(const FontDesc& rDesc);
			~Font();
			void DrawString(ID3D11DeviceContext* pDC, const Vector2f& position, ColourValue colour, std::string_view text);
		private:

			struct GlyphData
			{
				Vector2f uv0;
				Vector2f uv1;
				Vector2f offset0;
				Vector2f offset1;
				f32 xAdvance;
			};

			u8 m_glyphMap[256];
			std::vector<GlyphData> m_glyphData;
			ComPtr<ID3D11Texture2D> m_pTexture;
			ComPtr<ID3D11ShaderResourceView> m_pSRV;
			ComPtr<ID3D11SamplerState> m_pSampler;
			ComPtr<ID3D11Buffer> m_pVertexBuffer;
			u32 m_defaultAdvance;
		};

	}
}
namespace Play3d
{

	namespace UI
	{
		FontId GetDebugFont();
		void DrawPrintf(FontId hFont, const Vector2f& position, ColourValue colour, const char* fmt, ... );
		void DrawString(FontId hFont, const Vector2f& position, ColourValue colour, std::string_view text);
		bool DrawButton(Sprite::SpriteAtlasId hAtlas, Graphics::TextureId texture, u32 index);
	}
}



//-----------------------------------------------------------
// Play3dImpl\SystemApi.h

namespace Play3d
{
	namespace System 
	{
		bool IsInitialised();
		result_t Initialise();
		result_t BeginFrame();
		result_t EndFrame();
		result_t Shutdown();

		f64 GetElapsedTime();
		f32 GetDeltaTime();

		void* LoadFileData(const char* filePath, size_t& sizeOut);
		void ReleaseFileData(void* pMemory);
	}
}



//-----------------------------------------------------------
// Play3dImpl\DemoApi.h


namespace Play3d
{
	namespace Demo
	{
		void UpdateDebugCamera();
		
		void SetDebugCameraMatrices();

		void DrawDebugGrid(f32 fInterval = 1.0f, f32 fAxisLength = 1.0f);

		Graphics::MaterialId CreateComplexMaterial();
	}
}



#endif // __PLAY3D__

#ifdef PLAY_IMPLEMENTATION
////////////////// BEGIN IMPLEMENTATION SECTION //////////////////////////
#include <cstdarg>
#include <cstdio>
#include <windows.h>
#include <Xinput.h>
#include <charconv>
#include <d3dcompiler.h>

//-----------------------------------------------------------


//-----------------------------------------------------------
// Play3dImpl\AudioApi.cpp




//-----------------------------------------------------------
// Play3dImpl\Audio_Impl.h



namespace Play3d
{
	namespace Audio
	{
		class Audio_Impl
		{
			PLAY_NONCOPYABLE(Audio_Impl);
			PLAY_SINGLETON_INTERFACE(Audio_Impl);

			Audio_Impl();
			~Audio_Impl();
		public:
			void BeginFrame();

			void EndFrame();

			VoiceId PlaySound(SoundId soundId, f32 fGain, f32 fPan);

			void StopSound(VoiceId voiceId);

		private:
			u32 AllocateVoice();
			void ReleaseVoice(u32 slot);

			class AudioVoiceCB : public IXAudio2VoiceCallback
			{
			public:
				AudioVoiceCB() : m_bFinished(false) {}
				~AudioVoiceCB() {}
				void OnStreamEnd() { AllowRelease(); }
				void OnVoiceProcessingPassEnd() {}
				void OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
				void OnBufferEnd(void* pBufferContext) {}
				void OnBufferStart(void* pBufferContext) {}
				void OnLoopEnd(void* pBufferContext) {}
				void OnVoiceError(void* pBufferContext, HRESULT Error) { AllowRelease(); }

				bool IsFinished() const { return m_bFinished; }
				void Reset() { m_bFinished = false; }
				void AllowRelease() { m_bFinished = true; }
			private:
				std::atomic<bool> m_bFinished;
			};

			struct InternalVoice
			{
				IXAudio2SourceVoice* m_pVoice;
				AudioVoiceCB* m_pCallback;
			};

		private:

			ComPtr<IXAudio2> m_pXAudio2;
			IXAudio2MasteringVoice* m_pMasterVoice;
			std::vector<InternalVoice> m_voices;
			std::vector<u32> m_freelist;
		};
	}
}

namespace Play3d
{
	namespace Audio
	{

		SoundId LoadSoundFromFile(const char* filePath)
		{
			SoundId soundId;
			size_t sizeBytes;
			void* pData = System::LoadFileData(filePath, sizeBytes);
			if(pData)
			{
				SoundDesc desc;
				desc.m_pData = pData;
				desc.m_sizeBytes = sizeBytes;
				soundId = Resources::CreateAsset<Sound>(desc);
			}
			return soundId;
		}

		VoiceId PlaySound(SoundId soundId, f32 fGain, f32 fPan)
		{
			return Audio_Impl::Instance().PlaySound(soundId, fGain, fPan);
		}

		void StopSound(VoiceId voiceId)
		{
			return Audio_Impl::Instance().StopSound(voiceId);
		}

	}

} // namespace Play3d

//-----------------------------------------------------------
// Play3dImpl\Audio_Impl.cpp



#pragma comment(lib, "xaudio2.lib")

namespace Play3d
{
	namespace Audio
	{
		PLAY_SINGLETON_IMPL(Audio_Impl);

		Audio_Impl::Audio_Impl()
			: m_pMasterVoice(nullptr)
		{
			HRESULT hr;
			hr = XAudio2Create(&m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
			PLAY_ASSERT_MSG(SUCCEEDED(hr), "Failed XAudio create.");

			hr = m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice);
			PLAY_ASSERT_MSG(SUCCEEDED(hr), "Failed XAudio create master voice.");
		}

		Audio_Impl::~Audio_Impl()
		{
			if (m_pMasterVoice)
			{
				m_pMasterVoice->DestroyVoice();
			}

			for (InternalVoice& it : m_voices)
			{
				PLAY_SAFE_DELETE(it.m_pCallback);
			}
		}

		void Audio_Impl::BeginFrame()
		{

		}

		void Audio_Impl::EndFrame()
		{
			for (u32 slot = 0; slot < (u32)m_voices.size(); ++slot)
			{
				if (m_voices[slot].m_pCallback->IsFinished())
				{
					if (m_voices[slot].m_pVoice)
					{
						m_voices[slot].m_pVoice->DestroyVoice();
						m_voices[slot].m_pVoice = nullptr;
					}
					m_voices[slot].m_pCallback->Reset();

					ReleaseVoice(slot);
				}
			}
		}

		VoiceId Audio_Impl::PlaySound(SoundId soundId, f32 fGain, f32 fPan)
		{
			VoiceId voiceId;
			HRESULT hr;
			u32 slot = AllocateVoice();
			if(slot != ~0)
			{
				Sound* pSound = Resources::ResourceManager<Sound>::Instance().GetPtr(soundId);
				if (pSound)
				{
					IXAudio2SourceVoice* pVoice = nullptr;
					hr = m_pXAudio2->CreateSourceVoice(&pVoice, &pSound->m_format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, m_voices[slot].m_pCallback);
					PLAY_ASSERT(SUCCEEDED(hr));

					hr = pVoice->SubmitSourceBuffer(&pSound->m_buffer);
					PLAY_ASSERT(SUCCEEDED(hr));

					hr = pVoice->SetVolume(fGain);
					PLAY_ASSERT(SUCCEEDED(hr));

					f32 fLeft = cos(fPan * kfHalfPi);
					f32 fRight = sin(fPan * kfHalfPi);
					f32 channelVolumes[] = { fLeft, fRight };
					hr = pVoice->SetChannelVolumes(2, channelVolumes);
					PLAY_ASSERT(SUCCEEDED(hr));

					hr = pVoice->Start();
					PLAY_ASSERT(SUCCEEDED(hr));

					m_voices[slot].m_pVoice = pVoice;

					voiceId = VoiceId(slot);
				}
			}
			return voiceId;
		}

		void Audio_Impl::StopSound(VoiceId voiceId)
		{
			HRESULT hr;
			if (voiceId.IsValid())
			{
				u32 slot = voiceId.GetValue();
				hr = m_voices[slot].m_pVoice->Stop(0, 0);
				hr = m_voices[slot].m_pVoice->FlushSourceBuffers();
				m_voices[slot].m_pCallback->AllowRelease();
			}
		}

		u32 Audio_Impl::AllocateVoice()
		{
			u32 slot = ~0u;
			if (m_freelist.empty())
			{
				slot = (u32)m_voices.size();
				InternalVoice v;
				v.m_pCallback = new AudioVoiceCB();
				v.m_pVoice = nullptr;
				m_voices.push_back(v);
			}
			else
			{
				slot = m_freelist.back();
				m_freelist.pop_back();
			}
			return slot;
		}

		void Audio_Impl::ReleaseVoice(u32 slot)
		{
			m_freelist.push_back(slot);
		}

	}
}





//-----------------------------------------------------------
// Play3dImpl\DebugApi.cpp





namespace Play3d
{
	namespace Debug
	{
		void Put(const char* pStr)
		{
			OutputDebugStringA(pStr);
			::fputs(pStr, stdout);
		}

		void Printf(const char* pFmtStr, ...)
		{
			static constexpr u32 kMaxLogBuffer = 1024;
			char strBuffer[kMaxLogBuffer];
			va_list args;
			va_start(args, pFmtStr);
			vsprintf_s(strBuffer, kMaxLogBuffer, pFmtStr, args);
			va_end(args);

			Debug::Put(strBuffer);
		}

		void Tracef(const char* pStrFilename, unsigned int lineNum, const char* pFmtStr, ...)
		{
			static constexpr u32 kMaxLogBuffer = 1024;
			char strBuffer[kMaxLogBuffer];
			va_list args;
			va_start(args, pFmtStr);
			vsprintf_s(strBuffer, kMaxLogBuffer, pFmtStr, args);
			va_end(args);

			Debug::Printf("%s(%u): %s\n", pStrFilename, lineNum, strBuffer);
		}

		void HLine()
		{
			Debug::Put("--------------------------------------------------------\n");
		}
	}
}



//-----------------------------------------------------------
// Play3dImpl\DemoApi.cpp



namespace Play3d
{
	namespace Demo
	{

		struct DemoImpl
		{
			Vector3f m_eyePosition = Vector3f(0.f, 1.f, -3.5f);
			Vector3f m_vForward = Vector3f(0,0,1);
			Vector3f m_vRight = Vector3f(1,0,0);
			Vector3f m_vUp = Vector3f(0,1,0);

			f32 m_fAzimuth = 0.f;
			f32 m_fElevation = kfPi / 16;
			bool m_bIsCaptured = false;
		};

		static DemoImpl s_demoState;

		void UpdateDebugCamera()
		{
			constexpr f32 kfRotateSpeed = 0.005f;
			constexpr f32 kfSpeed = 1.0f;

			f32 dT = System::GetDeltaTime();
			Input::MouseState mouseState = Input::GetMouseState();

			if (mouseState.m_rightButton)
			{
				if (!s_demoState.m_bIsCaptured)
				{
					Input::CaptureMouse(true);
					s_demoState.m_bIsCaptured = true;
				}

				s_demoState.m_fAzimuth += mouseState.m_deltaX * -kfRotateSpeed;
				s_demoState.m_fElevation += mouseState.m_deltaY * kfRotateSpeed;

				if (s_demoState.m_fAzimuth > kfPi) { s_demoState.m_fAzimuth -= kfTwoPi; }
				if (s_demoState.m_fAzimuth < -kfPi) { s_demoState.m_fAzimuth += kfTwoPi; }

				if (s_demoState.m_fElevation > kfHalfPi) { s_demoState.m_fElevation = kfHalfPi; }
				if (s_demoState.m_fElevation < -kfHalfPi) { s_demoState.m_fElevation = -kfHalfPi; }
			}
			else if (s_demoState.m_bIsCaptured)
			{
				Input::CaptureMouse(false);
				s_demoState.m_bIsCaptured = false;
			}

			f32 cosAz = cos(s_demoState.m_fAzimuth);
			f32 sinAz = sin(s_demoState.m_fAzimuth);
			f32 cosEl = cos(s_demoState.m_fElevation);
			f32 sinEl = sin(s_demoState.m_fElevation);

			s_demoState.m_vRight = Vector3f(cosAz, 0.f, -sinAz);
			s_demoState.m_vUp = Vector3f(sinAz * sinEl, cosEl, cosAz * sinEl);
			s_demoState.m_vForward = Vector3f(sinAz * cosEl, -sinEl, cosAz * cosEl);

			Vector3f vDelta(0.f, 0.f, 0.f);

			if (Input::IsKeyDown('W'))
			{
				vDelta.z += 1;
			}
			if (Input::IsKeyDown('S'))
			{
				vDelta.z += -1;
			}
			if (Input::IsKeyDown('A'))
			{
				vDelta.x += 1;
			}
			if (Input::IsKeyDown('D'))
			{
				vDelta.x += -1;
			}
			if (Input::IsKeyDown('E'))
			{
				vDelta.y += -1;
			}
			if (Input::IsKeyDown('Q'))
			{
				vDelta.y += 1;
			}

			s_demoState.m_eyePosition += s_demoState.m_vForward * vDelta.z * dT * kfSpeed;
			s_demoState.m_eyePosition += s_demoState.m_vRight * vDelta.x * dT * kfSpeed;
			s_demoState.m_eyePosition += s_demoState.m_vUp * vDelta.y * dT * kfSpeed;

		}

		void SetDebugCameraMatrices()
		{
			Graphics::SurfaceSize surfaceSize = Graphics::GetDisplaySurfaceSize();
			
			f32 fovY(kfPi / 4.f), aspect((f32)surfaceSize.m_width / (f32)surfaceSize.m_height), nearZ(0.1f), farZ(10.f);

			Matrix4x4f view = MatrixLookatRH(s_demoState.m_eyePosition, s_demoState.m_eyePosition + s_demoState.m_vForward, s_demoState.m_vUp);
			Matrix4x4f project = MatrixPerspectiveProjectRH(fovY, aspect, nearZ, farZ);

			Graphics::SetViewport(Graphics::Viewport(surfaceSize)); // prob scissor too.
			Graphics::SetViewMatrix(view);
			Graphics::SetProjectionMatrix(project);
		}

		void DrawDebugGrid(f32 fInterval, f32 fAxisLength)
		{
			Graphics::BeginPrimitiveBatch();

			 Graphics::DrawLine(Vector3f(0, 0, 0), Vector3f(fAxisLength, 0, 0), Colour::Red);
			 Graphics::DrawLine(Vector3f(0, 0, 0), Vector3f(0, fAxisLength, 0), Colour::Green);
			 Graphics::DrawLine(Vector3f(0, 0, 0), Vector3f(0, 0, fAxisLength), Colour::Blue);

			const int kSize = 10;
			const f32 fSize = (f32)kSize;

			for (int i = -kSize; i <= kSize; ++i)
			{
				f32 t = fInterval * i;
				Graphics::DrawLine(Vector3f(t, 0, -fSize), Vector3f(t, 0, fSize), Colour::Darkgray);
				Graphics::DrawLine(Vector3f(-fSize, 0, t), Vector3f(fSize, 0, t), Colour::Darkgray);
			}

			Graphics::EndPrimitiveBatch();
		}

		Graphics::MaterialId CreateComplexMaterial()
		{
			using namespace Graphics;
			ShaderId customPixelShader;
			{
				static const char* hlslCode = R"(
				struct PSInput
				{
					float4 position : SV_POSITION;
					float4 colour : COLOUR;
				};
				float4 PS_Main(PSInput input) : SV_TARGET
				{
					return float4(input.position.x * 0.001, input.position.y*0.001, 0.5, 1.0);
				})";

				ShaderCompilerDesc compilerOptions = {};
				compilerOptions.m_name = "Example PS Shader";
				compilerOptions.m_type = Graphics::ShaderType::PIXEL_SHADER;
				compilerOptions.m_hlslCode = hlslCode;
				compilerOptions.m_entryPoint = "PS_Main";
				compilerOptions.m_defines.push_back({ "LIGHTS", "4" });
				customPixelShader = Shader::Compile(compilerOptions);
			}

			ComplexMaterialDesc desc = {};
			desc.m_state.m_cullMode = Graphics::CullMode::BACK;
			desc.m_state.m_fillMode = Graphics::FillMode::SOLID;
			desc.m_PixelShader = customPixelShader;

			return Resources::CreateAsset<Graphics::Material>(desc);
		}

	}
}


//-----------------------------------------------------------
// Play3dImpl\Font.cpp





//-----------------------------------------------------------
// Play3dImpl\Graphics_Impl.h




//-----------------------------------------------------------
// Play3dImpl\PrimitiveBatch_Impl.h



namespace Play3d
{
	namespace Graphics
	{

		struct PrimitiveVertex
		{
			Vector3f position;
			ColourValue colour;
		};

		class PrimitiveBatch
		{
		public:
			PrimitiveBatch(ID3D11Device* pDevice, u32 kMaxVertexCount);
			~PrimitiveBatch();

			void AppendPoint(const Vector3f& v1, ColourValue c1);
			void AppendLine(const Vector3f& v1, const Vector3f& v2, ColourValue c1, ColourValue c2);
			void AppendTriangle(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, ColourValue c1, ColourValue c2, ColourValue c3);

			void Flush(ID3D11DeviceContext* pContext);

			void Bind(ID3D11DeviceContext* pContext);

			void DrawPoints(ID3D11DeviceContext* pContext);

			void DrawLines(ID3D11DeviceContext* pContext);

			void DrawTriangles(ID3D11DeviceContext* pContext);
		private:
		
			std::vector<PrimitiveVertex> m_points; // Point List
			std::vector<PrimitiveVertex> m_lines; // Line List
			std::vector<PrimitiveVertex> m_triangles; // Triangle List
			ComPtr<ID3D11Buffer> m_pVertexBuffer; // single vertex buffer, each list appended with an offset up to 3 draw calls are made.
			u32 m_totalVertexCount;
			u32 m_maxVertexCount;
			u32 m_pointVertexCount;
			u32 m_lineVertexCount;
			u32 m_triangleVertexCount;

		};
	}
}



//-----------------------------------------------------------
// Play3dImpl\ShaderConstantBuffer_Impl.h



namespace Play3d {

	namespace Graphics {
		template<typename T> class ShaderConstantBuffer_Impl
		{
		public:
			ShaderConstantBuffer_Impl()
				: m_bIsDirty(false)
			{

			}
			void Init(ID3D11Device* pDevice)
			{
				D3D11_BUFFER_DESC desc = {};
				desc.ByteWidth = sizeof(T);
				desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;

				HRESULT hr = pDevice->CreateBuffer(&desc, NULL, &m_pBuffer);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "Could not create Constant Buffer");

				m_bIsDirty = true;
			}

			void Bind(ID3D11DeviceContext* pDC, u32 slot)
			{
				ID3D11Buffer* buffers[] = { m_pBuffer.Get() };
				pDC->VSSetConstantBuffers(slot, 1, buffers);
				pDC->PSSetConstantBuffers(slot, 1, buffers);
			}

			void UpdateGPU(ID3D11DeviceContext* pDC)
			{
				if (m_bIsDirty)
				{
					D3D11_MAPPED_SUBRESOURCE data;
					HRESULT hr = pDC->Map(m_pBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &data);
					if (SUCCEEDED(hr))
					{
						memcpy(data.pData, &m_cpuData, sizeof(T));
						pDC->Unmap(m_pBuffer.Get(), 0);
					}
					m_bIsDirty = false;
				}
			}

			bool IsDirty() const { return m_bIsDirty; }

			T& Get() { m_bIsDirty = true;  return m_cpuData; }

			const T& Get() const { return m_cpuData; }

		private:
			T m_cpuData;
			ComPtr<ID3D11Buffer> m_pBuffer;
			bool m_bIsDirty;
		};
	}
}
namespace Play3d
{
	namespace Graphics
	{
		

		struct MaterialShaderKey
		{
			union {
				struct {
					u32 m_pixelShader : 1;
					u32 m_useTexture0 : 1;
					u32 m_useLighting : 1;
					u32 m_lightCount : 2;
				} m_bits;
				u32 m_value;
			};
			
			static constexpr u32 kUsedBits = 5;
			static constexpr u32 kPermutations = 1 << kUsedBits;
			static constexpr u32 kMaxLights = 4;
		};

		class Graphics_Impl
		{
			PLAY_NONCOPYABLE(Graphics_Impl);
		public:
			using WindowCallback = std::function<int(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)>;

			static Graphics_Impl& Instance() { return *ms_pInstance; }
			static void Initialise();
			static void Destroy();

			result_t PostInitialise();
			result_t BeginFrame();
			result_t EndFrame();

			void Flush();

			HINSTANCE GetHInstance() const { return m_hInstance; }
			HWND GetHWnd() const { return m_hWnd; }

			ID3D11Device* GetDevice() const { return m_pDevice.Get(); }
			ID3D11DeviceContext* GetDeviceContext() const { return m_pDeviceContext.Get(); }

			PrimitiveBatch* AllocatePrimitiveBatch();

			void DrawPrimitveBatch(PrimitiveBatch* pBatch);

			void DrawMesh(const Mesh* pMesh);

			void SetViewport(const Viewport& v);

			void SetViewMatrix(const Matrix4x4f& m);

			void SetProjectionMatrix(const Matrix4x4f& m);

			void SetWorldMatrix(const Matrix4x4f& m);

			void UpdateUITransform();

			void SetMaterial(MaterialId materialId);

			SurfaceSize GetDisplaySurfaceSize() const;

			void TempPrepFontDraw();

			ShaderId GetMaterialShader(MaterialShaderKey key);

			void SetLightPosition(u32 index, const Vector3f& vPosition);

			void SetLightDirection(u32 index, const Vector3f& vDirection);

			void SetLightColour(u32 index, ColourValue colour);

			void QueueGenerateMips(ID3D11ShaderResourceView* pSRV);

			void RegisterWindowCallback(WindowCallback callback);

		private:
			Graphics_Impl();
			~Graphics_Impl();

			result_t InitWindow();

			result_t ReleaseWindow();

			result_t InitDirectX();

			result_t RecreateRenderTargetView();

			result_t RecreateDepthBuffer();

			result_t CompileInternalShaders();

			void CompileMaterialShader(MaterialShaderKey& key);

			result_t CreateInputLayouts();

			result_t CreatePipelineState();

			bool UpdateMessageLoop();

			void UpdateConstantBuffers();

			result_t Resize(u32 width, u32 height);

			static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		private:

			static Graphics_Impl* ms_pInstance;

			HINSTANCE m_hInstance;
			HWND m_hWnd;

			std::vector<WindowCallback> m_wndCallbacks;

			ComPtr<ID3D11Device> m_pDevice;
			ComPtr<ID3D11DeviceContext> m_pDeviceContext;
			ComPtr<IDXGIDevice> m_pDXGIDevice;
			ComPtr<IDXGIFactory2> m_pDXGIFactory;
			ComPtr<IDXGIAdapter> m_pDXGIAdapter;
			ComPtr<IDXGIOutput> m_pDXGIOutput;
			ComPtr<IDXGISwapChain1> m_pSwapChain;
			ComPtr<ID3D11Texture2D> m_pDefaultDepthBuffer;
			ComPtr<ID3D11RenderTargetView> m_pBackBufferRTV;
			ComPtr<ID3D11DepthStencilView> m_pDefaultDSV;

			ComPtr<ID3D11InputLayout> m_pPrimitiveInputLayout;
			ComPtr<ID3D11InputLayout> m_pMeshInputLayout;
			ComPtr<ID3D11InputLayout> m_pFontInputLayout;

			struct FrameConstantData
			{
				Matrix4x4f viewMtx;
				Matrix4x4f projectionMtx;
				Matrix4x4f viewProjectionMtx;
			};
			ShaderConstantBuffer_Impl<FrameConstantData> m_frameConstants; // b0

			struct DrawConstantData
			{
				Matrix4x4f mvpMtx;
				Matrix4x4f worldMtx;
				Matrix4x4f normalMtx; // inverse_transpose(world)
			};
			ShaderConstantBuffer_Impl<DrawConstantData> m_drawConstants; // b1

			
			struct LightConstantData
			{
				Vector4f lightPos[MaterialShaderKey::kMaxLights];
				Vector4f lightDir[MaterialShaderKey::kMaxLights];
				Vector4f lightColour[MaterialShaderKey::kMaxLights];
			};
			ShaderConstantBuffer_Impl<LightConstantData> m_lightConstants; // b2


			ShaderConstantBuffer_Impl<MaterialConstantData> m_materialConstants; // b3

			struct UIFrameConstantData
			{
				Matrix4x4f viewProjectionMtx; // ortho matrix suited to text with +Y down the screen, origin top left
				Vector4f viewportRect; // left, top, width, height (e.g. 0, 0, 1024, 768)
			};
			ShaderConstantBuffer_Impl<UIFrameConstantData> m_uiFrameConstants; // b0


			u32 m_nSurfaceWidth;
			u32 m_nSurfaceHeight;

			ShaderId m_primitiveBatchVS;
			ShaderId m_primitiveBatchPS;
			ShaderId m_fontVS;
			ShaderId m_fontPS;
			ShaderId m_meshShaders[MaterialShaderKey::kPermutations];

			MaterialId m_activeMaterial;

			ComPtr<ID3D11RasterizerState> m_pFallbackRasterState;

			ComPtr<ID3D11BlendState> m_pBlendStateOpaque;
			ComPtr<ID3D11BlendState> m_pBlendStateAdditive;

			std::vector<PrimitiveBatch*> m_primitiveBatchRing;
			u32 m_nNextPrimitiveBatch;

			std::vector<ID3D11ShaderResourceView*> m_mipQueue;
		};
	}
};
namespace Play3d
{
	namespace UI
	{

		Font::Font(const FontDesc& rDesc)
		{
			HDC hdc = CreateCompatibleDC(NULL);
			HBITMAP hBitmap = CreateCompatibleBitmap(hdc, 1, 1);
			SelectObject(hdc, hBitmap);

			HFONT hFont = CreateFontA(rDesc.m_pointSize, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, ANSI_CHARSET, OUT_OUTLINE_PRECIS,
				CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, rDesc.m_fontName.c_str());

			SelectObject(hdc, hFont);

			u32 nGlyphs = (u32)rDesc.m_charSet.size();
			PLAY_ASSERT(0 < nGlyphs && nGlyphs < 256);


			memset(m_glyphMap, 0xff, 256);
			for (u32 i = 0; i < nGlyphs; ++i)
			{
				m_glyphMap[rDesc.m_charSet[i]] = (u8)i;
			}

			u32 scratchSizeBytes = 32 * 32;
			u8* pScratchImage = new u8[scratchSizeBytes];

			u32 imageWidth = rDesc.m_textureWidth;
			u32 imageHeight = rDesc.m_textureHeight;

			u32 imageSizeBytes = imageWidth * imageHeight;
			u8* pImage = new u8[imageSizeBytes];
			memset(pImage, 0, imageSizeBytes);

			GLYPHMETRICS glyphMetrics = {};
			MAT2 mat{ {0, 1}, {0, 0}, {0, 0}, {0, 1} };

			const u32 tilePadding = 1;
			u32 tilePosX = tilePadding;
			u32 tilePosY = tilePadding;
			u32 maxY = 0;

			const f32 invWidth = 1.0f / imageWidth;
			const f32 invHeight = 1.0f / imageHeight;

			m_glyphData.resize(nGlyphs);

			m_defaultAdvance = 0;
			u32 validGlyphCount = 0;

			for (u32 glyphIndex = 0; glyphIndex < nGlyphs; ++glyphIndex)
			{
				u32 charInSet = rDesc.m_charSet[glyphIndex];
				u32 result = GetGlyphOutlineA(hdc, charInSet, GGO_GRAY8_BITMAP, &glyphMetrics, scratchSizeBytes, pScratchImage, &mat);
				if (GDI_ERROR != result)
				{
					u32 rowPitch = (glyphMetrics.gmBlackBoxX + 4 - 1) & ~3;

					if ((tilePosX + rowPitch + tilePadding) >= imageWidth)
					{
						tilePosX = tilePadding;
						tilePosY += maxY + tilePadding;
						maxY = 0;
					}

					if (maxY < glyphMetrics.gmBlackBoxY)
					{
						maxY = glyphMetrics.gmBlackBoxY;
					}

					u8* pRasterPos = pImage + (tilePosY * imageWidth + tilePosX);

					for (u32 i = 0; i < glyphMetrics.gmBlackBoxY; ++i) // rows
					{
						for (u32 j = 0; j < rowPitch; ++j)
						{
							pRasterPos[j] = (u8)((u32)pScratchImage[i * rowPitch + j] * 255 / 64);
						}
						pRasterPos += imageWidth;
					}

					GlyphData& rGlyphData(m_glyphData[glyphIndex]);
					rGlyphData.uv0 = Vector2f(tilePosX * invWidth, tilePosY * invHeight);
					rGlyphData.uv1 = Vector2f((tilePosX + glyphMetrics.gmBlackBoxX) * invWidth, (tilePosY + glyphMetrics.gmBlackBoxY) * invHeight);
					rGlyphData.offset0 = Vector2f((f32)glyphMetrics.gmptGlyphOrigin.x, (f32)-glyphMetrics.gmptGlyphOrigin.y);
					rGlyphData.offset1 = rGlyphData.offset0 + Vector2f((f32)glyphMetrics.gmBlackBoxX, (f32)glyphMetrics.gmBlackBoxY);
					rGlyphData.xAdvance = glyphMetrics.gmCellIncX;
					tilePosX += rowPitch + tilePadding;

					PLAY_ASSERT_MSG(glyphMetrics.gmCellIncX >= 0, "Unexpected");
					m_defaultAdvance += (u32)glyphMetrics.gmCellIncX;
					++validGlyphCount;
				}
				else
				{
					Debug::Printf("Missing glyph (0x%x)\n", charInSet);
					m_glyphMap[charInSet] = 0xFF;
				}
			}

			if(validGlyphCount)
				m_defaultAdvance /= validGlyphCount; // average char width


			ID3D11Device* pDevice = Graphics::Graphics_Impl::Instance().GetDevice();

			{
				D3D11_TEXTURE2D_DESC desc = {};
				desc.Width = imageWidth;
				desc.Height = imageHeight;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Usage = D3D11_USAGE_IMMUTABLE;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA data;
				data.pSysMem = pImage;
				data.SysMemPitch = imageWidth;

				HRESULT hr = pDevice->CreateTexture2D(&desc, &data, &m_pTexture);
				PLAY_ASSERT(SUCCEEDED(hr));

				hr = pDevice->CreateShaderResourceView(m_pTexture.Get(), NULL, &m_pSRV);
				PLAY_ASSERT(SUCCEEDED(hr));
			}

			{
				D3D11_SAMPLER_DESC desc = {};
				desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

				HRESULT hr = pDevice->CreateSamplerState(&desc, &m_pSampler);
				PLAY_ASSERT(SUCCEEDED(hr));
			}

			delete[] pScratchImage;
			delete[] pImage;


			{
				const u32 kMaxVertexCount = 0x10000u;

				D3D11_BUFFER_DESC desc = {};
				desc.ByteWidth = sizeof(FontVertex) * kMaxVertexCount;
				desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;
				HRESULT hr = pDevice->CreateBuffer(&desc, NULL, &m_pVertexBuffer);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "Create Font VertexBuffer");
			}
		}

		Font::~Font()
		{

		}

		void Font::DrawString(ID3D11DeviceContext* pDC, const Vector2f& position, ColourValue colour, std::string_view text)
		{
			D3D11_MAPPED_SUBRESOURCE data;
			HRESULT hr = pDC->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &data);
			if (SUCCEEDED(hr))
			{
				FontVertex* pVertex = (FontVertex*)data.pData;

				Vector2f cursor(position);

				for (u32 i = 0; i < (u32)text.size(); ++i)
				{
					u8 glyphIndex = m_glyphMap[text[i]];
					if (0xFF == glyphIndex)
					{
						cursor.x += m_defaultAdvance;
						continue;
					}

					const GlyphData& glyph(m_glyphData[glyphIndex]);

					Vector2f v0 = cursor + glyph.offset0;
					Vector2f v1 = cursor + glyph.offset1;

					pVertex[0].position = Vector2f(v0.x, v0.y);
					pVertex[1].position = Vector2f(v1.x, v0.y);
					pVertex[2].position = Vector2f(v1.x, v1.y);
					pVertex[3].position = Vector2f(v0.x, v0.y);
					pVertex[4].position = Vector2f(v1.x, v1.y);
					pVertex[5].position = Vector2f(v0.x, v1.y);

					pVertex[0].uv = Vector2f(glyph.uv0.x, glyph.uv0.y);
					pVertex[1].uv = Vector2f(glyph.uv1.x, glyph.uv0.y);
					pVertex[2].uv = Vector2f(glyph.uv1.x, glyph.uv1.y);
					pVertex[3].uv = Vector2f(glyph.uv0.x, glyph.uv0.y);
					pVertex[4].uv = Vector2f(glyph.uv1.x, glyph.uv1.y);
					pVertex[5].uv = Vector2f(glyph.uv0.x, glyph.uv1.y);

					pVertex[0].colour = colour;
					pVertex[1].colour = colour;
					pVertex[2].colour = colour;
					pVertex[3].colour = colour;
					pVertex[4].colour = colour;
					pVertex[5].colour = colour;

					pVertex += 6;

					cursor.x += glyph.xAdvance;
				}

				pDC->Unmap(m_pVertexBuffer.Get(), 0);
			}



			Graphics::Graphics_Impl::Instance().TempPrepFontDraw();

			pDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			ID3D11Buffer* buffers[] = { m_pVertexBuffer.Get() };
			UINT strides[] = { sizeof(FontVertex) };
			UINT offsets[] = { 0 };
			pDC->IASetVertexBuffers(0, 1, buffers, strides, offsets);

			ID3D11ShaderResourceView* srvs[] = { m_pSRV.Get() };
			pDC->PSSetShaderResources(0, 1, srvs);

			ID3D11SamplerState* samplers[] = { m_pSampler.Get() };
			pDC->PSSetSamplers(0, 1, samplers);

			pDC->Draw((u32)text.size() * 6, 0);
		}

	}
}



//-----------------------------------------------------------
// Play3dImpl\GraphicsApi.cpp




namespace Play3d
{
	namespace Graphics
	{
		struct InternalState
		{
			PrimitiveBatch* m_pCurrentPrimitiveBatch;
		};
		static InternalState s_internalState;

		TextureId GetTempSurface(u32 width, u32 height, SurfaceFormat format)
		{
			return TextureId();
		}

		TextureId GetDisplaySurface()
		{
			return TextureId();
		}

		SurfaceSize GetDisplaySurfaceSize()
		{
			return { Graphics_Impl::Instance().GetDisplaySurfaceSize() };
		}

		void SetDepthTarget(TextureId textureId)
		{

		}

		void SetRenderTarget(TextureId textureId)
		{

		}

		void SetViewport(const Viewport& viewport)
		{
			Graphics_Impl::Instance().SetViewport(viewport);
		}

		void SetViewMatrix(const Matrix4x4f& m)
		{
			Graphics_Impl::Instance().SetViewMatrix(m);
		}

		void SetProjectionMatrix(const Matrix4x4f& m)
		{
			Graphics_Impl::Instance().SetProjectionMatrix(m);
		}

		void BeginPrimitiveBatch()
		{
			if (!s_internalState.m_pCurrentPrimitiveBatch)
			{
				s_internalState.m_pCurrentPrimitiveBatch = Graphics_Impl::Instance().AllocatePrimitiveBatch();
			}
		}

		void DrawPoint(const Vector3f& v1, ColourValue colour)
		{
			PLAY_ASSERT(s_internalState.m_pCurrentPrimitiveBatch);
			s_internalState.m_pCurrentPrimitiveBatch->AppendPoint(v1, colour);
		}

		void DrawLine(const Vector3f& v1, const Vector3f& v2, ColourValue colour)
		{
			PLAY_ASSERT(s_internalState.m_pCurrentPrimitiveBatch);
			s_internalState.m_pCurrentPrimitiveBatch->AppendLine(v1, v2, colour, colour);
		}

		void DrawLine(const Vector3f& v1, const Vector3f& v2, ColourValue c1, ColourValue c2)
		{
			PLAY_ASSERT(s_internalState.m_pCurrentPrimitiveBatch);
			s_internalState.m_pCurrentPrimitiveBatch->AppendLine(v1, v2, c1, c2);
		}

		void DrawTriangle(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, ColourValue colour)
		{
			PLAY_ASSERT(s_internalState.m_pCurrentPrimitiveBatch);
			s_internalState.m_pCurrentPrimitiveBatch->AppendTriangle(v1, v2, v3, colour, colour, colour);
		}

		void DrawTriangle(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, ColourValue c1, ColourValue c2, ColourValue c3)
		{
			PLAY_ASSERT(s_internalState.m_pCurrentPrimitiveBatch);
			s_internalState.m_pCurrentPrimitiveBatch->AppendTriangle(v1, v2, v3, c1, c2, c3);
		}

		void DrawQuad(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector3f& v4, ColourValue colour)
		{
			PLAY_ASSERT(s_internalState.m_pCurrentPrimitiveBatch);
			s_internalState.m_pCurrentPrimitiveBatch->AppendTriangle(v1, v2, v3, colour, colour, colour);
			s_internalState.m_pCurrentPrimitiveBatch->AppendTriangle(v2, v4, v3, colour, colour, colour);
		}

		void DrawQuad(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector3f& v4, ColourValue c1, ColourValue c2, ColourValue c3, ColourValue c4)
		{
			PLAY_ASSERT(s_internalState.m_pCurrentPrimitiveBatch);
			s_internalState.m_pCurrentPrimitiveBatch->AppendTriangle(v1, v2, v3, c1, c2, c3);
			s_internalState.m_pCurrentPrimitiveBatch->AppendTriangle(v2, v4, v3, c2, c4, c3);
		}

		void EndPrimitiveBatch()
		{
			if (s_internalState.m_pCurrentPrimitiveBatch)
			{
				Graphics_Impl::Instance().DrawPrimitveBatch(s_internalState.m_pCurrentPrimitiveBatch);
				s_internalState.m_pCurrentPrimitiveBatch = nullptr;
			}
		}

		void DrawMesh(MeshId hMesh, const Matrix4x4f& transform)
		{
			Graphics_Impl::Instance().SetWorldMatrix(transform);

			Mesh* pMesh = Resources::ResourceManager<Mesh>::Instance().GetPtr(hMesh);
			if (pMesh)
			{
				Graphics_Impl::Instance().DrawMesh(pMesh);
			}
		}

		void SetMaterial(MaterialId materialId)
		{
			Graphics_Impl::Instance().SetMaterial(materialId);

		}

		void SetLightPosition(u32 index, const Vector3f& vPosition)
		{
			Graphics_Impl::Instance().SetLightPosition(index, vPosition);
		}

		void SetLightDirection(u32 index, const Vector3f& vDirection)
		{
			Graphics_Impl::Instance().SetLightDirection(index, vDirection);
		}

		void SetLightColour(u32 index, ColourValue colour)
		{
			Graphics_Impl::Instance().SetLightColour(index, colour);
		}

		MeshId CreatePlane(f32 fHalfSizeX, f32 fHalfSizeZ, ColourValue colour /*= Colour::White*/, f32 fUVScale /*= 1.0f*/)
		{
			MeshBuilder builder;
			builder.AddVertex(Vector3f(-fHalfSizeX, 0, -fHalfSizeZ), Vector3f(0, 1, 0), Vector2f(0, 0), colour);
			builder.AddVertex(Vector3f( fHalfSizeX, 0, -fHalfSizeZ), Vector3f(0, 1, 0), Vector2f(fUVScale, 0), colour);
			builder.AddVertex(Vector3f( fHalfSizeX, 0, fHalfSizeZ), Vector3f(0, 1, 0), Vector2f(fUVScale, fUVScale), colour);
			builder.AddVertex(Vector3f(-fHalfSizeX, 0, fHalfSizeZ), Vector3f(0, 1, 0), Vector2f(0, fUVScale), colour);
			builder.AddQuad(0, 1, 2, 3);
			return builder.CreateMesh();
		}

		MeshId CreateMeshCube(f32 size, ColourValue colour)
		{
			return CreateMeshBox(size, size, size, colour);
		}

		MeshId CreateMeshBox(f32 sizeX, f32 sizeY, f32 sizeZ, ColourValue colour)
		{
			const f32 x = sizeX/2.f;
			const f32 y = sizeY/2.f;
			const f32 z = sizeZ/2.f;

			Vector3f positions[] = {
				Vector3f(-x, -y, -z),
				Vector3f(x, -y, -z),
				Vector3f(x, y, -z),
				Vector3f(-x, y, -z),

				Vector3f(-x, -y, z),
				Vector3f(x,  -y, z),
				Vector3f(x,  y, z),
				Vector3f(-x, y, z)
			};

			MeshBuilder builder;

			u32 indices[] = {
				0, 3, 2, 1,
				0, 1, 5, 4,
				0, 4, 7, 3,
				6, 5, 1, 2,
				6, 2, 3, 7,
				6, 7, 4, 5,
			};
			builder.AddIndexedFlatFace(4, indices, positions, colour);
			builder.AddIndexedFlatFace(4, indices + 4, positions, colour);
 			builder.AddIndexedFlatFace(4, indices + 8, positions, colour);
 			builder.AddIndexedFlatFace(4, indices + 12, positions, colour);
			builder.AddIndexedFlatFace(4, indices + 16, positions, colour);
 			builder.AddIndexedFlatFace(4, indices + 20, positions, colour);
			return builder.CreateMesh();
		}

		MeshId CreateCylinder(f32 fLength, f32 fRadius, u32 segments, ColourValue colour /*= Colour::White*/)
		{
			std::vector<Vector3f> shape(segments);
			f32 fStep = kfTwoPi / segments;
			for (u32 i = 0; i < segments; ++i)
			{
				f32 theta = -fStep * i;
				shape[i] = Vector3f(sin(theta) * fRadius, 0.f, cos(theta) * fRadius);
			}
			MeshBuilder builder;
			builder.Extrude(segments, shape.data(), colour, fLength);
			return builder.CreateMesh();
		}

		MeshId CreateSphere(f32 fRadius, u32 segments, u32 slices, ColourValue colour /*= Colour::White*/)
		{
			MeshBuilder builder;
			const f32 r0 = kfPi / (slices + 1);
			const f32 r1 = kfTwoPi / segments;

			for (u32 i = 0; i < slices; ++i)
			{		
				f32 phi = r0 * (i+1);
				f32 sphi = sin(phi);
				f32 cphi = cos(phi);

				for (u32 j = 0; j < segments; ++j)
				{
					f32 theta = r1 * (j + 1);
					f32 stheta = sin(theta);
					f32 ctheta = cos(theta);
					Vector3f N(ctheta * sphi, cphi, stheta * sphi);
					builder.AddVertex(N * fRadius, N, Vector2f(0, 0), colour);
				}
			}

			u32 iCap0 = builder.AddVertex(Vector3f(0, fRadius, 0), Vector3f(0, 1.f, 0), Vector2f(0, 0), colour);
			u32 iCapN = builder.AddVertex(Vector3f(0, -fRadius, 0), Vector3f(0, -1.f, 0), Vector2f(0, 0), colour);

			for (u32 i = 0; i < (slices-1); ++i)
			{
				for (u32 j = 0; j < segments; ++j)
				{
					u32 iA = i * segments + j;
					u32 iB = (i + 1) * segments + j;
					u32 iC = (i + 1) * segments + ((j + 1) % segments);
					u32 iD = i * segments + ((j + 1) % segments);
					builder.AddQuad(iA, iB, iC, iD);
				}
			}

			for (u32 i = 0; i < segments; ++i)
			{
				u32 iA = iCap0;
				u32 iB = i;
				u32 iC = (i + 1) % segments;
				builder.AddTriangle(iA, iB, iC);
			}

			u32 iEndCap0 = iCapN - segments - 1;
			for (u32 i = 0; i < segments; ++i)
			{
				u32 iA = iCapN;
				u32 iB = iEndCap0 + i;
				u32 iC = iEndCap0 + ((i + 1) % segments);
				builder.AddTriangle(iA, iB, iC);
			}

			return builder.CreateMesh();
		}

		MeshId CreatePlatonicTetrahedron(ColourValue colour /*= Colour::White*/)
		{
			const f32 a = sqrt(2.0f) / 3.0f;
			const f32 b = sqrt(6.0f) / 3.0f;
			const f32 c = -1.f/3.f;

			Vector3f positions[] = {
				Vector3f(0, 0, 1),
				Vector3f(2.f*a, 0, c),
				Vector3f(-a, b, c),
				Vector3f(-a, -b, c),
			};

			MeshBuilder builder;

			u32 indices[] = {
				0, 1, 2,
				0, 2, 3,
				0, 3, 1,
				1, 3, 2,
			};

			builder.AddIndexedFlatFace(3, indices, positions, colour);
			builder.AddIndexedFlatFace(3, indices + 3, positions, colour);
			builder.AddIndexedFlatFace(3, indices + 6, positions, colour);
			builder.AddIndexedFlatFace(3, indices + 9, positions, colour);
			return builder.CreateMesh();
		}

		MeshId CreatePlatonicHexahedron(ColourValue colour /*= Colour::White*/)
		{
			return CreateMeshCube(2.0f / sqrt(3.f));
		}

		Play3d::Graphics::MeshId CreatePlatonicOctahedron(ColourValue colour /*= Colour::White*/)
		{
			Vector3f positions[] = {
				Vector3f(1,0,0),
				Vector3f(-1,0,0),
				Vector3f(0,1,0),
				Vector3f(0,-1,0),
				Vector3f(0,0,1),
				Vector3f(0,0,-1),
			};

			MeshBuilder builder;

			u32 indices[] = {
				4,0,2,
				4,2,1,
				4,1,3,
				4,3,0,

				5,2,0,
				5,1,2,
				5,3,1,
				5,0,3,
			};

			builder.AddIndexedFlatFace(3, indices, positions, colour);
			builder.AddIndexedFlatFace(3, indices + 3, positions, colour);
			builder.AddIndexedFlatFace(3, indices + 6, positions, colour);
			builder.AddIndexedFlatFace(3, indices + 9, positions, colour);
			builder.AddIndexedFlatFace(3, indices + 12, positions, colour);
			builder.AddIndexedFlatFace(3, indices + 15, positions, colour);
			builder.AddIndexedFlatFace(3, indices + 18, positions, colour);
			builder.AddIndexedFlatFace(3, indices + 21, positions, colour);
			return builder.CreateMesh();
		}

		MeshId CreateMeshFromObjString(std::string_view objString, ColourValue colour, f32 fScale)
		{
			MeshBuilder builder;
			result_t result = builder.ParseObjFormat(objString, colour, fScale);
			if (RESULT_OK == result)
			{
				return builder.CreateMesh();
			}

			return MeshId();
		}

		MeshId CreateMeshFromObjFile(const char* filePath, ColourValue colour, f32 fScale)
		{
			MeshBuilder builder;
			result_t result = RESULT_FAIL;

			HANDLE hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				LARGE_INTEGER size;
				if (GetFileSizeEx(hFile, &size))
				{
					constexpr u32 kPadding = 16;
					char* pBuffer = new char[size.LowPart + kPadding];
					pBuffer[0] = '\0';
					memset(pBuffer + size.LowPart, 0, kPadding);

					DWORD bytesRead = 0;
					if (ReadFile(hFile, pBuffer, size.LowPart, &bytesRead, NULL))
					{
						result = builder.ParseObjFormat(std::string_view(pBuffer, bytesRead), colour, fScale);
					}
				}
				CloseHandle(hFile);
			}

			if (RESULT_OK == result)
			{
				return builder.CreateMesh();
			}
			return MeshId();
		}

		TextureId CreateTextureCheckerboard(u32 width, u32 height, ColourValue a, ColourValue b, u32 checkSize)
		{
			const u32 rowPitch = width;
			u32* pImageData = new u32[height * rowPitch];

			for (u32 i = 0; i < height; ++i)
			{
				u32 ti = (i / checkSize) % 2;
				for (u32 j = 0; j < width; ++j)
				{
					u32 tj = (j / checkSize + ti) % 2;
					pImageData[i * rowPitch + j] = tj ? a.as_u32(): b.as_u32();				
				}
			}

			TextureDesc desc;
			desc.m_width = width;
			desc.m_height = height;
			desc.m_format = TextureFormat::RGBA;
			desc.m_pImageData = pImageData;
			TextureId id = Resources::CreateAsset<Texture>(desc);

			delete[] pImageData;

			return id;
		}

		TextureId CreateTextureFromFile(const char* pFilePath)
		{
			IWICImagingFactory* pIWICFactory = nullptr;
			IWICBitmapDecoder* pDecoder = nullptr;
			IWICFormatConverter* pConvertedSourceBitmap = nullptr;
			IWICBitmapFrameDecode* pFrame = nullptr;

			TextureId textureId;

			HRESULT hr = CoCreateInstance(
				CLSID_WICImagingFactory,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&pIWICFactory)
			);

			int size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pFilePath, -1, nullptr, 0);
			std::wstring wFilePath((size_t)size, 't');
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pFilePath, -1, wFilePath.data(), size);

			hr = pIWICFactory->CreateDecoderFromFilename(
				wFilePath.c_str(),
				NULL,
				GENERIC_READ,
				WICDecodeMetadataCacheOnDemand,
				&pDecoder
			);

			if (SUCCEEDED(hr))
			{
				hr = pDecoder->GetFrame(0, &pFrame);
			}

			if (SUCCEEDED(hr))
			{
				PLAY_SAFE_RELEASE(pConvertedSourceBitmap);
				hr = pIWICFactory->CreateFormatConverter(&pConvertedSourceBitmap);
			}

			if (SUCCEEDED(hr))
			{
				hr = pConvertedSourceBitmap->Initialize(
					pFrame,
					GUID_WICPixelFormat32bppPRGBA,
					WICBitmapDitherTypeNone,
					NULL,
					0.f,
					WICBitmapPaletteTypeCustom
				);
			}

			if (SUCCEEDED(hr))
			{
				u32 width = 0;
				u32 height = 0;
				pConvertedSourceBitmap->GetSize(&width, &height);

				u32 sizeBytes = width * height * 4;
				std::vector<BYTE> pImageData(sizeBytes);

				hr = pConvertedSourceBitmap->CopyPixels(NULL, width * sizeof(u32), sizeBytes, pImageData.data());
				if (SUCCEEDED(hr))
				{
					TextureDesc desc;
					desc.m_width = width;
					desc.m_height = height;
					desc.m_format = TextureFormat::RGBA;
					desc.m_pImageData = pImageData.data();
					textureId = Resources::CreateAsset<Texture>(desc);
				}
			}

			PLAY_SAFE_RELEASE(pIWICFactory);
			PLAY_SAFE_RELEASE(pDecoder);
			PLAY_SAFE_RELEASE(pConvertedSourceBitmap);
			PLAY_SAFE_RELEASE(pFrame);

			return textureId;
		}

		SamplerId CreateLinearSampler()
		{
			SamplerDesc desc;
			desc.m_filter = FilterMode::BILINEAR;
			desc.m_addressMode = AddressMode::WRAP;
			return Resources::CreateAsset<Graphics::Sampler>(desc);
		}

	}
}



//-----------------------------------------------------------
// Play3dImpl\Graphics_Impl.cpp





//-----------------------------------------------------------
// Play3dImpl\ShaderCode_Impl.h




namespace Play3d
{
namespace Graphics
{
extern const char* HLSL_FontShader;
extern const char* HLSL_MeshShader;
extern const char* HLSL_PrimitiveBatchShader;
extern const char* HLSL_TestShader;

} // Graphics
} // Play3d


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "windowscodecs.lib")
namespace Play3d
{
	namespace Graphics
	{
		Graphics_Impl* Graphics_Impl::ms_pInstance = nullptr;

		bool Graphics_Impl::UpdateMessageLoop()
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					return true;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return false;
		}


		void Graphics_Impl::UpdateConstantBuffers()
		{
			ID3D11DeviceContext* pDC(m_pDeviceContext.Get());
			m_frameConstants.UpdateGPU(pDC);
			m_drawConstants.UpdateGPU(pDC);
			m_materialConstants.UpdateGPU(pDC);
			m_lightConstants.UpdateGPU(pDC);
			m_uiFrameConstants.UpdateGPU(pDC);
		}

		void Graphics_Impl::Initialise()
		{
			if (!ms_pInstance)
			{
				ms_pInstance = new Graphics_Impl;
			}
		}

		void Graphics_Impl::Destroy()
		{
			ComPtr<ID3D11Debug> pDebug;
			HRESULT hr = ms_pInstance->m_pDevice.As(&pDebug);

			delete ms_pInstance;
			ms_pInstance = nullptr;

			if (SUCCEEDED(hr))
			{
				pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
			}
		}

		Graphics_Impl::Graphics_Impl()
			: m_hInstance(GetModuleHandle(NULL))
			, m_hWnd(NULL)
			, m_nSurfaceWidth(0)
			, m_nSurfaceHeight(0)
			, m_nNextPrimitiveBatch(0)
		{
			InitWindow();
			InitDirectX();
		}

		Graphics_Impl::~Graphics_Impl()
		{
			for (auto& it : m_primitiveBatchRing)
			{
				PLAY_SAFE_DELETE(it);
			}

			Flush();
			ReleaseWindow();
		}

		result_t Graphics_Impl::PostInitialise()
		{
			CompileInternalShaders();
			CreateInputLayouts();
			CreatePipelineState();

			m_frameConstants.Init(m_pDevice.Get());
			m_drawConstants.Init(m_pDevice.Get());
			m_lightConstants.Init(m_pDevice.Get());
			m_materialConstants.Init(m_pDevice.Get());
			m_uiFrameConstants.Init(m_pDevice.Get());

			return RESULT_OK;
		}

		result_t Graphics_Impl::BeginFrame()
		{
			bool bQuit = UpdateMessageLoop();

			for (auto pSRV : m_mipQueue)
			{
				m_pDeviceContext->GenerateMips(pSRV);
			}
			m_mipQueue.clear();

			f32 fClearColour[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV.Get(), fClearColour);

			m_pDeviceContext->ClearDepthStencilView(m_pDefaultDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			ID3D11RenderTargetView* rtvs[] = { m_pBackBufferRTV.Get() };
			m_pDeviceContext->OMSetRenderTargets(1, rtvs, m_pDefaultDSV.Get());

			D3D11_VIEWPORT viewports[]
			{
				{0, 0, (f32)m_nSurfaceWidth, (f32)m_nSurfaceHeight, 0.f, 1.0f}
			};
			m_pDeviceContext->RSSetViewports(1, viewports);

			m_pDeviceContext->RSSetState(m_pFallbackRasterState.Get());

			m_pDeviceContext->OMSetBlendState(m_pBlendStateOpaque.Get(), NULL, 0xffffffff);

			UpdateUITransform();

			m_uiFrameConstants.UpdateGPU(m_pDeviceContext.Get());

			m_nNextPrimitiveBatch = 0;

			if (bQuit)
			{
				return RESULT_QUIT;
			}
			return RESULT_OK;
		}

		result_t Graphics_Impl::EndFrame()
		{
			m_pSwapChain->Present(1, 0);
			return RESULT_OK;
		}


		void Graphics_Impl::Flush()
		{
			if (m_pDeviceContext)
			{
				m_pDeviceContext->ClearState();
				m_pDeviceContext->Flush();
			}
		}

		PrimitiveBatch* Graphics_Impl::AllocatePrimitiveBatch()
		{
			PrimitiveBatch* pBatch = nullptr;
			if (m_nNextPrimitiveBatch < m_primitiveBatchRing.size())
			{
				pBatch = m_primitiveBatchRing[m_nNextPrimitiveBatch];
				++m_nNextPrimitiveBatch;
			}
			else
			{
				pBatch = new PrimitiveBatch(m_pDevice.Get(), 0x10000);
				m_primitiveBatchRing.push_back(pBatch);
				++m_nNextPrimitiveBatch;
			}
			PLAY_ASSERT(pBatch);
			return pBatch;
		}

		void Graphics_Impl::DrawPrimitveBatch(PrimitiveBatch* pBatch)
		{
			PLAY_ASSERT(pBatch);

			UpdateConstantBuffers();

			ID3D11DeviceContext* pDC = m_pDeviceContext.Get();

			pBatch->Flush(pDC);


			Shader* pVS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_primitiveBatchVS);
			Shader* pPS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_primitiveBatchPS);

			pDC->IASetInputLayout(m_pPrimitiveInputLayout.Get());

			pVS->Bind(pDC);
			pPS->Bind(pDC);


			pBatch->Bind(pDC);

			m_frameConstants.Bind(pDC, 0);


			pBatch->DrawPoints(pDC);
			pBatch->DrawLines(pDC);
			pBatch->DrawTriangles(pDC);
		}

		void Graphics_Impl::TempPrepFontDraw()
		{
			UpdateConstantBuffers();

			ID3D11DeviceContext* pDC = m_pDeviceContext.Get();

			pDC->OMSetBlendState(m_pBlendStateAdditive.Get(), NULL, 0xffffffff);

			Shader* pVS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_fontVS);
			Shader* pPS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_fontPS);
			pVS->Bind(pDC);
			pPS->Bind(pDC);

			pDC->IASetInputLayout(m_pFontInputLayout.Get());

			m_uiFrameConstants.Bind(pDC, 0);

		}

		ShaderId Graphics_Impl::GetMaterialShader(MaterialShaderKey key)
		{
			if (m_meshShaders[key.m_value].IsInvalid())
			{
				CompileMaterialShader(key);
			}
			return m_meshShaders[key.m_value];
		}

		void Graphics_Impl::SetLightPosition(u32 index, const Vector3f& vPosition)
		{
			m_lightConstants.Get().lightPos[index] = Vector4f(vPosition, 0.f);
		}

		void Graphics_Impl::SetLightDirection(u32 index, const Vector3f& vDirection)
		{
			m_lightConstants.Get().lightDir[index] = Vector4f(normalize(vDirection), 0.f);
		}

		void Graphics_Impl::SetLightColour(u32 index, ColourValue colour)
		{
			colour.as_float_rgba_srgb(&m_lightConstants.Get().lightColour[index].x);
		}

		void Graphics_Impl::QueueGenerateMips(ID3D11ShaderResourceView* pSRV)
		{
			m_mipQueue.push_back(pSRV);
		}

		void Graphics_Impl::RegisterWindowCallback(WindowCallback callback)
		{
			m_wndCallbacks.push_back(callback);
		}

		void Graphics_Impl::DrawMesh(const Mesh* pMesh)
		{
			PLAY_ASSERT(pMesh);

			UpdateConstantBuffers();

			ID3D11DeviceContext* pDC = m_pDeviceContext.Get();

			pDC->OMSetBlendState(m_pBlendStateOpaque.Get(), NULL, 0xffffffff);

			Material* pMaterial = Resources::ResourceManager<Material>::Instance().GetPtr(m_activeMaterial);
			if (pMaterial)
			{
				pDC->RSSetState(pMaterial->m_pRasterState.Get());

				Shader* pVS = Resources::ResourceManager<Shader>::Instance().GetPtr(pMaterial->m_VertexShader);
				if (!pVS)
				{
					pVS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_meshShaders[0]);
				}

				Shader* pPS = Resources::ResourceManager<Shader>::Instance().GetPtr(pMaterial->m_PixelShader);
				if (!pPS)
				{
					pPS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_meshShaders[1]);
				}
				pVS->Bind(pDC);
				pPS->Bind(pDC);

				ID3D11Buffer* buffers[] = { pMaterial->m_pMaterialConstants.Get() };
				pDC->VSSetConstantBuffers(3, 1, buffers);
				pDC->PSSetConstantBuffers(3, 1, buffers);

				ID3D11ShaderResourceView* textureBindings[kMaxMaterialTextureSlots];
				ID3D11SamplerState* samplerBindings[kMaxMaterialTextureSlots];

				for (u32 i = 0; i < kMaxMaterialTextureSlots; ++i)
				{
					if (pMaterial->m_texture[i].IsValid())
					{
						Texture* pTexture = Resources::ResourceManager<Texture>::Instance().GetPtr(pMaterial->m_texture[i]);
						Sampler* pSampler = Resources::ResourceManager<Sampler>::Instance().GetPtr(pMaterial->m_sampler[i]);

						textureBindings[i] = pTexture ? pTexture->m_pSRV.Get() : nullptr;
						samplerBindings[i] = pSampler ? pSampler->m_pSampler.Get() : nullptr;
					}
					else
					{
						textureBindings[i] = nullptr;
						samplerBindings[i] = nullptr;
					}
				}
				pDC->PSSetShaderResources(0, 4, textureBindings);
				pDC->PSSetSamplers(0, 4, samplerBindings);
			}
			else
			{
				pDC->RSSetState(m_pFallbackRasterState.Get());
				Shader* pVS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_meshShaders[0]);
				Shader* pPS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_meshShaders[1]);
				pVS->Bind(pDC);
				pPS->Bind(pDC);

				m_materialConstants.Bind(pDC, 3);
			}

			pDC->IASetInputLayout(m_pMeshInputLayout.Get());

			pMesh->Bind(pDC);

			m_frameConstants.Bind(pDC, 0);
			m_drawConstants.Bind(pDC, 1);
			m_lightConstants.Bind(pDC, 2);


			pDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			if (pMesh->m_pIndexBuffer)
			{
				pDC->DrawIndexed(pMesh->m_indexCount, 0, 0);
			}
			else
			{
				pDC->Draw(pMesh->m_vertexCount, 0);
			}
		}

		void Graphics_Impl::SetViewport(const Viewport& v)
		{
			D3D11_VIEWPORT viewports[]
			{
				{0, 0, (f32)m_nSurfaceWidth, (f32)m_nSurfaceHeight, 0.f, 1.0f}
			};
			m_pDeviceContext->RSSetViewports(1, (const D3D11_VIEWPORT*)&v);
		}

		void Graphics_Impl::SetViewMatrix(const Matrix4x4f& m)
		{
			FrameConstantData& t(m_frameConstants.Get());
			t.viewMtx = m;
			t.viewProjectionMtx = t.projectionMtx * t.viewMtx;
		}

		void Graphics_Impl::SetProjectionMatrix(const Matrix4x4f& m)
		{
			FrameConstantData& t(m_frameConstants.Get());
			t.projectionMtx = m;
			t.viewProjectionMtx = t.projectionMtx * t.viewMtx;
		}

		void Graphics_Impl::SetWorldMatrix(const Matrix4x4f& m)
		{
			DrawConstantData& t(m_drawConstants.Get());
			t.worldMtx = m;
			t.normalMtx = Matrix4x4f(m.Upper3x3(), Vector3f(0, 0, 0));
			t.mvpMtx = m_frameConstants.Get().viewProjectionMtx * m;
		}

		void Graphics_Impl::UpdateUITransform()
		{
			m_uiFrameConstants.Get().viewProjectionMtx = MatrixOrthoProjectRH(0.f, (f32)m_nSurfaceWidth, (f32)m_nSurfaceHeight, 0.f, 0.f, 1.f);
			m_uiFrameConstants.Get().viewportRect = Vector4f(0.f, 0.f, (f32)m_nSurfaceWidth, (f32)m_nSurfaceHeight);

		}

		void Graphics_Impl::SetMaterial(MaterialId materialId)
		{
			m_activeMaterial = materialId;
		}

		SurfaceSize Graphics_Impl::GetDisplaySurfaceSize() const
		{
			return { m_nSurfaceWidth, m_nSurfaceHeight };
		}

		result_t Graphics_Impl::InitWindow()
		{
			SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

			u32 width = 800;
			u32 height = 600;
			const wchar_t* windowName = L"Play3d";

			WNDCLASSEXW wcex;

			wcex.cbSize = sizeof(WNDCLASSEX);

			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = Graphics_Impl::MainWndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = m_hInstance;
			wcex.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION);
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wcex.lpszMenuName = nullptr;
			wcex.lpszClassName = windowName;
			wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

			RegisterClassExW(&wcex);

			UINT dwStyle = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
			RECT rect = { 0, 0, (LONG)width, (LONG)height };
			AdjustWindowRect(&rect, dwStyle, FALSE);
			m_hWnd = CreateWindowW(windowName, windowName, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, m_hInstance, this);

			if (!m_hWnd)
			{
				return RESULT_FAIL;
			}

			ShowWindow(m_hWnd, SW_SHOWNORMAL);
			UpdateWindow(m_hWnd);

			m_nSurfaceWidth = width;
			m_nSurfaceHeight = height;

			return RESULT_OK;
		}

		result_t Graphics_Impl::ReleaseWindow()
		{
			if (m_hWnd)
			{
				DestroyWindow(m_hWnd);
				m_hWnd = NULL;
				return RESULT_OK;
			}
			return RESULT_FAIL;
		}

		result_t Graphics_Impl::InitDirectX()
		{

			HRESULT hr;

			DWORD createDeviceFlags = 0;
#ifdef _DEBUG
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			D3D_FEATURE_LEVEL featureLevel;
			hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
			if (FAILED(hr))
			{
				return RESULT_FAIL;
			}

			hr = m_pDevice.As(&m_pDXGIDevice);
			if (FAILED(hr))
			{
				return RESULT_FAIL;
			}

			hr = m_pDXGIDevice->GetAdapter(&m_pDXGIAdapter);
			if (FAILED(hr))
			{
				return RESULT_FAIL;
			}

			hr = m_pDXGIAdapter->GetParent(IID_PPV_ARGS(&m_pDXGIFactory));
			if (FAILED(hr))
			{
				return RESULT_FAIL;
			}

			DXGI_SWAP_CHAIN_DESC1 swapDesc = {};
			swapDesc.Width = m_nSurfaceWidth;
			swapDesc.Height = m_nSurfaceHeight;
			swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapDesc.Stereo = FALSE;
			swapDesc.SampleDesc.Count = 1;
			swapDesc.SampleDesc.Quality = 0;
			swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapDesc.BufferCount = 2;
			swapDesc.Scaling = DXGI_SCALING_STRETCH;
			swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
			swapDesc.Flags = 0;

			hr = m_pDXGIFactory->CreateSwapChainForHwnd(m_pDevice.Get(), m_hWnd, &swapDesc, NULL, NULL, &m_pSwapChain);
			if (FAILED(hr))
			{
				return RESULT_FAIL;
			}

			result_t result = RecreateRenderTargetView();
			if (result != RESULT_OK)
			{
				return result;
			}

			result = RecreateDepthBuffer();
			if (result != RESULT_OK)
			{
				return result;
			}

			return RESULT_OK;
		}

		result_t Graphics_Impl::Resize(u32 width, u32 height)
		{
			if (!(m_pDevice && m_pDeviceContext && m_pSwapChain))
			{
				return RESULT_FAIL;
			}

			HRESULT hr;

			m_nSurfaceWidth = width;
			m_nSurfaceHeight = height;

			m_pDeviceContext->OMSetRenderTargets(0, NULL, NULL);

			m_pDeviceContext->Flush();

			m_pBackBufferRTV.Reset();

			hr = m_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr))
			{
				return RESULT_FAIL;
			}

			result_t result = RecreateRenderTargetView();
			if (result != RESULT_OK)
			{
				return result;
			}

			result = RecreateDepthBuffer();
			if (result != RESULT_OK)
			{
				return result;
			}

			UpdateUITransform();

			return RESULT_OK;
		}

		result_t Graphics_Impl::RecreateRenderTargetView()
		{
			ComPtr<ID3D11Texture2D> pBackBufferTexture;
			HRESULT hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBufferTexture));
			if (FAILED(hr))
			{
				return RESULT_FAIL;
			}

			D3D11_RENDER_TARGET_VIEW_DESC viewDesc = {};
			viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // Apply Gamma conversion in the view.
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			viewDesc.Texture2D.MipSlice = 0;
			hr = m_pDevice->CreateRenderTargetView(pBackBufferTexture.Get(), &viewDesc, &m_pBackBufferRTV);
			if (FAILED(hr))
			{
				return RESULT_FAIL;
			}
			return RESULT_OK;
		}

		result_t Graphics_Impl::RecreateDepthBuffer()
		{
			m_pDefaultDepthBuffer.Reset();
			m_pDefaultDSV.Reset();

			D3D11_TEXTURE2D_DESC desc = {};
			desc.Width = m_nSurfaceWidth;
			desc.Height = m_nSurfaceHeight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_D32_FLOAT;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			HRESULT hr = m_pDevice->CreateTexture2D(&desc, NULL, &m_pDefaultDepthBuffer);
			if (FAILED(hr))
			{
				return RESULT_FAIL;
			}

			hr = m_pDevice->CreateDepthStencilView(m_pDefaultDepthBuffer.Get(), NULL, &m_pDefaultDSV);
			if (FAILED(hr))
			{
				return RESULT_FAIL;
			}

			return RESULT_OK;
		}

		result_t Graphics_Impl::CompileInternalShaders()
		{
			{
				ShaderCompilerDesc desc;
				desc.m_name = "HLSL_PrimitiveBatchShader";
				desc.m_hlslCode = HLSL_PrimitiveBatchShader;
				desc.m_type = ShaderType::VERTEX_SHADER;
				desc.m_entryPoint = "VS_Main";

				m_primitiveBatchVS = Shader::Compile(desc);

				desc.m_type = ShaderType::PIXEL_SHADER;
				desc.m_entryPoint = "PS_Main";

				m_primitiveBatchPS = Shader::Compile(desc);
			}

			{
				ShaderCompilerDesc desc;
				desc.m_name = "HLSL_FontShader";
				desc.m_hlslCode = HLSL_FontShader;
				desc.m_type = ShaderType::VERTEX_SHADER;
				desc.m_entryPoint = "VS_Main";

				m_fontVS = Shader::Compile(desc);

				desc.m_type = ShaderType::PIXEL_SHADER;
				desc.m_entryPoint = "PS_Main";

				m_fontPS = Shader::Compile(desc);
			}

			{
				MaterialShaderKey key;
				key.m_value = 0;
				CompileMaterialShader(key);
				key.m_bits.m_pixelShader = 1;
				CompileMaterialShader(key);
			}

			return RESULT_OK;
		}

		void Graphics_Impl::CompileMaterialShader(MaterialShaderKey& key)
		{
			PLAY_ASSERT_MSG(m_meshShaders[key.m_value].IsInvalid(), "Duplicate shader permutation");
			ShaderCompilerDesc desc;
			desc.m_hlslCode = HLSL_MeshShader;
			desc.m_name = "MeshUber";
			desc.m_defines.clear();

			if (key.m_bits.m_pixelShader)
			{
				desc.m_type = ShaderType::PIXEL_SHADER;
				desc.m_entryPoint = "PS_Main";
				desc.m_name += "_PS";
			}
			else
			{
				desc.m_type = ShaderType::VERTEX_SHADER;
				desc.m_entryPoint = "VS_Main";
				desc.m_name += "_VS";
			}


			if (key.m_bits.m_useTexture0)
			{
				desc.m_defines.push_back({ "USE_TEXTURE_0", "1" });
				desc.m_name += "_T0";
			}

			if (key.m_bits.m_useLighting)
			{
				desc.m_defines.push_back({ "USE_LIGHTING", "1" });
				desc.m_name += "_L";
			}

			char t[16];
			sprintf_s(t, 16, "%u", key.m_bits.m_lightCount);
			desc.m_defines.push_back({ "LIGHT_COUNT",  t });
			desc.m_name += "_LC";
			desc.m_name += t;

			m_meshShaders[key.m_value] = Shader::Compile(desc);
		}

		result_t Graphics_Impl::CreateInputLayouts()
		{
			{
				Shader* pVS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_primitiveBatchVS);

				D3D11_INPUT_ELEMENT_DESC vertexFormat[] = {
					{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(PrimitiveVertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"COLOUR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(PrimitiveVertex, colour), D3D11_INPUT_PER_VERTEX_DATA, 0},
				};

				HRESULT hr = m_pDevice->CreateInputLayout(vertexFormat, 2, pVS->GetByteCode(), pVS->GetByteCodeSize(), &m_pPrimitiveInputLayout);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "CreateInputLayout");
			}
			{
				Shader* pVS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_meshShaders[0]);

				D3D11_INPUT_ELEMENT_DESC vertexFormat[] = {
					{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"COLOUR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				};

				HRESULT hr = m_pDevice->CreateInputLayout(vertexFormat, 4, pVS->GetByteCode(), pVS->GetByteCodeSize(), &m_pMeshInputLayout);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "CreateInputLayout");
			}
			{
				Shader* pVS = Resources::ResourceManager<Shader>::Instance().GetPtr(m_fontVS);

				D3D11_INPUT_ELEMENT_DESC vertexFormat[] = {
					{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(UI::FontVertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(UI::FontVertex, uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"COLOUR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(UI::FontVertex, colour), D3D11_INPUT_PER_VERTEX_DATA, 0},
				};

				HRESULT hr = m_pDevice->CreateInputLayout(vertexFormat, 3, pVS->GetByteCode(), pVS->GetByteCodeSize(), &m_pFontInputLayout);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "CreateInputLayout");
			}
			return RESULT_OK;
		}

		result_t Graphics_Impl::CreatePipelineState()
		{
			HRESULT hr;
			{
				D3D11_RASTERIZER_DESC desc = {};
				desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
				desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
				desc.FrontCounterClockwise = FALSE;
				desc.DepthBias = 0;
				desc.DepthBiasClamp = 0.f;
				desc.SlopeScaledDepthBias = 0.f;
				desc.DepthClipEnable = TRUE;
				desc.ScissorEnable = FALSE;
				desc.MultisampleEnable = FALSE;
				desc.AntialiasedLineEnable = FALSE;
				hr = m_pDevice->CreateRasterizerState(&desc, &m_pFallbackRasterState);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "Invalid RasterState");
			}
			{
				D3D11_BLEND_DESC desc = {};
				desc.AlphaToCoverageEnable = FALSE;
				desc.IndependentBlendEnable = FALSE;
				desc.RenderTarget[0].BlendEnable = FALSE;
				desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
				desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				hr = m_pDevice->CreateBlendState(&desc, &m_pBlendStateOpaque);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "Invalid BlendState");

				desc.RenderTarget[0].BlendEnable = TRUE;
				desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				hr = m_pDevice->CreateBlendState(&desc, &m_pBlendStateAdditive);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "Invalid BlendState");
			}
			return RESULT_OK;
		}

		LRESULT CALLBACK Graphics_Impl::MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			Graphics_Impl* pImpl = reinterpret_cast<Graphics_Impl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			if (pImpl)
			{
				for (WindowCallback callback : pImpl->m_wndCallbacks)
				{
					callback(hWnd, message, wParam, lParam);
				}
			}

			switch (message)
			{
			case WM_CREATE:
			{
				LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
				SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
				return 0;
			}
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_SIZE:
			{
				u32 width = LOWORD(lParam);
				u32 height = HIWORD(lParam);

				if (width && height && pImpl)
				{
					pImpl->Resize(width, height);
				}
				return 0;
			}

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			return 0;
		}

	}

}



//-----------------------------------------------------------
// Play3dImpl\InputApi.cpp




//-----------------------------------------------------------
// Play3dImpl\Input_Impl.h



namespace Play3d
{
	namespace Input
	{
		class Input_Impl
		{
			PLAY_NONCOPYABLE(Input_Impl);
			PLAY_SINGLETON_INTERFACE(Input_Impl);

			Input_Impl();
			~Input_Impl();
		public:
			void BeginFrame();

			void EndFrame();

			void UpdateKeyboard();

			void UpdateXInput();

			bool IsKeyDown(u32 keycode);

			bool IsKeyUp(u32 keycode);

			bool IsKeyPressed(u32 keycode);

			bool IsPadConnected(u32 padId);

			bool IsPadButtonUp(u32 padId, ButtonId button);

			bool IsPadButtonDown(u32 padId, ButtonId button);

			bool IsPadButtonPressed(u32 padId, ButtonId button);

			f32 GetPadAxis(u32 padId, AxisId axis);

			const MouseState& GetMouseState();

			void CaptureMouse(bool bEnable);
		private:
			void UpdateMouse();

			int HandleWindowMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		private:
			MouseState m_mouseState;

			BYTE* m_pKeyState;
			BYTE* m_pPrevKeyState;

			XINPUT_STATE* m_XInputPrevState;
			XINPUT_STATE* m_XInputState;

			f32* m_XInputDeadZoneMags;
			u32 m_padConnectedFlags;

			POINT m_mouseHidePos;

		};
	}
}

namespace Play3d
{
	namespace Input
	{

		bool IsKeyPressed(u32 keycode)
		{
			return Input_Impl::Instance().IsKeyPressed(keycode);
		}

		bool IsKeyDown(u32 keycode)
		{
			return Input_Impl::Instance().IsKeyDown(keycode);
		}

		bool IsButtonPressed(InputDevice device, ButtonId buttonId)
		{
			return Input_Impl::Instance().IsPadButtonPressed((u32)device, buttonId);
		}


		bool IsButtonDown(InputDevice device, ButtonId buttonId)
		{
			return Input_Impl::Instance().IsPadButtonDown((u32)device, buttonId);
		}

		float GetAxis(InputDevice device, AxisId axisId)
		{
			return Input_Impl::Instance().GetPadAxis((u32)device, axisId);
		}

		const MouseState& GetMouseState()
		{
			return Input_Impl::Instance().GetMouseState();
		}

		void CaptureMouse(bool bEnable)
		{
			return Input_Impl::Instance().CaptureMouse(bEnable);
		}

	}
}


//-----------------------------------------------------------
// Play3dImpl\Input_Impl.cpp



#pragma comment(lib, "XInput")

namespace Play3d
{
	namespace Input
	{
		PLAY_SINGLETON_IMPL(Input_Impl);

		u32 ConvertInputKeyCode(u32 value)
		{
			PLAY_ASSERT(value < 256);
			return value;
		}
		u32 ConvertButtonId(ButtonId value)
		{
			static u32 s_lut[] = {
				  XINPUT_GAMEPAD_DPAD_UP // Button_DPad_Up,
				, XINPUT_GAMEPAD_DPAD_DOWN // Button_DPad_Down,
				, XINPUT_GAMEPAD_DPAD_LEFT // Button_DPad_Left,
				, XINPUT_GAMEPAD_DPAD_RIGHT // Button_DPad_Right,
				, XINPUT_GAMEPAD_X // Button_X,
				, XINPUT_GAMEPAD_Y // Button_Y,
				, XINPUT_GAMEPAD_A // Button_A,
				, XINPUT_GAMEPAD_B // Button_B,
				, XINPUT_GAMEPAD_LEFT_SHOULDER // Button_Shoulder_Left,
				, XINPUT_GAMEPAD_RIGHT_SHOULDER // Button_Shoulder_Right,
				, XINPUT_GAMEPAD_LEFT_THUMB // Button_Stick_Left,
				, XINPUT_GAMEPAD_RIGHT_THUMB // Button_Stick_Right,
				, XINPUT_GAMEPAD_START // Button_Start,
				, XINPUT_GAMEPAD_BACK // Button_Back,
				, 0 // Button_Left_Trigger, // special case
				, 0 // Button_Right_Trigger, // special case
			};
			static_assert(sizeof(s_lut) / sizeof(u32) == (u32)ButtonId::BUTTON_COUNT, "Incorrect lookup table size");

			return s_lut[static_cast<u32>(value)];
		}

		inline float ComputePadDeadZone(float LX, float LY, float deadZone)
		{

			float magnitude = sqrt(LX * LX + LY * LY);

			float normalizedLX = LX / magnitude;
			float normalizedLY = LY / magnitude;

			float normalizedMagnitude = 0.f;

			if (magnitude > deadZone)
			{
				if (magnitude > 32767) magnitude = 32767;

				magnitude -= deadZone;

				normalizedMagnitude = magnitude / (32767 - deadZone);
			}
			else // if the controller is in the deadzone zero out the magnitude
			{
				magnitude = 0.f;
				normalizedMagnitude = 0.f;
			}

			return normalizedMagnitude;
		}

		inline f32 XInputTriggerToFloat(BYTE value)
		{
			return (float)value / 255;
		}

		Input_Impl::Input_Impl()
			: m_pKeyState(new BYTE[512])
			, m_pPrevKeyState(new BYTE[256])
			, m_XInputPrevState(new XINPUT_STATE[XUSER_MAX_COUNT])
			, m_XInputState(new XINPUT_STATE[XUSER_MAX_COUNT])
			, m_XInputDeadZoneMags(new float[XUSER_MAX_COUNT * 2])
		{
			memset(m_pKeyState, 0, sizeof(BYTE) * 256);
			memset(m_pPrevKeyState, 0, sizeof(BYTE) * 256);

			Graphics::Graphics_Impl::Instance().RegisterWindowCallback(
				[&](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return this->HandleWindowMessages(hWnd, message, wParam, lParam); }
			);

			::GetCursorPos(&m_mouseHidePos);
			CaptureMouse(false);
		}

		Input_Impl::~Input_Impl()
		{
			RAWINPUTDEVICE Rid[1];
			Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
			Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
			Rid[0].dwFlags = RIDEV_REMOVE;
			Rid[0].hwndTarget = NULL;
			RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

			delete[] m_pKeyState;
			delete[] m_pPrevKeyState;

			m_pKeyState = m_pPrevKeyState = nullptr;

			delete[] m_pPrevKeyState;
			delete[] m_XInputState;
			delete[] m_XInputDeadZoneMags;
		}


		void Input_Impl::BeginFrame()
		{
			UpdateKeyboard();
			UpdateXInput();
		}

		void Input_Impl::EndFrame()
		{
			UpdateMouse();
		}

		bool Input_Impl::IsKeyDown(u32 keycode)
		{
			u32 vkeyCode = ConvertInputKeyCode(keycode);
			PLAY_ASSERT_MSG(vkeyCode < 256, "Unexpected virtual key code");
			return (m_pKeyState[vkeyCode] & 0x80) > 0; // High order bit represents state.
		}

		bool Input_Impl::IsKeyUp(u32 keycode)
		{
			u32 vkeyCode = ConvertInputKeyCode(keycode);
			PLAY_ASSERT_MSG(vkeyCode < 256, "Unexpected virtual key code");
			return (m_pKeyState[vkeyCode] & 0x80) == 0;
		}

		bool Input_Impl::IsKeyPressed(u32 keycode)
		{
			u32 vkeyCode = ConvertInputKeyCode(keycode);
			PLAY_ASSERT_MSG(vkeyCode < 256, "Unexpected virtual key code");
			return ((m_pPrevKeyState[vkeyCode] & 0x80) == 0) && ((m_pKeyState[vkeyCode] & 0x80) > 0);
		}

		bool Input_Impl::IsPadConnected(u32 padId)
		{
			if (padId >= XUSER_MAX_COUNT)
				return false;

			return (m_padConnectedFlags & (1 << padId)) != 0;
		}

		bool Input_Impl::IsPadButtonUp(u32 padId, ButtonId button)
		{
			if (IsPadConnected(padId))
			{
				const XINPUT_STATE& padState(m_XInputState[padId]);

				switch (button)
				{
				case ButtonId::BUTTON_LEFT_TRIGGER:
					return padState.Gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
				case ButtonId::BUTTON_RIGHT_TRIGGER:
					return padState.Gamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
				}

				u32 buttonMask = ConvertButtonId(button);
				return (padState.Gamepad.wButtons & buttonMask) == 0;
			};
			return false;
		}

		bool Input_Impl::IsPadButtonDown(u32 padId, ButtonId button)
		{
			if (IsPadConnected(padId))
			{
				const XINPUT_STATE& padState(m_XInputState[padId]);

				switch (button)
				{
				case ButtonId::BUTTON_LEFT_TRIGGER:
					return padState.Gamepad.bLeftTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
				case ButtonId::BUTTON_RIGHT_TRIGGER:
					return padState.Gamepad.bRightTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
				}

				u32 buttonMask = ConvertButtonId(button);
				return (padState.Gamepad.wButtons & buttonMask) != 0;
			};
			return false;
		}

		bool Input_Impl::IsPadButtonPressed(u32 padId, ButtonId button)
		{
			if (IsPadConnected(padId))
			{
				const XINPUT_STATE& prevPadState(m_XInputPrevState[padId]);
				const XINPUT_STATE& padState(m_XInputState[padId]);

				switch (button)
				{
				case ButtonId::BUTTON_LEFT_TRIGGER:
					return padState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD && prevPadState.Gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
				case ButtonId::BUTTON_RIGHT_TRIGGER:
					return padState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD && prevPadState.Gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
				}

				u32 buttonMask = ConvertButtonId(button);
				return (padState.Gamepad.wButtons & buttonMask) != 0 && (prevPadState.Gamepad.wButtons & buttonMask) == 0;
			};
			return false;
		}

		f32 Input_Impl::GetPadAxis(u32 padId, AxisId axis)
		{
			if (IsPadConnected(padId))
			{
				const XINPUT_STATE& padState(m_XInputState[padId]);

				switch (axis)
				{
				case AxisId::AXIS_LEFTSTICK_X: return (f32)padState.Gamepad.sThumbLX / 0x7FFF * m_XInputDeadZoneMags[padId * 2];
				case AxisId::AXIS_LEFTSTICK_Y: return (f32)padState.Gamepad.sThumbLY / 0x7FFF * m_XInputDeadZoneMags[padId * 2];
				case AxisId::AXIS_RIGHTSTICK_X: return (f32)padState.Gamepad.sThumbRX / 0x7FFF * m_XInputDeadZoneMags[padId * 2 + 1];
				case AxisId::AXIS_RIGHTSTICK_Y: return (f32)padState.Gamepad.sThumbRY / 0x7FFF * m_XInputDeadZoneMags[padId * 2 + 1];

				case AxisId::AXIS_LEFT_TRIGGER_PRESSURE: return XInputTriggerToFloat(padState.Gamepad.bLeftTrigger);
				case AxisId::AXIS_RIGHT_TRIGGER_PRESSURE: return XInputTriggerToFloat(padState.Gamepad.bRightTrigger);
				default:
					return 0.f;
				}
			};
			return false;
		}

		const MouseState& Input_Impl::GetMouseState()
		{
			return m_mouseState;
		}

		void Input_Impl::CaptureMouse(bool bEnable)
		{
			HWND hWnd = Graphics::Graphics_Impl::Instance().GetHWnd();
			if (bEnable)
			{
				int ret;
				do { ret = ::ShowCursor(FALSE); } while (ret > -1);

				RAWINPUTDEVICE Rid[1];
				Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
				Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
				Rid[0].dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK; //  RIDEV_NOLEGACY | RIDEV_CAPTUREMOUSE;
				Rid[0].hwndTarget = hWnd;
				RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

				::GetCursorPos(&m_mouseHidePos);
			}
			else
			{
				RAWINPUTDEVICE Rid[1];
				Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
				Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
				Rid[0].dwFlags = 0;
				Rid[0].hwndTarget = hWnd;
				RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

				::SetCursorPos(m_mouseHidePos.x, m_mouseHidePos.y);

				::ShowCursor(TRUE);
			}	
		}

		void Input_Impl::UpdateXInput()
		{
			DWORD result;

			m_padConnectedFlags = 0;
			memset(m_XInputDeadZoneMags, 0, sizeof(float) * XUSER_MAX_COUNT * 2);

			std::swap(m_XInputPrevState, m_XInputState);

			for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
			{

				ZeroMemory(&m_XInputState[i], sizeof(XINPUT_STATE));
				result = XInputGetState(i, &m_XInputState[i]);

				if (ERROR_SUCCESS == result)
				{
					m_padConnectedFlags |= 1 << i;
					m_XInputDeadZoneMags[i * 2] = ComputePadDeadZone(m_XInputState[i].Gamepad.sThumbLX, m_XInputState[i].Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
					m_XInputDeadZoneMags[i * 2 + 1] = ComputePadDeadZone(m_XInputState[i].Gamepad.sThumbRX, m_XInputState[i].Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
				}
			}
		}

		void Input_Impl::UpdateKeyboard()
		{
			std::swap(m_pKeyState, m_pPrevKeyState);
			GetKeyboardState(m_pKeyState);
		}

		void Input_Impl::UpdateMouse()
		{
			m_mouseState.m_deltaX = 0.f;
			m_mouseState.m_deltaY = 0.f;
		}

		int Input_Impl::HandleWindowMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_INPUT: {

				if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT)
				{
					constexpr u32 kInputBufferSize = 64;
					UINT dwSize = kInputBufferSize;
					BYTE lpb[kInputBufferSize];

					GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

					if (dwSize <= kInputBufferSize)
					{
						GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
						RAWINPUT* raw = (RAWINPUT*)lpb;

						if (raw->header.dwType == RIM_TYPEMOUSE
							&& raw->data.mouse.usFlags == 0
							)
						{
							m_mouseState.m_deltaX += raw->data.mouse.lLastX;
							m_mouseState.m_deltaY += raw->data.mouse.lLastY;

							if ((raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) > 0)
							{
								m_mouseState.m_leftButton = true;
							}

							if ((raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) > 0)
							{
								m_mouseState.m_leftButton = false;
							}

							if ((raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) > 0)
							{
								m_mouseState.m_rightButton = true;
							}

							if ((raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) > 0)
							{
								m_mouseState.m_rightButton = false;
							}
						}
					}
				}

				break;
			}

			case WM_MOUSEMOVE:
				m_mouseState.m_x = (f32)LOWORD(lParam);
				m_mouseState.m_y = (f32)HIWORD(lParam);
				break;
			};

			return 0;
		}

	}
}



//-----------------------------------------------------------
// Play3dImpl\Material.cpp



namespace Play3d
{
	namespace Graphics
	{
		D3D11_CULL_MODE TranslateCullMode(CullMode mode)
		{
			switch (mode)
			{
			case CullMode::BACK: return D3D11_CULL_BACK;
			case CullMode::FRONT: return D3D11_CULL_FRONT;
			case CullMode::NONE: return D3D11_CULL_NONE;
			}
			return D3D11_CULL_NONE;
		}

		Material::Material(const SimpleMaterialDesc& rDesc)
		{
			auto pDevice = Graphics_Impl::Instance().GetDevice();
			SetupState(pDevice, rDesc.m_state);
			SetupConstantBuffer(pDevice, &rDesc.m_constants, sizeof(MaterialConstantData));
			SetupTextureBindings(pDevice, rDesc.m_texture, rDesc.m_sampler);


			MaterialShaderKey key;
			key.m_value = 0u;

			if (rDesc.m_bEnableLighting)
			{
				key.m_bits.m_useLighting = 1;
				PLAY_ASSERT_MSG(0 < rDesc.m_lightCount && rDesc.m_lightCount < MaterialShaderKey::kMaxLights, "Invalid light count.");
				key.m_bits.m_lightCount = rDesc.m_lightCount;
			}
			if (rDesc.m_texture[0].IsValid())
			{
				key.m_bits.m_useTexture0 = 1;
			}
			m_VertexShader = Graphics_Impl::Instance().GetMaterialShader(key);

			key.m_bits.m_pixelShader = 1;
			m_PixelShader = Graphics_Impl::Instance().GetMaterialShader(key);
		}

		Material::Material(const ComplexMaterialDesc& rDesc)
		{
			auto pDevice = Graphics_Impl::Instance().GetDevice();
			SetupState(pDevice, rDesc.m_state);
			SetupConstantBuffer(pDevice, &rDesc.m_pConstantData, rDesc.m_dataSize);
			m_VertexShader = rDesc.m_VertexShader;
			m_PixelShader = rDesc.m_PixelShader;
			SetupTextureBindings(pDevice, rDesc.m_texture, rDesc.m_sampler);
		}

		Material::~Material()
		{

		}

		void Material::SetupState(ID3D11Device* pDevice, const MaterialStateSettings& state)
		{
			
			{
				D3D11_RASTERIZER_DESC desc = {};
				desc.FillMode = state.m_fillMode == FillMode::SOLID ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
				desc.CullMode = TranslateCullMode(state.m_cullMode);
				desc.FrontCounterClockwise = FALSE;
				desc.DepthBias = 0;
				desc.DepthBiasClamp = 0.f;
				desc.SlopeScaledDepthBias = 0.f;
				desc.DepthClipEnable = TRUE;
				desc.ScissorEnable = FALSE;
				desc.MultisampleEnable = FALSE;
				desc.AntialiasedLineEnable = FALSE;

				pDevice->CreateRasterizerState(&desc, &m_pRasterState);
			}
		}

		void Material::SetupConstantBuffer(ID3D11Device* pDevice, const void* pData, size_t size)
		{
			if(pData && size > 0)
			{
				D3D11_BUFFER_DESC desc = {};
				desc.ByteWidth = (u32)size; 
				desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;

				D3D11_SUBRESOURCE_DATA data = {};
				data.pSysMem = pData;

				HRESULT hr = pDevice->CreateBuffer(&desc, &data, &m_pMaterialConstants);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "Could not create Material Constant Buffer");
			}
		}

		void Material::SetupTextureBindings(ID3D11Device* pDevice, const TextureId* pTextureId, const SamplerId* pSamplerId)
		{
			for (u32 i = 0; i < kMaxMaterialTextureSlots; ++i)
			{
				m_texture[i] = pTextureId[i];
				m_sampler[i] = pSamplerId[i];
			}
		}

	}
}



//-----------------------------------------------------------
// Play3dImpl\Mesh.cpp



namespace Play3d
{
	namespace Graphics
	{

		Mesh::Mesh(const MeshDesc& rDesc)
			: m_indexCount(rDesc.m_indexCount)
			, m_vertexCount(rDesc.m_vertexCount)
		{
			for (u32 i = 0; i < rDesc.m_streamCount; ++i)
			{
				AddStream(rDesc.m_pStreams[i]);
			}
		}

		Mesh::~Mesh()
		{
			for (auto& it : m_streamBuffers)
			{
				PLAY_SAFE_RELEASE(it);
			}
			PLAY_SAFE_RELEASE(m_pIndexBuffer);
		}

		void Mesh::AddStream(const StreamInfo& info)
		{
			bool bIsDynamic = (info.m_flags & StreamFlags::DYNAMIC_STREAM);

			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = (UINT)info.m_dataSize;
			desc.Usage = bIsDynamic ? D3D11_USAGE::D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
			desc.BindFlags = info.m_type == StreamType::INDEX ? D3D11_BIND_INDEX_BUFFER : D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = bIsDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA data = {};
			if (!bIsDynamic)
			{
				PLAY_ASSERT_MSG(info.m_pData, "A non-dynamic stream needs initial data.");
				data.pSysMem = info.m_pData;
			}
	
			ID3D11Buffer* pBuffer;
			HRESULT hr = Graphics_Impl::Instance().GetDevice()->CreateBuffer(&desc, bIsDynamic ? NULL : &data, &pBuffer);
			
			PLAY_ASSERT_MSG(SUCCEEDED(hr), "Create Vertex Buffer Stream");

			if (info.m_type == StreamType::INDEX)
			{
				m_pIndexBuffer = pBuffer;
			}
			else
			{
				m_streamInfos.push_back(info);
				m_streamBuffers.push_back(pBuffer);
				UINT stride;
				switch (info.m_type)
				{
				case StreamType::POSITION:
					stride = sizeof(f32) * 3;
					break;
				case StreamType::COLOUR: // u32/colour
					stride = sizeof(u32);
					break;
				case StreamType::NORMAL: // float3
					stride = sizeof(f32) * 3;
					break;
				case StreamType::UV: // float2
					stride = sizeof(f32) * 2;
					break;
				default:
					stride = info.m_stride;
					break;
				}
				m_strides.push_back(stride);
				m_offsets.push_back(info.m_offset);
			}
		}

		void Mesh::Bind(ID3D11DeviceContext* pDC) const
		{
			if (m_pIndexBuffer)
			{
				pDC->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			}

			for (auto it : m_streamBuffers)
			{
				pDC->IASetVertexBuffers(0, (UINT)m_streamBuffers.size(), m_streamBuffers.data(), m_strides.data(), m_offsets.data());
			}
		}

	}
}


//-----------------------------------------------------------
// Play3dImpl\MeshBuilder.cpp



namespace Play3d
{
	namespace Graphics
	{

		u32 MeshBuilder::AddVertex(const Vector3f& position, const Vector3f& normal, const Vector2f& uv, ColourValue colour)
		{
			u32 i0 = (u32)m_positions.size();
			m_positions.push_back(position);
			m_normals.push_back(normal);
			m_uvs.push_back(uv);
			m_colours.push_back(colour);
			return i0;
		}

		void MeshBuilder::AddFace(u32 vertexCount, const Vector3f* positions, ColourValue colour, bool flip /*= false*/)
		{
			u32 i0 = (u32)m_positions.size();

			Vector3f E0 = positions[1] - positions[0];
			Vector3f E1 = positions[2] - positions[0];
			Vector3f N = normalize(cross(E0, E1));

			if (flip)
				N = -N;

			Vector3f T = normalize(E0);
			Vector3f B = normalize(cross(T, N));

			for (u32 i = 0; i < vertexCount; ++i)
			{
				u32 j = flip ? i : vertexCount - i - 1;
				m_positions.push_back(positions[j]);
				m_colours.push_back(colour);
				m_normals.push_back(N);
				m_uvs.push_back(Vector2f(dot(T, positions[j]), dot(B, positions[j])));
			}

			for (u32 i = 1; i < (vertexCount - 1); ++i)
			{
				m_indices.push_back(i0);
				m_indices.push_back(i0 + i);
				m_indices.push_back(i0 + i + 1);
			}
		}

		void MeshBuilder::AddFace(u32 vertexCount, const Vector3f* positions, const ColourValue* colours, const Vector3f* normals, const Vector2f* uvs, bool flip)
		{
			u32 i0 = (u32)m_positions.size();
			for (u32 i = 0; i < vertexCount; ++i)
			{
				u32 j = flip ? i : vertexCount - i - 1;
				const Vector3f& P(positions[j]);
				m_positions.push_back(positions[j]);
				m_colours.push_back(colours[j]);
				m_normals.push_back(normals[j]);
				m_uvs.push_back(uvs[j]);
			}

			for (u32 i = 1; i < (vertexCount - 1); ++i)
			{
				m_indices.push_back(i0);
				m_indices.push_back(i0 + i);
				m_indices.push_back(i0 + i + 1);
			}
		}

		void MeshBuilder::AddTriangle(u32 i0, u32 i1, u32 i2)
		{
			m_indices.push_back(i0);
			m_indices.push_back(i1);
			m_indices.push_back(i2);
		}

		void MeshBuilder::AddTriangle(const Vector3f& P0, const Vector3f& P1, const Vector3f& P2, ColourValue colour, bool flip /*= false*/)
		{
			Vector3f positions[3] = { P0, P1, P2 };
			AddFace(3, positions, colour, flip);
		}

		void MeshBuilder::AddQuad(u32 i0, u32 i1, u32 i2, u32 i3)
		{
			m_indices.push_back(i0);
			m_indices.push_back(i1);
			m_indices.push_back(i2);

			m_indices.push_back(i0);
			m_indices.push_back(i2);
			m_indices.push_back(i3);
		}

		void MeshBuilder::AddQuad(const Vector3f& P0, const Vector3f& P1, const Vector3f& P2, const Vector3f& P3, ColourValue colour, bool flip /*= false*/)
		{
			Vector3f positions[4] = { P0, P1, P2 ,P3 };
			AddFace(4, positions, colour, flip);
		}

		void MeshBuilder::AddFlatFace(u32 vertexCount, const Vector3f* positions, ColourValue colour, bool flip)
		{
			PLAY_ASSERT(vertexCount >= 3);

			const Vector3f& P0(positions[0]);
			const Vector3f& P1(positions[1]);
			const Vector3f& P2(positions[2]);

			Vector3f E0 = P1 - P0;
			Vector3f E1 = P2 - P0;
			Vector3f N = normalize(cross(E0, E1));

			if (flip)
				N = -N;

			Vector3f T = normalize(E0);
			Vector3f B = normalize(cross(T, N));

			u32 i0 = (u32)m_positions.size();
			for (u32 i = 0; i < vertexCount; ++i)
			{
				u32 j = flip ? i : vertexCount - i - 1;
				const Vector3f& P(positions[j]);
				m_positions.push_back(P);
				m_colours.push_back(colour);
				m_normals.push_back(N);
				m_uvs.push_back(Vector2f(dot(T, P), dot(B, P))); // planar project onto basis vectors.
			}

			for (u32 i = 1; i < (vertexCount - 1); ++i)
			{
				m_indices.push_back(i0);
				m_indices.push_back(i0 + i);
				m_indices.push_back(i0 + i + 1);
			}
		}

		void MeshBuilder::AddIndexedFlatFace(u32 indexCount, const u32* indices, const Vector3f* positions, ColourValue colour, bool flip)
		{
			PLAY_ASSERT(indexCount >= 3);

			const Vector3f& P0(positions[indices[0]]);
			const Vector3f& P1(positions[indices[1]]);
			const Vector3f& P2(positions[indices[2]]);

			Vector3f E0 = P1 - P0;
			Vector3f E1 = P2 - P0;
			Vector3f N = normalize(cross(E0, E1));

			if (flip)
				N = -N;

			Vector3f T = normalize(E0);
			Vector3f B = normalize(cross(T, N));

			u32 i0 = (u32)m_positions.size();
			for (u32 i = 0; i < indexCount; ++i)
			{
				u32 j = flip ? i : indexCount - i - 1;
				const Vector3f& P(positions[indices[j]]);
				m_positions.push_back(P);
				m_colours.push_back(colour);
				m_normals.push_back(N);
				m_uvs.push_back(Vector2f(dot(T, P) + 0.5f, dot(B, P) + 0.5f));
			}

			for (u32 i = 1; i < (indexCount - 1); ++i)
			{
				m_indices.push_back(i0);
				m_indices.push_back(i0 + i);
				m_indices.push_back(i0 + i + 1);
			}
		}

		void MeshBuilder::Extrude(u32 vertexCount, const Vector3f* positions, ColourValue colour, f32 fLength)
		{
			std::vector<Vector3f> extrudedPositions(vertexCount);
			for (u32 i = 0; i < vertexCount; ++i)
			{
				extrudedPositions[i] = positions[i];
				extrudedPositions[i].y += fLength;
			}

			AddFlatFace(vertexCount, positions, colour, false);
			AddFlatFace(vertexCount, extrudedPositions.data(), colour, true);

			{
				u32 i = 0;
				for (; i < (vertexCount - 1); ++i)
				{
					AddQuad(positions[i], extrudedPositions[i], extrudedPositions[i + 1], positions[i + 1], colour, false);
				}
				AddQuad(positions[i], extrudedPositions[i], extrudedPositions[0], positions[0], colour, false);
			}
		}

		result_t MeshBuilder::ParseObjFormat(std::string_view objString, ColourValue colour, f32 fScale)
		{
			std::vector<Vector3f> positions;
			std::vector<Vector2f> texCoords;
			std::vector<Vector3f> normals;

			u32 nTotalFaceCount = 0;

			static constexpr u32 kMaxFaceVerts = 4;
			Vector3f facePositions[kMaxFaceVerts];
			Vector3f faceNormals[kMaxFaceVerts];
			Vector2f faceUVs[kMaxFaceVerts];
			ColourValue faceColours[kMaxFaceVerts];
			for (u32 i = 0; i < kMaxFaceVerts; ++i)
			{
				faceColours[i] = colour;
			}

			std::vector<std::string_view> args;

			size_t end = objString.size();
			size_t pos = 0;
			while (pos < end)
			{
				size_t offset = objString.find_first_of('\n', pos);
				if (std::string_view::npos == offset)
				{	
					pos = end;
				}
				else if(offset > 0)
				{
					args.clear();
					size_t lineLength = offset - pos;
					std::string_view line = objString.substr(pos, lineLength);
					size_t argLen = line.find_first_of(' ', 0);
					while (argLen != std::string_view::npos)
					{
						args.push_back(line.substr(0, argLen));
						line.remove_prefix(argLen + 1);
						argLen = line.find_first_of(' ', 0) ;
					}
					if (!line.empty())
					{
						args.push_back(line);
					}

					if (!args.empty())
					{
						if ("v" == args[0] && args.size() > 3)
						{
							Vector3f v;
							std::from_chars(args[1].data(), args[1].data() + args[1].size(), v.x);
							std::from_chars(args[2].data(), args[2].data() + args[2].size(), v.y);
							std::from_chars(args[3].data(), args[3].data() + args[3].size(), v.z);
							positions.push_back(v);
						}
						else if ("vn" == args[0] && args.size() > 3)
						{
							Vector3f v;
							std::from_chars(args[1].data(), args[1].data() + args[1].size(), v.x);
							std::from_chars(args[2].data(), args[2].data() + args[2].size(), v.y);
							std::from_chars(args[3].data(), args[3].data() + args[3].size(), v.z);
							normals.push_back(v);
						}
						else if ("vt" == args[0] && args.size() > 2)
						{
							Vector2f v;
							std::from_chars(args[1].data(), args[1].data() + args[1].size(), v.x);
							std::from_chars(args[2].data(), args[2].data() + args[2].size(), v.y);
							texCoords.push_back(v);
						}
						else if ("f" == args[0] && args.size() > 3)
						{
							u32 nFaceVerts = (u32)args.size() - 1;
							PLAY_ASSERT(nFaceVerts <= kMaxFaceVerts);

							for (u32 i = 0; i < nFaceVerts; ++i)
							{
								const std::string_view& rArg(args[i+1]);
								
								u32 nIndexArgs = 0;
								std::string_view indexArg[3];
								size_t t0 = rArg.find_first_of('/', 0);
								if (t0 != std::string_view::npos)
								{
									size_t t1 = rArg.find_first_of('/', t0+1);
									++nIndexArgs;
									if (t1 != std::string_view::npos)
									{
										++nIndexArgs;
										indexArg[0] = rArg.substr(0, t0);
										indexArg[1] = rArg.substr(t0 + 1, t1);
										indexArg[2] = rArg.substr(t1 + 1);
									}
									else
									{
										indexArg[0] = rArg.substr(0, t0);
										indexArg[1] = rArg.substr(t0 + 1);
									}
								}
								else
								{
									indexArg[0] = rArg;
								}

								u32 indices[3] = { 0,0,0 };
								for(u32 j = 0; j < 3; ++j)
								{
									if (!indexArg[0].empty())
									{
										std::from_chars(indexArg[j].data(), indexArg[j].data() + indexArg[j].size(), indices[j]);
									}
								}

								if (indices[0])
								{
									facePositions[i] = positions.at(indices[0] - 1) * fScale;
								}
								else
								{
									facePositions[i] = Vector3f(0, 0, 0);
								}

								if (indices[1])
								{
									faceUVs[i] = texCoords.at(indices[1] - 1);
								}
								else
								{
									faceUVs[i] = Vector2f(0, 0);
								}

								if (indices[2])
								{
									faceNormals[i] = normals.at(indices[2] - 1);
								}
								else
								{
									faceNormals[i] = Vector3f(0, 0, 0);
								}
							}
						
							AddFace(nFaceVerts, facePositions, faceColours, faceNormals, faceUVs);
							++nTotalFaceCount;
						}
						else
						{

						}
					}
					pos += lineLength + 1;
				}
				else
				{
					++pos;
				}
				
			}

			Debug::Printf("Parse OBJ v=%u vt=%u vn=%u f=%u\n", (u32)positions.size(), (u32)texCoords.size(), (u32)normals.size(), nTotalFaceCount);

			return RESULT_OK;
		}

		MeshId MeshBuilder::CreateMesh()
		{
			MeshDesc desc;
			StreamInfo streamInfos[5];
			streamInfos[0].m_type = StreamType::POSITION;
			streamInfos[0].m_pData = m_positions.data();
			streamInfos[0].m_dataSize = m_positions.size() * sizeof(Vector3f);

			streamInfos[1].m_type = StreamType::COLOUR;
			streamInfos[1].m_pData = m_colours.data();
			streamInfos[1].m_dataSize = m_colours.size() * sizeof(u32);

			streamInfos[2].m_type = StreamType::NORMAL;
			streamInfos[2].m_pData = m_normals.data();
			streamInfos[2].m_dataSize = m_normals.size() * sizeof(Vector3f);

			streamInfos[3].m_type = StreamType::UV;
			streamInfos[3].m_pData = m_uvs.data();
			streamInfos[3].m_dataSize = m_uvs.size() * sizeof(Vector2f);

			streamInfos[4].m_type = StreamType::INDEX;
			streamInfos[4].m_pData = m_indices.data();
			streamInfos[4].m_dataSize = m_indices.size() * sizeof(u32);

			desc.m_pStreams = streamInfos;
			desc.m_streamCount = 5;
			desc.m_vertexCount = (u32)m_positions.size();
			desc.m_indexCount = (u32)m_indices.size();

			return Resources::CreateAsset<Mesh>(desc);
		}

		void MeshBuilder::Reset()
		{
			m_positions.clear();
			m_colours.clear();
			m_normals.clear();
			m_uvs.clear();
			m_indices.clear();
		}

	}
}



//-----------------------------------------------------------
// Play3dImpl\Play3dApi.cpp








//-----------------------------------------------------------
// Play3dImpl\PrimitiveBatch_Impl.cpp


namespace Play3d
{
	namespace Graphics
	{

		PrimitiveBatch::PrimitiveBatch(ID3D11Device* pDevice, u32 kMaxVertexCount)
		 : m_totalVertexCount(0)
		 , m_maxVertexCount(kMaxVertexCount)
		 , m_pointVertexCount(0)
		 , m_lineVertexCount(0)
		 , m_triangleVertexCount(0)
		{
			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = sizeof(PrimitiveVertex) * kMaxVertexCount;
			desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			HRESULT hr = pDevice->CreateBuffer(&desc, NULL, &m_pVertexBuffer);
			PLAY_ASSERT_MSG(SUCCEEDED(hr), "CreateBuffer");
		}

		PrimitiveBatch::~PrimitiveBatch()
		{

		}

		void PrimitiveBatch::AppendPoint(const Vector3f& v1, ColourValue c1)
		{
			if((m_totalVertexCount + 1) < m_maxVertexCount)
			{
				m_points.push_back({ v1, c1 });
				m_totalVertexCount += 1;
			}
		}

		void PrimitiveBatch::AppendLine(const Vector3f& v1, const Vector3f& v2, ColourValue c1, ColourValue c2)
		{
			m_lines.push_back({ v1, c1 });
			m_lines.push_back({ v2, c2 });
		}

		void PrimitiveBatch::AppendTriangle(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, ColourValue c1, ColourValue c2, ColourValue c3)
		{
			m_triangles.push_back({ v1, c1 });
			m_triangles.push_back({ v2, c2 });
			m_triangles.push_back({ v3, c3 });
		}

		void PrimitiveBatch::Flush(ID3D11DeviceContext* pContext)
		{
			D3D11_MAPPED_SUBRESOURCE data;
			HRESULT hr = pContext->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &data);
			if (SUCCEEDED(hr))
			{
				m_pointVertexCount = (u32)m_points.size();
				m_lineVertexCount = (u32)m_lines.size();
				m_triangleVertexCount = (u32)m_triangles.size();

				PrimitiveVertex* pVertexOut = (PrimitiveVertex*)data.pData;
				memcpy(pVertexOut, m_points.data(), m_pointVertexCount * sizeof(PrimitiveVertex));
				pVertexOut += m_pointVertexCount;
				
				memcpy(pVertexOut, m_lines.data(), m_lineVertexCount * sizeof(PrimitiveVertex));
				pVertexOut += m_lineVertexCount;
				
				memcpy(pVertexOut, m_triangles.data(), m_triangleVertexCount * sizeof(PrimitiveVertex));

				pContext->Unmap(m_pVertexBuffer.Get(), 0);


				m_totalVertexCount = 0;
				m_points.clear();
				m_lines.clear();
				m_triangles.clear();
			}

		}

		void PrimitiveBatch::Bind(ID3D11DeviceContext* pContext)
		{
			ID3D11Buffer* buffers[] = { m_pVertexBuffer.Get() };
			UINT strides[] = { sizeof(PrimitiveVertex) };
			UINT offsets[] = { 0 };

			pContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);
		}

		void PrimitiveBatch::DrawPoints(ID3D11DeviceContext* pContext)
		{
			if(m_pointVertexCount > 0)
			{
				pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
				pContext->Draw(m_pointVertexCount, 0);
			}
		}

		void PrimitiveBatch::DrawLines(ID3D11DeviceContext* pContext)
		{
			if(m_lineVertexCount > 0)
			{
				pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
				pContext->Draw(m_lineVertexCount, m_pointVertexCount);
			}
		}

		void PrimitiveBatch::DrawTriangles(ID3D11DeviceContext* pContext)
		{
			if (m_triangleVertexCount > 0)
			{
				pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				pContext->Draw(m_triangleVertexCount, m_pointVertexCount + m_lineVertexCount);
			}
		}

	}
}

//-----------------------------------------------------------
// Play3dImpl\ResourcesApi.cpp



namespace Play3d
{
	namespace Resources
	{

		IdKey<AsyncLoadingTask> AsyncLoadAssets(std::string_view pathToAssetPack)
		{
			return IdKey<AsyncLoadingTask>();
		}

		bool AssetsLoaded(IdKey<AsyncLoadingTask> hAsyncLoad)
		{
			return true;
		}

		Play3d::result_t LoadAssets(std::string_view pathToAssetPack)
		{
			return 0;
		}

	}
}



//-----------------------------------------------------------
// Play3dImpl\Sampler.cpp



namespace Play3d
{
	namespace Graphics
	{
		D3D11_FILTER TranslateFilterMode(FilterMode mode)
		{
			switch (mode)
			{
			case FilterMode::BILINEAR:
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			case FilterMode::POINT:
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
			default:
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
			};
		}

		D3D11_TEXTURE_ADDRESS_MODE TranslateAddressMode(AddressMode mode)
		{
			switch (mode)
			{
			case AddressMode::CLAMP:
				return D3D11_TEXTURE_ADDRESS_CLAMP;
			case AddressMode::WRAP:
				return D3D11_TEXTURE_ADDRESS_WRAP;
			default:
				return D3D11_TEXTURE_ADDRESS_CLAMP;
			};
		}

		Sampler::Sampler(const SamplerDesc& rDesc)
		{
			ID3D11Device* pDevice = Graphics::Graphics_Impl::Instance().GetDevice();

			D3D11_SAMPLER_DESC desc = {};
			desc.Filter = TranslateFilterMode(rDesc.m_filter);
			D3D11_TEXTURE_ADDRESS_MODE mode = TranslateAddressMode(rDesc.m_addressMode);
			desc.AddressU = mode;
			desc.AddressV = mode;
			desc.AddressW = mode;

			HRESULT hr = pDevice->CreateSamplerState(&desc, &m_pSampler);
			PLAY_ASSERT(SUCCEEDED(hr));

		}
	}
}


//-----------------------------------------------------------
// Play3dImpl\Shader.cpp



#pragma comment(lib,"d3dcompiler.lib")

namespace Play3d
{
	namespace Graphics
	{

		Shader::Shader(const ShaderDesc& rDesc)
			: m_shaderType(rDesc.m_type)
			, m_pByteCode(nullptr)
			, m_sizeBytes(0)
		{
			auto pDevice(Graphics_Impl::Instance().GetDevice());
			
			HRESULT hr;

			std::string name(rDesc.m_name);

			switch (rDesc.m_type)
			{
			case ShaderType::VERTEX_SHADER:
			{
				ComPtr<ID3D11VertexShader> pVertexShader;
				hr = pDevice->CreateVertexShader(rDesc.m_pByteCode, rDesc.m_sizeBytes, NULL, &pVertexShader);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "Failed to create vertex shader.");

				hr = pVertexShader.As(&m_pShader);
				PLAY_ASSERT(SUCCEEDED(hr));

				name += "_VS";
				break;
			}
			case ShaderType::PIXEL_SHADER:
			{
				ComPtr<ID3D11PixelShader> pPixelShader;
				hr = pDevice->CreatePixelShader(rDesc.m_pByteCode, rDesc.m_sizeBytes, NULL, &pPixelShader);
				PLAY_ASSERT_MSG(SUCCEEDED(hr), "Failed to create pixel shader.");

				hr = pPixelShader.As(&m_pShader);
				PLAY_ASSERT(SUCCEEDED(hr));

				name += "_PS";
				break;
			}
			default:
				PLAY_ASSERT(false);
			};

			m_pByteCode = _aligned_malloc(rDesc.m_sizeBytes, 16);
			PLAY_ASSERT(m_pByteCode);
			memcpy(m_pByteCode, rDesc.m_pByteCode, rDesc.m_sizeBytes);
			m_sizeBytes = rDesc.m_sizeBytes;

			
			hr = m_pShader->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.size(), name.data());
			PLAY_ASSERT(SUCCEEDED(hr));

		}

		Shader::~Shader()
		{
			if (m_pByteCode)
			{
				_aligned_free(m_pByteCode);
				m_pByteCode = nullptr;
			}
		}

		
		void Shader::Bind(ID3D11DeviceContext* pContext)
		{
			HRESULT hr;
			switch (m_shaderType)
			{
			case ShaderType::VERTEX_SHADER:
			{
				ComPtr<ID3D11VertexShader> pVertexShader;
				hr = m_pShader.As(&pVertexShader);
				PLAY_ASSERT(SUCCEEDED(hr));
				pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);
				break;
			}
			case ShaderType::PIXEL_SHADER:
			{
				ComPtr<ID3D11PixelShader> pPixelShader;
				hr = m_pShader.As(&pPixelShader);
				PLAY_ASSERT(SUCCEEDED(hr));
				pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);
				break;
			}
			default:
				PLAY_ASSERT(false);
			};
		}

		ShaderId Shader::Compile(const ShaderCompilerDesc& rDesc)
		{
			Debug::Printf("Compiling %s : %s\n", rDesc.m_name.c_str(), rDesc.m_entryPoint.c_str());

			UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;	
			if (rDesc.m_flags & (u32)ShaderCompilationFlags::DEBUG)
			{
				flags |= D3DCOMPILE_DEBUG;
			}

			const char* pTarget;
			switch (rDesc.m_type)
			{
				case ShaderType::VERTEX_SHADER:
					pTarget = "vs_5_0";
					break;
				case ShaderType::PIXEL_SHADER:
					pTarget = "ps_5_0";
					break;
				default:
					pTarget = "vs_5_0";
			};

			std::vector<D3D_SHADER_MACRO> defines = rDesc.m_defines;
			defines.push_back({NULL,NULL});
			


			ComPtr<ID3DBlob> pShaderBlob;
			ComPtr<ID3DBlob> pErrorBlob;

			HRESULT hr = D3DCompile(
				rDesc.m_hlslCode.c_str(),
				rDesc.m_hlslCode.size(),
				NULL,
				defines.data(),
				NULL,
				rDesc.m_entryPoint.c_str(),
				pTarget,
				flags,
				0,
				&pShaderBlob,
				&pErrorBlob
			);

			if (FAILED(hr))
			{
				if (pErrorBlob)
				{
					Debug::Printf("!! Shader Compilation Failed !! : \n--------------------------------\n\n%s\n\n--------------------------------", (const char*)pErrorBlob->GetBufferPointer());
				}
				return ShaderId();
			}

			ShaderDesc desc;
			desc.m_pByteCode = pShaderBlob->GetBufferPointer();
			desc.m_sizeBytes = pShaderBlob->GetBufferSize();
			desc.m_type = rDesc.m_type;
			desc.m_name = rDesc.m_name;

			return Resources::CreateAsset<Shader>(desc);
		}
	}
}



//-----------------------------------------------------------
// Play3dImpl\ShaderCode_Impl.cpp



namespace Play3d
{
namespace Graphics
{

const char* HLSL_FontShader = R"(
cbuffer UIFrameConstantData : register(b0)
{
	float4x4 viewProjectionMtx;
	float4 viewportRect;
};
struct VSInput
{
	float2 position : POSITION;
	float2 uv : UV;
	float4 colour : COLOUR;
};
struct PSInput
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float2 uv : UV;
};
PSInput VS_Main(VSInput input)
{
	PSInput output;
	output.position = mul(viewProjectionMtx, float4(input.position.xy, -0.1f, 1.0f));
	output.uv = input.uv;
	output.colour = pow(input.colour, 2.2); // Gamma correct from sRGB.
	return output;
}
Texture2D g_texture0 : register(t0);
SamplerState g_sampler0 : register(s0);
float4 PS_Main(PSInput input) : SV_TARGET
{
	float alpha = g_texture0.Sample(g_sampler0, input.uv).r;
	return input.colour * float4(1,1,1,alpha);
}

)"; // HLSL_FontShader
const char* HLSL_MeshShader = R"(
#define MAX_LIGHTS 4
cbuffer FrameConstantData : register(b0)
{
	float4x4 viewMtx;
	float4x4 projectionMtx;
	float4x4 viewProjectionMtx;
};
cbuffer DrawConstantData : register(b1)
{
	float4x4 mvpMtx;
	float4x4 worldMtx;
	float4x4 normalMtx;
};
cbuffer LightConstantData : register(b2)
{
	float4 lightPos[MAX_LIGHTS];
	float4 lightDir[MAX_LIGHTS];
	float4 lightColour[MAX_LIGHTS];
};
cbuffer MaterialConstantData : register(b3)
{
	float4 diffuseColour;
	float4 specularColour;
};
struct VSInput
{
	float3 position : POSITION;
	float4 colour : COLOUR;
	float3 normal : NORMAL;
	float2 uv : UV;
};
struct PSInput
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float3 normal : NORMAL;
	float2 uv : UV;
};
PSInput VS_Main(VSInput input)
{
	PSInput output;
	output.position = mul(mvpMtx, float4(input.position.xyz, 1.0f));
	output.colour = pow(input.colour, 2.2); // Gamma correct from sRGB.
	output.normal = mul(normalMtx, float4(input.normal.xyz, 0.0f));
	output.uv = input.uv;
	return output;
}
#if USE_TEXTURE_0
Texture2D g_texture0 : register(t0);
SamplerState g_sampler0 : register(s0);
#endif
float4 PS_Main(PSInput input) : SV_TARGET
{
#if USE_TEXTURE_0
	float4 vTexture0 = g_texture0.Sample(g_sampler0, input.uv);
	float4 vAlbedo = input.colour * diffuseColour * vTexture0;
#else
	float4 vAlbedo = input.colour * diffuseColour;
#endif
#if USE_LIGHTING
	float3 N = normalize(input.normal);
	float3 vDiffuse = float3(0, 0, 0);
	[unroll] for (uint i = 0; i < LIGHT_COUNT; ++i)
	{
		float3 L = lightDir[i];
		float I = max(dot(N, L), 0);
		vDiffuse += lightColour[i] * I;
	}
#else
	float3 vDiffuse = float3(1.0, 1.0, 1.0);
#endif
	return float4(vAlbedo.rgb * vDiffuse, 1);
}

)"; // HLSL_MeshShader
const char* HLSL_PrimitiveBatchShader = R"(
cbuffer perFrameConstantBuffer : register(b0)
{
	float4x4 viewMtx;
	float4x4 projectionMtx;
	float4x4 viewProjectionMtx;
};
struct VSInput
{
	float3 position : POSITION;
	float4 colour : COLOUR;
};
struct PSInput
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
};
PSInput VS_Main(VSInput input)
{
	PSInput output;
	output.position = mul(viewProjectionMtx, float4(input.position.xyz, 1.0f));
	output.colour = pow(input.colour, 2.2);
	return output;
}
float4 PS_Main(PSInput input) : SV_TARGET
{
	return input.colour;
}

)"; // HLSL_PrimitiveBatchShader
const char* HLSL_TestShader = R"(
cbuffer perFrameConstantBuffer : register(b0)
{
	float4x4 viewProjectMtx;
	float4x4 screenProjectMtx;
};
struct VSInput
{
	float3 position : POSITION;
	float4 color : COLOR;
};
struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};
PSInput VS_Main(VSInput input)
{
	PSInput output;
	output.position = mul(viewProjectMtx, float4(input.position.xyz, 1.0f));
	output.color = input.color;
	return output;
}
float4 PS_Main(PSInput input) : SV_TARGET
{
	return input.color;
}

)"; // HLSL_TestShader
} // Graphics
} // Play3d


//-----------------------------------------------------------
// Play3dImpl\Sound.cpp



namespace Play3d
{
	namespace Audio
	{
		struct RiffChunk
		{
			u32 m_id;
			u32 m_size;
		};

		Sound::Sound(const SoundDesc& rDesc)
			: m_format{}
			, m_buffer{}
			, m_pData(nullptr)
			, m_sizeBytes(0)
		{

			const u8* p(static_cast<const u8*>(rDesc.m_pData));
			const u8* pEnd(p + rDesc.m_sizeBytes);

			const RiffChunk* pRiffChunk(reinterpret_cast<const RiffChunk*>(p)); // RIFF
			PLAY_ASSERT_MSG(pRiffChunk->m_id == 'FFIR', "RIFF chunk invalid");
			p += sizeof(RiffChunk);

			const u32* pWave(reinterpret_cast<const u32*>(p)); // WAVE
			PLAY_ASSERT_MSG(*pWave == 'EVAW', "WAVE chunk invalid");
			p += sizeof(u32);

			const RiffChunk* pChunk(reinterpret_cast<const RiffChunk*>(p));
			p += sizeof(RiffChunk);

			bool bFmt = false;
			bool bData = false;

			while(p < pEnd)
			{
				switch (pChunk->m_id)
				{
				case ' tmf':
					PLAY_ASSERT_MSG(sizeof(PCMWAVEFORMAT) <= pChunk->m_size, "_fmt chunk invalid");
					memcpy(&m_format, p, sizeof(PCMWAVEFORMAT));
					bFmt = true;
					break;

				case 'atad':
					m_pData = _aligned_malloc(pChunk->m_size, 16);
					PLAY_ASSERT(m_pData);
					memcpy(m_pData, p, pChunk->m_size);
					m_sizeBytes = pChunk->m_size;
					bData = true;
					break;
				}

				p += pChunk->m_size;

				pChunk = reinterpret_cast<const RiffChunk*>(p);
				p += sizeof(RiffChunk);
			}

			PLAY_ASSERT_MSG(bFmt && bData, "Invalid Sound Data");

			
			m_buffer.AudioBytes = static_cast<u32>(m_sizeBytes);
			m_buffer.pAudioData = static_cast<const BYTE*>(m_pData);
			m_buffer.Flags = XAUDIO2_END_OF_STREAM;
		}

		Sound::~Sound()
		{
			if (m_pData)
			{
				_aligned_free(m_pData);
			}
		}

	}
}


//-----------------------------------------------------------
// Play3dImpl\SpriteApi.cpp



namespace Play3d
{
	namespace Sprite
	{

		void DrawSprite(SpriteAtlasId hAtlas, Graphics::TextureId texture, u32 index)
		{

		}

	}
}


//-----------------------------------------------------------
// Play3dImpl\SystemApi.cpp







namespace Play3d
{
	namespace System
	{
		struct SystemImpl
		{
			SystemImpl()
			{
				m_clockFrequency.QuadPart = 0;
				m_currentTimeMicroSeconds = 0;
				m_fElapsedTime = 0.f;
				m_fDeltaTime = 0.f;

				LARGE_INTEGER timestamp;
				QueryPerformanceFrequency(&m_clockFrequency);
				QueryPerformanceCounter(&timestamp);
				m_prevTimeMicroSeconds = (timestamp.QuadPart * 1000000) / m_clockFrequency.QuadPart;		
			}

			void BeginFrame()
			{
				LARGE_INTEGER timestamp;
				QueryPerformanceCounter(&timestamp);
				m_currentTimeMicroSeconds = (timestamp.QuadPart * 1000000) / m_clockFrequency.QuadPart;
				m_deltaTimeMicroSeconds = m_currentTimeMicroSeconds - m_prevTimeMicroSeconds;

				m_fDeltaTime = (f32)m_deltaTimeMicroSeconds / 1000000.0f;
				m_fElapsedTime += (f64)m_fDeltaTime;
			}

			void EndFrame()
			{
				m_prevTimeMicroSeconds = m_currentTimeMicroSeconds;
			}

			static SystemImpl* ms_pInstance;

			LARGE_INTEGER m_clockFrequency;
			u64 m_prevTimeMicroSeconds;
			u64 m_currentTimeMicroSeconds;
			u64 m_deltaTimeMicroSeconds;

			f64 m_fElapsedTime;
			f32 m_fDeltaTime;
		};
		SystemImpl* SystemImpl::ms_pInstance = nullptr;

		bool IsInitialised()
		{
			return SystemImpl::ms_pInstance != nullptr;
		}

		result_t Initialise()
		{
			if(!SystemImpl::ms_pInstance)
			{
				SystemImpl::ms_pInstance = new SystemImpl;

				Graphics::Graphics_Impl::Initialise();
				Input::Input_Impl::Initialise();
				Audio::Audio_Impl::Initialise();

				Graphics::Graphics_Impl::Instance().PostInitialise();
			}

			return RESULT_OK;
		}

		result_t BeginFrame()
		{
			PLAY_ASSERT(SystemImpl::ms_pInstance);
			SystemImpl::ms_pInstance->BeginFrame();

			result_t result = Graphics::Graphics_Impl::Instance().BeginFrame();
			if (result != RESULT_OK)
			{
				return result;
			}

			Input::Input_Impl::Instance().BeginFrame();
			Audio::Audio_Impl::Instance().BeginFrame();

			return RESULT_OK;
		}

		result_t EndFrame()
		{
			PLAY_ASSERT(SystemImpl::ms_pInstance);
			SystemImpl::ms_pInstance->EndFrame();
			result_t result = Graphics::Graphics_Impl::Instance().EndFrame();
			if (result != RESULT_OK)
			{
				return result;
			}
			Input::Input_Impl::Instance().EndFrame();
			Audio::Audio_Impl::Instance().EndFrame();

			return RESULT_OK;
		}

		result_t Shutdown()
		{
			if(SystemImpl::ms_pInstance)
			{
				Graphics::Graphics_Impl::Instance().Flush();

				Resources::ResourceManager<Graphics::Material>::Instance().ReleaseAll();
				Resources::ResourceManager<Graphics::Shader>::Instance().ReleaseAll();
				Resources::ResourceManager<Graphics::Mesh>::Instance().ReleaseAll();
				Resources::ResourceManager<UI::Font>::Instance().ReleaseAll();

				Audio::Audio_Impl::Destroy();
				Input::Input_Impl::Destroy();
				Graphics::Graphics_Impl::Destroy();
				
				PLAY_SAFE_DELETE(SystemImpl::ms_pInstance);
			}
			return RESULT_OK;
		}

		f64 GetElapsedTime()
		{
			PLAY_ASSERT(SystemImpl::ms_pInstance);
			return SystemImpl::ms_pInstance->m_fElapsedTime;
		}

		f32 GetDeltaTime()
		{
			PLAY_ASSERT(SystemImpl::ms_pInstance);
			return SystemImpl::ms_pInstance->m_fDeltaTime;
		}

		void* LoadFileData(const char* filePath, size_t& sizeOut)
		{
			sizeOut = 0;
			void* pMemoryRet = nullptr;

			HANDLE hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				LARGE_INTEGER size;
				if (GetFileSizeEx(hFile, &size))
				{
					constexpr u32 kPadding = 16;
					size_t allocSize = (size_t)size.LowPart + kPadding;
					u8* pBuffer = (u8*)_aligned_malloc(allocSize, 16);
					PLAY_ASSERT(pBuffer);

					pBuffer[0] = '\0';
					memset(pBuffer + size.LowPart, 0, kPadding);

					DWORD bytesRead = 0;
					if (ReadFile(hFile, pBuffer, size.LowPart, &bytesRead, NULL))
					{
						sizeOut = size.QuadPart;
						pMemoryRet = pBuffer;
					}
					else
					{
						ReleaseFileData(pBuffer);
					}
				}
				CloseHandle(hFile);
			}
			return pMemoryRet;
		}

		void ReleaseFileData(void* pMemory)
		{
			if (pMemory)
			{
				_aligned_free(pMemory);
			}
		}
	}
}

#ifndef PLAY_USE_CLIENT_MAIN
extern int PlayMain();
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	return PlayMain();
}
#endif


//-----------------------------------------------------------
// Play3dImpl\Texture.cpp




namespace Play3d
{
	namespace Graphics
	{
		DXGI_FORMAT TranslateTextureFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::GRAYSCALE:  return DXGI_FORMAT_R8_UNORM;
			case TextureFormat::RGBA:	return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			default:
				return DXGI_FORMAT_R8_UNORM;
			};
		}

		u32 TexturePitchByFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::GRAYSCALE:  return 1;
			case TextureFormat::RGBA:	return 4;
			default:
				return 4;
			};
		}

		Texture::Texture(const TextureDesc& rDesc)
		{
			ID3D11Device* pDevice = Graphics::Graphics_Impl::Instance().GetDevice();

			D3D11_TEXTURE2D_DESC desc = {};
			desc.Width = rDesc.m_width;
			desc.Height = rDesc.m_height;
			desc.ArraySize = 1;
			desc.Format = TranslateTextureFormat(rDesc.m_format);
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.CPUAccessFlags = 0;
			if(rDesc.m_bGenerateMips)
			{
				desc.MipLevels = (u32)log2(std::min(rDesc.m_width, rDesc.m_height));
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE ;
				desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			}
			else
			{
				desc.MipLevels = 1;
				desc.Usage = rDesc.m_pImageData ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.MiscFlags = 0;
			}

			std::vector<D3D11_SUBRESOURCE_DATA> subresources(desc.MipLevels);

			u32 mipWidth = rDesc.m_width;
			for (u32 i = 0; i < desc.MipLevels; ++i)
			{
				subresources[i].pSysMem = rDesc.m_pImageData;
				subresources[i].SysMemPitch = mipWidth * TexturePitchByFormat(rDesc.m_format);
				mipWidth /= 2;
			}

			HRESULT hr = pDevice->CreateTexture2D(&desc, rDesc.m_pImageData ? subresources.data() : nullptr, &m_pTexture);
			PLAY_ASSERT(SUCCEEDED(hr));

			hr = pDevice->CreateShaderResourceView(m_pTexture.Get(), nullptr, &m_pSRV);
			PLAY_ASSERT(SUCCEEDED(hr));

			if (rDesc.m_bGenerateMips)
			{
				Graphics::Graphics_Impl::Instance().QueueGenerateMips(m_pSRV.Get());
			}
		}

	}
}



//-----------------------------------------------------------
// Play3dImpl\TypesApi.cpp



namespace Play3d
{


} // namespace Play3d


//-----------------------------------------------------------
// Play3dImpl\UIApi.cpp



namespace Play3d
{

	namespace UI
	{

		FontId GetDebugFont()
		{
			static FontId s_debugFontId;
			if (s_debugFontId.IsInvalid())
			{
				UI::FontDesc desc;
				desc.m_fontName = "Arial";
				desc.m_pointSize = 20;
				desc.m_charSet = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789(){}[]@~#.,<>=+/";
				desc.m_textureWidth = 512;
				desc.m_textureHeight = 64;
				s_debugFontId = Resources::CreateAsset<UI::Font>(desc);
			}
			return s_debugFontId;
		}

		void DrawPrintf(FontId hFont, const Vector2f& position, ColourValue colour, const char* fmt, ...)
		{
			static constexpr u32 kBufferSize = 1024;
			char strBuffer[kBufferSize];
			va_list args;
			va_start(args, fmt);
			vsprintf_s(strBuffer, kBufferSize, fmt, args);
			va_end(args);

			DrawString(hFont, position, colour, strBuffer);
		}

		void DrawString(FontId hFont, const Vector2f& position, ColourValue colour,  std::string_view text)
		{
			Font* pFont = Resources::ResourceManager<Font>::Instance().GetPtr(hFont);
			if (pFont)
			{
				auto* pDC = Graphics::Graphics_Impl::Instance().GetDeviceContext();
				pFont->DrawString(pDC, position, colour, text);
			}
		}

		bool DrawButton(Sprite::SpriteAtlasId hAtlas, Graphics::TextureId texture, u32 index)
		{
			return false;
		}

	}
}
////////////////// END IMPLEMENTATION SECTION ////////////////////////////
#endif PLAY_IMPLEMENTATION
