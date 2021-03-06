//
// Created by Mike Smith on 2021/2/11.
//

#pragma once

#ifdef _MSC_VER
#define LUISA_FORCE_INLINE __forceinline
#define LUISA_NEVER_INLINE __declspec(noinline)
#define LUISA_EXPORT extern "C" __declspec(dllexport)
#else
#define LUISA_FORCE_INLINE [[gnu::always_inline, gnu::hot]] inline
#define LUISA_NEVER_INLINE [[gnu::noinline]]
#define LUISA_EXPORT extern "C" [[gnu::visibility("default")]]
#endif

#if defined(_WINDOWS) || defined(_WIN32) || defined(_WIN64)
#define LUISA_PLATFORM_WINDOWS
#elif defined(__unix__) || defined(__unix) || defined(__APPLE__)
#define LUISA_PLATFORM_UNIX
#endif

#include <string>
#include <vector>
#include <string_view>
#include <filesystem>

namespace luisa {

[[nodiscard]] void *aligned_alloc(size_t alignment, size_t size) noexcept;
void aligned_free(void *p) noexcept;
[[nodiscard]] size_t pagesize() noexcept;

[[nodiscard]] std::string_view dynamic_module_prefix() noexcept;
[[nodiscard]] std::string_view dynamic_module_extension() noexcept;
[[nodiscard]] void *dynamic_module_load(const std::filesystem::path &path) noexcept;
void dynamic_module_destroy(void *handle) noexcept;
[[nodiscard]] void *dynamic_module_find_symbol(void *handle, std::string_view name) noexcept;
[[nodiscard]] std::filesystem::path dynamic_module_path(std::string_view name, const std::filesystem::path &search_path) noexcept;
[[nodiscard]] std::string demangle(const char *name) noexcept;

struct TraceItem {
    std::string module;
    uint64_t address;
    std::string symbol;
    size_t offset;
};

[[nodiscard]] LUISA_NEVER_INLINE std::vector<TraceItem> backtrace() noexcept;

}// namespace luisa
