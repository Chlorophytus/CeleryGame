// Celerygame Vulkan window singleton
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
#include "../include/celerygame_vulkan_window.hpp"
#include "../include/celerygame_cfg.hpp"
#include "../include/celerygame_console.hpp"
using namespace celerygame;

void vulkan::window::init(std::string &&window_name, VkExtent2D extents,
                          bool fullscreen) {
  console::log_namespace("vulkan::window::init", [&window_name, &extents,
                                                  &fullscreen](auto &name) {
    if (vulkan::window::get() == nullptr) {
      vulkan::window::set(SDL_CreateWindow(
          window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
          extents.width, extents.height,
          SDL_WINDOW_VULKAN | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0)));
    } else {
      console::log(console::priority::warning, name,
                   ": singleton already exists.\n");
    }
  });
}

void vulkan::window::deinit() {
  console::log_namespace("vulkan::window::deinit", [](auto &name) {
    if (vulkan::window::get() != nullptr) {
      SDL_DestroyWindow(vulkan::window::get());
      vulkan::window::set(nullptr);
    } else {
      console::log(console::priority::warning, name,
                   ": can't doublefree a singleton.\n");
    }
  });
}
