#include "ErrorHandling.h"

#include <gl/glut.h>

const Char*	glErrorString( Int32 errorCode ) {
	return (Char*)gluErrorString(errorCode);
}