#version 450 compatibility
#pragma import_defines(SHADOW_CAST)
#ifdef SHADOW_CAST

void main()
{
	gl_Position = ftransform();
}

#else // not SHADOW_CAST

uniform mat4 view2ShadowMatrix;

out vData
{
	vec3	modelPos;
	vec3	modelCameraPos;
	vec3	modelNormal;
	vec3	viewPos;
	vec3	viewNormal;
	vec3	shadowPos;
} vertOut;

void main()
{
	vec4 viewVertPos = gl_ModelViewMatrix * gl_Vertex;

	vertOut.modelPos = gl_Vertex.xyz;
	vertOut.modelCameraPos = (inverse(gl_ModelViewMatrix))[3].xyz;
	vertOut.modelNormal = gl_Normal;
	vertOut.viewPos = viewVertPos.xyz / viewVertPos.w;
	vertOut.viewNormal = normalize(gl_NormalMatrix*gl_Normal);
	vertOut.shadowPos = (view2ShadowMatrix*viewVertPos).xyz;

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}

#endif // SHADOW_CAST or not