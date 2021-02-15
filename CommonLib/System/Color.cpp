#include "System/Color.h"

Color<Byte>	Color<Byte>::White(255, 255, 255, 255);
Color<Byte>	Color<Byte>::Black(0, 0, 0, 255);
Color<Byte>	Color<Byte>::Red(255, 0, 0, 255);
Color<Byte>	Color<Byte>::Green(0, 255, 0, 255);
Color<Byte>	Color<Byte>::Blue(0, 0, 255, 255);

Color<Float32>	Color<Float32>::White( 1.0f, 1.0f, 1.0f, 1.0f);
Color<Float32>	Color<Float32>::Black( 0.0f, 0.0f, 0.0f, 1.0f);
Color<Float32>	Color<Float32>::Red(1.0f, 0.0f, 0.0f, 1.0f);
Color<Float32>	Color<Float32>::Green( 0.0f, 1.0f, 0.0f, 1.0f);
Color<Float32>	Color<Float32>::Blue( 0.0f, 0.0f, 1.0f, 1.0f);