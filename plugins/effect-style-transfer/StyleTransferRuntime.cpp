#include "StyleTransferRuntime.h"
#include <stdexcept>
#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cmath>

constexpr float IMAGENET_MEAN[3] = { 0.485f, 0.456f, 0.406f }; // R,G,B
constexpr float IMAGENET_STD[3] = { 0.229f, 0.224f, 0.225f };

inline float clamp01(float v)
{
    return std::max(0.0f, std::min(1.0f, v));
}

StyleTransferRuntime::StyleTransferRuntime() {
    // graph optimization
    so_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
}

StyleTransferRuntime ::~StyleTransferRuntime() {
    // the order of destruction is correct thanks to unique_ptr / RAII
}

bool StyleTransferRuntime::EnsureSession() {
    return initialized_ && session_ != nullptr;
}

void StyleTransferRuntime::Init(const std::wstring& model_path, bool try_cuda, int device_id)
{
    initialized_ = false;
    cuda_enabled_ = false;
    cuda_device_id_ = device_id;

    if (try_cuda) {
        // connecting CUDA EP
        OrtCUDAProviderOptions opts{};
        opts.device_id = device_id;
        opts.arena_extend_strategy = 0;
        opts.gpu_mem_limit = SIZE_MAX;
        opts.cudnn_conv_algo_search = OrtCudnnConvAlgoSearchExhaustive;
        opts.do_copy_in_default_stream = 1;

        auto status = Ort::GetApi().SessionOptionsAppendExecutionProvider_CUDA(so_, &opts);
        if (status != nullptr) {
            const char* msg = Ort::GetApi().GetErrorMessage(status);
            Utils::DebugPrint("CUDA EP append failed: %s\n", msg ? msg : "(null)");
            Ort::GetApi().ReleaseStatus(status);
            cuda_enabled_ = false;   // CUDA did not turn on
        }
        else {
            cuda_enabled_ = true;    // CUDA has been successfully connected.
            Utils::DebugPrint("CUDA EP appended successfully (device %d)\n", device_id);
        }
    }

    // создаём сессию
    try {
        session_.reset(new Ort::Session(env_, model_path.c_str(), so_));
    }
    catch (const Ort::Exception& ex) {
        Utils::DebugPrint("Ort::Session failed: %s\n", ex.what());
        session_.reset();
        initialized_ = false;
        return;
    }
    catch (const std::exception& ex) {
        Utils::DebugPrint("Session creation std::exception: %s\n", ex.what());
        session_.reset();
        initialized_ = false;
        return;
    }
    catch (...) {
        Utils::DebugPrint("Session creation: unknown exception\n");
        session_.reset();
        initialized_ = false;
        return;
    }

    BuildInputOutputInfo();
    initialized_ = true;
}

void StyleTransferRuntime::BuildInputOutputInfo() {
    Ort::AllocatorWithDefaultOptions alloc;

    // input
    {
        auto name = session_->GetInputNameAllocated(0, alloc);
        input_name_ = name.get();

        auto ti = session_->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo();
        input_shape_ = ti.GetShape(); // for example {1,3,-1,-1} or {1,-1,-1,3}

        auto shape = ti.GetShape();   // will show -1,-1 for dynamic H,W
        auto type = ti.GetElementType(); // ORT_TENSOR_ELEMENT_DATA_TYPE_FLOAT
      
        // We define the layout
        // 1x3xHxW → NCHW; 1xHxWx3 → NHWC
        if (input_shape_.size() == 4) {
            if (input_shape_[1] == 3) input_is_nchw_ = true;
            else if (input_shape_[3] == 3) input_is_nchw_ = false;
            else {
                // unusual shape - default NCHW
                input_is_nchw_ = true;
            }
        }
        else {
            // fallback
            input_is_nchw_ = true;
        }
    }

    // output
    {
        auto name = session_->GetOutputNameAllocated(0, alloc);
        output_name_ = name.get();
        auto to = session_->GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo();
        output_shape_ = to.GetShape();
    }
}

