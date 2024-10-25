#version 450 compatibility

out	vec4	objPos;

void main()
{
	objPos = gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}