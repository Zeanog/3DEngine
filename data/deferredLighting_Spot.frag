
uniform sampler2D tDiffuse;
uniform sampler2D tPositions;
uniform sampler2D tNormals;
uniform sampler2D tShadowMap;

uniform vec3	vLightPos;
uniform vec3	vLightDirection;
uniform float	fLightCosCutoff;

uniform float   fConstantAttenuation;
uniform float   fLinearAttenuation;
uniform float   fQuadraticAttenuation;
uniform float   fExponent;
uniform vec3	vEyePos;

uniform mat4    mDepthMVP;

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

	float intensity = 0.0;
	if( normal.w > 0 ) {
		vec3 spotToLight = normalize( vLightPos - position.xyz );

		vec3 lightDir = normalize(vLightDirection);
		vec3 lightToSpotDir = normalize(-spotToLight);

		if( dot(normalize(vLightDirection), lightToSpotDir) > fLightCosCutoff ) {
			intensity = max( dot(normal.xyz, -lightDir), 0.0 );
			if( intensity > 0.0 ) {
				float distance = length(spotToLight);
				intensity *= 1.0 / (fConstantAttenuation + fLinearAttenuation * distance + fQuadraticAttenuation * (distance * distance));
			}
			intensity *= readShadowMap(position.xyz);
		}
	} else {
		intensity = 1.0;
	}
    
    gl_FragColor = image * intensity;
}