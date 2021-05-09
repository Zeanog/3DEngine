
varying vec4		position;

void main() {
	gl_FragColor = vec4( position.z, position.z, position.z, 1 );
	gl_FragDepth = position.z;
}