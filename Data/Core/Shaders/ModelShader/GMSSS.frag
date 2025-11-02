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

vec4 Blur5x5(sampler2D texIn, vec2 uv, vec2 pix)
{
	vec4 WS = texture(texIn, (uv + vec2(-1,-1))*pix);
	vec4 WN = texture(texIn, (uv + vec2(-1,1))*pix);
	vec4 ES = texture(texIn, (uv + vec2(1,-1))*pix);
	vec4 EN = texture(texIn, (uv + vec2(1,1))*pix);

	vec4 WS1 = texture(texIn, (uv + vec2(-1,-3))*pix);
	vec4 WN1 = texture(texIn, (uv + vec2(-1,3))*pix);
	vec4 ES1 = texture(texIn, (uv + vec2(1,-3))*pix);
	vec4 EN1 = texture(texIn, (uv + vec2(1,3))*pix);

	vec4 W1S = texture(texIn, (uv + vec2(-3,-1))*pix);
	vec4 W1N = texture(texIn, (uv + vec2(-3,1))*pix);
	vec4 E1S = texture(texIn, (uv + vec2(3,-1))*pix);
	vec4 E1N = texture(texIn, (uv + vec2(3,1))*pix);

	return (WS + WN + ES + EN + WS1 + WN1 + ES1 + EN1 + W1S + W1N + E1S + E1N) / 12.0;
}

vec3 SSS(vec3 subdermalColor, vec4 subdermalBlur, float dotNL, float curvature, float thickness)
{
	float sqrtCurv = sqrt(curvature);
	float dotNLN = min(0.0, dotNL);
	vec3 dotSSS = vec3(sqrtCurv / (1.0 + 3.0*dotNLN*dotNLN));
	vec3 sss = max(vec3(0), mix(vec3(dotNL), dotSSS, subdermalBlur.rgb*(subdermalBlur.a*(1.0-thickness)*sqrtCurv)));
	float sssMix = smoothstep(0.0, 0.1, subdermalColor.r+subdermalColor.g+subdermalColor.b);
	return mix(vec3(1), subdermalBlur.rgb*sss, sssMix);
}

void main()
{
	vec3 viewLight = vec3(0,0,1);
	vec3 mainlightColor = vec3(1,1,1);
#ifdef GM_MAX_LIGHTNUM
	viewLight = -viewDirAndSpotExponent[0].xyz;
	mainlightColor = colorAndRange[0].rgb;
#endif // GM_MAX_LIGHTNUM

	vec2 pixSize = 1.0/screenSize.xy;
	// RGB = SSS color, A = Curvature
	vec4 texel_SSSC = texture(texSSSC, gl_TexCoord[0].st);
	vec4 baseColor = texture(texBaseColor, gl_TexCoord[0].st);
	vec4 outColor = baseColor;
	vec3 subdermalColor = texel_SSSC.rgb;

	float lengthV = length(vertOut.viewPos);
	vec3 viewVertDir = normalize(vertOut.viewPos);
	vec3 viewTangent = normalize(vertOut.viewTang);
	vec3 viewBinorm = normalize(vertOut.viewBinormal);
	vec3 viewNorm = normalize(vertOut.viewNormal);
	mat3 tang2View = mat3(viewTangent, viewBinorm, viewNorm);

	/* shadow */
	float shadow = 1.0;
#ifdef SHADOW_RECEIVE
	float fragmentNoise = mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, Noise(gl_FragCoord.xy * pixSize));
	shadow = clamp(Shadow(vertOut.shadowPos) + fragmentNoise, 0, 1);
#endif // SHADOW_RECEIVE

#ifdef SSS_BLUR
	float dotNL = dot(viewNorm, viewLight);
	gl_FragColor = vec4((dotNL*0.5+0.5)*shadow*subdermalColor*gl_FrontMaterial.diffuse.rgb,1);
#else // not SSS_BLUR

	vec4 texel_MRAT = texture(texMRAT, gl_TexCoord[0].st); // R = Metallic, G = Roughness, B = AO, A = Thickness
	vec4 texel_n = texture(texNormal, gl_TexCoord[0].st);
	vec3 texel_detail_n = texture(texDetailNormal, gl_TexCoord[0].st*2).rgb;
	vec4 subdermalBlur = Blur5x5(texSSSBlur, gl_FragCoord.st - 0.5, pixSize);

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
	const float dotNL_1 = max(dotNL,minFact);
	const float dotNH = max(dot(viewTexNorm, viewHalf), minFact);
	const float dotVN = max(dot(-viewVertDir, viewTexNorm), minFact);
	const float dotVH = max(dot(-viewVertDir, viewHalf), minFact);

	float curvature = texel_SSSC.a;
	float metallic = texel_MRAT.r;
	float roughness = texel_MRAT.g;
	float ambientOcc = texel_MRAT.b;
	float thickness = 0.5*texel_MRAT.a;
	vec3 localReflect = normalize((osg_ViewMatrixInverse*vec4(reflect(viewVertDir, viewTexNorm),0.0)).xyz);
	vec4 colorMin = vec4(mix(vec3(0.04), outColor.rgb, metallic), 1.0);

	/* reflect Environment BRDF */
	vec4 reflectEnv = ReflectEnvironment(localReflect, roughness);
	reflectEnv.rgb *= mainlightColor*ambientOcc;
	
	/* ambient BRDF */
	vec3 ambient = mix(vec3(0.1, 0.12, 0.13), vec3(0.02), max(0.5*(1.0-localReflect.z),0))*outColor.rgb*ambientOcc;
	/* subdermal BSSDF */
	vec3 subdermal = SSS(subdermalColor, subdermalBlur, dotNL, curvature, thickness);
	/* epidermis BRDF */
	vec3 epidermis = ((1-metallic)*dotNL_1*shadow)*baseColor.rgb*gl_FrontMaterial.diffuse.rgb;
	/* diffuse BRDF */
	vec3 diffuse = mix(subdermal, epidermis, thickness*0.5)*mainlightColor;

	/* Microfacet Specular BRDF */
	vec4 specularBRDF = vec4(mainlightColor,1)*smoothstep(-0.01,0.1, dotNL)
		*specD(roughness, dotNH)
		*specG(roughness, dotNL_1, dotVN)
		*specF(colorMin, dotVH)
		/(4.0*dotNL_1*dotVN);

	vec3 fresnel = EnvDFGLazarov(colorMin.rgb, 1.0-roughness, dotVN);
	outColor.rgb = mix(ToneMapping(ambient+diffuse), reflectEnv.rgb, fresnel) + specularBRDF.rgb;

	float alpha = outColor.a*gl_FrontMaterial.diffuse.a;
	outColor.a = alpha + step(CUT_ALPHA,alpha)*((fresnel.r+fresnel.g+fresnel.b)*0.3333+specularBRDF.a);

	gl_FragColor = outColor;

#endif // SSS_BLUR
}
#endif // SHADOW_CAST or not