#version 330 core

uniform sampler3D perlworl;
uniform sampler3D worl;

uniform mat4 invView;
uniform mat4 invProj;
uniform float time;		
uniform vec2 resolution;	

uniform vec3 camera;

uniform float coverage;	
uniform float cloudType;
uniform float anvilBias;
uniform float sunIntensity;	
uniform float earthRadius;				
uniform float cloudStartRadius;	
uniform float cloudEndRadius;
uniform float toneMapperEyeExposure;	
uniform vec3 toneMapperColor;

uniform float eccentricity;
uniform float silver_intensity;
uniform float silver_spread;

//preetham variables
in vec3 vSunDirection;
in float vSunfade;
in vec3 vBetaR;
in vec3 vBetaM;
in float vSunE;
in vec3 vAmbient;
in vec3 vSunColor;

out vec4 color;

const float pi = 3.141592653589793238462643383279502884197169;
const float mieDirectionalG = 0.8;
const float rayleighZenithLength = 8.4E3;
const float mieZenithLength = 1.25E3;
const vec3 up = vec3( 0.0, 1.0, 0.0 );

const float sunAngularDiameterCos = cos(0.02);

float remap(const float originalValue, const float originalMin, const float originalMax, const float newMin, const float newMax)
{
	return newMin + (((originalValue - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

float HG(float costheta, float g) {
	return ((1.0 - g*g) / (pow(1.0 + g*g - 2.0 * g * costheta, 1.5))) / (4.0 * pi);
}

vec3 preetham(const vec3 dirToAtomsphere) {
	float zenithAngle = acos( max( 0.0, dot( up, normalize( dirToAtomsphere ) ) ) );
	float inv = 1.0 / ( cos( zenithAngle ) + 0.15 * pow( 93.885 - ( ( zenithAngle * 180.0 ) / pi ), -1.253 ) );
	float sR = rayleighZenithLength * inv;
	float sM = mieZenithLength * inv;

	vec3 Fex = exp( -( vBetaR * sR + vBetaM * sM ) );

	float cosTheta = dot( normalize( dirToAtomsphere ), vSunDirection );

	float rPhase = 3/(16*pi) * ( 1.0 + pow( cosTheta*0.5+0.5, 2.0 ) );
	vec3 betaRTheta = vBetaR * rPhase;

	float mPhase = HG( cosTheta, mieDirectionalG );
	vec3 betaMTheta = vBetaM * mPhase;

	vec3 Lin = pow( vSunE * ( ( betaRTheta + betaMTheta ) / ( vBetaR + vBetaM ) ) * ( 1.0 - Fex ), vec3( 1.5 ) );
	Lin *= mix( vec3( 1.0 ), pow( vSunE * ( ( betaRTheta + betaMTheta ) / ( vBetaR + vBetaM ) ) * Fex, vec3( 1.0 / 2.0 ) ), clamp( pow( 1.0 - dot( up, vSunDirection ), 5.0 ), 0.0, 1.0 ) );

	vec3 L0 = vec3( 0.5 ) * Fex;

	float sundisk = smoothstep( sunAngularDiameterCos, sunAngularDiameterCos + 0.00002, cosTheta );
	L0 += ( vSunE * 19000.0 * Fex ) * sundisk;

	vec3 texColor = ( Lin + L0 ) * 0.04 + vec3( 0.0, 0.0003, 0.00075 );

	vec3 retColor = pow( texColor, vec3( 1.0 / ( 1.2 + ( 1.2 * vSunfade ) ) ) );

    retColor = mix(retColor, toneMapperColor, 0.2);
	return retColor;
}

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}


//	Simplex 3D Noise 
//	by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(vec3 v){ 
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


const vec3 RANDOM_VECTORS[6] = vec3[6]
(
	vec3( 0.38051305f,  0.92453449f, -0.02111345f),
	vec3(-0.50625799f, -0.03590792f, -0.86163418f),
	vec3(-0.32509218f, -0.94557439f,  0.01428793f),
	vec3( 0.09026238f, -0.27376545f,  0.95755165f),
	vec3( 0.28128598f,  0.42443639f, -0.86065785f),
	vec3(-0.16852403f,  0.14748697f,  0.97460106f)
);

float GetHeightFractionForPoint(vec3 worldPosition)
{ 
	float heightFraction = (length(worldPosition) - cloudStartRadius) / (cloudEndRadius - cloudStartRadius); 
	return clamp(heightFraction, 0.0, 1.0);
}

vec4 mixGradients(const float cloudType)
{
	const vec4 STRATUS_GRADIENT = vec4(0.0, 0.1, 0.2, 0.3);
	const vec4 STRATOCUMULUS_GRADIENT = vec4(0.02, 0.2, 0.48, 0.625);
	const vec4 CUMULUS_GRADIENT = vec4(0.0, 0.1625, 0.88, 0.98);
	
	float stratus = 1.0f - clamp(cloudType * 2.0f, 0.0, 1.0);
	float stratocumulus = 1.0f - abs(cloudType - 0.5f) * 2.0f;
	float cumulus = clamp(cloudType - 0.5f, 0.0, 1.0) * 2.0f;
	return STRATUS_GRADIENT * stratus + STRATOCUMULUS_GRADIENT * stratocumulus + CUMULUS_GRADIENT * cumulus;
}

float densityHeightGradient(const float heightFraction, const float cloudType) 
{
	vec4 cloudGradient = mixGradients(cloudType);
	float density = smoothstep(cloudGradient.x, cloudGradient.y, heightFraction) - smoothstep(cloudGradient.z, cloudGradient.w, heightFraction);
	return density;
}

vec3 raySphereIntersection(const vec3 ro, const vec3 rd, const float r) 
{	
    float b = 2 * dot(rd, ro);
    float c = dot(ro, ro) - (r * r);
	float dis = sqrt((b*b) - 4*c);
	float t1 = (-b - dis)*0.5;
	float t2 = (-b + dis)*0.5;
    float tm = max(t1, t2);
	vec3 result = ro+tm * rd;
    return result;
}

float cloudDensity(vec3 p, const float LOD) {
	float heightFraction = GetHeightFractionForPoint(p);

	p += 1000*snoise(p*0.0003);

	vec4 lowFrequencyNoises = textureLod(perlworl, p*0.000005, LOD);
	float lowFrequencyFBM = lowFrequencyNoises.g * 0.625 + lowFrequencyNoises.b * 0.25 + lowFrequencyNoises.a * 0.125;
	
	vec3 highFrequencyNoises = texture(worl, p*0.003, LOD).rgb;
	float highFrequencyFBM = highFrequencyNoises.r * 0.625 + highFrequencyNoises.g * 0.25 + highFrequencyNoises.b * 0.125;

	float baseCloud = remap(lowFrequencyNoises.r, -(1.0 - lowFrequencyFBM), 1.0, 0.0, 1.0);

	baseCloud = remap(lowFrequencyFBM, highFrequencyFBM, 1.0, 0.0, 1.0);

	float grad = densityHeightGradient(heightFraction, cloudType);
	float new_coverage = pow(coverage, remap(heightFraction, 0.7, 0.8, 1.0, mix(1.0, 0.5, anvilBias)));

	float coverageCloud = remap(baseCloud*grad, 1 - new_coverage, 1.0, 0.0, 1.0);
	coverageCloud *= new_coverage;

	return clamp(coverageCloud, 0.0, 1.0);

}

vec4 raymarch(const vec3 start, const vec3 marchStep, const int depth, const vec3 sunColor, const vec3 ambientLightColor){
    vec3 p = start;

    vec3 lightDir = vSunDirection * length(marchStep);
    float costheta = dot(normalize(lightDir), normalize(marchStep));
    float phase = max(HG(costheta, eccentricity), silver_intensity * HG(costheta, 0.99-silver_spread));
	
	vec3 resultColor = vec3(0.0);
	float alpha = 0.0;
	float density = 0.0;

	for(int i = 0; i < depth; i++){
		p += marchStep;

		density = cloudDensity(p, 1.0);

		vec3 lightPos = p;
		float scatterDensity = 0.0;

		if(density > 0.0){
			
			for (int j = 0; j < 6; j++){
				lightPos += (lightDir + length(lightDir) * RANDOM_VECTORS[j]*  float(j));
				scatterDensity += cloudDensity(lightPos, float(j));
			}

			lightPos += lightDir * length(lightDir);
			scatterDensity += cloudDensity(lightPos, 1.0);

			float beers = max(exp(-2.0*scatterDensity), exp(-2.0*scatterDensity * 0.25) * 0.7);	
			float powderSugar = 1.0 - exp(-2.0 * scatterDensity);
			float lightEnergy = 5.0 * beers * powderSugar * phase;
					
			vec3 ambientLightColorComponent = ambientLightColor * 0.7;
			vec3 sunColorComponent = GetHeightFractionForPoint(p) *  pow(sunColor, vec3(sunIntensity));		
			vec3 cloudColor = ambientLightColorComponent + vec3(1.0) * sunColorComponent * lightEnergy;

			cloudColor -= clamp(costheta, 0.0, vSunDirection.y*2);

			resultColor += (1.0 - alpha) * cloudColor * density;
			
			alpha += (1.0 - alpha) * density;

			if (alpha > 0.99){
				break;
			}
		}
	}
    return vec4(resultColor, alpha);
}


void main()
{
    vec2 screenSpaceFragmentCoordsInTextureCoords = gl_FragCoord.xy / resolution;
	vec4 screenSpaceFragmentCoordsInViewSpace = invProj * vec4(vec2(2.0 * screenSpaceFragmentCoordsInTextureCoords - 1.0), 1.0, 1.0);
    screenSpaceFragmentCoordsInViewSpace = vec4(screenSpaceFragmentCoordsInViewSpace.xy, -1.0, 0.0);
    vec3 fragmentWorldSpacePosition = (invView * screenSpaceFragmentCoordsInViewSpace).xyz;

	vec3 viewDir = normalize(fragmentWorldSpacePosition);
	vec3 cameraEarthPos = vec3(camera.x, camera.y + earthRadius, camera.z);		

	vec3 ambientLightColor = preetham(normalize(vec3(0.3, 0.2, 0.0)));
    vec3 sunColor = preetham(normalize(vSunDirection));
	
	vec3 start = raySphereIntersection(cameraEarthPos, viewDir, cloudStartRadius);
	vec3 end = raySphereIntersection(cameraEarthPos, viewDir, cloudEndRadius);
		
	float depth = (mix(100.0, 50.0, dot(viewDir, vec3(0.0, 1.0, 0.0))));
	float waterShade = 1.0;

	if(end.y < earthRadius){
		waterShade = mix(0.6, 0.0, -(end.y - earthRadius)*0.0000008);
		viewDir.y = -viewDir.y;
		start = raySphereIntersection(cameraEarthPos, viewDir, cloudStartRadius);
		end = raySphereIntersection(cameraEarthPos, viewDir, cloudEndRadius);
	}

	vec3 marchStep = viewDir * (length(end-start) / depth);

	vec3 resultColor = preetham(viewDir);
	vec4 raymarchResult = raymarch(start, marchStep, int(depth), sunColor, resultColor);
	
	resultColor = raymarchResult.a * raymarchResult.xyz + (1.0-raymarchResult.a) * resultColor;
	resultColor *= waterShade;

	resultColor = vec3(1.0) - exp(-resultColor.xyz * toneMapperEyeExposure);

	color = vec4(resultColor, 1.0);
}
