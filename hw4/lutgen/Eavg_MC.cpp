Vec3f IntegrateEmu(Vec3f V, float roughness, float NdotV, Vec3f Ei) {
    Vec3f Eavg = Vec3f(0.0f);
    const int sample_count = 1024;
    Vec3f N = Vec3f(0.0, 0.0, 1.0);

    samplePoints sampleList = squareToCosineHemisphere(sample_count);
    for (int i = 0; i < sample_count; i++) {
        Vec3f L = sampleList.directions[i];
        Vec3f H = normalize(V + L);

        float NoL = std::max(L.z, 0.0f);
        float NoH = std::max(H.z, 0.0f);
        float VoH = std::max(dot(V, H), 0.0f);
        float NoV = std::max(dot(N, V), 0.0f);

        // TODO: To calculate Eavg here
        Eavg += Ei * 2.f *  NoV;
    }

    return Eavg / sample_count;
}
