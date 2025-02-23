#version 450 compatibility

#pragma import_defines(SHADOW_CAST)
#pragma import_defines(SHADOW_RECEIVE)
#pragma import_defines(GM_MAX_LIGHTNUM)

#define M_PI 3.14159265358979
const float NOISE_GRANULARITY = 0.5/255.0;

#ifdef GM_MAX_LIGHTNUM
layout(std140) uniform LightDataBlock
{
    vec4 viewPosAndCut[GM_MAX_LIGHTNUM]; // xyz = viewPos, w = spotCosCutoff
    vec4 viewDirAndSpotExponent[GM_MAX_LIGHTNUM]; // xyz = view light dir, w = spotExponent
    vec4 colorAndRange[GM_MAX_LIGHTNUM]; // xyz = light color, w = light fade range
    vec4 lightNum;// x = light num, yzw = padding
};
#endif // GM_MAX_LIGHTNUM

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

#ifdef SHADOW_RECEIVE
uniform sampler2D texShadow;
float Shadow(vec3 shadowPos)
{
	const vec2 shadowTexSize = vec2(1024.0);
	const vec2 pixUnit2 = 2.0/shadowTexSize;

	vec4 shadow00 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(3,3)*pixUnit2, 0)-shadowPos.z);
	vec4 shadow05 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(-2,3)*pixUnit2, 0)-shadowPos.z);
	vec4 shadow22 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(1,1)*pixUnit2, 0)-shadowPos.z);
	vec4 shadow23 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(0,1)*pixUnit2, 0)-shadowPos.z);
	vec4 shadow32 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(1,0)*pixUnit2, 0)-shadowPos.z);
	vec4 shadow33 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy, 0)-shadowPos.z);
	vec4 shadow50 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy+vec2(-3,2)*pixUnit2, 0)-shadowPos.z);
	vec4 shadow55 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy+vec2(2,2)*pixUnit2, 0)-shadowPos.z);

	vec4 shadowSum = shadow00 + shadow05 + shadow22 + shadow23 + shadow32 + shadow33 + shadow50 + shadow55;
	float sampleCount = 32.0;

	float shadow = (shadowSum.x + shadowSum.y + shadowSum.z + shadowSum.w)/sampleCount;
	if(shadow>0.0 && shadow < 1.0)
	{
		vec4 shadow02 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(1,3)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow03 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(0,3)*pixUnit2, 0)-shadowPos.z);	

		vec4 shadow12 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(1,2)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow13 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(0,2)*pixUnit2, 0)-shadowPos.z);
	
		vec4 shadow20 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(3,1)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow21 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(2,1)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow24 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(-1,1)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow25 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(-2,1)*pixUnit2, 0)-shadowPos.z);

		vec4 shadow30 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(3,0)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow31 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(2,0)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow34 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(-1,0)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow35 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy-vec2(-2,0)*pixUnit2, 0)-shadowPos.z);

		vec4 shadow42 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy+vec2(-1,1)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow43 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy+vec2(0,1)*pixUnit2, 0)-shadowPos.z);

		vec4 shadow52 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy+vec2(-1,2)*pixUnit2, 0)-shadowPos.z);
		vec4 shadow53 = step(vec4(0.0), textureGather(texShadow, shadowPos.xy+vec2(0,2)*pixUnit2, 0)-shadowPos.z);

		shadowSum += shadow02 + shadow03 + shadow12 + shadow13
				+ shadow20 + shadow21 + shadow24 + shadow25
				+ shadow30 + shadow31 + shadow34 + shadow35
				+ shadow42 + shadow43 + shadow52 + shadow53;

		sampleCount = 96.0;
	}

	shadow = (shadowSum.x + shadowSum.y + shadowSum.z + shadowSum.w)/sampleCount;
	return shadow;
}
#endif // SHADOW_RECEIVE