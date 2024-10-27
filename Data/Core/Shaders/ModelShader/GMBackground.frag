#version 450 compatibility

const float NOISE_GRANULARITY = 0.5/255.0;

uniform vec3 screenSize;

in	vec4	objPos;

float Random(vec2 coords)
{
	return fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	float fragmentNoise = mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, Random(gl_FragCoord.xy / screenSize.xy));
	vec3 backColor = mix(vec3(0.4,0.45,0.5), vec3(0.2,0.22,0.25), exp2(-length(objPos.xyz)*0.03)) + fragmentNoise;
	gl_FragColor = vec4(backColor, 1.0);
}