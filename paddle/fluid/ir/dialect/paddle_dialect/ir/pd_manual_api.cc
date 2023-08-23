// Copyright (c) 2023 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "paddle/fluid/ir/dialect/paddle_dialect/ir/pd_manual_api.h"
#include "paddle/fluid/ir/dialect/paddle_dialect/ir/api_builder.h"
#include "paddle/fluid/ir/dialect/paddle_dialect/ir/pd_op.h"
#include "paddle/ir/core/builtin_op.h"

namespace paddle {
namespace dialect {
std::vector<ir::OpResult> concat_grad(std::vector<ir::OpResult> x,
                                      ir::OpResult out_grad,
                                      ir::OpResult axis) {
  auto combine_op =
      APIBuilder::Instance().GetBuilder()->Build<ir::CombineOp>(x);
  paddle::dialect::ConcatGradOp concat_grad_op =
      APIBuilder::Instance().GetBuilder()->Build<paddle::dialect::ConcatGradOp>(
          combine_op.out(), out_grad, axis);
  auto split_op = APIBuilder::Instance().GetBuilder()->Build<ir::SplitOp>(
      concat_grad_op.result(0));
  return split_op.outputs();
}
}  // namespace dialect
}  // namespace paddle
