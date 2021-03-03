#pragma once

#include "System/Typedefs.h"
#include <System/File.h>
#include <memory.h>
#include <math.h>

template< UInt32 _NumDimensions, class _TChild >
class AVector {
	CLASS_TYPEDEFS( AVector );

public:
	typedef _TChild	TChild;
	enum {
		NumDimensions = _NumDimensions
	};

protected:
	Float32 m_Data[ NumDimensions ];

public:
	AVector() {
	}

	AVector( Float32 val ) {
		for( UInt32 ix = 0; ix < NumDimensions; ++ix ) {
			m_Data[ix] = val;
		}
	}

	AVector( const TSelf& rhs ) {
		for( UInt32 ix = 0; ix < NumDimensions; ++ix ) {
			m_Data[ix] = rhs[ix];
		}
	}

	static const TChild&	Zero() {
		static TChild	zero( 0.0f );
		return zero;
	}

	Float32&		operator[]( Int32 index ) {
		assert( index < NumDimensions );
		return m_Data[ index ]; 
	}

	const Float32&		operator[]( Int32 index ) const {
		assert( index < NumDimensions );
		return m_Data[ index ];
	}

	Float32&		operator[]( UInt32 index ) {
		assert( index < NumDimensions );
		return m_Data[ index ]; 
	}

	const Float32&		operator[]( UInt32 index ) const {
		assert( index < NumDimensions );
		return m_Data[ index ];
	}

	TChild	operator-() const {
		TChild local;

		for( UInt32 ix = 0; ix < NumDimensions; ++ix ) {
			local[ix] = -m_Data[ix];
		}

		return local;
	}

	TChild&		operator=( const TChild& rhs ) {
		UInt32 numBytes = NumDimensions * sizeof(Float32);
		memcpy_s( m_Data, numBytes, rhs.m_Data, numBytes );
		return *this;
	}

	TChild		operator*( Float32 scale ) const {
		TChild scaled;
		for( UInt32 ix = 0; ix < NumDimensions; ++ix ) {
			scaled[ix] = m_Data[ix] * scale;
		}
		return scaled;
	}

	TChild&		operator*=( Float32 scale ) {
		for( UInt32 ix = 0; ix < NumDimensions; ++ix ) {
			m_Data[ix] *= scale;
		}
		return *(TChild*)this;
	}

	TChild&		operator+=(const TChild& rhs) {
		for (UInt32 ix = 0; ix < NumDimensions; ++ix) {
			m_Data[ix] += rhs[ix];
		}
		return *(TChild*)this;
	}

	TChild&		operator-=(const TChild& rhs) {
		for (UInt32 ix = 0; ix < NumDimensions; ++ix) {
			m_Data[ix] -= rhs[ix];
		}
		return *(TChild*)this;
	}

	Float32	Length() const {
		Float32 lengthSquared = 0.0f;
		for( UInt32 ix = 0; ix < NumDimensions; ++ix ) {
			lengthSquared += m_Data[ix] * m_Data[ix];
		}
		return sqrtf( lengthSquared );
	}

	void	Normalize() {
		Float32 length = Length();
		for( UInt32 ix = 0; ix < NumDimensions; ++ix ) {
			m_Data[ix] /= length;
		}
	}

	TChild	AsNormal() const {
		TChild norm(*(TChild*)this);
		norm.Normalize();
		return norm;
	}

	Bool	ReadFrom(json_value* root) {
		for (int ix = 0; ix < root->u.object.length; ++ix) {
			JsonSerializer<Float32>::ReadFrom( root->u.object.values[ix].value, m_Data[ix] );
		}
		
		return true;
	}
};

template< Byte _NumDimensions >
class Vector;

template<>
class Vector<2> : public AVector< 2, Vector<2> > {
	INHERITEDCLASS_TYPEDEFS( Vector, AVector );

public:

protected:

public:
	using TSuper::operator[];

	using AVector::AVector;

	Vector() {
	}

	Vector( Float32 xVal, Float32 yVal ) {
		m_Data[0] = xVal;
		m_Data[1] = yVal;
	}

	Vector( Float32 val ) : AVector( val ) {
	}
};

template<>
class Vector<3> : public AVector< 3, Vector<3> > {
	INHERITEDCLASS_TYPEDEFS( Vector, AVector );

public:

protected:

public:
	using TSuper::operator[];

	Vector() {
	}

	Vector( Float32 xVal, Float32 yVal, Float32 zVal ) {
		m_Data[0] = xVal;
		m_Data[1] = yVal;
		m_Data[2] = zVal;
	}

	Vector( Float32 val ) : AVector( val ) {
	}

	static const TSelf&		Up() {
		static TSelf	up( 0.0f, 1.0f, 0.0f );
		return up;
	}
};

inline Vector<3> operator+(const Vector<3> &lhs, const Vector<3> &rhs)
{
	Vector<3> result;

	for (UInt32 ix = 0; ix < 3; ++ix) {
		result[ix] = lhs[ix] + rhs[ix];
	}

	return(result);
}

inline Vector<3> operator-(const Vector<3> &lhs, const Vector<3> &rhs)
{
	Vector<3> result;

	for (UInt32 ix = 0; ix < 3; ++ix) {
		result[ix] = lhs[ix] - rhs[ix];
	}

	return(result);
}

template<>
class Vector<4> : public AVector< 4, Vector<4> > {
	INHERITEDCLASS_TYPEDEFS( Vector, AVector );

public:

protected:

public:
	using TSuper::operator[];

	Vector() {
	}

	Vector( Float32 xVal, Float32 yVal, Float32 zVal, Float32 wVal ) {
		m_Data[0] = xVal;
		m_Data[1] = yVal;
		m_Data[2] = zVal;
		m_Data[3] = wVal;
	}

	Vector( Float32 val ) : AVector( val ) {
	}

	TSelf&		operator=( const Vector<3>& rhs ) {
		UInt32 numBytes = Vector<3>::NumDimensions * sizeof(Float32);
		memcpy_s( m_Data, numBytes, &rhs[0], numBytes );
		return *this;
	}
};

template<Byte _NumDimensions>
class JsonSerializer<Vector<_NumDimensions>> {
public:
	static Vector<_NumDimensions>	ReadFrom(json_value* root) {
		Vector<_NumDimensions>	val;
		ReadFrom(root, val);
		return val;
	}

	static Bool	ReadFrom(json_value* root, Vector<_NumDimensions>& outVal) {
		for (UInt32 ix = 0; ix < root->u.array.length; ++ix) {
			outVal[ix] = JsonSerializer<Float32>::ReadFrom(root->u.array.values[ix]);
		}
		return true;
	}
};