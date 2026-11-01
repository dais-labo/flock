#include "flock/functions/aggregate/llm_rerank.hpp"

namespace flock {

template void AggregateFunctionBase::Initialize<LlmRerank>(duckdb::AggregateStateInput&, duckdb::data_ptr_t*,
                                                           idx_t);
template void AggregateFunctionBase::Operation<LlmRerank>(duckdb::Vector[], duckdb::AggregateInputData&, idx_t,
                                                          duckdb::Vector&, idx_t);
template void AggregateFunctionBase::Combine<LlmRerank>(duckdb::Vector&, duckdb::Vector&, duckdb::AggregateInputData&,
                                                        idx_t);

}// namespace flock
