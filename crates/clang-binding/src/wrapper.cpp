#include <clang/Format/Format.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/VirtualFileSystem.h>

// result string is following format:
// - if format success, first byte is '0' and the rest is the formatted code
// - if format failed, first byte is '1' and the rest is the error message
//
// note: build function will free the input content

char* build_success(char* content) {
    char* result = (char*)malloc(strlen(content) + 2);
    result[0] = '0';
    strcpy(result + 1, content);
    free(content);
    return result;
}

char* build_error(char* content) {
    char* result = (char*)malloc(strlen(content) + 2);
    result[0] = '1';
    strcpy(result + 1, content);
    free(content);
    return result;
}

namespace wrapper {

extern "C" char* wrapper_main(char* file_name, char* code) {
    llvm::Expected<clang::format::FormatStyle> style = clang::format::getStyle(clang::format::DefaultFormatStyle, file_name, clang::format::DefaultFallbackStyle);
    if (!style) {
        return build_error(strdup(toString(style.takeError()).c_str()));
    }

    size_t size = strlen(code);

    std::unique_ptr<llvm::MemoryBuffer> code_buf = llvm::MemoryBuffer::getMemBuffer(code);

    std::vector<clang::tooling::Range> ranges;
    ranges.push_back(clang::tooling::Range(0, size));

    clang::tooling::Replacements sort_changes = sortIncludes(*style, code_buf->getBuffer(), ranges, file_name);
    auto changed_code = clang::tooling::applyAllReplacements(code_buf->getBuffer(), sort_changes);
    if (!changed_code) {
        return build_error(strdup(toString(changed_code.takeError()).c_str()));
    }

    size = changed_code->size();
    ranges = { clang::tooling::Range(0, size) };
    clang::tooling::Replacements format_changes = reformat(*style, *changed_code, ranges, file_name);

    auto formatted_code = clang::tooling::applyAllReplacements(*changed_code, format_changes);
    if (!formatted_code) {
        return build_error(strdup(toString(formatted_code.takeError()).c_str()));
    }

    return build_success(strdup(formatted_code->c_str()));
}

}
