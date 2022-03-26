float atmosphereRadius = 2500.0f;  
float planetRadius = 1000.0f;  
float densityFalloff = 6.2f;

vec2 raySphere(vec3 center, float radius, vec3 rayOrigin, vec3 rayDirection){
	float a = dot(rayDirection, rayDirection);
    vec3 s0_r0 = rayOrigin - center;
    float b = 2.0 * dot(rayDirection, s0_r0);
    float c = dot(s0_r0, s0_r0) - (radius * radius);
	float disc = b * b - 4.0 * a* c;
    if (disc < 0.0) {
        return vec2(-1.0, -1.0);
    }else{
		return vec2(-b - sqrt(disc), -b + sqrt(disc)) / (2.0 * a);
	}
}


float densityAtPoint(vec3 densitySamplePoint){
	float heightAboveSurface = length(densitySamplePoint) - planetRadius;
	float height01 = heightAboveSurface / (atmosphereRadius - planetRadius); 
	float localDensity = exp(-height01 * densityFalloff) * (1 - height01); 
	return localDensity; 
}

float opticalDepth(vec3 rayOrigin, vec3 rayDirection, float rayLength){
	int numOpticalDepthPoints = 2;
	vec3 densitySamplePoint = rayOrigin; 
	float stepSize = rayLength / (numOpticalDepthPoints-1);
	float opticalDepth = 0; 

	for(int i = 0; i < numOpticalDepthPoints; i++){
		float localDensity = densityAtPoint(densitySamplePoint);
		opticalDepth += localDensity * stepSize;
		densitySamplePoint += rayDirection * stepSize;
	}

	return opticalDepth; 
}

vec3 calculateLight(vec3 rayOrigin, vec3 rayDirection, float rayLength, vec3 originalCol){
	int numInScatteringPoints = 2;
	vec3 inScatterPoint = rayOrigin; 
	float stepSize = rayLength / (numInScatteringPoints-1);
	vec3 inScatteredLight = vec3(0,0,0);
	float viewRayOpticalDepth = 0; 

	for(int i = 0; i < numInScatteringPoints; i++){
		vec3 dirToSun = normalize(inScatterPoint - ubo.atmosphereParams.v3LightPos);
		float sunRayLength = raySphere(vec3(0,0,0), atmosphereRadius, inScatterPoint, dirToSun).y; 
		float sunRayOpticalDepth = opticalDepth(inScatterPoint, dirToSun, sunRayLength); 
		viewRayOpticalDepth = opticalDepth(inScatterPoint, -rayDirection, stepSize*i); 
		float viewRayOpticalDepth = opticalDepth(inScatterPoint, -rayDirection, stepSize*i); 
		vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * ubo.atmosphereParams.v3InvWavelength); 
		float localDensity = densityAtPoint(inScatterPoint); 

		inScatteredLight += localDensity * transmittance * ubo.atmosphereParams.v3InvWavelength * stepSize; 
		inScatterPoint += rayDirection * stepSize;
	}

	float originalColorTransmittance = exp(-viewRayOpticalDepth); 
	return originalCol * originalColorTransmittance * inScatteredLight;
}