#include "ErrorHandling.h"

#include <gl/glut.h>

const Char*	glErrorString( UInt32 errorCode ) {
	return (Char*)gluErrorString(errorCode);
}