// Celerygame Vulkan getters/setters
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
#include "../include/celerygame_vulkan_getset.hpp"
#include "../include/celerygame_console.hpp"
using namespace celerygame;

// This is where we store the objects

static SDL_Window *window_cptr = nullptr;
static auto instance_ptr = std::unique_ptr<VkInstance>{nullptr};
// NOTE: Surfaces are pointers only.
static auto surface_ptr = std::unique_ptr<VkSurfaceKHR>{nullptr};
static auto logical_device_ptr = std::unique_ptr<VkDevice>{nullptr};
static auto physical_devices_ptr =
    std::unique_ptr<std::vector<VkPhysicalDevice>>{nullptr};
static auto swap_chain_ptr = std::unique_ptr<VkSwapchainKHR>{nullptr};
static auto images_ptr = std::unique_ptr<std::vector<VkImage>>{nullptr};
static auto image_views_ptr =
    std::unique_ptr<std::vector<VkImageView>>{nullptr};
// And this is where we store the functions themselves.

void celerygame::vulkan::init() {
  console::log_namespace("vulkan::init", [](auto &name) {
    physical_devices_ptr = std::make_unique<std::vector<VkPhysicalDevice>>();
    images_ptr = std::make_unique<std::vector<VkImage>>();
    image_views_ptr = std::make_unique<std::vector<VkImageView>>();
  });
}

// Accessors

std::vector<VkPhysicalDevice> *const
celerygame::vulkan::device::physical::access() {
  return physical_devices_ptr.get();
}
std::vector<VkImage> *const celerygame::vulkan::image::access() {
  return images_ptr.get();
}
std::vector<VkImageView> *const celerygame::vulkan::image_view::access() {
  return image_views_ptr.get();
}

// Getters

SDL_Window *const celerygame::vulkan::window::get() { return window_cptr; }
VkInstance *const celerygame::vulkan::instance::get() {
  return instance_ptr.get();
}
VkSurfaceKHR *const celerygame::vulkan::surface::get() {
  return surface_ptr.get();
}
VkDevice *const celerygame::vulkan::device::logical::get() {
  return logical_device_ptr.get();
}
VkSwapchainKHR *const celerygame::vulkan::swap_chain::get() {
  return swap_chain_ptr.get();
}

// Setters

void celerygame::vulkan::window::set(SDL_Window *ptr) {
  console::log_namespace("vulkan::window::set",
                         [&ptr](auto &name) { window_cptr = ptr; });
}
void celerygame::vulkan::instance::set(VkInstance *ptr) {
  console::log_namespace("vulkan::instance::set",
                         [&ptr](auto &name) { instance_ptr.reset(ptr); });
}
void celerygame::vulkan::surface::set(VkSurfaceKHR *ptr) {
  console::log_namespace("vulkan::surface::set",
                         [&ptr](auto &name) { surface_ptr.reset(ptr); });
}
void celerygame::vulkan::device::logical::set(VkDevice *ptr) {
  console::log_namespace("vulkan::device::logical::set",
                         [&ptr](auto &name) { logical_device_ptr.reset(ptr); });
}
void celerygame::vulkan::swap_chain::set(VkSwapchainKHR *ptr) {
  console::log_namespace("vulkan::swap_chain::set",
                         [&ptr](auto &name) { swap_chain_ptr.reset(ptr); });
}

void celerygame::vulkan::deinit() {
  console::log_namespace("vulkan::deinit", [](auto &name) {
    std::for_each(vulkan::image_view::access()->begin(),
                  vulkan::image_view::access()->end(), [](auto &&image_view) {
                    vkDestroyImageView(*vulkan::device::logical::get(),
                                       image_view, nullptr);
                  });
    console::log(console::priority::debug, name,
                 ": will delete vectors now.\n");
    physical_devices_ptr = nullptr;
    image_views_ptr = nullptr;
    images_ptr = nullptr;
  });
}
