#pragma once

#include "../System/Typedefs.h"
#include "../Math/Vector.h"
#include <System/File.h>

#include <math.h>

/////////////////////////////////////////////////////
template< Byte _NumRows, Byte _NumColumns, class TChild >
class AMatrix {
	CLASS_TYPEDEFS( AMatrix );

public:
	enum {
		NumRows = _NumRows,
		NumColumns = _NumColumns
	};

	typedef Vector<NumColumns>	TComponent;

protected:
	TComponent	m_Rows[ NumRows ];

public:
	UInt32	Size() const {
		return NumRows * NumColumns;
	}

	TComponent&	operator[]( UInt32 index ) {
		assert( index < NumRows );
		return m_Rows[ index ];
	}

	void	Copy( Float32* data, UInt32 size ) {
		assert( Size() == size );

		for( UInt32 iy = 0; iy < NumRows; ++iy ) {
			for( UInt32 ix = 0; ix < NumColumns; ++ix ) {
				data[iy * NumRows + ix] = m_Rows[iy][ix];
			}
		}
	}

	const TComponent&	operator[]( UInt32 index ) const {
		assert( index < NumRows );
		return m_Rows[ index ];
	}

	TComponent&	operator[]( Int32 index ) {
		assert( index < NumRows );
		return m_Rows[ index ];
	}

	const TComponent&	operator[]( Int32 index ) const {
		assert( index < NumRows );
		return m_Rows[ index ];
	}

	TChild&	operator=( const TChild& rhs ) {
		for( UInt32 ix = 0; ix < NumRows; ++ix ) {
			m_Rows[ ix ] = rhs[ ix ];
		}
	}

	void	SetToIdentity() {
		Identity( *(TChild*)this );
	}

	TSelf	ScaleBy( const TSelf& scale ) const {
		TSelf scaled;
		for( UInt32 ix = 0; ix < NumRows; ++ix ) {
			scaled[ix] = m_Rows[ix] * scale[ix];
		}
		return scaled;
	}

	static void	Identity( TChild& mat ) {
		assert( NumRows == NumColumns );

		for( UInt32 ix = 0; ix < NumRows; ++ix ) {
			for( UInt32 iy = 0; iy < NumColumns; ++iy ) {
				mat[ix][iy] = (ix == iy) ? 1.0f : 0.0f;
			}
		}
	}

	static TChild	Identity() {
		TChild inst;
		Identity(inst);
		return inst;
	}
};
/////////////////////////////////////////////////////

template< Byte _NumRows, Byte _NumColumns >
class Matrix;

template<>
class Matrix<3, 3> : public AMatrix< 3, 3, Matrix<3, 3> > {
	INHERITEDCLASS_TYPEDEFS( Matrix, AMatrix );

public:

protected:

public:
	using TSuper::operator[];

	Matrix() {
	}

	Matrix( const Vector<3>& xDir, const Vector<3>& yDir, const Vector<3>& zDir ) {
		m_Rows[0] = xDir;
		m_Rows[1] = yDir;
		m_Rows[2] = zDir;
	}

	TSelf	operator*( const TSelf& rhs ) const {
#define N(x,y) m_Rows[x][y]
#define M(x,y) rhs[x][y]
#define R(x,y) result[x][y]

		TSelf result;

		R(0,0) = N(0,0) * M(0,0) + N(0,1) * M(1,0) + N(0,2) * M(2,0);
		R(0,1) = N(0,0) * M(0,1) + N(0,1) * M(1,1) + N(0,2) * M(2,1);
		R(0,2) = N(0,0) * M(0,2) + N(0,1) * M(1,2) + N(0,2) * M(2,2);

		R(1,0) = N(1,0) * M(0,0) + N(1,1) * M(1,0) + N(1,2) * M(2,0);
		R(1,1) = N(1,0) * M(0,1) + N(1,1) * M(1,1) + N(1,2) * M(2,1);
		R(1,2) = N(1,0) * M(0,2) + N(1,1) * M(1,2) + N(1,2) * M(2,2);

		R(2,0) = N(2,0) * M(0,0) + N(2,1) * M(1,0) + N(2,2) * M(2,0);
		R(2,1) = N(2,0) * M(0,1) + N(2,1) * M(1,1) + N(2,2) * M(2,1);
		R(2,2) = N(2,0) * M(0,2) + N(2,1) * M(1,2) + N(2,2) * M(2,2);

#undef N
#undef M
#undef R

		for( UInt32 ix = 0; ix < NumRows; ++ix ) {
			result[ix].Normalize();
		}
		return result;
	}

