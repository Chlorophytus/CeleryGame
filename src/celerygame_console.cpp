// Celerygame console objects
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
#include "../include/celerygame_console.hpp"
using namespace celerygame;

static auto all_listeners = std::unique_ptr<console::listeners_t>{nullptr};
static auto current_priority = console::priority::debug;

void console::set_priority(priority p) { current_priority = p; }
console::priority console::get_priority() { return current_priority; }

std::string common_prelude(console::priority p) {
  // Formatter for string outputs
  auto formatter = std::stringstream{};
  formatter << "[";
  // What is the current time?
  auto now = std::chrono::system_clock::now();
  // Convert that to a time_t
  auto now_ti = std::chrono::system_clock::to_time_t(now);
  // Has to be stored in a fancyfied buffer
  char now_buf[64]{0x00};
  std::strftime(now_buf, sizeof(char) * 63, "%T", std::localtime(&now_ti));
  // Milliseconds are good to have
  auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                   now.time_since_epoch())
                   .count() %
               1000;
  // Time for formatting time
  formatter << now_buf;
  formatter << ".";
  formatter << std::setw(3) << std::setfill('0') << milli;
  formatter << std::setw(0) << std::setfill(' ') << "] ";

  switch (p) {
  case console::priority::emergency: {
    formatter << "[ EMG ] ";
    break;
  }
  case console::priority::alert: {
    formatter << "[ ALT ] ";
    break;
  }
  case console::priority::critical: {
    formatter << "[ CRT ] ";
    break;
  }
  case console::priority::error: {
    formatter << "[ ERR ] ";
    break;
  }
  case console::priority::warning: {
    formatter << "[ WRN ] ";
    break;
  }
  case console::priority::notice: {
    formatter << "[ NOT ] ";
    break;
  }
  case console::priority::informational: {
    formatter << "[ INF ] ";
    break;
  }
  case console::priority::debug: {
    formatter << "[ DBG ] ";
    break;
  }
  }
  return formatter.str();
}

void console::log_namespace(std::string &&name,
                            std::function<void(std::string &)> &&block) {
  console::log(console::priority::debug, "entering '", name, "'\n");
  block(name);
  console::log(console::priority::debug, "exiting '", name, "'\n");
}

void console::terminal_listener::prelude(
    std::string &str /**< [in] string to log */,
    console::priority p /**< [in] the severity of the line to output */) {
  str += common_prelude(p);
}

void console::terminal_listener::finalize(std::string &str) {
  std::fprintf(stderr, "%s", str.c_str());
}

console::file_listener::file_listener(
    std::filesystem::path
        &&file_path /**< [in] the path to the log file to write */)
    : _file{std::fopen(file_path.string().c_str(), "w")} {
  if (_file == nullptr) {
    throw std::runtime_error{"Couldn't open log file for write."};
  }
}

void console::file_listener::prelude(
    std::string &str /**< [in] string to log */,
    console::priority p /**< [in] the severity of the line to output */) {
  str += common_prelude(p);
}

void console::file_listener::finalize(std::string &str) {
  std::fprintf(_file, "%s", str.c_str());
}

console::file_listener::~file_listener() { std::fclose(_file); }

void console::init() {
  all_listeners = std::make_unique<console::listeners_t>();
}
console::listeners_t *const console::listeners() { return all_listeners.get(); }
void console::deinit() { all_listeners = nullptr; }
