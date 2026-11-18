#include "flock/model_manager/providers/handlers/anthropic.hpp"
#include "flock/model_manager/providers/provider.hpp"
#include "flock/model_manager/repository.hpp"
#include "nlohmann/json.hpp"
#include <gtest/gtest.h>

namespace flock {
using json = nlohmann::json;

class AnthropicHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Tests focus on JSON parsing and response handling
    }
};

// Test text response structure
TEST_F(AnthropicHandlerTest, TextResponseStructure) {
    json response = {
            {"id", "msg_01"},
            {"type", "message"},
            {"role", "assistant"},
            {"content", {{{"type", "text"}, {"text", "{\"items\": [\"result1\"]}"}}}},
            {"stop_reason", "end_turn"},
            {"model", "claude-3-haiku-20240307"}};

    EXPECT_TRUE(response["content"].is_array());
    EXPECT_FALSE(response["content"].empty());
    EXPECT_EQ(response["content"][0]["type"], "text");
    EXPECT_TRUE(response["content"][0].contains("text"));
}

// Test structured output response (using output_format/json_schema)
TEST_F(AnthropicHandlerTest, StructuredOutputResponse) {
    // With output_format, Claude returns JSON directly in text content
    json response = {
            {"id", "msg_01"},
            {"type", "message"},
            {"role", "assistant"},
            {"content", {{{"type", "text"}, {"text", "{\"items\": [\"result1\", \"result2\"]}"}}}},
            {"stop_reason", "end_turn"},
            {"model", "claude-3-haiku-20240307"}};

    EXPECT_TRUE(response["content"].is_array());
    EXPECT_EQ(response["content"][0]["type"], "text");

    // Parse the text content as JSON
    auto parsed = json::parse(response["content"][0]["text"].get<std::string>());
    EXPECT_TRUE(parsed.contains("items"));
    EXPECT_EQ(parsed["items"].size(), 2);
}

// Test valid stop reasons
TEST_F(AnthropicHandlerTest, ValidStopReasons) {
    std::vector<std::string> valid_reasons = {"end_turn", "stop_sequence", "tool_use"};

    for (const auto& reason: valid_reasons) {
        json response = {
                {"content", {{{"type", "text"}, {"text", "test"}}}},
                {"stop_reason", reason}};
        EXPECT_EQ(response["stop_reason"], reason);
    }
}

// Test max_tokens stop reason
TEST_F(AnthropicHandlerTest, MaxTokensStopReason) {
    json response = {
            {"content", {{{"type", "text"}, {"text", "partial response..."}}}},
            {"stop_reason", "max_tokens"}};

    EXPECT_EQ(response["stop_reason"], "max_tokens");
    // This should trigger TokenLimitExceededError in checkProviderSpecificResponse
}

// Test error response structure
TEST_F(AnthropicHandlerTest, ErrorResponseStructure) {
    json error_response = {
            {"type", "error"},
            {"error", {{"type", "invalid_request_error"}, {"message", "max_tokens must be greater than 0"}}}};

    EXPECT_EQ(error_response["type"], "error");
    EXPECT_TRUE(error_response["error"].contains("message"));
    EXPECT_EQ(error_response["error"]["type"], "invalid_request_error");
}

// Test header format
TEST_F(AnthropicHandlerTest, HeaderFormat) {
    std::string api_key = "sk-ant-api-test123";
    std::string api_version = ANTHROPIC_DEFAULT_API_VERSION;

    std::vector<std::string> expected_headers = {
            "x-api-key: " + api_key,
            "anthropic-version: " + api_version,
            "anthropic-beta: structured-outputs-2025-11-13"};

    EXPECT_EQ(expected_headers.size(), 3);
    EXPECT_TRUE(expected_headers[0].find("x-api-key") != std::string::npos);
    EXPECT_TRUE(expected_headers[1].find("anthropic-version") != std::string::npos);
    EXPECT_TRUE(expected_headers[1].find(ANTHROPIC_DEFAULT_API_VERSION) != std::string::npos);
    EXPECT_TRUE(expected_headers[2].find("anthropic-beta") != std::string::npos);
}

// Test request payload structure
TEST_F(AnthropicHandlerTest, RequestPayloadStructure) {
    json request = {
            {"model", "claude-3-haiku-20240307"},
            {"max_tokens", 1024},
            {"messages", {{{"role", "user"}, {"content", {{{"type", "text"}, {"text", "Hello"}}}}}}}};

    EXPECT_TRUE(request.contains("model"));
    EXPECT_TRUE(request.contains("max_tokens"));
    EXPECT_TRUE(request.contains("messages"));
    EXPECT_EQ(request["max_tokens"], 1024);
    EXPECT_EQ(request["messages"][0]["role"], "user");
}