bool StyleTransferRuntime::RunBGRA(unsigned char* pBGRA, int width, int height, double mix, bool tw2) {
    if (!EnsureSession()) return false;

    // form an input tensor for a specific H,W
    const int64_t H = height;
    const int64_t W = width;

    std::vector<int64_t> in_shape;
    if (IsNCHW()) in_shape = { 1, 3, H, W };
    else          in_shape = { 1, H, W, 3 };

    const size_t elem_count = size_t(1) * size_t(3) * size_t(H) * size_t(W);
    std::vector<float> input(elem_count);
    
    if (tw2)
        Preprocess_BGRA_to_Float_TW2(pBGRA, width, height, input.data(), IsNCHW());
    else
        Preprocess_BGRA_to_Float(pBGRA, width, height, input.data());

    Ort::MemoryInfo mi = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value in_tensor = Ort::Value::CreateTensor<float>(mi, input.data(), input.size(), in_shape.data(), in_shape.size());

    // prepare the names
    const char* in_names[] = { input_name_.c_str() };
    const char* out_names[] = { output_name_.c_str() };

    // one run
    try
    {
        auto outputs = session_->Run(Ort::RunOptions{ nullptr }, in_names, &in_tensor, 1, out_names, 1);
        if (outputs.size() != 1 || !outputs[0].IsTensor()) return false;

        // read output
        auto& out_val = outputs[0];
        auto out_info = out_val.GetTensorTypeAndShapeInfo();
        auto out_shape = out_info.GetShape();

        int outH = 0, outW = 0;
        if (IsNCHW()) {  // [1, 3, H, W]
            outH = static_cast<int>(out_shape[2]);
            outW = static_cast<int>(out_shape[3]);
        }
        else {           // [1, H, W, 3]
            outH = static_cast<int>(out_shape[1]);
            outW = static_cast<int>(out_shape[2]);
        }

        Utils::DebugPrint("Out shape: ");
        for (auto d : out_shape) Utils::DebugPrint("%lld ", (long long)d);
        Utils::DebugPrint("\n");

        // We expect identical H, W and 3 channels
        // (if the model returns [1,3,H,W] or [1,H,W,3] - supported)
        const float* out_data = out_val.GetTensorData<float>();

        if (tw2)
            Postprocess_Float_to_BGRA_TW2(out_data, outW, outH, pBGRA, width, height, mix, IsNCHW());
        else
            Postprocess_Float_to_BGRA(out_data, outW, outH, pBGRA, width, height, mix);
    }
    catch (const Ort::Exception& ex) {
        Utils::DebugPrint("Ort::Session failed: %s\n", ex.what());
        return false;
    }
    
    return true;
}

void StyleTransferRuntime::Preprocess_BGRA_to_Float(const unsigned char* srcBGRA, int w, int h, float* dst) {
    // Convert BGRA → RGB float, normalize by scale_
    // NCHW: dst[c*H*W + y*W + x]
    // NHWC: dst[(y*W + x)*3 + c]
    const float s = scale_;
    if (IsNCHW()) {
        const size_t HW = size_t(w) * size_t(h);
        float* R = dst + 0 * HW;
        float* G = dst + 1 * HW;
        float* B = dst + 2 * HW;

        size_t idx = 0;
        for (int y = 0; y < h; ++y) {
            const unsigned char* row = srcBGRA + size_t(y) * size_t(w) * 4;
            for (int x = 0; x < w; ++x, ++idx) {
                const unsigned char b = row[x * 4 + 0];
                const unsigned char g = row[x * 4 + 1];
                const unsigned char r = row[x * 4 + 2];
                R[idx] = r * s;
                G[idx] = g * s;
                B[idx] = b * s;
            }
        }
    }
    else {
        size_t i = 0;
        for (int y = 0; y < h; ++y) {
            const unsigned char* row = srcBGRA + size_t(y) * size_t(w) * 4;
            for (int x = 0; x < w; ++x) {
                const unsigned char b = row[x * 4 + 0];
                const unsigned char g = row[x * 4 + 1];
                const unsigned char r = row[x * 4 + 2];
                dst[i++] = r * s;
                dst[i++] = g * s;
                dst[i++] = b * s;
            }
        }
    }
}

