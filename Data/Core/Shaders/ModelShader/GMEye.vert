#version 450 compatibility
#pragma import_defines(SHADOW_CAST)
#ifdef SHADOW_CAST

void main()
{
	gl_Position = ftransform();
}

#else // not SHADOW_CAST

uniform mat4 view2ShadowMatrix;

attribute vec3		tangent;
attribute vec3		binormal;

out vData
{
	vec4	objPos;
	vec3	viewPos;
	vec3	viewNormal;
	vec3	viewTang;
	vec3	viewBinormal;
	vec3	shadowPos;
} vertOut;

void main()
{
	vec4 viewVertPos = gl_ModelViewMatrix * gl_Vertex;

	vertOut.objPos = gl_Vertex;
	vertOut.viewPos = viewVertPos.xyz / viewVertPos.w;
	vertOut.viewNormal = normalize(gl_NormalMatrix*gl_Normal);
	vertOut.viewTang = normalize(gl_NormalMatrix*tangent);
	vertOut.viewBinormal = normalize(gl_NormalMatrix*binormal);
	vertOut.shadowPos = (view2ShadowMatrix*viewVertPos).xyz;

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}

#endif // SHADOW_CAST or not