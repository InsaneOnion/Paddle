/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once

#include <algorithm>
#include <vector>

#include "paddle/fluid/framework/op_registry.h"
#include "paddle/fluid/framework/operator.h"
#include "paddle/phi/kernels/funcs/eigen/common.h"
#include "paddle/phi/kernels/funcs/eigen/eigen_function.h"

#define MAX_RANK_SUPPORTED 8

namespace paddle {
namespace operators {
inline std::vector<int> get_expand_shape(
    const framework::ExecutionContext& ctx) {
  if (ctx.HasInput("Shape")) {
    auto* shape_tensor = ctx.Input<phi::DenseTensor>("Shape");
    auto* shape_data = shape_tensor->data<int>();
    phi::DenseTensor cpu_shape_tensor;
    if (shape_tensor->place().GetType() == phi::AllocationType::GPU) {
      paddle::framework::TensorCopySync(
          *shape_tensor, phi::CPUPlace(), &cpu_shape_tensor);
      shape_data = cpu_shape_tensor.data<int>();
    }
#ifdef PADDLE_WITH_XPU
    if (shape_tensor->place().GetType() == phi::AllocationType::XPU) {
      paddle::framework::TensorCopySync(
          *shape_tensor, phi::CPUPlace(), &cpu_shape_tensor);
      shape_data = cpu_shape_tensor.data<int>();
    }
#endif
    auto vec_shape =
        std::vector<int>(shape_data, shape_data + shape_tensor->numel());
    return vec_shape;
  }

  auto list_expand_shapes_tensor =
      ctx.MultiInput<phi::DenseTensor>("expand_shapes_tensor");
  if (list_expand_shapes_tensor.size() > 0) {
    // get tensor from
    std::vector<int> vec_expand_shape;
    for (size_t i = 0; i < list_expand_shapes_tensor.size(); ++i) {
      auto tensor = list_expand_shapes_tensor[i];
      if (tensor->place().GetType() == phi::AllocationType::GPU) {
        phi::DenseTensor temp;
        paddle::framework::TensorCopySync(*tensor, phi::CPUPlace(), &temp);
        vec_expand_shape.push_back(*temp.data<int32_t>());
      }
#ifdef PADDLE_WITH_XPU
      else if (tensor->place().GetType() ==  // NOLINT
               phi::AllocationType::XPU) {   // NOLINT
        phi::DenseTensor temp;
        paddle::framework::TensorCopySync(*tensor, phi::CPUPlace(), &temp);
        vec_expand_shape.push_back(*temp.data<int32_t>());
      }
#endif
      else {  // NOLINT
        vec_expand_shape.push_back(*tensor->data<int32_t>());
      }
    }
    return vec_expand_shape;
  } else {
    return ctx.Attr<std::vector<int>>("shape");
  }
}
}  // namespace operators
}  // namespace paddle
