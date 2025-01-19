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

uniform sampler2D texShadow;

float Shadow(vec3 shadowPos)
{
	const vec2 shadowTexSize = vec2(1024.0);
	const vec2 pixUnit2 = 2.0/shadowTexSize;

	vec4 shadow11 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(2,2)*pixUnit2, 0)-shadowPos.z);

	vec4 shadow22 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(1,1)*pixUnit2, 0)-shadowPos.z);
	vec4 shadow23 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(0,1)*pixUnit2, 0)-shadowPos.z);

	vec4 shadow32 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(1,0)*pixUnit2, 0)-shadowPos.z);
	vec4 shadow33 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy, 0)-shadowPos.z);

	vec4 shadow44 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy+vec2(1,1)*pixUnit2, 0)-shadowPos.z);

	float shadow = shadow11.x + shadow11.y + shadow11.z + shadow11.w
				+ shadow22.x + shadow22.y + shadow22.z + shadow22.w
				+ shadow23.x + shadow23.y + shadow23.z + shadow23.w
				+ shadow32.x + shadow32.y + shadow32.z + shadow32.w
				+ shadow33.x + shadow33.y + shadow33.z + shadow33.w
				+ shadow44.x + shadow44.y + shadow44.z + shadow44.w;
	float sampleCount = 24.0;

	if(shadow>0.5 && shadow < 23.5)
	{
		
		
		vec4 shadow12 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(1,2)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow13 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(0,2)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow14 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(-1,2)*pixUnit2, 0)-shadowPos.z);
	
		vec4 shadow21 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(2,1)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow24 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(-1,1)*pixUnit2, 0)-shadowPos.z);

		vec4 shadow31 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(2,0)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow34 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(-1,0)*pixUnit2, 0)-shadowPos.z);

		vec4 shadow41 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy+vec2(-2,1)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow42 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy+vec2(-1,1)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow43 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy+vec2(0,1)*pixUnit2, 0)-shadowPos.z);



		shadow += shadow12.x + shadow12.y + shadow12.z + shadow12.w
				+ shadow13.x + shadow13.y + shadow13.z + shadow13.w
				+ shadow14.x + shadow14.y + shadow14.z + shadow14.w
				+ shadow21.x + shadow21.y + shadow21.z + shadow21.w
				+ shadow24.x + shadow24.y + shadow24.z + shadow24.w
				+ shadow31.x + shadow31.y + shadow31.z + shadow31.w
				+ shadow34.x + shadow34.y + shadow34.z + shadow34.w
				+ shadow41.x + shadow41.y + shadow41.z + shadow41.w
				+ shadow42.x + shadow42.y + shadow42.z + shadow42.w
				+ shadow43.x + shadow43.y + shadow43.z + shadow43.w;

		sampleCount = 64.0;
	}
	return shadow/sampleCount;
}