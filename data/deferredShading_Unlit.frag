
varying vec4		position;
varying vec3		normal;

uniform vec4		vColor;

void main( void )
{
	gl_FragData[0]		= vColor;
	gl_FragData[1]		= vec4(position.xyz, 0);
	gl_FragData[2]		= vec4(normal.xyz, 0);
}