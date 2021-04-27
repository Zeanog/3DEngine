
void main() {
	gl_FragColor = vec4( gl_Position.z, gl_Position.z, gl_Position.z, 1 );
	gl_FragDepth = gl_Position.z;
}