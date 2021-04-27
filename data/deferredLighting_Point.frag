
uniform sampler2D tDiffuse;
uniform sampler2D tPositions;
uniform sampler2D tNormals;
uniform sampler2D tShadowMap;

uniform vec3	vLightPos;
uniform float   fConstantAttenuation;
uniform float   fLinearAttenuation;
uniform float   fQuadraticAttenuation;

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

	vec3 vec2Light = vLightPos - position.xyz;
	vec3 dir2Light = normalize(vec2Light);

	float distance = length(vec2Light);
	float intensity = 1.0 / (fConstantAttenuation + fLinearAttenuation * distance + fQuadraticAttenuation * (distance * distance));
		
	float nDotl = max(dot(normal, dir2Light), 0.0);
	intensity *= nDotl;

	gl_FragColor = image * intensity;
}