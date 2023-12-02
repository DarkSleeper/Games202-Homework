float DistributionGGX(vec3 N, vec3 H, float roughness)
{
   // TODO: To calculate GGX NDF here
   float a = roughness * roughness;
   float a_2 = a * a;
   float NoH = max(dot(N, H), 0.0);
   float NoH_2 = NoH * NoH;
   float denom = NoH_2 * (a_2 - 1.0) + 1.0;
   return a_2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    // TODO: To calculate Smith G1 here
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    // TODO: To calculate Smith G here
    return GeometrySchlickGGX(max(dot(N, L), 0.0), roughness) * GeometrySchlickGGX(max(dot(N, V), 0.0), roughness);
}

vec3 fresnelSchlick(vec3 F0, vec3 V, vec3 H)
{
    // TODO: To calculate Schlick F here
    return F0 + (1.0 - F0) * pow(1.0 - max(dot(V, H), 0.0), 5.0);
}

vec3 MultiScatterBRDF(float NdotL, float NdotV)
{
  vec3 albedo = pow(texture2D(uAlbedoMap, vTextureCoord).rgb, vec3(2.2));

  vec3 E_o = texture2D(uBRDFLut, vec2(NdotL, uRoughness)).xyz;
  vec3 E_i = texture2D(uBRDFLut, vec2(NdotV, uRoughness)).xyz;

  vec3 E_avg = texture2D(uEavgLut, vec2(0, uRoughness)).xyz;
  // copper
  vec3 edgetint = vec3(0.827, 0.792, 0.678);
  vec3 F_avg = AverageFresnel(albedo, edgetint);
  
  // TODO: To calculate fms and missing energy here
  vec3 fms = (1.0 - E_o) * (1.0 - E_i) / PI / (1.0 - E_avg);
  vec3 fadd = F_avg * E_avg / (1.0 - F_avg * ( 1.0 - E_avg));

  return fms * fadd;
  
}
