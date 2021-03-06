// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
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

#include "lite/operators/sequence_pool_concat_op.h"
#include "lite/core/op_registry.h"

namespace paddle {
namespace lite {
namespace operators {

bool SequencePoolConcatOp::CheckShape() const {
  CHECK_GE(param_.X.size(), 1)
      << "The number of input sequences is at least two.";
  CHECK_OR_FALSE(param_.Out);
  return true;
}

bool SequencePoolConcatOp::InferShapeImpl() const {
  int out_dim = 0;
  for (int i = 0; i < param_.X.size(); ++i) {
    out_dim += param_.X[i]->dims().count(1, param_.X[i]->dims().size());
  }
  int seq_num = param_.X[0]->lod()[0].size() - 1;

  std::vector<std::vector<uint64_t>> lod(1);
  for (int i = 0; i < seq_num + 1; ++i) {
    lod[0].push_back(i);
  }
  param_.Out->set_lod(lod);
  param_.Out->Resize({seq_num, out_dim});
  return true;
}

bool SequencePoolConcatOp::AttachImpl(const cpp::OpDesc &opdesc,
                                      lite::Scope *scope) {
  auto input_list = opdesc.Input("X");
  param_.X.clear();
  for (auto var : input_list) {
    param_.X.push_back(scope->FindVar(var)->GetMutable<lite::Tensor>());
  }
  param_.Out =
      scope->FindVar(opdesc.Output("Out").front())->GetMutable<lite::Tensor>();
  CHECK(param_.Out) << "Output(Out) of Sequence Concat Op should not be null.";
  param_.pool_type = opdesc.GetAttr<std::vector<std::string>>("pooltype");
  return true;
}

}  // namespace operators
}  // namespace lite
}  // namespace paddle

REGISTER_LITE_OP(sequence_pool_concat,
                 paddle::lite::operators::SequencePoolConcatOp);
