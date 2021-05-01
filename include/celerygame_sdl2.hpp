// Celerygame include for SDL2 window/event management
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
namespace sdl2 {
/// An SDL2 Vulkan window with a task to handle its events
class window_task : public runloop::task {
  SDL_Window *window = nullptr;

  VkInstance instance;
  VkApplicationInfo application;
  VkInstanceCreateInfo creator;
  VkDebugUtilsMessengerEXT debug;
  VkDebugUtilsMessengerCreateInfoEXT debug_creator;

  U16 width;
  U16 height;
  bool fullscreen;

public:
  bool is_validation_capable() const;
  void perform() override;

  window_task(const std::string & /**< [in] window title */,
              U16 /**< [in] window width */, U16 /**< [in] window height */,
              bool /**< [in] window should be fullscreen? */);
  ~window_task() override;
};
} // namespace sdl2
} // namespace celerygame
/* vim: set ts=2 sw=2 et: */
