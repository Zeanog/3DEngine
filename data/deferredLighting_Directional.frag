
uniform sampler2D tDiffuse;
uniform sampler2D tPositions;
uniform sampler2D tNormals;
uniform sampler2D tShadowMap;

uniform vec3 vLightDir;

uniform mat4 mDepthMVP;

float readShadowMap(vec3 viewspace_pos)
{
	vec4 lightspace_pos = mDepthMVP * vec4(viewspace_pos, 1.0);

	const float bias = 0.0001;
	vec4 shadowPixel = texture(tShadowMap, lightspace_pos.xy);
	float shadowDepth = shadowPixel.z - bias;
	if( lightspace_pos.z < shadowDepth ) {
		return 1.0;
	}

	return 0.0f;
}

void main( void )
{
	vec4 image = texture2D( tDiffuse, gl_TexCoord[0].xy );
	vec4 position = texture2D( tPositions, gl_TexCoord[0].xy );
	vec4 normal = texture2D( tNormals, gl_TexCoord[0].xy );
	normal = normalize(normal);

	vec4 outColor = image;
	
	if( normal.w > 0 ) {
		float nDotl;
		nDotl = dot(normal.xyz, -vLightDir);
		outColor *= max(0.0, nDotl);

		float shadow = readShadowMap(position.xyz);
		outColor *= shadow;
	}

	gl_FragColor = outColor;
}

