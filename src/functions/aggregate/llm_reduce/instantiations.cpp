#include "flock/functions/aggregate/llm_reduce.hpp"

namespace flock {

template void AggregateFunctionBase::Initialize<LlmReduce>(duckdb::AggregateStateInput&, duckdb::data_ptr_t*,
                                                           idx_t);
template void AggregateFunctionBase::Operation<LlmReduce>(duckdb::Vector[], duckdb::AggregateInputData&, idx_t,
                                                          duckdb::Vector&, idx_t);
template void AggregateFunctionBase::Combine<LlmReduce>(duckdb::Vector&, duckdb::Vector&, duckdb::AggregateInputData&,
                                                        idx_t);
template void LlmReduce::Finalize<AggregateFunctionType::REDUCE>(duckdb::Vector&, duckdb::AggregateFinalizeInputData&,
                                                                 duckdb::Vector&, idx_t, idx_t);

}// namespace flock
