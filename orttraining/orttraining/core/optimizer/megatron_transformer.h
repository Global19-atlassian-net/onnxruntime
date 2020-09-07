// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "core/optimizer/graph_transformer.h"
#include "core/graph/graph_utils.h"

namespace onnxruntime {

class MegatronTransformer : public GraphTransformer {
 public:
  MegatronTransformer(int32_t horizontal_parallel_rank, int32_t horizontal_parallel_size,
                      std::unordered_map<std::string, std::string>& updated_weight_names,
                      std::unordered_set<std::string>& weights_to_train,
                      const std::unordered_set<std::string>& compatible_execution_providers = {}) noexcept
      : GraphTransformer("MegatronTransformer", compatible_execution_providers),
        horizontal_parallel_rank_(horizontal_parallel_rank),
        horizontal_parallel_size_(horizontal_parallel_size),
        updated_weight_names_(updated_weight_names),
        weights_to_train_(weights_to_train) {}

  Status ApplyImpl(Graph& graph, bool& modified, int graph_level,
                   const logging::Logger& logger) const override;

 private:
  Status TransformMLP(Graph& graph, bool& modified, int graph_level, const logging::Logger& logger,
                      std::vector<Node*>& nodes_to_clear_shape) const;

  Status TransformSelfAttention(Graph& graph, bool& modified, int graph_level, const logging::Logger& logger,
                                std::vector<Node*>& nodes_to_clear_shape,
                                std::unordered_set<Node*>& self_attention_dropout_nodes) const;

  Status TransformT5SelfAttention(Graph& graph, bool& modified, int graph_level, const logging::Logger& logger,
                                  std::vector<Node*>& nodes_to_clear_shape,
                                  std::unordered_set<Node*>& self_attention_dropout_nodes, int32_t& counter) const;

  Status TransformT5MLP(Graph& graph, bool& modified, int graph_level,
                        const logging::Logger& logger,
                        std::vector<Node*>& nodes_to_clear_shape,
                        std::unordered_set<Node*>& self_attention_dropout_nodes, int32_t& counter) const;

  Status TransformDropout(Graph& graph, bool& modified, int graph_level, const logging::Logger& logger,
                          std::unordered_set<Node*>& self_attention_dropout_nodes, int32_t& counter) const;

  NodeArg& PartitionWeightByColumn(Graph& graph, const NodeArg& input_arg, int stride = 1) const;

  NodeArg& PartitionWeightByRow(Graph& graph, const NodeArg& input_arg) const;

  const int32_t horizontal_parallel_rank_;
  const int32_t horizontal_parallel_size_;
  std::unordered_map<std::string, std::string>& updated_weight_names_;
  std::unordered_set<std::string>& weights_to_train_;
};

}  // namespace onnxruntime