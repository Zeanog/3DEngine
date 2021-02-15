
uniform sampler2D tDiffuse;
uniform sampler2D tPositions;
uniform sampler2D tNormals;
uniform sampler2D tShadowMap;

uniform vec3 vLightPos;
uniform float fLightRadius;

uniform mat4 mDepthMVP;

float readShadowMap(vec3 vertexPos_ModelSpace)
{
	vec4 textureCoordinates = mDepthMVP * vec4(vertexPos_ModelSpace, 1);

	const float bias = 0.0001;
	float depthValue = texture2D( tShadowMap, textureCoordinates.xy ).z - bias;
	return (textureCoordinates.z < depthValue) ? 1.0 : 0.0;
}

void main( void )
{
	vec4 image = texture2D( tDiffuse, gl_TexCoord[0].xy );
	vec4 position = texture2D( tPositions, gl_TexCoord[0].xy );
	vec3 normal = texture2D( tNormals, gl_TexCoord[0].xy ).xyz;
	
	normal = normalize(normal);

	vec4 outColor = vec4(0.0, 0.0, 0.0, 1.0);
	float nDotl;

	vec3 vec2Light = vLightPos - position.xyz;
	vec3 dir2Light = normalize(vec2Light);

	//This needs to ramp differently to make low level 'mag' show up better 
	float mag = mix(0.0, 1.0, (fLightRadius - length(vec2Light)) / fLightRadius);
		
	nDotl = max(dot(normal, dir2Light), 0.0);
	mag *= mix(0.0, 1.0, nDotl);

	outColor += mag * image;

	gl_FragColor = outColor;
}

