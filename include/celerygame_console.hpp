// Celerygame include for console objects
//
// Copyright 2021 Roland Metivier <metivier.roland@chlorophyt.us>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once
#include "celerygame.hpp"
namespace celerygame {
namespace console {
/// RFC 5424 log levels
enum class priority : U8 {
  emergency = 0x01,     ///< System is unusable
  alert = 0x02,         ///< Action must be taken immediately
  critical = 0x04,      ///< Critical conditions
  error = 0x08,         ///< Error conditions
  warning = 0x10,       ///< Warning conditions
  notice = 0x20,        ///< Normal but significant condition
  informational = 0x40, ///< Informational messages
  debug = 0x80          ///< Debug-level messages
};

/// Abstract base class for a console listener
class listener {
protected:
  /// Pre-flight routines for console lines, usually prefixes to prepend
  virtual void
  prelude(std::string &str, /**< [in,out] the string to append to */
          priority p /**< [in] the severity of the line to output */) = 0;

  /// Finalization routines for console lines, usually logging to I/O
  virtual void finalize(
      std::string &str /**< [in] the post-flighted string to finalize */) = 0;

  /// End call for stringification
  void stringify(std::string &current /**< [in] the current string*/) {}

  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, std::string head, Ts... more) {
    current += head;
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, const char *head, Ts... more) {
    current += head;
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, bool head, Ts... more) {
    current += (head ? "true" : "false");
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, U8 head, Ts... more) {
    current += std::to_string(head);
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, U16 head, Ts... more) {
    current += std::to_string(head);
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, U32 head, Ts... more) {
    current += std::to_string(head);
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, U64 head, Ts... more) {
    current += std::to_string(head);
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, S8 head, Ts... more) {
    current += std::to_string(head);
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, S16 head, Ts... more) {
    current += std::to_string(head);
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, S32 head, Ts... more) {
    current += std::to_string(head);
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, S64 head, Ts... more) {
    current += std::to_string(head);
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, F32 head, Ts... more) {
    current += std::to_string(head);
    stringify(current, more...);
  }
  /// Stringify an argument
  template <class... Ts>
  void stringify(std::string &current, F64 head, Ts... more) {
    current += std::to_string(head);
    stringify(current, more...);
  }

public:
  template <class... Ts>
  void
  log(priority p, /**< [in] The severity of the line to log */
      Ts... more /**< [in] A parameter pack containing the line's parts */) {
    auto str = std::string{""};

    prelude(str, p);
    stringify(str, more...);
    finalize(str);
  }
};

/// A listener that logs to stderr
class terminal_listener : public listener {
public:
  void prelude(std::string &, priority);
  void finalize(std::string &);
};

/// A data structure for storing all the console listeners currently used
using listeners_t = std::vector<std::unique_ptr<listener>>;

/// Initialize the console logging infrastructure
void init();

/// Cleanup the console logging infrastructure
void deinit();

/// Get a listing of all active console listeners
listeners_t *const listeners();

/// Log to all active console listeners
template <class... Ts>
void log(priority p /**< [in] The severity of the line to log */,
         Ts... more /**< [in] A parameter pack containing the line's parts */) {
  auto listeners_listing = listeners();
  if (listeners_listing == nullptr) {
    throw std::runtime_error{"Listeners should exist."};
  }
  for (auto &&listener : *listeners_listing) {
    listener->log(p, more...);
  }
}
} // namespace console
} // namespace celerygame
/* vim: set ts=2 sw=2 et: */
