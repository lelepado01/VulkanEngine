
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
	// vec4 frustumPlanes[6]; 
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


float norm(vec3 v){
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float angle(vec3 v1, vec3 v2) {
	return acos(dot(v1, v2) / (norm(v1)*norm(v2)));
}

const float M_PI = 3.14159265359; 


//    Simplex 3D Noise
//    by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
float simplex_noise(vec3 v){
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);
// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;
// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );
  //  x0 = x0 - 0. + 0.0 * C
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;
// Permutations
  i = mod(i, 289.0 );
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));
  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;
  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);
//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;
// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}

// Permutation polynomial: (34x^2 + x) mod 289
vec3 permute(vec3 x) {
  return mod((34.0 * x + 1.0) * x, 289.0);
}

vec3 dist(vec3 x, vec3 y, vec3 z,  bool manhattanDistance) {
  return manhattanDistance ?  abs(x) + abs(y) + abs(z) :  (x * x + y * y + z * z);
}

vec2 worley(vec3 P, float jitter, bool manhattanDistance) {
float K = 0.142857142857; // 1/7
float Ko = 0.428571428571; // 1/2-K/2
float  K2 = 0.020408163265306; // 1/(7*7)
float Kz = 0.166666666667; // 1/6
float Kzo = 0.416666666667; // 1/2-1/6*2

	vec3 Pi = mod(floor(P), 289.0);
 	vec3 Pf = fract(P) - 0.5;

	vec3 Pfx = Pf.x + vec3(1.0, 0.0, -1.0);
	vec3 Pfy = Pf.y + vec3(1.0, 0.0, -1.0);
	vec3 Pfz = Pf.z + vec3(1.0, 0.0, -1.0);

	vec3 p = permute(Pi.x + vec3(-1.0, 0.0, 1.0));
	vec3 p1 = permute(p + Pi.y - 1.0);
	vec3 p2 = permute(p + Pi.y);
	vec3 p3 = permute(p + Pi.y + 1.0);

	vec3 p11 = permute(p1 + Pi.z - 1.0);
	vec3 p12 = permute(p1 + Pi.z);
	vec3 p13 = permute(p1 + Pi.z + 1.0);

	vec3 p21 = permute(p2 + Pi.z - 1.0);
	vec3 p22 = permute(p2 + Pi.z);
	vec3 p23 = permute(p2 + Pi.z + 1.0);

	vec3 p31 = permute(p3 + Pi.z - 1.0);
	vec3 p32 = permute(p3 + Pi.z);
	vec3 p33 = permute(p3 + Pi.z + 1.0);

	vec3 ox11 = fract(p11*K) - Ko;
	vec3 oy11 = mod(floor(p11*K), 7.0)*K - Ko;
	vec3 oz11 = floor(p11*K2)*Kz - Kzo; // p11 < 289 guaranteed

	vec3 ox12 = fract(p12*K) - Ko;
	vec3 oy12 = mod(floor(p12*K), 7.0)*K - Ko;
	vec3 oz12 = floor(p12*K2)*Kz - Kzo;

	vec3 ox13 = fract(p13*K) - Ko;
	vec3 oy13 = mod(floor(p13*K), 7.0)*K - Ko;
	vec3 oz13 = floor(p13*K2)*Kz - Kzo;

	vec3 ox21 = fract(p21*K) - Ko;
	vec3 oy21 = mod(floor(p21*K), 7.0)*K - Ko;
	vec3 oz21 = floor(p21*K2)*Kz - Kzo;

	vec3 ox22 = fract(p22*K) - Ko;
	vec3 oy22 = mod(floor(p22*K), 7.0)*K - Ko;
	vec3 oz22 = floor(p22*K2)*Kz - Kzo;

	vec3 ox23 = fract(p23*K) - Ko;
	vec3 oy23 = mod(floor(p23*K), 7.0)*K - Ko;
	vec3 oz23 = floor(p23*K2)*Kz - Kzo;

	vec3 ox31 = fract(p31*K) - Ko;
	vec3 oy31 = mod(floor(p31*K), 7.0)*K - Ko;
	vec3 oz31 = floor(p31*K2)*Kz - Kzo;

	vec3 ox32 = fract(p32*K) - Ko;
	vec3 oy32 = mod(floor(p32*K), 7.0)*K - Ko;
	vec3 oz32 = floor(p32*K2)*Kz - Kzo;

	vec3 ox33 = fract(p33*K) - Ko;
	vec3 oy33 = mod(floor(p33*K), 7.0)*K - Ko;
	vec3 oz33 = floor(p33*K2)*Kz - Kzo;

	vec3 dx11 = Pfx + jitter*ox11;
	vec3 dy11 = Pfy.x + jitter*oy11;
	vec3 dz11 = Pfz.x + jitter*oz11;

	vec3 dx12 = Pfx + jitter*ox12;
	vec3 dy12 = Pfy.x + jitter*oy12;
	vec3 dz12 = Pfz.y + jitter*oz12;

	vec3 dx13 = Pfx + jitter*ox13;
	vec3 dy13 = Pfy.x + jitter*oy13;
	vec3 dz13 = Pfz.z + jitter*oz13;

	vec3 dx21 = Pfx + jitter*ox21;
	vec3 dy21 = Pfy.y + jitter*oy21;
	vec3 dz21 = Pfz.x + jitter*oz21;

	vec3 dx22 = Pfx + jitter*ox22;
	vec3 dy22 = Pfy.y + jitter*oy22;
	vec3 dz22 = Pfz.y + jitter*oz22;

	vec3 dx23 = Pfx + jitter*ox23;
	vec3 dy23 = Pfy.y + jitter*oy23;
	vec3 dz23 = Pfz.z + jitter*oz23;

	vec3 dx31 = Pfx + jitter*ox31;
	vec3 dy31 = Pfy.z + jitter*oy31;
	vec3 dz31 = Pfz.x + jitter*oz31;

	vec3 dx32 = Pfx + jitter*ox32;
	vec3 dy32 = Pfy.z + jitter*oy32;
	vec3 dz32 = Pfz.y + jitter*oz32;

	vec3 dx33 = Pfx + jitter*ox33;
	vec3 dy33 = Pfy.z + jitter*oy33;
	vec3 dz33 = Pfz.z + jitter*oz33;

	vec3 d11 = dist(dx11, dy11, dz11, manhattanDistance);
	vec3 d12 =dist(dx12, dy12, dz12, manhattanDistance);
	vec3 d13 = dist(dx13, dy13, dz13, manhattanDistance);
	vec3 d21 = dist(dx21, dy21, dz21, manhattanDistance);
	vec3 d22 = dist(dx22, dy22, dz22, manhattanDistance);
	vec3 d23 = dist(dx23, dy23, dz23, manhattanDistance);
	vec3 d31 = dist(dx31, dy31, dz31, manhattanDistance);
	vec3 d32 = dist(dx32, dy32, dz32, manhattanDistance);
	vec3 d33 = dist(dx33, dy33, dz33, manhattanDistance);

	vec3 d1a = min(d11, d12);
	d12 = max(d11, d12);
	d11 = min(d1a, d13); // Smallest now not in d12 or d13
	d13 = max(d1a, d13);
	d12 = min(d12, d13); // 2nd smallest now not in d13
	vec3 d2a = min(d21, d22);
	d22 = max(d21, d22);
	d21 = min(d2a, d23); // Smallest now not in d22 or d23
	d23 = max(d2a, d23);
	d22 = min(d22, d23); // 2nd smallest now not in d23
	vec3 d3a = min(d31, d32);
	d32 = max(d31, d32);
	d31 = min(d3a, d33); // Smallest now not in d32 or d33
	d33 = max(d3a, d33);
	d32 = min(d32, d33); // 2nd smallest now not in d33
	vec3 da = min(d11, d21);
	d21 = max(d11, d21);
	d11 = min(da, d31); // Smallest now in d11
	d31 = max(da, d31); // 2nd smallest now not in d31
	d11.xy = (d11.x < d11.y) ? d11.xy : d11.yx;
	d11.xz = (d11.x < d11.z) ? d11.xz : d11.zx; // d11.x now smallest
	d12 = min(d12, d21); // 2nd smallest now not in d21
	d12 = min(d12, d22); // nor in d22
	d12 = min(d12, d31); // nor in d31
	d12 = min(d12, d32); // nor in d32
	d11.yz = min(d11.yz,d12.xy); // nor in d12.yz
	d11.y = min(d11.y,d12.z); // Only two more to go
	d11.y = min(d11.y,d11.z); // Done! (Phew!)
	return sqrt(d11.xy); // F1, F2

}


vec4 getWaterAnimation(){
	int maxTime = 100; 
	float r = maxTime - abs((push.time % (maxTime)) - 2*maxTime);
	vec2 F = worley(vertexPosition* 0.1f, 1.0f, false);
	vec4 color = FoamColor;
	if (F.y-F.x > 0.05f) color = DeepWaterColor;
	return color; 
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

