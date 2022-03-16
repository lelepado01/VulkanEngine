
#version 450

layout (location = 0) in float vertexHeight;
layout (location = 1) in vec3 vertexPosition;
layout (location = 2) in vec3 vertexNormal;

layout(location = 0) out vec4 outColor;

struct TerrainMaterial {
	float shininess; 
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct AtmosphereData {
	vec3 v3cameraDir;
	vec3 v3LightPos;		// The direction vector to the light source
	vec3 v3InvWavelength;	// 1 / pow(wavelength, 4) for the red, green, and blue channels
	float fCameraHeight;	// The camera's current height
	float fOuterRadius;		// The outer (atmosphere) radius
	float fInnerRadius;		// The inner (planetary) radius
	float fKrESun;			// Kr * ESun
	float fKmESun;			// Km * ESun
	float fKr4PI;			// Kr * 4 * PI
	float fKm4PI;			// Km * 4 * PI
	float fScale;			// 1 / (fOuterRadius - fInnerRadius)
	float fScaleDepth;		// The scale depth (i.e. the altitude at which the atmosphere's average density is found)
};

layout (set = 0, binding = 0) uniform GlobalUniformBuffer {
    Light lightParams;
    TerrainMaterial terrainMaterialParams;
	AtmosphereData atmosphereParams; 
} ubo;

layout (push_constant) uniform Push {
    mat4 PVMatrix;
    vec3 cameraPosition;
	int time; 
} push;


vec3 getFaceNormal(vec3 position) {
    vec3 dx = dFdx(position);
    vec3 dy = dFdy(position);
    return normalize(cross(dy, dx));
}

const vec4 DeepWaterColor = vec4(0.3,0.3,1, 1);
const vec4 SandColor = vec4(0.9,0.9,0.1,1);
const vec4 LightGrassColor = vec4(0.1,0.9,0.0,1);
const vec4 DarkGrassColor = vec4(0.0,0.6,0.0,1);
const vec4 LightRockColor = vec4(0.8,0.8,0.8,1);
const vec4 DarkRockColor = vec4(0.5,0.5,0.5,1);
const vec4 SnowColor = vec4(1,1,1,1);
const vec4 FoamColor = vec4(0.5,0.8,1,1);

const float M_PI = 3.14159265359; 


float norm(vec3 v){
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float angle(vec3 v1, vec3 v2) {
	return acos(dot(v1, v2) / (norm(v1)*norm(v2)));
}

vec4 getWaterAnimation(){
	return DeepWaterColor; 
}

vec4 getColor(float height){

	if (height < 998) return getWaterAnimation();
	
	float terrainSlope = angle(normalize(vertexPosition), normalize(vertexNormal)); 
	float slopePerc = 2*terrainSlope / M_PI; 
	if (terrainSlope > 0.45f){
		return LightRockColor; 
	}

	if (height < 1000) return SandColor;
	if (height < 1010) return mix(LightGrassColor, DarkRockColor, slopePerc);
	if (height < 1015) return mix(DarkGrassColor, DarkRockColor, slopePerc);
	if (height < 1017) return DarkRockColor;

	return SnowColor;
}

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


vec4 getTerrainBaseColor(){
	vec3 norm; 
	if (vertexHeight > 998) {
		norm = mix(getFaceNormal(vertexPosition), normalize(vertexNormal), 0.9f);
	} else {
		norm = mix(getFaceNormal(vertexPosition), normalize(vertexNormal), 0.05f);
	}

	vec3 lightDir = normalize(-ubo.lightParams.direction);

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(lightDir, reflectDir), 0.0f), ubo.terrainMaterialParams.shininess);

	float diff = max(dot(norm, lightDir), 0.0f);

	vec3 specular = ubo.lightParams.specular * (spec * ubo.terrainMaterialParams.specular);
	vec3 ambient = ubo.lightParams.ambient * ubo.terrainMaterialParams.ambient;
	vec3 diffuse = ubo.lightParams.diffuse * (diff * ubo.terrainMaterialParams.diffuse);

	return vec4((ambient + diffuse + specular), 1.0f) * getColor(vertexHeight);
}


void main() {

	vec4 originalColor = getTerrainBaseColor();
	
	// float sceneDepth = gl_FragCoord.w * planetRadius / 2; 
	// vec3 rayOrigin = push.cameraPosition; 
	// vec3 rayDirection = normalize(ubo.atmosphereParams.v3cameraDir);  //per pixel?

	// float dstToOcean = raySphere(vec3(0,0,0), planetRadius, rayOrigin, rayDirection).x;
	// float dstToSurface = min(dstToOcean, sceneDepth);

	// vec2 hitInfo = raySphere(vec3(0,0,0), atmosphereRadius, rayOrigin, rayDirection);
	// float dstToAtmosphere = hitInfo.x;
	// float dstThroughAtmosphere = min(hitInfo.y, dstToSurface - dstToAtmosphere);

	// if(dstThroughAtmosphere > 0.0f){
	// 	float epsilon = 0.0001f;
	// 	vec3 pointInAtmosphere = rayOrigin + rayDirection * (dstToAtmosphere + epsilon);  
	// 	vec4 light = vec4(calculateLight(pointInAtmosphere, rayDirection, dstThroughAtmosphere - epsilon*2, originalColor.xyz), 1.0f); 
	
	// 	originalColor = originalColor * (1 - light) + light;
	// }

	outColor = originalColor; 
	// outColor = vec4(sceneDepth); 
}

