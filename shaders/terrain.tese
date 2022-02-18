#version 450

layout(triangles, fractional_even_spacing) in;

layout (location = 0) in vec3 vertexNormalIn[]; 

layout (location = 0) out float vertexHeight;
layout (location = 1) out vec3 vertexPosition;
layout (location = 2) out vec3 vertexNormal;


layout (push_constant) uniform Push {
    mat4 PVMatrix;
    vec3 cameraPosition;
} push;

vec4 interpolate(vec4 a, vec4 b, vec4 c){
    return gl_TessCoord.x * a + gl_TessCoord.y * b + gl_TessCoord.z * c;
}


void main() {
    gl_Position = interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);

    vertexNormal = (vertexNormalIn[0] + vertexNormalIn[1] + vertexNormalIn[2]) / 3.0f;
    vertexPosition = gl_Position.xyz;
    vertexHeight = distance(gl_Position.xyz, vec3(0,0,0));
    
    gl_Position = push.PVMatrix * gl_Position;
}
