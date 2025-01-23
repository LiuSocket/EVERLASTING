const float CUT_ALPHA = 0.001;

#ifdef SHADOW_CAST

void main()
{
	gl_FragColor = vec4(1,1,1,1);
}

#else // not SHADOW_CAST

uniform mat4			osg_ViewMatrixInverse;
uniform sampler2D		texBaseColor;
uniform sampler2D		texMRA;
uniform sampler2D		texIllumination;
uniform sampler2D		texNormal;
uniform sampler2D		texEnvProbe;

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
	vec3 viewNorm = normalize(vertOut.viewNormal);

	vec4 texel_p = texture(texMRA, gl_TexCoord[0].st); // R = Metallic,G = Roughness,B = AO
	vec4 texel_i = texture(texIllumination, gl_TexCoord[0].st);
	vec4 texel_n = texture(texNormal, gl_TexCoord[0].st);

	vec3 normalTangent = normalize(texel_n.xyz-vec3(0.5));
	vec3 viewTexNorm = normalize(
		mat3(normalize(vertOut.viewTang)
			,normalize(vertOut.viewBinormal)
			,viewNorm)
		*normalTangent);

	vec3 viewHalf = normalize(viewLight-viewVertDir);
	const float minFact = 1e-8;
	float dotNL = dot(viewTexNorm, viewLight);
	float dotNL_1 = max(dotNL,minFact);
	float dotNH = max(dot(viewTexNorm, viewHalf),minFact);
	float dotVN = max(dot(-viewVertDir, viewTexNorm),minFact);
	float dotVH = max(dot(-viewVertDir, viewHalf),minFact);

	float metallic = texel_p.r;
	float roughness = texel_p.g;
	float ambientOcc = texel_p.b;
	vec3 localReflect = normalize((osg_ViewMatrixInverse*vec4(reflect(viewVertDir, viewTexNorm),0.0)).xyz);
	vec4 colorMin = vec4(mix(vec3(0.04), outColor.rgb, metallic), 1.0);

	/* shadow */
	float shadow = 1.0;
#ifdef SHADOW_RECEIVE
	shadow = Shadow(vertOut.shadowPos);
#endif // SHADOW_RECEIVE

	/* Reflect Environment BRDF */
	vec4 reflectEnv = ReflectEnvironment(localReflect, roughness);
	reflectEnv.rgb *= mainlightColor;
	
	/* ambient BRDF */
	vec4 ambient = vec4(mix(vec3(0.2, 0.24, 0.26), vec3(0.04), max(0.5*(1.0-localReflect.z),0)), 1.0);

	/* Diffuse BRDF */
	vec3 diffuseL = (shadow*max(0,dotNL))*mainlightColor;
	vec3 diffuseFact = (1-metallic)*diffuseL*gl_FrontMaterial.diffuse.rgb;

	/* Microfacet Specular BRDF */
	vec4 specularBRDF = vec4(mainlightColor,1)*smoothstep(-0.01,0.1, dotNL)
		*specD(roughness,dotNH)
		*specG(roughness, dotNL_1, dotVN)
		*specF(colorMin, dotVH)
		/(4.0*dotNL_1*dotVN);

	vec3 fresnel = EnvDFGLazarov(colorMin.rgb, 1.0-roughness, dotVN);
	outColor.rgb = mix(ToneMapping(diffuseFact+ambient.rgb)*outColor.rgb, reflectEnv.rgb, fresnel) + specularBRDF.rgb;

	/* illumination */
	vec4 illumination = texel_i;
	outColor.rgb = mix(outColor.rgb, vec3(1), illumination.rgb);

	float alpha = outColor.a*gl_FrontMaterial.diffuse.a;
	outColor.a = alpha + step(CUT_ALPHA,alpha)*((fresnel.r+fresnel.g+fresnel.b)*0.3333+specularBRDF.a);

	gl_FragColor = outColor;
}

#endif // SHADOW_CAST or not