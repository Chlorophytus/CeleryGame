// Celerygame include for run loop handling
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
namespace runloop {
/// Stub task for time triggered run loop
class task {
protected:
  bool _shall_quit =
      false; /**< Set to true then the next tick deletes this task */
public:
  bool should_quit() const; /**< Will this task be deleted? */
  virtual void perform();   /**< Performs the task */
  virtual ~task();
};

/// A data structure for storing time-triggered tasks (runs per loop cycle)
using tasks_t = std::forward_list<std::unique_ptr<task>>;

/// Initialize the run loop infrastructure
void init();

/// Main run loop tick handler. Returns false if the run loop should quit.
bool tick();

/// Get all tasks in the run loop
tasks_t *const tasks();

/// Cleanup the run loop infrastructure
void deinit();
} // namespace runloop
} // namespace celerygame
/* vim: set ts=2 sw=2 et: */
