#version 450 compatibility

#pragma import_defines(SHADOW_RECEIVE)

uniform sampler2DArray baseTex;

in vec2 texCoord;
in vec4 viewPos;
in vec3 viewNormal;

vec3 ToneMapping(vec3 x)
{
	const float A = 2.51;
	const float B = 0.03;
	const float C = 2.43;
	const float D = 0.59;
	const float E = 0.14;
	return pow((x * (A * x + B)) / (x * (C * x + D) + E), vec3(1.0/2.2));
}

void main()
{
	// vec3 baseCoord = texCoord;
	// vec4 baseColor = texture(baseTex, baseCoord);
	// vec3 viewVertUp = normalize(viewNormal);
	// vec3 viewDir = normalize(viewPos.xyz);

	// const float minFact = 1e-8;
	// float dotVUL = dot(viewVertUp, viewLight);
	// vec3 diffuse = vec3(max(dotVUL,minFact));
	// vec3 color = baseColor.rgb * (0.02+0.98*diffuse);

	// color = ToneMapping(color);

	gl_FragColor = vec4(viewNormal, 1.0);
}