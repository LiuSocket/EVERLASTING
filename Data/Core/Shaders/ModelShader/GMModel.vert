#version 450 compatibility

attribute vec3		tangent;
attribute vec3		binormal;

out vData
{
	vec4	objPos;
	vec3	viewPos;
	vec3	viewNormal;
	vec3	viewTang;
	vec3	viewBinormal;
} vertOut;

void main()
{
	vec4 viewVertPos = gl_ModelViewMatrix * gl_Vertex;

	vertOut.objPos = gl_Vertex;
	vertOut.viewPos = viewVertPos.xyz / viewVertPos.w;
	vertOut.viewNormal = normalize(gl_NormalMatrix*gl_Normal);
	vertOut.viewTang = normalize(gl_NormalMatrix*tangent);
	vertOut.viewBinormal = normalize(gl_NormalMatrix*binormal);

	gl_Position = gl_ProjectionMatrix * viewVertPos;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}