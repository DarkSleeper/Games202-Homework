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
