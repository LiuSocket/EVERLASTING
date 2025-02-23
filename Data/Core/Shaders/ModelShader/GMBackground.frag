#ifdef SHADOW_CAST

void main()
{
	gl_FragColor = vec4(1,1,1,1);
}

#else // not SHADOW_CAST

uniform vec3 screenSize;

in	vec4	objPos;
in	vec3	viewPos;
in	vec3	shadowPos;

void main()
{
	float fragmentNoise = mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, Noise(gl_FragCoord.xy / screenSize.xy));
	vec3 backColor = mix(vec3(0.4,0.45,0.5), vec3(0.6,0.63,0.66), exp2(-length(objPos.xyz)*0.03)) + fragmentNoise;

#ifdef SHADOW_RECEIVE
	float shadow = clamp(Shadow(shadowPos) + fragmentNoise, 0, 1);
	backColor *= mix(vec3(0.51,0.54,0.57), vec3(1), shadow);
#endif // SHADOW_RECEIVE

	gl_FragColor = vec4(backColor, 1.0);
}

#endif // SHADOW_CAST or not