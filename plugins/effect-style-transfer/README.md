# Effect Style Transfer

<figure>
  <img src="./img/Mona_Lisa.gif" alt="Mona Lisa styles" width="256">
  <figcaption>Mona Lisa in different style-transfer variants.</figcaption>
</figure>

## Installation

When opening `Effect.sln` or `test_onnx.sln`, make sure to restore the NuGet packages so that **ONNX Runtime** is installed correctly.

1. Open `Effect.sln` or `test_onnx.sln` in **Visual Studio 2019**.
2. In **Solution Explorer**, right-click the solution and select **Restore NuGet Packages**.
   - This option may not appear immediately after opening the solution.
3. After the restore completes, the following folder should be available:

   `packages\Microsoft.ML.OnnxRuntime.1.16.0\`

## Runtime Notes

- **Microsoft.ML.OnnxRuntime 1.16.0** is the latest package version that still includes **Win32** (`x86`) binaries.
- The package is installed through NuGet in **Visual Studio 2019** and referenced via `packages.config`.
- **Important:** **ONNX Runtime** does not provide **CUDA binaries for x86/Win32**. As a result, GPU/CUDA acceleration is **not available** in 32-bit builds.
- The CUDA execution provider is supported only in **x64** builds.

## Models

The original models were converted from **PyTorch** to **ONNX**.

### Sources

- ~~**Source 1.** Standard style set (for example, *rain_princess*):  
  `pytorch/examples/fast_neural_style`~~
- **Source 2.** Extended style set (for example, *candy*, *crystal*, *la_muse*, *mosaic*, *starry_night*):  
  `ebylmz/fast-neural-style-transfer`
- **Source 3.** Custom-trained styles:  
  *paints drawing*, *pencil*, *scream*

## Model Input and Output

Different models expect different input formats, so the preprocessing and postprocessing pipeline must match the specific model being used.

### 1. `paints drawing.onnx`, `pencil`, `scream`

These models typically expect input **without ImageNet normalization**.

Expected input:
- `float32` image tensor
- usually **RGB**
- value range: **[0..255]**

Additional notes:
- Some models may also require resizing the input image to a fixed resolution, depending on how the model was exported or trained.

### 2. `candy_cw2.0_sw900000.0_tw2.0.onnx`, `crystal_grove_cw2.0_sw900000.0_tw2.0.onnx`

These models are exported `TransformNet` models from the `ebylmz/fast-neural-style-transfer` repository.

They expect input normalized using **ImageNet** statistics:

1. `ToTensor()`  
   Converts the image to `float32` and scales it to the **[0..1]** range.
2. `Normalize(mean=IMAGENET_MEAN, std=IMAGENET_STD)`

Where:
- `IMAGENET_MEAN = [0.485, 0.456, 0.406]`
- `IMAGENET_STD  = [0.229, 0.224, 0.225]`

For these models, the output usually needs to be **de-normalized** back to **[0..1]** or **[0..255]** before saving or displaying the result.