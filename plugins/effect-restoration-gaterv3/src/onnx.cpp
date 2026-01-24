#include "./onnx.h"

Ort::SessionOptions COnnx::GetSessionOptions(unsigned int threads)
{
	Ort::SessionOptions session_options;
	session_options.SetIntraOpNumThreads(threads);
	session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);
	return session_options;
}

COnnx::COnnx(std::wstring wsModel, unsigned int threads)
	: m_env(ORT_LOGGING_LEVEL_WARNING, "gater-v3")
	, m_session(m_env, wsModel.c_str(), GetSessionOptions(threads))
	, m_allocator{}
	, m_inputName(m_session.GetInputNameAllocated(0, m_allocator))
	, m_outputName(m_session.GetOutputNameAllocated(0, m_allocator))
{
}
void COnnx::Apply(unsigned char* pixels, int w, int h, int tile, int overlap)
{
	// output accumulator and weight map for blending
	std::vector<float> output_accumulator(h * w * 3, 0.0f);
	std::vector<float> weight_map(h * w, 0.0f);
	const int step = tile - overlap;

	// process image tile by tile
	for (int tile_y = 0; tile_y < h; tile_y += step)
	{
		for (int tile_x = 0; tile_x < w; tile_x += step)
		{
			// calculate actual tile dimensions (handle edges)
			int actual_tile_w = std::min(tile, w - tile_x);
			int actual_tile_h = std::min(tile, h - tile_y);

			// extract tile from source image
			std::vector<unsigned char> tile_pixels(actual_tile_h * actual_tile_w * 4);
			for (int y = 0; y < actual_tile_h; ++y)
			{
				for (int x = 0; x < actual_tile_w; ++x)
				{
					int src_idx = ((tile_y + y) * w + (tile_x + x)) * 4;
					int dst_idx = (y * actual_tile_w + x) * 4;
					for (int ch = 0; ch < 4; ++ch) tile_pixels[dst_idx + ch] = pixels[src_idx + ch];
				}
			}

			// convert tile to tensor and run inference
			auto tile_tensor = ToTensor(tile_pixels.data(), actual_tile_w, actual_tile_h);
			auto output_tensors = RunInference(tile_tensor, actual_tile_w, actual_tile_h);
			float* tile_output = output_tensors[0].GetTensorMutableData<float>();

			// create weight map for this tile (feathering at edges for smooth blending)
			std::vector<float> tile_weights(actual_tile_h * actual_tile_w);
			for (int y = 0; y < actual_tile_h; ++y)
			{
				for (int x = 0; x < actual_tile_w; ++x)
				{
					// calculate distance from tile edges (for feathering)
					float weight_x = 1.0f;
					float weight_y = 1.0f;

					// left edge feathering
					if (tile_x > 0 && x < overlap)
						weight_x = static_cast<float>(x) / overlap;
					// right edge feathering
					else if (tile_x + actual_tile_w < w && x >= actual_tile_w - overlap)
						weight_x = static_cast<float>(actual_tile_w - x) / overlap;

					// top edge feathering
					if (tile_y > 0 && y < overlap)
						weight_y = static_cast<float>(y) / overlap;
					// bottom edge feathering
					else if (tile_y + actual_tile_h < h && y >= actual_tile_h - overlap)
						weight_y = static_cast<float>(actual_tile_h - y) / overlap;

					tile_weights[y * actual_tile_w + x] = weight_x * weight_y;
				}
			}

			// accumulate tile output with weights
			for (int y = 0; y < actual_tile_h; ++y)
			{
				for (int x = 0; x < actual_tile_w; ++x)
				{
					int global_idx = (tile_y + y) * w + (tile_x + x);
					int tile_idx = y * actual_tile_w + x;
					float weight = tile_weights[tile_idx];

					for (int ch = 0; ch < 3; ++ch)
					{
						int chw_tile = ch * actual_tile_h * actual_tile_w + tile_idx;
						int chw_global = global_idx * 3 + ch;
						output_accumulator[chw_global] += tile_output[chw_tile] * weight;
					}

					weight_map[global_idx] += weight;
				}
			}
		}
	}

	// normalize accumulated output by weight map and write back to pixels
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			int idx = y * w + x;
			float weight = weight_map[idx];
			if (weight > 0.0f)
			{
				for (int ch = 0; ch < 3; ++ch)
				{
					int global_idx = idx * 3 + ch;
					float v = output_accumulator[global_idx] / weight;
					v = std::min(1.0f, std::max(0.0f, v));
					pixels[idx * 4 + ch] = static_cast<unsigned char>(v * 255.0f);
				}
			}
		}
	}

	return;
}
std::vector<float> COnnx::ToTensor(unsigned char* pixels, int w, int h)
{
	std::vector<float> tensor(1 * 3 * h * w);
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x)
			for (int ch = 0; ch < 3; ++ch)
			{
				int hwc = (y * w + x) * 4 + ch; // 4 channels in source (RGBA)
				int chw = ch * h * w + y * w + x;
				tensor[chw] = pixels[hwc] / 255.0f;
			}
	return tensor;
}
void COnnx::FromTensor(unsigned char* outPixels, int w, int h, float* tensor)
{
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x)
			for (int ch = 0; ch < 3; ++ch)
			{
				int chw = ch * h * w + y * w + x;
				int hwc = (y * w + x) * 4 + ch;
				float v = tensor[chw];
				v = std::min(1.0f, std::max(0.0f, v));
				outPixels[hwc] = static_cast<unsigned char>(v * 255.0f);
			}
	return;
}
std::vector<Ort::Value> COnnx::RunInference(std::vector<float>& input, int w, int h)
{
	Ort::MemoryInfo mem_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
	std::vector<int64_t> input_shape = {1, 3, h, w};
	Ort::Value input_tensor = Ort::Value::CreateTensor<float>(mem_info, input.data(), input.size(), input_shape.data(), input_shape.size());

	const char* input_name = m_inputName.get();
	const char* output_name = m_outputName.get();

	return m_session.Run(Ort::RunOptions{nullptr}, &input_name, &input_tensor, 1, &output_name, 1);
}