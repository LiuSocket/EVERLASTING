#version 450 compatibility
#pragma import_defines(SHADOW_CAST)
#ifdef SHADOW_CAST

void main()
{
	gl_Position = ftransform();
}

#else // not SHADOW_CAST

uniform mat4 view2ShadowMatrix;

out	vec4	objPos;
out vec3	viewPos;
out vec3	shadowPos;

void main()
{
	vec4 viewVertPos = gl_ModelViewMatrix * gl_Vertex;

	objPos = gl_Vertex;
	viewPos = viewVertPos.xyz / viewVertPos.w;
	shadowPos = (view2ShadowMatrix*viewVertPos).xyz;
	gl_Position = ftransform();
}

#endif // SHADOW_CAST or not