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
#include "../include/celerygame_vulkan_utils.hpp"
#include "../include/celerygame_console.hpp"
using namespace celerygame;

/// Given a Vulkan version integer, convert to a semantic versioning identifier.
std::string vulkan::utils::stringify_version_info(U32 version) {
  return std::string{"v"} + std::to_string(VK_API_VERSION_MAJOR(version)) +
         "." + std::to_string(VK_API_VERSION_MINOR(version)) + "." +
         std::to_string(VK_API_VERSION_PATCH(version));
}