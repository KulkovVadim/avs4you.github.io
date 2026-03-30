#pragma once
#define NOMINMAX
#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>
#include <mutex>
#include <windows.h>
#include <cstdio>
#include "utils.h"

struct StyleTransferRuntime {
    void Init(const std::wstring& model_path, bool try_cuda = false, int device_id = 0);
    bool RunBGRA(unsigned char* pBGRA, int width, int height, double mix, bool tw2 = false);
    StyleTransferRuntime();
    ~StyleTransferRuntime();
private:

    bool EnsureSession();
    void BuildInputOutputInfo();

    void Preprocess_BGRA_to_Float_TW2(const unsigned char* pBGRA, int width, int height, float* out, bool nchw);
    void Preprocess_BGRA_to_Float(const unsigned char* srcBGRA, int w, int h, float* dst);
    void Postprocess_Float_to_BGRA(const float* src, int srcW, int srcH, unsigned char* dstBGRA, int dstW, int dstH, double mix);
    void Postprocess_Float_to_BGRA_TW2(const float* out, int srcW, int srcH, unsigned char* pBGRA, int dstW, int dstH,double mix,bool nchw);

    bool IsNCHW() const { return input_is_nchw_; }
private:
    bool initialized_ = false;
    bool cuda_enabled_ = false;
    int  cuda_device_id_ = 0;

    // ORT
    Ort::Env env_{ ORT_LOGGING_LEVEL_WARNING, "StyleTransfer" };
    Ort::SessionOptions so_;
    std::unique_ptr<Ort::Session> session_;

    std::string input_name_;
    std::string output_name_;
    std::vector<int64_t> input_shape_;   // 1x3xH xW or 1xH xW x3 (dynamic model, H/W often -1)
    std::vector<int64_t> output_shape_;  // similarly
    bool input_is_nchw_ = true;

    // normalization (default 0..1)
    float scale_ = 1.0f;
};
