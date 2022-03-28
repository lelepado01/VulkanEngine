
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

vec4 getFogColor(float dist){
	float redAccent = min(abs(dot(vertexPosition, ubo.lightParams.direction)), 50) / 50; 
	return mix(vec4(0.3,0.3,0.3,1.0), vec4(redAccent,0.2,0.2,1.0), dist); 
}



// Written by GLtracy

// math const
const float PI = 3.14159265359;
const float MAX = 2000.0;
// scatter const
const float R_INNER = 1.0;
const float R = R_INNER + 0.5;

const int NUM_OUT_SCATTER = 8;
const int NUM_IN_SCATTER = 80;

// ray intersects sphere
// e = -b +/- sqrt( b^2 - c )
vec2 ray_vs_sphere( vec3 p, vec3 dir, float r ) {
	float b = dot( p, dir );
	float c = dot( p, p ) - r * r;
	
	float d = b * b - c;
	if ( d < 0.0 ) {
		return vec2( MAX, -MAX );
	}
	d = sqrt( d );
	
	return vec2( -b - d, -b + d );
}

// Mie
// g : ( -0.75, -0.999 )
//      3 * ( 1 - g^2 )               1 + c^2
// F = ----------------- * -------------------------------
//      8pi * ( 2 + g^2 )     ( 1 + g^2 - 2 * g * c )^(3/2)
float phase_mie( float g, float c, float cc ) {
	float gg = g * g;
	
	float a = ( 1.0 - gg ) * ( 1.0 + cc );

	float b = 1.0 + gg - 2.0 * g * c;
	b *= sqrt( b );
	b *= 2.0 + gg;	
	
	return ( 3.0 / 8.0 / PI ) * a / b;
}

// Rayleigh
// g : 0
// F = 3/16PI * ( 1 + c^2 )
float phase_ray( float cc ) {
	return ( 3.0 / 16.0 / PI ) * ( 1.0 + cc );
}

float density( vec3 p, float ph ) {
	return exp( -max( length( p ) - R_INNER, 0.0 ) / ph );
}

float optic( vec3 p, vec3 q, float ph ) {
	vec3 s = ( q - p ) / float( NUM_OUT_SCATTER );
	vec3 v = p + s * 0.5;
	
	float sum = 0.0;
	for ( int i = 0; i < NUM_OUT_SCATTER; i++ ) {
		sum += density( v, ph );
		v += s;
	}
	sum *= length( s );
	
	return sum;
}

vec3 in_scatter( vec3 o, vec3 dir, vec2 e, vec3 l ) {
	const float ph_ray = 0.05;
    const float ph_mie = 0.02;
    
    const vec3 k_ray = vec3( 3.8, 13.5, 33.1 );
    const vec3 k_mie = vec3( 21.0 );
    const float k_mie_ex = 1.1;
    
	vec3 sum_ray = vec3( 0.0 );
    vec3 sum_mie = vec3( 0.0 );
    
    float n_ray0 = 0.0;
    float n_mie0 = 0.0;
    
	float len = ( e.y - e.x ) / float( NUM_IN_SCATTER );
    vec3 s = dir * len;
	vec3 v = o + dir * ( e.x + len * 0.5 );
    
    for ( int i = 0; i < NUM_IN_SCATTER; i++, v += s ) {   
		float d_ray = density( v, ph_ray ) * len;
        float d_mie = density( v, ph_mie ) * len;
        
        n_ray0 += d_ray;
        n_mie0 += d_mie;
        
#if 0
        vec2 e = ray_vs_sphere( v, l, R_INNER );
        e.x = max( e.x, 0.0 );
        if ( e.x < e.y ) {
           continue;
        }
#endif
        
        vec2 f = ray_vs_sphere( v, l, R );
		vec3 u = v + l * f.y;
        
        float n_ray1 = optic( v, u, ph_ray );
        float n_mie1 = optic( v, u, ph_mie );
		
        vec3 att = exp( - ( n_ray0 + n_ray1 ) * k_ray - ( n_mie0 + n_mie1 ) * k_mie * k_mie_ex );
        
		sum_ray += d_ray * att;
        sum_mie += d_mie * att;
	}
	
	float c  = dot( dir, -l );
	float cc = c * c;
    vec3 scatter =
        sum_ray * k_ray * phase_ray( cc ) +
     	sum_mie * k_mie * phase_mie( -0.78, c, cc );
    
	
	return 10.0 * scatter;
}

// angle : pitch, yaw
mat3 rot3xy( vec2 angle ) {
	vec2 c = cos( angle );
	vec2 s = sin( angle );
	
	return mat3(
		c.y      ,  0.0, -s.y,
		s.y * s.x,  c.x,  c.y * s.x,
		s.y * c.x, -s.x,  c.y * c.x
	);
}

// ray direction
vec3 ray_dir( float fov, vec2 size, vec2 pos ) {
	vec2 xy = pos - size * 0.5;

	float cot_half_fov = tan( radians( 90.0 - fov * 0.5 ) );	
	float z = size.y * 0.5 * cot_half_fov;
	
	return normalize( vec3( xy, -z ) );
}

// void mainImage( out vec4 fragColor, in vec2 fragCoord )
// {
	
// }





void main() {
	vec4 originalColor = getTerrainBaseColor();

	float d = distance(push.cameraPosition, vertexPosition); 
	float fogFactor = getFogFactor(d); 
	vec4 fogColor = getFogColor(fogFactor); 

	float originalZ = gl_FragCoord.z / gl_FragCoord.w;
	if (originalZ > 1000){
		outColor = vec4(1.0, 0.0, 0.0, 1.0);
	} else {
	    outColor = mix(originalColor, fogColor, fogFactor);
	}

	// ATMOSPHERE STUFF
	// // default ray dir
	// vec3 dir = ray_dir( 45.0, vec2(1200, 800), gl_FragCoord.xy );
	
	// // default ray origin
	// vec3 eye = push.cameraPosition;

	// // rotate camera
	// // mat3 rot = rot3xy( vec2( 0.0, iTime * 0.5 ) );
	// // dir = rot * dir;
	// // eye = rot * eye;
	
	// // sun light dir
	// vec3 l = ubo.lightParams.direction;
			  
	// vec2 e = ray_vs_sphere( eye, dir, R );
	// if ( e.x > e.y ) {
	// 	outColor = vec4( 0.0, 0.0, 0.0, 1.0 );
    //     return;
	// }
	
	// vec2 f = ray_vs_sphere( eye, dir, R_INNER );
	// e.y = min( e.y, f.x );

	// vec3 I = in_scatter( eye, dir, e, l );
	
	// outColor = vec4( pow( I, vec3( 1.0 / 2.2 ) ), 1.0 );

}

