Vec3f ImportanceSampleGGX(Vec2f Xi, Vec3f N, float roughness) {

    float a = roughness * roughness;

    // 半球面上采样方向，并转换到N为z轴的空间上（虽然传入的N就是世界坐标的z轴,即可以直接输出p）
    //TODO: in spherical space - Bonus 1
    float theta_m = atan(a * sqrt(Xi.x) / sqrt(1 - Xi.x));
    float phi_h = 2 * PI * Xi.y;

    //TODO: from spherical space to cartesian space - Bonus 1
    Vec3f p = Vec3f(cos(phi_h) * sin(theta_m), sin(phi_h) * sin(theta_m), cos(theta_m));

    //TODO: tangent coordinates - Bonus 1
    // x,y,z -> tan, bitan, N

    //TODO: transform H to tangent space - Bonus 1

    return normalize(p);
}


float GeometrySchlickGGX(float NdotV, float roughness) {
    // TODO: To calculate Schlick G1 here - Bonus 1
    float a = roughness;
    float k = (a * a) / 2.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float GeometrySmith(float roughness, float NoV, float NoL) {
    float ggx2 = GeometrySchlickGGX(NoV, roughness);
    float ggx1 = GeometrySchlickGGX(NoL, roughness);

    return ggx1 * ggx2;
}

Vec3f IntegrateBRDF(Vec3f V, float roughness) {
    float A = 0.0;
    float B = 0.0;
    float C = 0.0;
    const int sample_count = 1024;
    Vec3f N = Vec3f(0.0, 0.0, 1.0);
    for (int i = 0; i < sample_count; i++) {
        Vec2f Xi = Hammersley(i, sample_count);
        Vec3f H = ImportanceSampleGGX(Xi, N, roughness);
        Vec3f L = normalize(H * 2.0f * dot(V, H) - V);

        float NoL = std::max(L.z, 0.0f);
        float NoH = std::max(H.z, 0.0f);
        float VoH = std::max(dot(V, H), 0.0f);
        float NoV = std::max(dot(N, V), 0.0f);
        
        // TODO: To calculate (fr * ni) / p_o here - Bonus 1
        // pdf上的i和o关系混乱，最好自己推一下weight公式
        float fr = VoH * GeometrySmith(roughness, NoV, NoL) / NoV / NoH;

        A += fr;
        B += fr;
        C += fr;

        // Split Sum - Bonus 2
        
    }

    return { A / sample_count, B / sample_count, C / sample_count };
}