// Test request with system prompt
TEST_F(AnthropicHandlerTest, RequestWithSystemPrompt) {
    json request = {
            {"model", "claude-3-haiku-20240307"},
            {"max_tokens", 1024},
            {"system", "You are a helpful assistant."},
            {"messages", {{{"role", "user"}, {"content", {{{"type", "text"}, {"text", "Hello"}}}}}}}};

    EXPECT_TRUE(request.contains("system"));
    EXPECT_EQ(request["system"], "You are a helpful assistant.");
}

// Test request with output_format for structured output
TEST_F(AnthropicHandlerTest, RequestWithOutputFormat) {
    json output_format = {
            {"type", "json_schema"},
            {"schema", {{"type", "object"}, {"properties", {{"items", {{"type", "array"}, {"minItems", 1}, {"maxItems", 1}, {"items", {{"type", "string"}}}}}}}, {"required", {"items"}}, {"additionalProperties", false}}}};

    json request = {
            {"model", "claude-3-haiku-20240307"},
            {"max_tokens", 1024},
            {"messages", {{{"role", "user"}, {"content", "test"}}}},
            {"output_format", output_format}};

    EXPECT_TRUE(request.contains("output_format"));
    EXPECT_EQ(request["output_format"]["type"], "json_schema");
    EXPECT_TRUE(request["output_format"]["schema"]["properties"].contains("items"));
}

// Test image content structure
TEST_F(AnthropicHandlerTest, ImageContentStructure) {
    json image_content = {
            {"type", "image"},
            {"source", {
                               {"type", "base64"},
                               {"media_type", "image/png"},
                               {"data", "iVBORw0KGgo..."}// Truncated base64
                       }}};

    EXPECT_EQ(image_content["type"], "image");
    EXPECT_EQ(image_content["source"]["type"], "base64");
    EXPECT_TRUE(image_content["source"].contains("media_type"));
    EXPECT_TRUE(image_content["source"].contains("data"));
}

// Test API URL construction
TEST_F(AnthropicHandlerTest, APIUrlConstruction) {
    std::string base_url = "https://api.anthropic.com/v1/";
    std::string completion_url = base_url + "messages";

    EXPECT_EQ(completion_url, "https://api.anthropic.com/v1/messages");
}

// Test mixed content array
TEST_F(AnthropicHandlerTest, MixedContentArray) {
    json content = json::array({{{"type", "text"}, {"text", "What is in this image?"}},
                                {{"type", "image"}, {"source", {{"type", "base64"}, {"media_type", "image/jpeg"}, {"data", "base64data"}}}}});

    EXPECT_EQ(content.size(), 2);
    EXPECT_EQ(content[0]["type"], "text");
    EXPECT_EQ(content[1]["type"], "image");
}

// Test response with thinking and text content blocks
TEST_F(AnthropicHandlerTest, MultipleContentBlocks) {
    // Claude may return thinking blocks along with text when using extended thinking
    json response = {
            {"content", {{{"type", "thinking"}, {"thinking", "Let me analyze this step by step..."}}, {{"type", "text"}, {"text", "{\"items\": [\"result\"]}"}}}},
            {"stop_reason", "end_turn"}};

    EXPECT_EQ(response["content"].size(), 2);
    // The handler should extract the text block for structured output
    EXPECT_EQ(response["content"][1]["type"], "text");
}

// Exposes the streaming reconstruction for testing (no request is sent)
class AnthropicStreamProbe : public AnthropicModelManager {
public:
    AnthropicStreamProbe() : AnthropicModelManager("test-key", ANTHROPIC_DEFAULT_API_VERSION, true) {}
    using AnthropicModelManager::ReconstructFromStreamedChunks;
};

static std::string StreamEvent(const std::string& event, const std::string& data, const std::string& eol = "\n") {
    return "event: " + event + eol + "data: " + data + eol + eol;
}

static std::string TextStream(const std::string& eol = "\n") {
    return StreamEvent("message_start", R"({"type":"message_start","message":{"id":"msg_1","usage":{"input_tokens":25,"output_tokens":1}}})", eol) +
           StreamEvent("content_block_start", R"({"type":"content_block_start","index":0,"content_block":{"type":"text","text":""}})", eol) +
           StreamEvent("ping", R"({"type": "ping"})", eol) +
           StreamEvent("content_block_delta", R"({"type":"content_block_delta","index":0,"delta":{"type":"text_delta","text":"{\"items\": "}})", eol) +
           StreamEvent("content_block_delta", R"({"type":"content_block_delta","index":0,"delta":{"type":"text_delta","text":"[\"a\", \"b\"]}"}})", eol) +
           StreamEvent("content_block_stop", R"({"type":"content_block_stop","index":0})", eol) +
           StreamEvent("message_delta", R"({"type":"message_delta","delta":{"stop_reason":"end_turn","stop_sequence":null},"usage":{"output_tokens":12}})", eol) +
           StreamEvent("message_stop", R"({"type":"message_stop"})", eol);
}

