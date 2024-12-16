#include <algorithm>
#include <cstdint>
#include <cstring>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "lib.h"

struct FormatResult {
    enum class Status { Success, Ignored, Error };

    Status status;
    std::string formatted_content;
    std::string error;

    FormatResult(Status s, const std::string &fc = "",
                 const std::string &err = "")
        : status(s), formatted_content(fc), error(err) {}
};

// 動的メモリをWASM側で扱うための関数
extern "C" {
__attribute__((visibility("default"))) uint64_t
foro_malloc(uint64_t size, uint64_t alignment) {
    // 簡易的な実装例(実際にはalignmentに対応する必要がある可能性があります)
    // alignmentは無視している例です
    void *ptr = std::malloc((size_t)size);
    return (uint64_t)ptr;
}

__attribute__((visibility("default"))) void
foro_free(uint64_t ptr, uint64_t size, uint64_t alignment) {
    std::free((void *)ptr);
}
}

static uint8_t *to_array_result(const std::vector<uint8_t> &arr) {
    size_t total_size = 8 + arr.size();
    uint8_t *buffer = (uint8_t *)std::malloc(total_size);
    if (!buffer) {
        throw std::bad_alloc();
    }

    uint64_t len_le = (uint64_t)arr.size();
    for (int i = 0; i < 8; ++i) {
        buffer[i] = (uint8_t)((len_le >> (8 * i)) & 0xFF);
    }

    std::memcpy(buffer + 8, arr.data(), arr.size());

    return buffer;
}

static nlohmann::json foro_main_with_json(const nlohmann::json &input) {
    if (!input.contains("target") || !input["target"].is_string()) {
        return nlohmann::json{
            {"plugin-panic", "Missing or invalid 'target' field"}};
    }
    if (!input.contains("target-content") ||
        !input["target-content"].is_string()) {
        return nlohmann::json{
            {"plugin-panic", "Missing or invalid 'target-content' field"}};
    }

    std::string target = input["target"].get<std::string>();
    std::string target_content = input["target-content"].get<std::string>();

    if (is_ignored(target)) {
        return nlohmann::json{{"format-status", "ignored"}};
    }

    Result r = ::format(target_content, target, "");

    nlohmann::json result;
    if (!r.error) {
        result["format-status"] = "success";
        result["formatted-content"] = r.content;
    } else {
        result["format-status"] = "error";
        result["format-error"] = r.content;
    }

    return result;
}

extern "C" {

__attribute__((visibility("default"))) uint64_t foro_main(uint64_t ptr,
                                                          uint64_t len) {
    // ptrをchar*として扱いJSONを読み込む
    const uint8_t *data = (const uint8_t *)ptr;
    std::string input_str((const char *)data, (size_t)len);

    nlohmann::json v;
    try {
        v = nlohmann::json::parse(input_str);
    } catch (const std::exception &e) {
        // JSONパースエラー
        nlohmann::json err_json = {
            {"plugin-panic", std::string("JSON parse error: ") + e.what()}};
        auto b = nlohmann::json::to_cbor(err_json);
        uint8_t *result_ptr = to_array_result(b);
        return (uint64_t)result_ptr;
    }

    nlohmann::json result_json = foro_main_with_json(v);

    // 結果をJSONでシリアライズ (例ではCBORやJSON文字列など任意フォーマットでOK)
    // Rust側は serde_json::to_vec -> JSONのバイナリ表現 (UTF-8文字列)
    // 同様にUTF-8文字列とする
    std::string result_str = result_json.dump();
    std::vector<uint8_t> result_vec(result_str.begin(), result_str.end());

    uint8_t *result_ptr = to_array_result(result_vec);

    return (uint64_t)result_ptr;
}

} // extern "C"

// dummy main for making happy the compiler
int main() {
    printf("This is a dummy main function.\n");
    return 1;
}
