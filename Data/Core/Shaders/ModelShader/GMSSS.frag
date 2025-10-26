#pragma import_defines(SSS_BLUR)

const float CUT_ALPHA = 0.001;

#ifdef SHADOW_CAST

void main()
{
	gl_FragColor = vec4(1,1,1,1);
}

#else // not SHADOW_CAST

uniform vec3			screenSize;
uniform mat4			osg_ViewMatrixInverse;
uniform sampler2D		texBaseColor;
uniform sampler2D		texMRAT;
uniform sampler2D		texSSSC;
uniform sampler2D		texNormal;
uniform sampler2D		texDetailNormal;
uniform sampler2D		texEnvProbe;
uniform sampler2D		texSSSBlur;

in vData
{
	vec4	objPos;
	vec3	viewPos;
	vec3	viewNormal;
	vec3	viewTang;
	vec3	viewBinormal;
	vec3	shadowPos;
} vertOut;

/* Reflect Environment BRDF */
vec4 ReflectEnvironment(vec3 localReflect, float roughness)
{
	float scaleXY = sqrt((1.0-abs(localReflect.z))/max(1e-9,1.0-localReflect.z*localReflect.z));
	vec2 texCoord = vec2(0.25+0.5*step(0.0,localReflect.z),0.5)+vec2(0.25,0.5)*localReflect.xy*scaleXY;
	float mipmapLevel = log2(roughness*80.0+1.0);
	vec4 texel = textureLod(texEnvProbe, texCoord, mipmapLevel);
	vec3 color = texel.rgb*texel.a*texel.a;
	return vec4(color*16.0, 1.0);
}

vec3 SSS(vec3 sssDeep, float dotNL, float curvature, float thickness)
{
	float sqrtCurv = sqrt(curvature);
	float dotNLN = min(0.0, dotNL);
	return max(vec3(0), mix(vec3(dotNL), vec3(sqrtCurv / (1.0 + 3.0*dotNLN*dotNLN)), (1.0-thickness)*sssDeep*sqrtCurv));
}

