// Celerygame run loop handling
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
#include "../include/celerygame_runloop.hpp"
#include "../include/celerygame_console.hpp"
using namespace celerygame;

static auto all_tasks = std::unique_ptr<runloop::tasks_t>{nullptr};

/// Stub for task deletion calls.
runloop::task::~task() {}

void runloop::task::perform() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      _shall_quit = true;
      return;
    }
  }
}

bool runloop::task::should_quit() const { return _shall_quit; }

void runloop::init() {
  console::log(console::priority::notice, "Starting run loop.\n");
  all_tasks = std::make_unique<runloop::tasks_t>();
}

runloop::tasks_t *const runloop::tasks() { return all_tasks.get(); }

bool runloop::tick() {
  if (all_tasks == nullptr) {
    throw std::runtime_error{
        "Tasks must exist. Did you mean to init the tasks infrastructure "
        "beforehand?"};
  }
  auto has_no_tasks = all_tasks->empty();
  if (has_no_tasks) {
    console::log(console::priority::informational,
                 "No tasks left to perform.\n");
    return false;
  }

  // has to be over here, preempt to suppress UB
  all_tasks->remove_if([](auto &&task) { return task->should_quit(); });

  for (auto &&task : *all_tasks) {
    task->perform();
  }

  return true;
}
void runloop::deinit() {
  console::log(console::priority::notice, "Quitting run loop.\n");
  all_tasks = nullptr;
}
