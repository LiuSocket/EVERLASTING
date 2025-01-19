#ifdef SHADOW_CAST

void main()
{
	gl_FragColor = vec4(1,1,1,1);
}

#else // not SHADOW_CAST

uniform mat4			osg_ViewMatrixInverse;
uniform sampler2D		texBaseColor;
uniform sampler2D		texEnvProbe;

in vData
{
	vec3	modelPos;
	vec3	modelCameraPos;
	vec3	modelNormal;
	vec3	viewPos;
	vec3	viewNormal;
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
	float coordLen2Center = length(gl_TexCoord[0].st*2-vec2(1));
	vec3 viewLight = normalize(gl_LightSource[0].position.xyz);
	float lengthV = length(vertOut.viewPos);
	vec3 viewVertDir = normalize(vertOut.viewPos);
	vec3 viewNorm = normalize(vertOut.viewNormal);
	vec3 viewHalf = normalize(viewLight-viewVertDir);
	const float minFact = 1e-8;
	float dotNL = dot(viewNorm, viewLight);
	float dotNL_1 = max(dotNL,minFact);
	float dotNH = max(dot(viewNorm, viewHalf),minFact);
	float dotVN = max(dot(-viewVertDir, viewNorm),minFact);
	float dotVH = max(dot(-viewVertDir, viewHalf),minFact);

	const float eyeRadius = 1.09;
	const float modelIrisPosZ = 0.97;
	const float eyeRefractRatio = 1.0/1.3;
	vec3 modelVertDir = normalize(vertOut.modelPos - vertOut.modelCameraPos);
	vec3 modelNorm = normalize(vertOut.modelNormal);
	// the base color and roughness of eye must consider the refraction of cornea
	vec3 modelRefractDir = normalize(refract(modelVertDir, modelNorm, eyeRefractRatio));
	float lenVert2Iris = max(0.0, vertOut.modelPos.z - modelIrisPosZ);
	vec3 modelIrisPos = vertOut.modelPos + modelRefractDir*(lenVert2Iris/max(1e-9,abs(modelRefractDir.z)));
	vec2 baseUV = (modelIrisPos.xy+eyeRadius)/(2.0*eyeRadius);

	float roughnessIn = 1.0-0.7*smoothstep(0.5, 0.56, coordLen2Center); // inner layer
	const float roughnessOut = 0.1; // out layer

	vec4 baseColor = texture(texBaseColor, baseUV);
	vec4 outColor = baseColor;

	vec3 localReflect = normalize((osg_ViewMatrixInverse*vec4(reflect(viewVertDir, viewNorm),0.0)).xyz);
	const vec3 minColor = vec3(0.04);
	const vec4 colorMinOut = vec4(minColor, 0.0);
	const vec4 colorMinIn = vec4(minColor, 1.0);

	/* shadow */
	float shadow = Shadow(vertOut.shadowPos);

	vec3 lightAmbDif = gl_LightSource[0].ambient.rgb + gl_LightSource[0].diffuse.rgb;
	/* Reflect Environment BRDF of out layer */
	vec4 reflectOut = ReflectEnvironment(localReflect, roughnessOut);
	reflectOut.rgb *= lightAmbDif;
	/* Reflect Environment BRDF of inner layer */
	vec4 reflectIn = ReflectEnvironment(localReflect, roughnessIn);
	reflectIn.rgb *= lightAmbDif;
	
	/* ambient BRDF */
	vec4 ambient = vec4(mix(vec3(0.2, 0.24, 0.26), vec3(0.04), max(0.5*(1.0-localReflect.z),0)), 1.0);

	/* Diffuse BRDF */
	vec3 diffuseL = (shadow*max(0,dotNL))*gl_LightSource[0].diffuse.rgb;
	vec3 diffuseFact = diffuseL*gl_FrontMaterial.diffuse.rgb;

	vec4 specularCommon = gl_LightSource[0].specular*(smoothstep(-0.01,0.1, dotNL)/(4.0*dotNL_1*dotVN));
	/* Specular of out layer */
	vec4 specularOut = (specD(roughnessOut, dotNH)*specG(roughnessOut, dotNL_1, dotVN))*specF(colorMinOut, dotVH);
	/* Specular of in layer */
	vec4 specularIn = (specD(roughnessIn, dotNH)*specG(roughnessIn, dotNL_1, dotVN))*specF(colorMinIn, dotVH);

	vec3 fresnelOut = EnvDFGLazarov(minColor, 1.0-roughnessOut, dotVN);
	vec3 fresnelIn = EnvDFGLazarov(minColor, 1.0-roughnessIn, dotVN);
	outColor.rgb = mix(ToneMapping(diffuseFact+ambient.rgb)*outColor.rgb, reflectIn.rgb, fresnelIn) + specularIn.rgb;
	outColor.rgb = mix(outColor.rgb, reflectOut.rgb, fresnelOut) + specularOut.rgb;

	float fresnelAlphaOut = (fresnelOut.r+fresnelOut.g+fresnelOut.b)*0.3333;
	float alpha = outColor.a*gl_FrontMaterial.diffuse.a;
	outColor.a = alpha + specularOut.a + mix(specularIn.a, 1.0, fresnelAlphaOut);

	gl_FragColor = outColor;
}

#endif // SHADOW_CAST or not