#ifndef FORO_CLANG_FORMA_LIB_H_
#define FORO_CLANG_FORMA_LIB_H_
#include <sstream>
#include <vector>

struct Result {
  bool error;
  std::string content;
};

auto version() -> std::string;
auto format(const std::string str, const std::string assumedFileName,
            const std::string style) -> Result;
auto format_byte(const std::string str, const std::string assumedFileName,
                 const std::string style, const std::vector<unsigned> ranges)
    -> Result;
auto format_line(const std::string str, const std::string assumedFileName,
                 const std::string style, const std::vector<unsigned> ranges)
    -> Result;
auto set_fallback_style(const std::string style) -> void;
auto set_sort_includes(const bool sort) -> void;
auto dump_config(const std::string style, const std::string FileName,
                 const std::string code) -> Result;
auto is_ignored(const std::string path) -> bool;

auto defaultFormatStyle() -> std::string;

#endif