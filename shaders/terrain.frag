
#version 450

layout (location = 0) in float vertexHeight;
layout (location = 1) in vec3 vertexPosition;
layout (location = 2) in vec3 vertexNormal;

layout(location = 0) out vec4 outColor;

struct TerrainMaterial {
    vec4 color;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Light {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
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

vec4 getColor(float height){
        
    if (height > -0.00009) return DeepWaterColor;
    if (height > -0.09) return SandColor;
    if (height > -0.5) return LightGrassColor;
    if (height > -0.9) return DarkGrassColor;
    if (height > -1.6) return DarkRockColor;

    return SnowColor;
}


void main() {
    
    vec4 lightDirection = vec4(0.0f, -1.0f, 0.0f, 0.0f);
    vec4 lightSpecular = vec4(0.5f, 0.5f, 0.5f, 0.0f);
    vec4 lightAmbient = vec4(0.2f, 0.2f, 0.2f, 0.0f);
    vec4 lightDiffuse = vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
    vec4 materialColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    vec4 materialAmbient =  vec4(0.2f, 0.2f, 0.2f, 0.0f);
    vec4 materialDiffuse = vec4(0.5f, 0.5f, 0.5f, 0.0f);
    vec4 materialSpecular = vec4(0.5f, 0.5f, 0.5f, 0.0f);

    
    float shininess = 0.02f;
    vec3 norm = mix(getFaceNormal(vertexPosition), normalize(vertexNormal), 0.1f);

    vec3 lightDir = normalize(-lightDirection.xyz);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(lightDir, reflectDir), 0.0f), shininess);

    float diff = max(dot(norm, lightDir), 0.0f);

    vec3 specular = lightSpecular.xyz * (spec * materialSpecular.xyz);
    vec3 ambient = lightAmbient.xyz * materialAmbient.xyz;
    vec3 diffuse = lightDiffuse.xyz * (diff * materialDiffuse.xyz);
    
    outColor = vec4((ambient + diffuse + specular), 1.0f) * getColor(vertexHeight);
    
//    float shininess = ubo.terrainMaterialParams.color.w;
//    vec3 norm = getFaceNormal(vertexPosition); //mix(getFaceNormal(vertexPosition), normalize(vertexNormal), 0.2f);

//    vec3 lightDir = normalize(-ubo.lightParams.direction.xyz);

//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(lightDir, reflectDir), 0.0f), shininess);

//    float diff = max(dot(norm, lightDir), 0.0f);

//    vec3 specular = ubo.lightParams.specular.xyz * (spec * ubo.terrainMaterialParams.specular.xyz);
//    vec3 ambient = ubo.lightParams.ambient.xyz * ubo.terrainMaterialParams.ambient.xyz;
//    vec3 diffuse = ubo.lightParams.diffuse.xyz * (diff * ubo.terrainMaterialParams.diffuse.xyz);

//    outColor = vec4((ambient + diffuse + specular), 1.0f) * getColor(vertexHeight);
}