void StyleTransferRuntime::Postprocess_Float_to_BGRA(const float* src, int srcW, int srcH, unsigned char* dstBGRA, int dstW, int dstH, double mix)
{
    const float invs = (scale_ == 0.0f) ? 1.0f : (1.0f / scale_);
    const double a = (mix < 0.0) ? 0.0 : (mix > 1.0 ? 1.0 : mix);

    const int Hsrc = srcH;
    const int Wsrc = srcW;
    const int Hdst = dstH;
    const int Wdst = dstW;

    const int H = std::min(Hsrc, Hdst);
    const int W = std::min(Wsrc, Wdst);

    auto clampu8 = [](float v)->unsigned char {
        if (v < 0.f) v = 0.f;
        if (v > 255.f) v = 255.f;
        return static_cast<unsigned char>(v + 0.5f);
    };

    if (IsNCHW()) {
        // src: [1, 3, Hsrc, Wsrc]
        const size_t planeSize = static_cast<size_t>(Wsrc) * static_cast<size_t>(Hsrc);
        const float* R = src + 0 * planeSize;
        const float* G = src + 1 * planeSize;
        const float* B = src + 2 * planeSize;

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const size_t srcIdx = static_cast<size_t>(y) * Wsrc + x;
                const size_t dstIdx = static_cast<size_t>(y) * Wdst + x;
                const size_t idxBGRA = dstIdx * 4;

                float r = R[srcIdx] * invs;
                float g = G[srcIdx] * invs;
                float b = B[srcIdx] * invs;

                float b0 = dstBGRA[idxBGRA + 0];
                float g0 = dstBGRA[idxBGRA + 1];
                float r0 = dstBGRA[idxBGRA + 2];
                unsigned char a0 = dstBGRA[idxBGRA + 3];

                float rf = float((1.0 - a) * r0 + a * r);
                float gf = float((1.0 - a) * g0 + a * g);
                float bf = float((1.0 - a) * b0 + a * b);

                dstBGRA[idxBGRA + 0] = clampu8(bf);
                dstBGRA[idxBGRA + 1] = clampu8(gf);
                dstBGRA[idxBGRA + 2] = clampu8(rf);
                dstBGRA[idxBGRA + 3] = a0;
            }
        }
    }
    else {
        // src: [1, Hsrc, Wsrc, 3]
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const size_t srcIdx = static_cast<size_t>(y) * Wsrc + x;
                const size_t dstIdx = static_cast<size_t>(y) * Wdst + x;
                const size_t idxBGRA = dstIdx * 4;
                const size_t idxNHWC = srcIdx * 3;

                float r = src[idxNHWC + 0] * invs;
                float g = src[idxNHWC + 1] * invs;
                float b = src[idxNHWC + 2] * invs;

                float b0 = dstBGRA[idxBGRA + 0];
                float g0 = dstBGRA[idxBGRA + 1];
                float r0 = dstBGRA[idxBGRA + 2];
                unsigned char a0 = dstBGRA[idxBGRA + 3];

                float rf = float((1.0 - a) * r0 + a * r);
                float gf = float((1.0 - a) * g0 + a * g);
                float bf = float((1.0 - a) * b0 + a * b);

                dstBGRA[idxBGRA + 0] = clampu8(bf);
                dstBGRA[idxBGRA + 1] = clampu8(gf);
                dstBGRA[idxBGRA + 2] = clampu8(rf);
                dstBGRA[idxBGRA + 3] = a0;
            }
        }
    }
}

void StyleTransferRuntime::Preprocess_BGRA_to_Float_TW2(const unsigned char* pBGRA,
    int width, int height,
    float* out, bool nchw)
{
    const int H = height;
    const int W = width;

    if (nchw)
    {
        // out shape: [1,3,H,W], channel order: R, G, B
        const size_t planeSize = static_cast<size_t>(H) * static_cast<size_t>(W);
        float* pR = out + planeSize * 0;
        float* pG = out + planeSize * 1;
        float* pB = out + planeSize * 2;

        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                const size_t idx = static_cast<size_t>(y) * W + x;
                const size_t idxBGRA = idx * 4;

                const float b = pBGRA[idxBGRA + 0] / 255.0f;
                const float g = pBGRA[idxBGRA + 1] / 255.0f;
                const float r = pBGRA[idxBGRA + 2] / 255.0f;

                pR[idx] = (r - IMAGENET_MEAN[0]) / IMAGENET_STD[0];
                pG[idx] = (g - IMAGENET_MEAN[1]) / IMAGENET_STD[1];
                pB[idx] = (b - IMAGENET_MEAN[2]) / IMAGENET_STD[2];
            }
        }
    }
    else
    {
        // out shape: [1,H,W,3], channel order: R, G, B
        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                const size_t idx = static_cast<size_t>(y) * W + x;
                const size_t idxBGRA = idx * 4;
                const size_t idxOut = idx * 3;

                const float b = pBGRA[idxBGRA + 0] / 255.0f;
                const float g = pBGRA[idxBGRA + 1] / 255.0f;
                const float r = pBGRA[idxBGRA + 2] / 255.0f;

                out[idxOut + 0] = (r - IMAGENET_MEAN[0]) / IMAGENET_STD[0];
                out[idxOut + 1] = (g - IMAGENET_MEAN[1]) / IMAGENET_STD[1];
                out[idxOut + 2] = (b - IMAGENET_MEAN[2]) / IMAGENET_STD[2];
            }
        }
    }
}

