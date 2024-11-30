#version 450 compatibility

#pragma import_defines(SHADOW_CAST)

#define M_PI 3.14159265358979

vec3 ToneMapping(vec3 x)
{
	const float A = 2.51;
	const float B = 0.03;
	const float C = 2.43;
	const float D = 0.59;
	const float E = 0.14;
	return pow((x * (A * x + B)) / (x * (C * x + D) + E), vec3(1.0/2.2));
}

/* the normal distribution function (NDF) */
float specD(float roughness, float dotNH)
{
	float alf = roughness*roughness;
	float alf2 = alf*alf;
	float specD = dotNH*dotNH*(alf2-1.0)+1.0;
	return alf2/(M_PI*specD*specD);	
}

/* specular geometric attenuation term, use the Schlick model for GGX */
float specG(float roughness, float dotNL, float dotVN)
{
	float k = (roughness+1.0)*(roughness+1.0)/8.0;
	float gLight = dotNL / (dotNL*(1.0-k)+k);
	float gVertex = dotVN / (dotVN*(1.0-k)+k);
	return gLight*gVertex;
}

/* For Fresnel, we made the typical choice of using Schlick’s approximation
, but with a minor mod-ification: we use a Spherical Gaussianapproximation */
vec4 specF(vec4 colorMin, float dotVH)
{
	return colorMin + (1.0 - colorMin)*exp2((-5.55473*dotVH - 6.98316)*dotVH);
}

/* noise float between 0.0~1.0 */
float Noise(vec2 co)
{
	return fract(sin(dot(co, vec2(12.9898,78.233))) * 43758.5453);
}

vec3 EnvDFGLazarov(vec3 F0, float gloss, float dotVN)
{
	const vec4 p0 = vec4( 0.5745, 1.548, -0.02397, 1.301 );
	const vec4 p1 = vec4( 0.5753, -0.2511, -0.02066, 0.4755 );
	vec4 t = gloss * p0 + p1;
	float bias = clamp( t.x * min( t.y, exp2( -7.672 * dotVN ) ) + t.z, 0, 1);
	float delta = clamp( t.w, 0, 1);
	float scale = delta - bias;
	bias *= clamp( 17.0 *(F0.r+F0.g+F0.b), 0, 1);
	return F0 * scale + bias;
}

#define SAMPLECOUNT 16
#define SAMPLECOUNT_FLOAT 16.0
#define INV_SAMPLECOUNT (1.0 / SAMPLECOUNT_FLOAT)
uniform sampler2DShadow texShadow;

float Shadow(vec3 shadowPos)
{
	// float softFactor = osgShadow_softnessWidth * shadowProj.w;
	// vec4 smCoord  = shadowProj;
	// vec3 jitterCoord = vec3( gl_FragCoord.xy / osgShadow_jitteringScale, 0.0 );
	// float shadow = 0.0;
	// // First "cheap" sample test
	// const float pass_div = 1.0 / (2.0 * 4.0);
	// for ( int i = 0; i < 4; ++i )
	// {
	// 	// Get jitter values in [0,1]; adjust to have values in [-1,1]
	// 	vec4 offset = 2.0 * texture3D( osgShadow_jitterTexture, jitterCoord ) -1.0;
	// 	jitterCoord.z += INV_SAMPLECOUNT;

	// 	smCoord.xy = shadowProj.xy  + (offset.xy) * softFactor;
	// 	shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x * pass_div;

	// 	smCoord.xy = shadowProj.xy  + (offset.zw) * softFactor;
	// 	shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x *pass_div;
	// }
	// // skip all the expensive shadow sampling if not needed
	// if ( shadow * (shadow -1.0) != 0.0 )
	// {
	// 	shadow *= pass_div;
	// 	for (int i=0; i<SAMPLECOUNT - 4; ++i)
	// 	{
	// 		vec4 offset = 2.0 * texture3D( osgShadow_jitterTexture, jitterCoord ) - 1.0;
	// 		jitterCoord.z += 1.0 / SAMPLECOUNT_FLOAT;

	// 		smCoord.xy = shadowProj.xy  + offset.xy * softFactor;
	// 		shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x * INV_SAMPLECOUNT;

	// 		smCoord.xy = shadowProj.xy  + offset.zw * softFactor;
	// 		shadow +=  shadow2DProj( osgShadow_shadowTexture, smCoord ).x * INV_SAMPLECOUNT;
	// 	}
	// }
	// // apply shadow, modulo the ambient bias
	// return osgShadow_ambientBias.x + shadow * osgShadow_ambientBias.y;

	float shadow = texture(texShadow, shadowPos).r;
	return shadow;
}