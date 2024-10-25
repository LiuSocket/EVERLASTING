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
	vec3 backColor = vec3(mix(0.6, 0.3, exp2(-length(objPos.xyz)*0.02)) + fragmentNoise);
	gl_FragColor = vec4(backColor, 1.0);
}