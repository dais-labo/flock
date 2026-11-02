#pragma once

#include "flock/functions/llm_function_bind_data.hpp"
#include "flock/functions/scalar/scalar.hpp"

namespace flock {

class AiClassify : public ScalarFunctionBase {
public:
    static duckdb::unique_ptr<duckdb::FunctionData> Bind(duckdb::BindScalarFunctionInput& input);
    static void Execute(duckdb::DataChunk& args, duckdb::ExpressionState& state, duckdb::Vector& result);
};

}// namespace flock
