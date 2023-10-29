// Shadow map related variables
#define NUM_SAMPLES 100
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 30

#define SHADOW_MAP_SIZE 1024.0
#define FILTER_RADIUS 40.0
#define BLOCKER_SEARTCH_RADIUS 75.0
#define WLIGHT 75.0

#define EPS 0.001

float unpack(vec4 rgbaDepth);

vec2 poissonDisk[NUM_SAMPLES];

void poissonDiskSamples( const in vec2 randomSeed );

float useShadowMap(sampler2D shadowMap, vec4 shadowCoord){
  float depth = unpack(texture2D(shadowMap, shadowCoord.xy));
  return (shadowCoord.z <= depth + EPS) ? 1.0 : 0.0;
}

float PCF(sampler2D shadowMap, vec4 coords) {
  poissonDiskSamples(coords.xy);
  float visibility = 1.0;
  float filter_scale = FILTER_RADIUS / SHADOW_MAP_SIZE;
  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    vec2 sample_coord = coords.xy + poissonDisk[i] * filter_scale;
    visibility += useShadowMap(shadowMap, vec4(sample_coord, coords.z, 1.0));
  }
  return visibility / float(NUM_SAMPLES);
}

float blocker_depth;

float PCSS(sampler2D shadowMap, vec4 coords){

  // STEP 1: avgblocker depth
  poissonDiskSamples(coords.xy);
  float blocker_filter_scale = BLOCKER_SEARTCH_RADIUS / SHADOW_MAP_SIZE;
  blocker_depth = 0.0;
  int blocker_count = 0;
  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    vec2 sample_coord = coords.xy + poissonDisk[i] * blocker_filter_scale;
    float depth = unpack(texture2D(shadowMap, sample_coord));
    if (coords.z > depth + EPS) {
      //blocker
      blocker_depth += depth;
      blocker_count++;
    }
  }
  float w_penumbra;
  if (blocker_count == 0) {
    w_penumbra = FILTER_RADIUS;
  } else {
    blocker_depth = blocker_depth / float(blocker_count);

    // STEP 2: penumbra size
    float receiver_depth = coords.z;
    w_penumbra = (receiver_depth - blocker_depth) * WLIGHT / blocker_depth;
  }

  // STEP 3: filtering
  poissonDiskSamples(coords.xy);
  float visibility = 1.0;
  float filter_scale = w_penumbra / SHADOW_MAP_SIZE;
  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    vec2 sample_coord = coords.xy + poissonDisk[i] * filter_scale;
    visibility += useShadowMap(shadowMap, vec4(sample_coord, coords.z, 1.0));
  }
  return visibility / float(NUM_SAMPLES);

}

vec3 blinnPhong();

void main(void) {

  float visibility;
  vec3 shadowCoord = vPositionFromLight.xyz / vPositionFromLight.w;
  shadowCoord = (shadowCoord + 1.0) / 2.0;
  //visibility = useShadowMap(uShadowMap, vec4(shadowCoord, 1.0));
  //visibility = PCF(uShadowMap, vec4(shadowCoord, 1.0));
  visibility = PCSS(uShadowMap, vec4(shadowCoord, 1.0));

  vec3 phongColor = blinnPhong();

  gl_FragColor = vec4(phongColor * visibility, 1.0);
  //gl_FragColor = vec4(vec3(blocker_depth), 1.0);
}