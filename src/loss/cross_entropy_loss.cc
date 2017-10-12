//------------------------------------------------------------------------------
// Copyright (c) 2016 by contributors. All Rights Reserved.
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
//------------------------------------------------------------------------------

/*
Author: Chao Ma (mctt90@gmail.com)
This file is the implementation of CrossEntropyLoss class.
*/

#include "src/loss/cross_entropy_loss.h"

namespace xLearn {

// Given predictions (data samples) and labels, return
// cross-entropy loss value
double CrossEntropyLoss::Evalute(const std::vector<real_t>& pred,
                                 const std::vector<real_t>& label) {
  CHECK_EQ(pred.empty(), false);
  double val = 0.0;
  for (size_t i = 0; i < pred.size(); ++i) {
    real_t y = label[i] > 0 ? 1.0 : -1.0;
    double t = pred[i];
    double expnyt = exp(-y*t);
    val += log1p(expnyt);
  }
  return val;
}

// Given data sample and current model, calculate gradient
// and update model
void CrossEntropyLoss::CalcGrad(const DMatrix* matrix,
                                Model& model,
                                Updater* updater) {
  CHECK_NOTNULL(matrix);
  CHECK_GT(matrix->row_length, 0);
  CHECK_NOTNULL(updater);
  size_t row_len = matrix->row_length;
  // Calculate gradient
  for (size_t i = 0; i < row_len; ++i) {
    SparseRow* row = matrix->row[i];
    double score = score_func_->CalcScore(row, model);
    // partial gradient
    real_t y = matrix->Y[i] > 0 ? 1.0 : -1.0;
    double expnyt = exp(-y*score);
    real_t pg = -y*expnyt/(1+expnyt);
    // real gradient and update
    score_func_->CalcGrad(row,   // sparse row
       model,                    // curret model
       pg,                       // partial gradient
       updater,                  // updater
       matrix->scale[i]);        // scale for normalization
  }
}

} // namespace xLearn
