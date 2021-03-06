// Copyright (C) 2020  Matthew Glazar
// See end of file for extended copyright information.

#ifndef QUICK_LINT_JS_GMO_H
#define QUICK_LINT_JS_GMO_H

#include <cstddef>
#include <cstdint>
#include <quick-lint-js/constant-divider.h>
#include <string_view>

namespace quick_lint_js {
struct gmo_message;

enum class endian {
  little,
  big,
};

template <endian E>
class native_gmo_file;

// gmo_file gives access to data within a GNU MO file.
//
// A GNU MO file stores translations. It is generated by GNU gettext's msgfmt
// command.
//
// TODO(strager): Make this parser robust. gmo_file currently trusts the input
// and assumes it is valid.
class gmo_file {
 public:
  using offset_type = std::uint32_t;
  using word_type = std::uint32_t;

  explicit gmo_file(const void *data) noexcept;

  word_type string_count() const noexcept;

  std::string_view original_string_at(word_type index) const noexcept;
  std::string_view translated_string_at(word_type index) const noexcept;

  std::string_view find_translation(gmo_message original) const noexcept;

  static constexpr word_type hash_string(std::string_view s) noexcept {
    // This function implements the hashpjw routine documented in:
    //
    // Compilers: Principles, Techniques, and Tools, first edition (1986),
    // by Alfred V. Aho, Monica S. Lam, Ravi Sethi, and Jeffrey D. Ullman;
    // chapter 7 Run-Time Environments,
    // section 6 Symbol Tables,
    // figure 7.35,
    // page 436.

    std::uint32_t hash = 0;
    for (char c : s) {
      hash = (hash << 4) + static_cast<unsigned char>(c);
      std::uint32_t g = hash & 0xf0000000;
      hash ^= (g >> 24);
      hash ^= g;
    }
    return hash;
  }

 private:
  word_type hash_table_size() const noexcept;

  endian get_endian() const noexcept;

  const std::uint8_t *data_;
  constant_divider<word_type> hash_table_size_;
  constant_divider<word_type> hash_table_probe_;

  template <endian>
  friend class native_gmo_file;
};

// An un-translated message.
struct gmo_message {
  std::string_view message;
  gmo_file::word_type hash;

  explicit constexpr gmo_message(const char *raw_message, std::size_t length)
      : message(raw_message, length),
        hash(gmo_file::hash_string(this->message)) {}
};

inline constexpr gmo_message operator""_gmo_message(const char *raw_message,
                                                    std::size_t length) {
  return gmo_message(raw_message, length);
}
}

#endif

// quick-lint-js finds bugs in JavaScript programs.
// Copyright (C) 2020  Matthew Glazar
//
// This file is part of quick-lint-js.
//
// quick-lint-js is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// quick-lint-js is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with quick-lint-js.  If not, see <https://www.gnu.org/licenses/>.
