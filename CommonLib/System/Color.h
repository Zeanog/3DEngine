#pragma once

#include "Typedefs.h"

template< typename _TData >
class AColor {
public:
	typedef _TData	TData;
	static const UInt32	NumComponents = 4;

protected:
	TData	m_Components[NumComponents];

public:
	AColor() {
	}

	AColor( const AColor& color ) {
	}

	AColor( TData red, TData green, TData blue, TData alpha ) {
		Red() = red;
		Green() = green;
		Blue() = blue;
		Alpha() = alpha;
	}

	AColor( TData red, TData green, TData blue ) {
		Red() = red;
		Green() = green;
		Blue() = blue;
		Alpha() = TData();
	}

	TData&	operator[]( UInt32 index ) {
		return m_Components[ index ];
	}

	const TData&	operator[]( UInt32 index ) const {
		return m_Components[ index ];
	}

	TData&	Red() {
		return m_Components[0];
	}

	const TData&	Red() const {
		return m_Components[0];
	}

	TData&	Green() {
		return m_Components[1];
	}

	const TData&	Green() const {
		return m_Components[1];
	}

	TData&	Blue() {
		return m_Components[2];
	}
	
	const TData&	Blue() const {
		return m_Components[2];
	}

	TData&	Alpha() {
		return m_Components[3];
	}

	const TData&	Alpha() const {
		return m_Components[3];
	}
};

template< typename TData >
class Color;

template<>
class Color<Byte> : public AColor<Byte> {
	INHERITEDCLASS_TYPEDEFS( Color, AColor<Byte> );

public:
	static TSelf	White;
	static TSelf	Black;
	static TSelf	Red;
	static TSelf	Green;
	static TSelf	Blue;

public:
	Color() {
	}

	Color( const AColor& color ) : AColor( color ) {
	}

	Color( TData red, TData green, TData blue, TData alpha ) : AColor( red, green, blue, alpha ) {
	}

	Color( TData red, TData green, TData blue ) : AColor( red, green, blue ) {
	}
};

template<>
class Color<Float32> : public AColor<Float32> {
	INHERITEDCLASS_TYPEDEFS( Color, AColor<Float32> );

public:
	static TSelf	White;
	static TSelf	Black;
	static TSelf	Red;
	static TSelf	Green;
	static TSelf	Blue;

public:
	Color() {
	}

	Color( const AColor& color ) : AColor( color ) {
	}

	Color( TData red, TData green, TData blue, TData alpha ) : AColor( red, green, blue, alpha ) {
	}

	Color( TData red, TData green, TData blue ) : AColor( red, green, blue ) {
	}
};
