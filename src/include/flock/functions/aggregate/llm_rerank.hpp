#pragma once

#include "flock/functions/aggregate/aggregate.hpp"
#include "flock/functions/llm_function_bind_data.hpp"

namespace flock {

class LlmRerank : public AggregateFunctionBase {
public:
    explicit LlmRerank() = default;

    nlohmann::json SlidingWindow(nlohmann::json& tuples);
    std::vector<int> RerankBatch(const nlohmann::json& tuples);

public:
    static duckdb::unique_ptr<duckdb::FunctionData> Bind(duckdb::BindAggregateFunctionInput& input);

    static void Initialize(duckdb::AggregateStateInput& input, duckdb::data_ptr_t* states, idx_t count) {
        AggregateFunctionBase::Initialize<LlmRerank>(input, states, count);
    }
    static void Operation(duckdb::Vector inputs[], duckdb::AggregateInputData& aggr_input_data, idx_t input_count,
                          duckdb::Vector& states, idx_t count) {
        AggregateFunctionBase::Operation<LlmRerank>(inputs, aggr_input_data, input_count, states, count);
    }
    static void ClusterUpdate(duckdb::Vector inputs[], duckdb::AggregateInputData& aggr_input_data, idx_t input_count,
                              const duckdb::ClusteredAggr& clustered, idx_t count) {
        AggregateFunctionBase::ClusterUpdate<LlmRerank>(inputs, aggr_input_data, input_count, clustered, count);
    }
    static void Combine(duckdb::Vector& source, duckdb::Vector& target, duckdb::AggregateInputData& aggr_input_data,
                        idx_t count) {
        AggregateFunctionBase::Combine<LlmRerank>(source, target, aggr_input_data, count);
    }
    static void Destroy(duckdb::Vector& states, duckdb::AggregateInputData& aggr_input_data, idx_t count) {
        AggregateFunctionBase::Destroy<LlmRerank>(states, aggr_input_data, count);
    }
    static void Finalize(duckdb::Vector& states, duckdb::AggregateFinalizeInputData& aggr_input_data, duckdb::Vector& result,
                         idx_t count, idx_t offset);
};

}// namespace flock
