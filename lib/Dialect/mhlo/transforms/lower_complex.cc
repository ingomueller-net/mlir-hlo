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

// Thsi file implements passes to convert complex operations to equivalent real
// value operations. This does not include removing complex values from function
// argument or return types.

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <numeric>

#include "third_party/llvm/llvm-project/llvm/include/llvm/ADT/STLExtras.h"
#include "third_party/llvm/llvm-project/mlir/include/mlir/IR/Attributes.h"
#include "third_party/llvm/llvm-project/mlir/include/mlir/IR/MLIRContext.h"
#include "third_party/llvm/llvm-project/mlir/include/mlir/IR/Operation.h"
#include "third_party/llvm/llvm-project/mlir/include/mlir/IR/PatternMatch.h"
#include "third_party/llvm/llvm-project/mlir/include/mlir/IR/TypeUtilities.h"
#include "third_party/llvm/llvm-project/mlir/include/mlir/IR/Types.h"
#include "third_party/llvm/llvm-project/mlir/include/mlir/Pass/Pass.h"
#include "third_party/llvm/llvm-project/mlir/include/mlir/Pass/PassRegistry.h"
#include "third_party/tensorflow/compiler/mlir/hlo/include/mlir-hlo/Dialect/mhlo/IR/hlo_ops.h"
#include "third_party/tensorflow/compiler/mlir/hlo/include/mlir-hlo/Dialect/mhlo/transforms/passes.h"
#include "third_party/tensorflow/compiler/mlir/hlo/include/mlir-hlo/utils/hlo_utils.h"

using mlir::FunctionPass;
using mlir::OwningRewritePatternList;
using mlir::PassRegistration;
using mlir::PassWrapper;

namespace {
class LowerComplex : public PassWrapper<LowerComplex, FunctionPass> {
 public:
  explicit LowerComplex() : PassWrapper<LowerComplex, FunctionPass>() {}

  /// Performs the lowering to MHLO dialect.
  void runOnFunction() override;
};
}  // end anonymous namespace

namespace mlir {
namespace hlo {
namespace {

#include "third_party/tensorflow/compiler/mlir/hlo/lib/Dialect/mhlo/transforms/generated_lower_complex.inc"

}  // end anonymous namespace

void PopulateComplexLoweringPatterns(MLIRContext* context,
                                     OwningRewritePatternList* patterns) {
  populateWithGenerated(context, patterns);
}
}  // end namespace hlo
}  // end namespace mlir

// Lowers the complex operations that can be represented using other operations.
void LowerComplex::runOnFunction() {
  // Add lowering patterns to the list.
  OwningRewritePatternList patterns;
  mlir::hlo::PopulateComplexLoweringPatterns(&getContext(), &patterns);

  applyPatternsAndFoldGreedily(getFunction(), patterns);
}

static PassRegistration<LowerComplex> pass(
    "mhlo-test-lower-complex",
    "Lower complex operations into non-complex operations");