TEST_F(AnthropicHandlerTest, StreamedTextIsReconstructed) {
    AnthropicStreamProbe probe;
    auto result = probe.ReconstructFromStreamedChunks(TextStream());

    ASSERT_TRUE(result.contains("content"));
    ASSERT_EQ(result["content"].size(), 1);
    EXPECT_EQ(result["content"][0]["type"], "text");
    EXPECT_EQ(result["content"][0]["text"], "{\"items\": [\"a\", \"b\"]}");
    EXPECT_EQ(result["stop_reason"], "end_turn");
    EXPECT_EQ(result["usage"]["input_tokens"], 25);
    EXPECT_EQ(result["usage"]["output_tokens"], 12);
}

TEST_F(AnthropicHandlerTest, StreamedTextLineEndingsAndFraming) {
    AnthropicStreamProbe probe;
    auto expected = probe.ReconstructFromStreamedChunks(TextStream());

    // CRLF line endings
    EXPECT_EQ(probe.ReconstructFromStreamedChunks(TextStream("\r\n")), expected);
    // No space after the colon, plus a keep-alive comment
    std::string compact = ": keep-alive\n" + TextStream();
    for (const std::string field: {"event: ", "data: "}) {
        for (size_t pos = compact.find(field); pos != std::string::npos; pos = compact.find(field, pos)) {
            compact.erase(pos + field.size() - 1, 1);
        }
    }
    EXPECT_EQ(probe.ReconstructFromStreamedChunks(compact), expected);
    // Body ends without the blank line after message_stop
    std::string unterminated = TextStream();
    unterminated.resize(unterminated.size() - 1);
    EXPECT_EQ(probe.ReconstructFromStreamedChunks(unterminated), expected);
}

TEST_F(AnthropicHandlerTest, StreamedToolUseIsReconstructed) {
    AnthropicStreamProbe probe;
    std::string sse =
            StreamEvent("message_start", R"({"type":"message_start","message":{"usage":{"input_tokens":40}}})") +
            StreamEvent("content_block_start", R"({"type":"content_block_start","index":0,"content_block":{"type":"tool_use","id":"toolu_1","name":"flock_response","input":{}}})") +
            StreamEvent("content_block_delta", R"({"type":"content_block_delta","index":0,"delta":{"type":"input_json_delta","partial_json":""}})") +
            StreamEvent("content_block_delta", R"({"type":"content_block_delta","index":0,"delta":{"type":"input_json_delta","partial_json":"{\"items\": "}})") +
            StreamEvent("content_block_delta", R"({"type":"content_block_delta","index":0,"delta":{"type":"input_json_delta","partial_json":"\"only\"}"}})") +
            StreamEvent("message_delta", R"({"type":"message_delta","delta":{"stop_reason":"tool_use"},"usage":{"output_tokens":7}})") +
            StreamEvent("message_stop", R"({"type":"message_stop"})");
    auto result = probe.ReconstructFromStreamedChunks(sse);

    ASSERT_EQ(result["content"].size(), 1);
    EXPECT_EQ(result["content"][0]["type"], "tool_use");
    EXPECT_EQ(result["content"][0]["name"], "flock_response");
    EXPECT_EQ(result["content"][0]["input"]["items"], json::array({"only"}));
    EXPECT_EQ(result["stop_reason"], "tool_use");
    EXPECT_EQ(result["usage"]["input_tokens"], 40);
    EXPECT_EQ(result["usage"]["output_tokens"], 7);
}

TEST_F(AnthropicHandlerTest, StreamedThinkingIsSkipped) {
    AnthropicStreamProbe probe;
    std::string sse =
            StreamEvent("content_block_delta", R"({"type":"content_block_delta","index":0,"delta":{"type":"thinking_delta","thinking":"hmm"}})") +
            StreamEvent("content_block_delta", R"({"type":"content_block_delta","index":1,"delta":{"type":"text_delta","text":"answer"}})") +
            StreamEvent("message_stop", R"({"type":"message_stop"})");
    auto result = probe.ReconstructFromStreamedChunks(sse);

    EXPECT_EQ(result["content"][0]["text"], "answer");
    EXPECT_FALSE(result.contains("usage"));
}

TEST_F(AnthropicHandlerTest, StreamedErrorOrEmptyReturnsNull) {
    AnthropicStreamProbe probe;
    std::string error_sse =
            StreamEvent("content_block_delta", R"({"type":"content_block_delta","index":0,"delta":{"type":"text_delta","text":"partial"}})") +
            StreamEvent("error", R"({"type":"error","error":{"type":"overloaded_error","message":"Overloaded"}})");
    EXPECT_TRUE(probe.ReconstructFromStreamedChunks(error_sse).is_null());

    EXPECT_TRUE(probe.ReconstructFromStreamedChunks("").is_null());
    EXPECT_TRUE(probe.ReconstructFromStreamedChunks(StreamEvent("ping", R"({"type": "ping"})") +
                                                    StreamEvent("message_stop", R"({"type":"message_stop"})"))
                        .is_null());
}

}// namespace flock
