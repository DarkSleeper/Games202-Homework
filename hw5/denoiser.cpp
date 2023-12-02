void Denoiser::Reprojection(const FrameInfo &frameInfo) {
    int height = m_accColor.m_height;
    int width = m_accColor.m_width;
    Matrix4x4 preWorldToScreen =
        m_preFrameInfo.m_matrix[m_preFrameInfo.m_matrix.size() - 1];
    Matrix4x4 preWorldToCamera =
        m_preFrameInfo.m_matrix[m_preFrameInfo.m_matrix.size() - 2];
//#pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // TODO: Reproject
            auto id = int(frameInfo.m_id(x, y));
            if (id < 0) continue; // background
            auto m = m_preFrameInfo.m_matrix[id];
            auto inv_m = Inverse(frameInfo.m_matrix[id]);
            auto pre_screen = (preWorldToScreen * m * inv_m)(frameInfo.m_position(x, y), Float3::EType::Point);
            if (pre_screen.x < 0 || pre_screen.x >= width || pre_screen.y < 0 || pre_screen.y >= height) {
                m_valid(x, y) = false;
                m_misc(x, y) = Float3(0.f);
            } else {
                if (m_preFrameInfo.m_id(pre_screen.x, pre_screen.y) != frameInfo.m_id(x, y)) {
                    m_valid(x, y) = false;
                    m_misc(x, y) = Float3(0.f);
                } else {
                    m_valid(x, y) = true;
                    m_misc(x, y) = m_accColor(pre_screen.x, pre_screen.y);
                }
            }
            //m_valid(x, y) = false;
            //m_misc(x, y) = Float3(0.f);
        }
    }
    std::swap(m_misc, m_accColor);
}

void Denoiser::TemporalAccumulation(const Buffer2D<Float3> &curFilteredColor) {
    int height = m_accColor.m_height;
    int width = m_accColor.m_width;
    int kernelRadius = 3;
#pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // TODO: Temporal clamp
            int sample_num = 0;
            Float3 sum_c = Float3(0.f);
            Float3 sum_c2 = Float3(0.f);
            for (int j = y - kernelRadius; j <= y + kernelRadius; j++) {
                for (int i = x - kernelRadius; i <= x + kernelRadius; i++) {
                    if (i < 0 || i >= width || j < 0 || j >= height) continue;
                    auto &color = curFilteredColor(i, j);
                    sum_c += color;
                    sum_c2 += color * color;
                    sample_num++;
                }
            }
            auto mean = sum_c / sample_num;
            auto variance = sum_c2 / sample_num - mean * mean;
            auto sigma = Float3(sqrtf(variance.x), sqrtf(variance.y), sqrtf(variance.z));
            Float3 color = Clamp(m_accColor(x, y), mean - sigma * m_colorBoxK, mean + sigma * m_colorBoxK);
            // TODO: Exponential moving average
            float alpha = 1.0f;
            if (m_valid(x, y)) {
                alpha = m_alpha;
            }
            m_misc(x, y) = Lerp(color, curFilteredColor(x, y), alpha);
        }
    }
    std::swap(m_misc, m_accColor);
}

Buffer2D<Float3> Denoiser::Filter(const FrameInfo &frameInfo) {
    int height = frameInfo.m_beauty.m_height;
    int width = frameInfo.m_beauty.m_width;
    Buffer2D<Float3> filteredImage = CreateBuffer2D<Float3>(width, height);
    int kernelRadius = 16;
    float m_sigmaCoord2 = 0.5f / (m_sigmaCoord * m_sigmaCoord);
    float m_sigmaColor2 = 0.5f / (m_sigmaColor * m_sigmaColor);
    float m_sigmaNormal2 = 0.5f / (m_sigmaNormal * m_sigmaNormal);
    float m_sigmaPlane2 = 0.5f / (m_sigmaPlane * m_sigmaPlane);
#pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // TODO: Joint bilateral filter
            float sum_of_weights = 0.0f;
            Float3 sum_of_weighted_values = Float3(0.0f);
            for (int j = y - kernelRadius; j <= y + kernelRadius; j++) {
                for (int i = x - kernelRadius; i <= x + kernelRadius; i++) {
                    if (i < 0 || i >= width || j < 0 || j >= height) continue;

                    float w_coord = ((i - x) * (i - x) + (j - y) * (j - y)) * m_sigmaCoord2;
                    float w_color = SqrLength(frameInfo.m_beauty(x, y) - frameInfo.m_beauty(i, j)) * m_sigmaColor2;
                    float dnormal = SafeAcos(Dot(frameInfo.m_normal(x, y), frameInfo.m_normal(i, j)));
                    float w_normal = dnormal * dnormal * m_sigmaNormal2;

                    float dplane;
                    auto distance = frameInfo.m_position(i, j) - frameInfo.m_position(x, y);
                    auto vector_out = Length(distance);
                    if (vector_out < 0.0001f) dplane = 0.f;
                    else dplane = Dot(frameInfo.m_normal(x, y), distance / vector_out);
                    float w_plane = dplane * dplane * m_sigmaPlane2;

                    float weight = std::expf(-w_coord - w_color - w_normal - w_plane);
                    sum_of_weights += weight;
                    sum_of_weighted_values += frameInfo.m_beauty(i, j) * weight;
                }
            }
            if (sum_of_weights < 0.00001f) filteredImage(x, y) = Float3(0.f);
            else filteredImage(x, y) = sum_of_weighted_values / sum_of_weights;
        }
    }
    return filteredImage;
}
