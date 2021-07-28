// Celerygame Vulkan utilities
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
#include "celerygame_vulkan_getset.hpp"
namespace celerygame {
namespace vulkan {
namespace utils {
std::string stringify_version_info(U32);

/// Helper for casting Vulkan function pointers from their names.
template <class T> T procaddr_cast(const char *name) {
  auto pfn = reinterpret_cast<T>(vkGetInstanceProcAddr(*vulkan::instance::get(), name));
  if (pfn == nullptr) {
    console::log(console::priority::warning,
                 "procaddr_cast can't find Vulkan function: '", name, "'\n");
  }
  return pfn;
}
} // namespace utils
} // namespace vulkan
} // namespace celerygame
