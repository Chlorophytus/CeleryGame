// Celerygame include for Lua handling
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
#include "celerygame_runloop.hpp"
namespace celerygame {
namespace lua {
/// Initialises the Lua state with an init file
void init(std::filesystem::path &&);

class scripted_task : public runloop::task {
public:
  void perform() override;
};

/// Destroys the Lua state
void deinit();
} // namespace lua
} // namespace celerygame