void StyleTransferRuntime::Postprocess_Float_to_BGRA_TW2(const float* out, int srcW, int srcH, unsigned char* pBGRA, int dstW, int dstH, double mix, bool nchw)
{
    const int Hsrc = srcH;
    const int Wsrc = srcW;
    const int Hdst = dstH;
    const int Wdst = dstW;

    const int H = std::min(Hsrc, Hdst);
    const int W = std::min(Wsrc, Wdst);

    const float fmix = static_cast<float>(std::clamp(mix, 0.0, 1.0));
    const float fimix = 1.0f - fmix;

    if (nchw)
    {
        const size_t planeSize = static_cast<size_t>(Hsrc) * static_cast<size_t>(Wsrc);
        const float* pR = out + planeSize * 0;
        const float* pG = out + planeSize * 1;
        const float* pB = out + planeSize * 2;

        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                const size_t srcIdx = static_cast<size_t>(y) * Wsrc + x;
                const size_t dstIdx = static_cast<size_t>(y) * Wdst + x;
                const size_t idxBGRA = dstIdx * 4;

                float r = pR[srcIdx] * IMAGENET_STD[0] + IMAGENET_MEAN[0];
                float g = pG[srcIdx] * IMAGENET_STD[1] + IMAGENET_MEAN[1];
                float b = pB[srcIdx] * IMAGENET_STD[2] + IMAGENET_MEAN[2];

                r = clamp01(r);
                g = clamp01(g);
                b = clamp01(b);

                const unsigned char newR =
                    static_cast<unsigned char>(std::lround(r * 255.0f));
                const unsigned char newG =
                    static_cast<unsigned char>(std::lround(g * 255.0f));
                const unsigned char newB =
                    static_cast<unsigned char>(std::lround(b * 255.0f));

                // original pixel for mix
                const unsigned char origB = pBGRA[idxBGRA + 0];
                const unsigned char origG = pBGRA[idxBGRA + 1];
                const unsigned char origR = pBGRA[idxBGRA + 2];
                const unsigned char origA = pBGRA[idxBGRA + 3];

                const float outBf = origB * fimix + newB * fmix;
                const float outGf = origG * fimix + newG * fmix;
                const float outRf = origR * fimix + newR * fmix;

                pBGRA[idxBGRA + 0] =
                    static_cast<unsigned char>(std::lround(std::clamp(outBf, 0.0f, 255.0f)));
                pBGRA[idxBGRA + 1] =
                    static_cast<unsigned char>(std::lround(std::clamp(outGf, 0.0f, 255.0f)));
                pBGRA[idxBGRA + 2] =
                    static_cast<unsigned char>(std::lround(std::clamp(outRf, 0.0f, 255.0f)));
                pBGRA[idxBGRA + 3] = origA; // don't touch alpha
            }
        }
    }
    else
    {
        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                const size_t srcIdx = static_cast<size_t>(y) * Wsrc + x;
                const size_t dstIdx = static_cast<size_t>(y) * Wdst + x;
                const size_t idxBGRA = dstIdx * 4;
                const size_t idxOut = srcIdx * 3;

                float r = out[idxOut + 0] * IMAGENET_STD[0] + IMAGENET_MEAN[0];
                float g = out[idxOut + 1] * IMAGENET_STD[1] + IMAGENET_MEAN[1];
                float b = out[idxOut + 2] * IMAGENET_STD[2] + IMAGENET_MEAN[2];

                r = clamp01(r);
                g = clamp01(g);
                b = clamp01(b);

                const unsigned char newR =
                    static_cast<unsigned char>(std::lround(r * 255.0f));
                const unsigned char newG =
                    static_cast<unsigned char>(std::lround(g * 255.0f));
                const unsigned char newB =
                    static_cast<unsigned char>(std::lround(b * 255.0f));

                const unsigned char origB = pBGRA[idxBGRA + 0];
                const unsigned char origG = pBGRA[idxBGRA + 1];
                const unsigned char origR = pBGRA[idxBGRA + 2];
                const unsigned char origA = pBGRA[idxBGRA + 3];

                const float outBf = origB * fimix + newB * fmix;
                const float outGf = origG * fimix + newG * fmix;
                const float outRf = origR * fimix + newR * fmix;

                pBGRA[idxBGRA + 0] =
                    static_cast<unsigned char>(std::lround(std::clamp(outBf, 0.0f, 255.0f)));
                pBGRA[idxBGRA + 1] =
                    static_cast<unsigned char>(std::lround(std::clamp(outGf, 0.0f, 255.0f)));
                pBGRA[idxBGRA + 2] =
                    static_cast<unsigned char>(std::lround(std::clamp(outRf, 0.0f, 255.0f)));
                pBGRA[idxBGRA + 3] = origA;
            }
        }
    }
}