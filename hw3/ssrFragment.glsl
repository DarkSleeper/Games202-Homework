/*
 * Evaluate diffuse bsdf value.
 *
 * wi, wo are all in world space.
 * uv is in screen space, [0, 1] x [0, 1].
 *
 */
vec3 EvalDiffuse(vec3 wi, vec3 wo, vec2 uv) {
  vec3 diffuse = GetGBufferDiffuse(uv);
  vec3 normal = GetGBufferNormalWorld(uv);
  float cos = max(0., dot(normal, wi));
  return diffuse * cos * INV_PI;
}

/*
 * Evaluate directional light with shadow map
 * uv is in screen space, [0, 1] x [0, 1].
 *
 */
vec3 EvalDirectionalLight(vec2 uv) {
  vec3 Le = GetGBufferuShadow(uv) * uLightRadiance;
  return Le;
}

bool RayMarch(vec3 ori, vec3 dir, out vec3 hitPos) {
  float step = 0.07;
  const int max_steps = 100;

  vec3 martch = normalize(dir) * step;
  vec3 pos = ori;
  for (int i = 0; i < max_steps; i++)
  {
    vec2 uv = GetScreenCoordinate(pos);
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) break;
    float depth = GetGBufferDepth(uv);
    float pos_depth = GetDepth(pos);
    if (pos_depth > depth + 0.0001) {
      hitPos = pos;
      return true;
    }
    pos = pos + martch;
  }

  return false;
}

#define SAMPLE_NUM 10

void main() {
  float s = InitRand(gl_FragCoord.xy);
  
  vec3 wi = normalize(uLightDir);
  vec3 wo = normalize(uCameraPos - vPosWorld.xyz);
  vec2 uv = GetScreenCoordinate(vPosWorld.xyz);
  vec3 L_direct = EvalDirectionalLight(uv) * EvalDiffuse(wi, wo, uv);

  vec3 L_indirect = vec3(0.0);
  float pdf;
  vec3 hit_pos;
  vec3 b1, b2;
  for (int i = 0; i < SAMPLE_NUM; i++) 
  {
    vec3 dir = SampleHemisphereCos(s, pdf);
    vec3 normal = GetGBufferNormalWorld(uv);
    LocalBasis(normal, b1, b2);
    dir = mat3(b1, b2, normal) * dir;

    if (RayMarch(vPosWorld.xyz, dir, hit_pos)) {
      vec2 hit_uv = GetScreenCoordinate(hit_pos);
      vec3 hit_normal = GetGBufferNormalWorld(hit_uv);
      if (dot(normal, hit_normal) < 0.0001)
        L_indirect += EvalDiffuse(dir, wo, uv) / pdf * EvalDiffuse(wi, dir, hit_uv) * EvalDirectionalLight(hit_uv);
    }
  }
  L_indirect = L_indirect / float(SAMPLE_NUM);

  vec3 L = L_direct + L_indirect;

  vec3 color = pow(clamp(L, vec3(0.0), vec3(1.0)), vec3(1.0 / 2.2));
  gl_FragColor = vec4(vec3(color.rgb), 1.0);
}
