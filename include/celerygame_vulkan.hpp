// Celerygame include for Vulkan window singleton management
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
#include "celerygame_console.hpp"
namespace celerygame {
namespace vulkan {
/// Vulkan layer name list
using layer_list_t = std::forward_list<const char *>;

/// Vulkan extension name list
using extension_list_t = std::forward_list<const char *>;

/// Gets the Vulkan instance
VkInstance *const get_instance();

/// Look up a Vulkan function and cast it. Dangerous.
template <class T /**< [in] Vulkan function PFN definition */>
T procaddr_cast(
    const char *
        name /**< [in] Vulkan function to cast using vkGetInstanceProcAddr */) /**< [out] The function pointer */
{
  auto pfn = reinterpret_cast<T>(vkGetInstanceProcAddr(*get_instance(), name));
  if (pfn == nullptr) {
    console::log(console::priority::warning,
                 "procaddr_cast can't find Vulkan function: '", name, "'\n");
  }
  return pfn;
}

/// Stringify a Vulkan-encoded version
std::string stringify_version_info(U32);

/// Initialize Vulkan window singleton's component(s)
void init(const char *, U32, extension_list_t &&, layer_list_t &&, U16, U16,
          bool, bool);

/// Cleanup Vulkan window singleton's component(s)
void deinit();
} // namespace vulkan
} // namespace celerygame
/* vim: set ts=2 sw=2 et: */
