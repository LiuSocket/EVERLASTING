const float NOISE_GRANULARITY = 0.5/255.0;
uniform vec3 screenSize;

in	vec4	objPos;

void main()
{
	float fragmentNoise = mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, Noise(gl_FragCoord.xy / screenSize.xy));
	vec3 backColor = mix(vec3(0.4,0.45,0.5), vec3(0.6,0.63,0.66), exp2(-length(objPos.xyz)*0.03)) + fragmentNoise;
#ifdef SHADOW_RECEIVE
	//backColor *= Shadow(vec4(gl_FragCoord.xyx*0.01,1.0));
#endif // SHADOW_RECEIVE
	gl_FragColor = vec4(backColor, 1.0);
}
