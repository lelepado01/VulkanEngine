
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

layout (set = 0, binding = 0) uniform GlobalUniformBuffer {
    Light lightParams;
    TerrainMaterial terrainMaterialParams;
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
// ATMOSPHERE PARAMs
//
const vec4 AtmBlueColor = vec4(0.0, 0.5, 1.0, 1.0); 
const vec4 AtmRedColor = vec4(1.0, 0.5, 0.0, 1.0); 
const float MinCameraTerrainAngle = 0.7; 
const float MaxCameraTerrainAngle = 1.3; 


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

vec4 getFogColor(float dist){
	float redAccent = min(abs(dot(vertexPosition, ubo.lightParams.direction)), 50) / 50; 
	return mix(vec4(0.3,0.3,0.3,1.0), vec4(redAccent,0.2,0.2,1.0), dist); 
}

void main() {
	vec4 originalColor = getTerrainBaseColor();

	float d = distance(push.cameraPosition, vertexPosition); 
	float fogFactor = getFogFactor(d); 
	vec4 fogColor = getFogColor(fogFactor); 

	float lightTerrainAngle = acos(dot(ubo.lightParams.direction, vertexPosition) / (norm(ubo.lightParams.direction) * norm(vertexPosition)));
	vec4 fragAtmColor = mix(AtmBlueColor, AtmRedColor, lightTerrainAngle*1.5 / M_PI);

	float cameraTerrainAngle = acos(dot(push.cameraPosition, vertexPosition) / (norm(push.cameraPosition) * norm(vertexPosition)));

	float cameraTerrainAnglePerc = max(MinCameraTerrainAngle,min(cameraTerrainAngle, MaxCameraTerrainAngle)) - MinCameraTerrainAngle; 

	if (cameraTerrainAngle > MinCameraTerrainAngle  && cameraTerrainAngle < MaxCameraTerrainAngle){
		originalColor = mix(originalColor, fragAtmColor, cameraTerrainAnglePerc);
	} 

	outColor = mix(originalColor, fogColor, fogFactor);
}

