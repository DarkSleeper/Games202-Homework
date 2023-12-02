Vec3f IntegrateBRDF(Vec3f V, float roughness, float NdotV) {
    float A = 0.0;
    float B = 0.0;
    float C = 0.0;
    const int sample_count = 1024;
    Vec3f N = Vec3f(0.0, 0.0, 1.0);
    const float R0 = 0.04f;
    
    samplePoints sampleList = squareToCosineHemisphere(sample_count);
    for (int i = 0; i < sample_count; i++) {
        // TODO: To calculate (fr * ni) / p_o here
        Vec3f L = sampleList.directions[i];
        Vec3f H = normalize(V + L);
        float pdf = sampleList.PDFs[i];

        // float F = R0 + (1 - R0) * powf(1 - dot(V, H), 5);
        float F = 1.0f; // energy all reflected
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(roughness, NdotV, dot(N, L));
        float fr = F * D * G / 4 / NdotV / pdf;

        A += fr;
        B += fr;
        C += fr;
    }

    return {A / sample_count, B / sample_count, C / sample_count};
}
