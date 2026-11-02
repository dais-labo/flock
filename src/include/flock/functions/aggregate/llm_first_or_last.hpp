#pragma once

#include "flock/functions/aggregate/aggregate.hpp"
#include "flock/functions/llm_function_bind_data.hpp"

namespace flock {

class LlmFirstOrLast : public AggregateFunctionBase {
public:
    AggregateFunctionType function_type;

public:
    explicit LlmFirstOrLast() = default;

    int GetFirstOrLastTupleId(nlohmann::json& tuples);
    nlohmann::json Evaluate(nlohmann::json& tuples);

    static duckdb::unique_ptr<duckdb::FunctionData> Bind(duckdb::BindAggregateFunctionInput& input);


public:
    static void Initialize(duckdb::AggregateStateInput& input, duckdb::data_ptr_t* states, idx_t count) {
        AggregateFunctionBase::Initialize<LlmFirstOrLast>(input, states, count);
    }
    static void Operation(duckdb::Vector inputs[], duckdb::AggregateInputData& aggr_input_data, idx_t input_count,
                          duckdb::Vector& states, idx_t count) {
        AggregateFunctionBase::Operation<LlmFirstOrLast>(inputs, aggr_input_data, input_count, states, count);
    }
    static void ClusterUpdate(duckdb::Vector inputs[], duckdb::AggregateInputData& aggr_input_data, idx_t input_count,
                              const duckdb::ClusteredAggr& clustered, idx_t count) {
        AggregateFunctionBase::ClusterUpdate<LlmFirstOrLast>(inputs, aggr_input_data, input_count, clustered, count);
    }
    static void Combine(duckdb::Vector& source, duckdb::Vector& target, duckdb::AggregateInputData& aggr_input_data,
                        idx_t count) {
        AggregateFunctionBase::Combine<LlmFirstOrLast>(source, target, aggr_input_data, count);
    }
    static void Destroy(duckdb::Vector& states, duckdb::AggregateInputData& aggr_input_data, idx_t count) {
        AggregateFunctionBase::Destroy<LlmFirstOrLast>(states, aggr_input_data, count);
    }
    template<AggregateFunctionType function_type>
    static void Finalize(duckdb::Vector& states, duckdb::AggregateFinalizeInputData& aggr_input_data, duckdb::Vector& result,
                         idx_t count, idx_t offset) {
        FinalizeResults(states, aggr_input_data, result, count, offset, function_type);
    }
    static void FinalizeResults(duckdb::Vector& states, duckdb::AggregateInputData& aggr_input_data,
                                duckdb::Vector& result, idx_t count, idx_t offset, AggregateFunctionType function_type);
};

}// namespace flock
