
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

// 
// FOG CONSTs
//
const float FogMax = 300.0;
const float FogMin = 150.0;
const float MaxFogPerc = 0.2; 
const vec4 FogColor = vec4(0.5,0.5,0.5,1.0);

//
// TERRAIN COLOR CONSTs
//
const vec4 DeepWaterColor = vec4(0.3,0.3,1, 1);
const vec4 SandColor = vec4(0.9,0.9,0.1,1);
const vec4 LightGrassColor = vec4(0.1,0.9,0.0,1);
const vec4 DarkGrassColor = vec4(0.0,0.6,0.0,1);
const vec4 LightRockColor = vec4(0.8,0.8,0.8,1);
const vec4 DarkRockColor = vec4(0.5,0.5,0.5,1);
const vec4 SnowColor = vec4(1,1,1,1);
const vec4 FoamColor = vec4(0.5,0.8,1,1);

const float WATER_LEVEL = 998; 

//
// MATH CONSTs
//
const float M_PI = 3.14159265359; 



vec3 getFaceNormal(vec3 position) {
    vec3 dx = dFdx(position);
    vec3 dy = dFdy(position);
    return normalize(cross(dy, dx));
}

float norm(vec3 v){
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float angle(vec3 v1, vec3 v2) {
	return acos(dot(v1, v2) / (norm(v1)*norm(v2)));
}

vec4 getColor(float height){

	if (height < WATER_LEVEL) return DeepWaterColor;
	
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

vec4 getTerrainBaseColor(){
	vec3 norm; 
	if (vertexHeight > WATER_LEVEL) {
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

float getFogFactor(float dist) {
    if (dist<=FogMin) return 0;

    return min(1 - (FogMax - dist) / (FogMax - FogMin), MaxFogPerc);
}

void main() {
	vec4 originalColor = getTerrainBaseColor();

	float fogFactor = getFogFactor(distance(push.cameraPosition, vertexPosition)); 
    outColor = mix(originalColor, FogColor, fogFactor);
}