void main()
{
	vec3 viewLight = vec3(0,0,1);
	vec3 mainlightColor = vec3(1,1,1);
#ifdef GM_MAX_LIGHTNUM
	viewLight = -viewDirAndSpotExponent[0].xyz;
	mainlightColor = colorAndRange[0].rgb;
#endif // GM_MAX_LIGHTNUM

	vec4 baseColor = texture(texBaseColor, gl_TexCoord[0].st);
	vec4 outColor = baseColor;

	float lengthV = length(vertOut.viewPos);
	vec3 viewVertDir = normalize(vertOut.viewPos);
	vec3 viewTangent = normalize(vertOut.viewTang);
	vec3 viewBinorm = normalize(vertOut.viewBinormal);
	vec3 viewNorm = normalize(vertOut.viewNormal);
	mat3 tang2View = mat3(viewTangent, viewBinorm, viewNorm);

#ifdef SSS_BLUR

	float dotNL = dot(viewNorm, viewLight);
	gl_FragColor = vec4(dotNL*0.5+0.5,0,0,1);

#else // not SSS_BLUR

	vec4 texel_MRAT = texture(texMRAT, gl_TexCoord[0].st); // R = Metallic, G = Roughness, B = AO, A = Thickness
	vec4 texel_SSSC = texture(texSSSC, gl_TexCoord[0].st); // RGB = SSS color, A = Curvature
	vec4 texel_n = texture(texNormal, gl_TexCoord[0].st);
	vec3 texel_detail_n = texture(texDetailNormal, gl_TexCoord[0].st*3).rgb;

	vec3 tangentNormal = normalize(texel_n.xyz-vec3(0.5));
	vec3 tangentTangent = normalize(cross(vec3(0,1,0), tangentNormal));
	vec3 tangentBinorm = normalize(cross(tangentNormal, tangentTangent));
	mat3 tang2NormTangent = mat3(tangentTangent, tangentBinorm, tangentNormal);
	// detail normal
	vec3 normalDetailTangent = normalize(texel_detail_n-vec3(0.5));
	// normal + detail normal
	vec3 normalFinal = normalize(tang2NormTangent*normalDetailTangent);
	// final normal in view space
	vec3 viewTexNorm = normalize(tang2View*normalFinal);

	vec3 viewHalf = normalize(viewLight-viewVertDir);
	const float minFact = 1e-8;
	const float dotNL = dot(viewTexNorm, viewLight);

	vec4 blur00 = textureGather(texSSSBlur, (gl_FragCoord.st + vec2(-2,-2))/screenSize.xy, 0);
	vec4 blur01 = textureGather(texSSSBlur, (gl_FragCoord.st + vec2(-2,0))/screenSize.xy, 0);
	vec4 blur02 = textureGather(texSSSBlur, (gl_FragCoord.st + vec2(-2,2))/screenSize.xy, 0);

	vec4 blur10 = textureGather(texSSSBlur, (gl_FragCoord.st + vec2(0,-2))/screenSize.xy, 0);
	vec4 blur11 = textureGather(texSSSBlur, gl_FragCoord.st/screenSize.xy, 0);
	vec4 blur12 = textureGather(texSSSBlur, (gl_FragCoord.st + vec2(0,2))/screenSize.xy, 0);

	vec4 blur20 = textureGather(texSSSBlur, (gl_FragCoord.st + vec2(2,-2))/screenSize.xy, 0);
	vec4 blur21 = textureGather(texSSSBlur, (gl_FragCoord.st + vec2(2,0))/screenSize.xy, 0);
	vec4 blur22 = textureGather(texSSSBlur, (gl_FragCoord.st + vec2(2,2))/screenSize.xy, 0);

	vec4 blurCenter = vec4(dotNL*0.5+0.5);
	const vec4 cullValue = vec4(0.4);
	blur00 = mix(blur00, blurCenter, step(cullValue, blur00-blurCenter));
	blur01 = mix(blur01, blurCenter, step(cullValue, blur01-blurCenter));
	blur02 = mix(blur02, blurCenter, step(cullValue, blur02-blurCenter));
	blur10 = mix(blur10, blurCenter, step(cullValue, blur10-blurCenter));
	blur11 = mix(blur11, blurCenter, step(cullValue, blur11-blurCenter));
	blur12 = mix(blur12, blurCenter, step(cullValue, blur12-blurCenter));
	blur20 = mix(blur20, blurCenter, step(cullValue, blur20-blurCenter));
	blur21 = mix(blur21, blurCenter, step(cullValue, blur21-blurCenter));
	blur22 = mix(blur22, blurCenter, step(cullValue, blur22-blurCenter));

	vec4 sum4 = blur00 + blur01 + blur02 + blur10 + blur11 + blur12 + blur20 + blur21 + blur22;
	const float dotNLBlur = (sum4.x+sum4.y+sum4.z+sum4.w) / 18.0 - 1.0;
	const float dotNL_1 = max(dotNL,minFact);
	const float dotNH = max(dot(viewTexNorm, viewHalf), minFact);
	const float dotVN = max(dot(-viewVertDir, viewTexNorm), minFact);
	const float dotVH = max(dot(-viewVertDir, viewHalf), minFact);

	float metallic = texel_MRAT.r;
	float roughness = texel_MRAT.g;
	float ambientOcc = texel_MRAT.b;
	float thickness = 0.5*texel_MRAT.a;
	vec3 sssDeep = texel_SSSC.rgb;
	float curvature = texel_SSSC.a;
	vec3 localReflect = normalize((osg_ViewMatrixInverse*vec4(reflect(viewVertDir, viewTexNorm),0.0)).xyz);
	vec4 colorMin = vec4(mix(vec3(0.04), outColor.rgb, metallic), 1.0);
	vec3 sss = SSS(sssDeep, dotNLBlur, curvature, thickness);

	/* shadow */
	float shadow = 1.0;
#ifdef SHADOW_RECEIVE
	float fragmentNoise = mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, Noise(gl_FragCoord.xy / screenSize.xy));
	shadow = clamp(Shadow(vertOut.shadowPos) + fragmentNoise, 0, 1);
#endif // SHADOW_RECEIVE

	/* Reflect Environment BRDF */
	vec4 reflectEnv = ReflectEnvironment(localReflect, roughness);
	reflectEnv.rgb *= mainlightColor*ambientOcc;
	
	/* ambient BRDF */
	vec4 ambient = vec4(mix(vec3(0.2, 0.24, 0.26), vec3(0.04), max(0.5*(1.0-localReflect.z),0))*ambientOcc, 1.0);

	/* Diffuse BRDF */
	vec3 diffuseL = (vec3(1) - exp2(-shadow*20.0*max(vec3(0.1), sssDeep)))*sss*mainlightColor;
	vec3 diffuseFact = (1-metallic)*diffuseL*gl_FrontMaterial.diffuse.rgb;

	/* Microfacet Specular BRDF */
	vec4 specularBRDF = vec4(mainlightColor,1)*smoothstep(-0.01,0.1, dotNL)
		*specD(roughness, dotNH)
		*specG(roughness, dotNL_1, dotVN)
		*specF(colorMin, dotVH)
		/(4.0*dotNL_1*dotVN);

	vec3 fresnel = EnvDFGLazarov(colorMin.rgb, 1.0-roughness, dotVN);
	outColor.rgb = mix(ToneMapping(diffuseFact+ambient.rgb)*outColor.rgb, reflectEnv.rgb, fresnel) + specularBRDF.rgb;

	float alpha = outColor.a*gl_FrontMaterial.diffuse.a;
	outColor.a = alpha + step(CUT_ALPHA,alpha)*((fresnel.r+fresnel.g+fresnel.b)*0.3333+specularBRDF.a);

	gl_FragColor = outColor;//vec4(sssDeep,1);//

#endif // SSS_BLUR
}
#endif // SHADOW_CAST or not