	static TSelf		Euler( Float32 ax, Float32 ay, Float32 az ) {
		TSelf temp;
		//temp.SetToIdentity();
		temp[0][0] = cosf(az)*cosf(ay);	temp[0][1] = (-sinf(az))*cosf(ax)+cosf(az)*sinf(ay)*sinf(ax);		temp[0][2] = sinf(az)*sinf(ax)+cosf(az)*sinf(ay)*cosf(ax);
		temp[1][0] = sinf(az)*cosf(ay);	temp[1][1] = cosf(az)*cosf(ax)+sinf(az)*sinf(ay)*sinf(ax);			temp[1][2] = cosf(az)*(-sinf(ax))+sinf(az)*sinf(ay)*sinf(ax);
		temp[2][0] = -sinf(ay);			temp[2][1] = cosf(ay)*sinf(ax);									temp[2][2] = cosf(ay)*cosf(ax);
		return temp;
	}

	Vector<3>	Euler() const {
		Vector<3> angles;
		angles[2] = atan2f(m_Rows[2][1], m_Rows[2][2]);
		angles[0] = asinf(m_Rows[2][0]);
		angles[1] = -atan2f(m_Rows[1][0], m_Rows[0][0]);
		return angles;
	}
};

template<>
class Matrix<4, 4> : public AMatrix< 4, 4, Matrix<4, 4> > {
	INHERITEDCLASS_TYPEDEFS( Matrix, AMatrix );

public:

protected:

public:
	using TSuper::operator[];

	Matrix() {
	}

	Matrix( const Matrix<3, 3>& rot, const Vector<3>& pos ) {
		SetToIdentity();

		for( UInt32 vecIndex = 0; vecIndex < 3; ++vecIndex ) {
			for( UInt32 colIndex = 0; colIndex < 3; ++colIndex ) {
				m_Rows[colIndex][vecIndex] = rot[colIndex][vecIndex];
			}
		}

		for( UInt32 ix = 0; ix < 3; ++ix ) {
			m_Rows[3][ix] = pos[ix];
		}
	}

	/*Matrix(const Quaternion& rot, const Vector<3>& pos) {
		SetToIdentity();

		Matrix<3, 3> matRot;
		rot.Quaternion2Matrix(matRot);
		for (UInt32 vecIndex = 0; vecIndex < 3; ++vecIndex) {
			for (UInt32 colIndex = 0; colIndex < 3; ++colIndex) {
				m_Rows[colIndex][vecIndex] = matRot[colIndex][vecIndex];
			}
		}

		for (UInt32 ix = 0; ix < 3; ++ix) {
			m_Rows[3][ix] = pos[ix];
		}
	}*/

	TSelf	operator*( const TSelf& rhs ) const {
#define N(x,y) m_Rows[y][x]
#define M(x,y) rhs[x][y]
#define R(x,y) result[x][y]

		TSelf	result;

		R(0,0) = N(0,0) * M(0,0) + N(0,1) * M(1,0) + N(0,2) * M(2,0) + N(0,3) * M(3,0);
		R(0,1) = N(0,0) * M(0,1) + N(0,1) * M(1,1) + N(0,2) * M(2,1) + N(0,3) * M(3,1);
		R(0,2) = N(0,0) * M(0,2) + N(0,1) * M(1,2) + N(0,2) * M(2,2) + N(0,3) * M(3,2);
		R(0,3) = N(0,0) * M(0,3) + N(0,1) * M(1,3) + N(0,2) * M(2,3) + N(0,3) * M(3,3);

		R(1,0) = N(1,0) * M(0,0) + N(1,1) * M(1,0) + N(1,2) * M(2,0) + N(1,3) * M(3,0);
		R(1,1) = N(1,0) * M(0,1) + N(1,1) * M(1,1) + N(1,2) * M(2,1) + N(1,3) * M(3,1);
		R(1,2) = N(1,0) * M(0,2) + N(1,1) * M(1,2) + N(1,2) * M(2,2) + N(1,3) * M(3,2);
		R(1,3) = N(1,0) * M(0,3) + N(1,1) * M(1,3) + N(1,2) * M(2,3) + N(1,3) * M(3,3);

		R(2,0) = N(2,0) * M(0,0) + N(2,1) * M(1,0) + N(2,2) * M(2,0) + N(2,3) * M(3,0);
		R(2,1) = N(2,0) * M(0,1) + N(2,1) * M(1,1) + N(2,2) * M(2,1) + N(2,3) * M(3,1);
		R(2,2) = N(2,0) * M(0,2) + N(2,1) * M(1,2) + N(2,2) * M(2,2) + N(2,3) * M(3,2);
		R(2,3) = N(2,0) * M(0,3) + N(2,1) * M(1,3) + N(2,2) * M(2,3) + N(2,3) * M(3,3);

		R(3,0) = N(3,0) * M(0,0) + N(3,1) * M(1,0) + N(3,2) * M(2,0) + N(3,3) * M(3,0);
		R(3,1) = N(3,0) * M(0,1) + N(3,1) * M(1,1) + N(3,2) * M(2,1) + N(3,3) * M(3,1);
		R(3,2) = N(3,0) * M(0,2) + N(3,1) * M(1,2) + N(3,2) * M(2,2) + N(3,3) * M(3,2);
		R(3,3) = N(3,0) * M(0,3) + N(3,1) * M(1,3) + N(3,2) * M(2,3) + N(3,3) * M(3,3);

#undef N
#undef M
#undef R

		for( UInt32 ix = 0; ix < NumRows - 1; ++ix ) {
			result[ix].Normalize();
		}
		return result;
	}

