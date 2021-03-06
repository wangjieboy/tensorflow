/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/delegates/gpu/cl/selectors/convolution_transposed_selector.h"

#include "absl/memory/memory.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/convolution_transposed.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/convolution_transposed_3x3.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/convolution_transposed_3x3_thin.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/convolution_transposed_4x4.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/convolution_transposed_thin.h"
#include "tensorflow/lite/delegates/gpu/cl/tensor_type.h"

namespace tflite {
namespace gpu {
namespace cl {
namespace {

absl::Status SelectConvolutionTransposedAdreno(
    const ConvolutionTransposedAttributes& attr, const DeviceInfo& device_info,
    const OperationDef& op_def, std::unique_ptr<GPUOperation>* ptr) {
  if (IsConvolutionTransposedThinSupported(attr)) {
    ConvolutionTransposedThin conv =
        CreateConvolutionTransposedThin(device_info, op_def, attr);
    *ptr = absl::make_unique<ConvolutionTransposedThin>(std::move(conv));
  } else if (IsConvolutionTransposed3x3ThinSupported(attr)) {
    ConvolutionTransposed3x3Thin conv =
        CreateConvolutionTransposed3x3Thin(device_info, op_def, attr);
    *ptr = absl::make_unique<ConvolutionTransposed3x3Thin>(std::move(conv));
  } else {
    ConvolutionTransposed conv =
        CreateConvolutionTransposed(device_info, op_def, attr);
    *ptr = absl::make_unique<ConvolutionTransposed>(std::move(conv));
  }
  return absl::OkStatus();
}

absl::Status SelectConvolutionTransposedPowerVR(
    const ConvolutionTransposedAttributes& attr, const DeviceInfo& device_info,
    const OperationDef& op_def, std::unique_ptr<GPUOperation>* ptr) {
  if (IsConvolutionTransposedThinSupported(attr)) {
    ConvolutionTransposedThin conv =
        CreateConvolutionTransposedThin(device_info, op_def, attr);
    *ptr = absl::make_unique<ConvolutionTransposedThin>(std::move(conv));
  } else if (IsConvolutionTransposed3x3ThinSupported(attr)) {
    ConvolutionTransposed3x3Thin conv =
        CreateConvolutionTransposed3x3Thin(device_info, op_def, attr);
    *ptr = absl::make_unique<ConvolutionTransposed3x3Thin>(std::move(conv));
  } else if (IsConvolutionTransposed3x3Supported(op_def, attr)) {
    ConvolutionTransposed3x3 conv =
        CreateConvolutionTransposed3x3(device_info, op_def, attr);
    *ptr = absl::make_unique<ConvolutionTransposed3x3>(std::move(conv));
  } else if (IsConvolutionTransposed4x4Supported(op_def, attr)) {
    ConvolutionTransposed4x4 conv =
        CreateConvolutionTransposed4x4(device_info, op_def, attr);
    *ptr = absl::make_unique<ConvolutionTransposed4x4>(std::move(conv));
  } else {
    ConvolutionTransposed conv =
        CreateConvolutionTransposed(device_info, op_def, attr);
    *ptr = absl::make_unique<ConvolutionTransposed>(std::move(conv));
  }
  return absl::OkStatus();
}

absl::Status SelectConvolutionTransposedMali(
    const ConvolutionTransposedAttributes& attr, const DeviceInfo& device_info,
    const OperationDef& op_def, std::unique_ptr<GPUOperation>* ptr) {
  ConvolutionTransposed conv =
      CreateConvolutionTransposed(device_info, op_def, attr);
  *ptr = absl::make_unique<ConvolutionTransposed>(std::move(conv));
  return absl::OkStatus();
}
}  // namespace

absl::Status SelectConvolutionTransposed(
    const ConvolutionTransposedAttributes& attr, const DeviceInfo& device_info,
    const OperationDef& op_def, std::unique_ptr<GPUOperation>* ptr) {
  if (device_info.IsAdreno()) {
    return SelectConvolutionTransposedAdreno(attr, device_info, op_def, ptr);
  } else if (device_info.IsPowerVR() || device_info.IsAMD() ||
             device_info.IsNvidia() || device_info.IsIntel()) {
    return SelectConvolutionTransposedPowerVR(attr, device_info, op_def, ptr);
  } else if (device_info.IsMali()) {
    return SelectConvolutionTransposedMali(attr, device_info, op_def, ptr);
  } else {
    return SelectConvolutionTransposedAdreno(attr, device_info, op_def, ptr);
  }
}

}  // namespace cl
}  // namespace gpu
}  // namespace tflite
