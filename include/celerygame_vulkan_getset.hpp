// Celerygame Vulkan getters and setters
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
// This is a low-level piece of source code that is only for access/get/set.
// It helps us with global state.

namespace celerygame {
namespace vulkan {
/// Initialize getters and setters
void init();
/// Deinitialize everything in this global state.
void deinit();
namespace window {
SDL_Window *const get();
void set(SDL_Window *);
}
namespace instance {
VkInstance *const get();
void set(VkInstance *);
} // namespace instance
namespace surface {
VkSurfaceKHR *const get();
void set(VkSurfaceKHR *);
} // namespace surface
namespace device {
namespace logical {
VkDevice *const get();
void set(VkDevice *);
} // namespace logical
namespace physical {
std::vector<VkPhysicalDevice> *const access();
} // namespace physical
} // namespace device
namespace swap_chain {
VkSwapchainKHR *const get();
void set(VkSwapchainKHR *);
} // namespace swap_chain
namespace image {
std::vector<VkImage> *const access();
} // namespace image
namespace image_view {
std::vector<VkImageView> *const access();
} // namespace image_view
} // namespace vulkan
} // namespace celerygame
