#version 450 compatibility
#pragma import_defines(TERRAIN_RECT_MAX)

#ifndef TERRAIN_RECT_MAX
#define TERRAIN_RECT_MAX 19
#endif

layout(std140) uniform TerrainBlock
{
	// xy = position
	// z = scale
	// w = padding
	vec4 rectPosAndScale[TERRAIN_RECT_MAX];
};

out vec2 texCoord;
out vec4 viewPos;
out vec3 viewNormal;

void main()
{
	vec4 modelVertex = gl_Vertex;
	modelVertex.xy *= rectPosAndScale[gl_InstanceID].z;
	modelVertex.xy += rectPosAndScale[gl_InstanceID].xy;

	float elev = gl_InstanceID*sin(modelVertex.x)*cos(modelVertex.y);

	modelVertex.z = elev;
	viewPos = gl_ModelViewMatrix*modelVertex;
	viewNormal = vec3(fract(gl_InstanceID*0.1), fract(gl_InstanceID*0.03), 0.5);//normalize(gl_NormalMatrix*gl_Normal);
	texCoord = modelVertex.xy*0.01;
	gl_Position = gl_ProjectionMatrix*viewPos;
}