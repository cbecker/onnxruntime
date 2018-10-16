#pragma once

#include "core/graph/rewrite_rule.h"

namespace onnxruntime {
class ConvAddFusion : public RewriteRule {
 public:
  ConvAddFusion() noexcept : RewriteRule("ConvBNFusion", "Fusing BN into Conv") {
  }

 private:
  bool SatisfyCondition(const Node& node) override;

  Status Apply(GraphEditor* graph_editor, Node* node, bool* modified) override;
};

}  // namespace onnxruntime