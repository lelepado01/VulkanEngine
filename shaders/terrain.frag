
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


float norm(vec3 v){
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float angle(vec3 v1, vec3 v2) {
	return acos(dot(v1, v2) / (norm(v1)*norm(v2)));
}

vec4 getColor(float height){

	if (height < 998) return DeepWaterColor;
	
	if (angle(normalize(vertexPosition), normalize(vertexNormal)) > 0.45f){
		return LightRockColor; 
	}

	if (height < 1000) return SandColor;
	if (height < 1010) return LightGrassColor;
	if (height < 1015) return DarkGrassColor;
	if (height < 1020) return DarkRockColor;

	return SnowColor;
}


void main() {

	if (vertexHeight > 998) {
		vec3 norm = mix(getFaceNormal(vertexPosition), normalize(vertexNormal), 0.9f);
		vec3 lightDir = normalize(-ubo.lightParams.direction);

		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(lightDir, reflectDir), 0.0f), ubo.terrainMaterialParams.shininess);

		float diff = max(dot(norm, lightDir), 0.0f);

		vec3 specular = ubo.lightParams.specular * (spec * ubo.terrainMaterialParams.specular);
		vec3 ambient = ubo.lightParams.ambient * ubo.terrainMaterialParams.ambient;
		vec3 diffuse = ubo.lightParams.diffuse * (diff * ubo.terrainMaterialParams.diffuse);

		outColor = vec4((ambient + diffuse + specular), 1.0f) * getColor(vertexHeight);

	} else {
		vec3 norm = mix(getFaceNormal(vertexPosition), normalize(vertexNormal), 0.05f);
		vec3 lightDir = normalize(-ubo.lightParams.direction);

		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(lightDir, reflectDir), 0.0f), ubo.terrainMaterialParams.shininess);

		float diff = max(dot(norm, lightDir), 0.0f);

		vec3 specular = ubo.lightParams.specular * (spec * ubo.terrainMaterialParams.specular);
		vec3 ambient = ubo.lightParams.ambient * ubo.terrainMaterialParams.ambient;
		vec3 diffuse = ubo.lightParams.diffuse * (diff * ubo.terrainMaterialParams.diffuse);

		outColor = vec4((ambient + diffuse + specular), 1.0f) * getColor(vertexHeight);
	}
}

