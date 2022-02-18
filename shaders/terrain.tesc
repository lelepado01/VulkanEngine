#version 450

layout(vertices = 3) out;

layout (location = 0) in vec3 vertexNormalIn[]; 
layout (location = 0) out vec3 vertexNormalOut[]; 


layout (push_constant) uniform Push {
    mat4 PVMatrix;  
    vec3 cameraPosition;
} push;

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
	vec4 frustumPlanes[6]; 
} ubo;


vec3 getMiddlePoint(vec3 a, vec3 b){
    return (a + b) / 2;
}

vec3 getMiddlePoint(vec3 a, vec3 b,  vec3 c){
    return (a + b + c) / 3;
}

float lengthSquared3D(vec3 a, vec3 b){
    return pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2);
}


float getPercentageTransitionZone(float dist, float minRange, float maxRange){
    return (dist - minRange) / (maxRange-minRange);
}

float percentageToLOD(float perc, float minLOD, float maxLOD){
    return (maxLOD - minLOD) * perc + minLOD;
}

float getTessellationLevel(float dist){
          
    if (dist < 100.0f*100.0f){
        return 18.0f;
    }
    
    float transitionRange = 105.0f *105.0f;
    if (dist <= transitionRange && dist >= 100.0f*100.0f){
        float perc = getPercentageTransitionZone(dist, 100.0f*100.0f, transitionRange);
        return percentageToLOD(1-perc, 8.0f, 18.0f);
    }
    
    if (dist < 300.0f*300.0f){
        return 8.0f;
    }
    
    float transitionRange2 = 305.0f *305.0f;
    if (dist <= transitionRange2 && dist >= 300.0f*300.0f){
        float perc = getPercentageTransitionZone(dist, 100.0f*100.0f, transitionRange2);
        return percentageToLOD(1-perc, 4.0f, 8.0f);
    }
    
    if (dist < 500.0f*500.0f){
        return 4.0f;
    }
    
    float transitionRange3 = 505.0f *505.0f;
    if (dist <= transitionRange3 && dist >= 500.0f*500.0f){
        float perc = getPercentageTransitionZone(dist, 100.0f*100.0f, transitionRange3);
        return percentageToLOD(1-perc, 2.0f, 4.0f);
    }
    
    if (dist < 700.0f*700.0f){
        return 2.0f;
    }
    
    float transitionRange4 = 705.0f *705.0f;
    if (dist <= transitionRange4 && dist >= 700.0f*700.0f){
        float perc = getPercentageTransitionZone(dist, 100.0f*100.0f, transitionRange4);
        return percentageToLOD(1-perc, 1.0f, 2.0f);
    }
    
    return 1.0;
}


float DistancePlaneToPoint(vec3 center, vec3 extents, vec4 plane) {
	vec3 n = abs(plane.xyz);
	float r = dot(extents, n);
	float s = dot(vec4(center, 1.0f), plane);
	return s + r;
}

bool AABBOutsideFrustumTest(vec3 center, float extent) {
	for(int i = 0; i < 6; i++) {
		if (DistancePlaneToPoint(center, vec3(extent, extent, extent), ubo.frustumPlanes[i]) < 0.0f){
			return true;
		}
  	}
  	return false;
}


void main(void) {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	vertexNormalOut[gl_InvocationID] = vertexNormalIn[gl_InvocationID];

    if (gl_InvocationID == 0) {
        vec3 position1 = gl_in[gl_InvocationID].gl_Position.xyz;
        vec3 position2 = gl_in[gl_InvocationID+1].gl_Position.xyz;
        vec3 position3 = gl_in[gl_InvocationID+2].gl_Position.xyz;
 
        vec3 middlePoint1 = getMiddlePoint(position2, position3);
        vec3 middlePoint2 = getMiddlePoint(position3, position1);
        vec3 middlePoint3 = getMiddlePoint(position1, position2);
 
        vec3 middleOfTriangle = getMiddlePoint(middlePoint1, middlePoint2, middlePoint3); 
		vec3 middleOfTriangleClipSpace = (push.PVMatrix * vec4(middleOfTriangle, 1.0f)).xyz; 
		vec3 p1ClipSpace = (push.PVMatrix * vec4(position1, 1.0f)).xyz;  
		float triangleRadiusClipSpace = distance(middleOfTriangleClipSpace, p1ClipSpace); 

		// if (AABBOutsideFrustumTest(middleOfTriangleClipSpace, triangleRadiusClipSpace)) {
		// 	gl_TessLevelInner[0] = 0.0f; 
		// 	gl_TessLevelOuter[0] = 0.0f; 
		// 	gl_TessLevelOuter[1] = 0.0f; 
		// 	gl_TessLevelOuter[2] = 0.0f; 
		// }  else {
			gl_TessLevelInner[0] = getTessellationLevel(lengthSquared3D(push.cameraPosition, middleOfTriangle));
			gl_TessLevelOuter[0] = getTessellationLevel(lengthSquared3D(push.cameraPosition, middlePoint1));
			gl_TessLevelOuter[1] = getTessellationLevel(lengthSquared3D(push.cameraPosition, middlePoint2));
			gl_TessLevelOuter[2] = getTessellationLevel(lengthSquared3D(push.cameraPosition, middlePoint3));
		// }
	}
}