	TSelf&	operator=( const Matrix<3, 3>& rhs ) {
		for( UInt32 ix = 0; ix < Matrix<3, 3>::NumRows; ++ix ) {
			m_Rows[ ix ] = rhs[ ix ];
		}
		return *this;
	}

	static TSelf		Euler( Float32 ax, Float32 ay, Float32 az ) {
		TSelf temp;
		temp.SetToIdentity();
		temp[0][0] = cosf(az)*cosf(ay);	temp[0][1] = (-sinf(az))*cosf(ax)+cosf(az)*sinf(ay)*sinf(ax);		temp[0][2] = sinf(az)*sinf(ax)+cosf(az)*sinf(ay)*cosf(ax);
		temp[1][0] = sinf(az)*cosf(ay);	temp[1][1] = cosf(az)*cosf(ax)+sinf(az)*sinf(ay)*sinf(ax);			temp[1][2] = cosf(az)*(-sinf(ax))+sinf(az)*sinf(ay)*sinf(ax);
		temp[2][0] = -sinf(ay);			temp[2][1] = cosf(ay)*sinf(ax);									temp[2][2] = cosf(ay)*cosf(ax);
		return temp;
	}

	TSelf	ScaleBy( const Vector<3>& scale ) const {
		TSelf scaled( *this );
		for( UInt32 ix = 0; ix < Vector<3>::NumDimensions; ++ix ) {
			scaled[ix] *= scale[ix];
		}
		return scaled;
	}

	void	Position(const Vector<3>& pos) {
		for (UInt32 ix = 0; ix < 3; ++ix) {
			m_Rows[3][ix] = pos[ix];
		}
	}

	void	Position(Float32 x, Float32 y, Float32 z) {
		m_Rows[3][0] = x;
		m_Rows[3][1] = y;
		m_Rows[3][2] = z;
	}

	Vector<3>	Position() const {
		Vector<3> pos;
		for (UInt32 ix = 0; ix < 3; ++ix) {
			pos[ix] = m_Rows[3][ix];
		}
		return pos;
	}

	/*void	Rotation(Float32 pitch, Float32 yaw, Float32 roll) {
		Matrix<4, 4>::Euler(pitch, yaw, roll);

		for (UInt32 vecIndex = 0; vecIndex < 3; ++vecIndex) {
			for (UInt32 colIndex = 0; colIndex < 3; ++colIndex) {
				m_Rows[colIndex][vecIndex] = rot[colIndex][vecIndex];
			}
		}
	}*/
};

inline Vector<3> operator*(const Matrix<3, 3> &m, const Vector<3> &v)
{
	Vector<3> result;

	result[0] = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2];
	result[1] = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2];
	result[2] = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2];

	return(result);
}

inline Vector<3> operator*(const Vector<3> &v, const Matrix<3, 3> &m)
{
	Vector<3> result;

	result[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0];
	result[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1];
	result[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2];

	return(result);
}

inline Vector<3> &operator *= (Vector<3> &v, const Matrix<3, 3> &m)
{
	Float32 t0, t1;

	t0 = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0];
	t1 = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1];
	v[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2];
	v[0] = t0;
	v[1] = t1;

	return(v);
}

//template<Byte _NumRows, Byte _NumColumns>
//class TypeSerializer<Matrix<_NumRows, _NumColumns>> {
//public:
//	typedef Matrix<_NumRows, _NumColumns>	TData;
//
//public:
//	static Matrix<_NumRows, _NumColumns>	ReadFrom(File& file) {
//		Matrix<_NumRows, _NumColumns>	val;
//		ReadFrom(file, val);
//		return val;
//	}
//
//	static void	ReadFrom(File& file, Matrix<_NumRows, _NumColumns>& val) {
//		val.ReadFrom(file);
//	}
//
//	static void	WriteTo(File& file, const Matrix<_NumRows, _NumColumns>& val) {
//		val.WriteTo(file);
//	}
//};

#include "System\json.h"

template<Byte _NumRows, Byte _NumColumns>
class JsonSerializer<Matrix<_NumRows, _NumColumns>> {
public:
	/*static Matrix<_NumRows, _NumColumns>	ReadFrom(json_value* root) {
		Vector<_NumDimensions>	val;
		ReadFrom(root, val);
		return val;
	}*/

	static Bool	ReadFrom(json_value* root, Matrix<_NumRows, _NumColumns>& outVal) {
		return outVal.ReadFrom(root);
	}
};