// Celerygame include for Vulkan device management
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
#include "celerygame_vulkan.hpp"
namespace celerygame {
namespace vulkan {
namespace device {
/// Stores a Vulkan physical device, its queue families, and its properties.
class physical_device {
  VkPhysicalDevice _reference;
  VkPhysicalDeviceProperties _properties{};
  std::vector<VkQueueFamilyProperties> _queue_families{};
public:
  /// Creates a physical device representation and initializes its queue family storage.
  physical_device(VkPhysicalDevice &&);

  /// Gets all Vulkan queue families implemented on this device.
  const std::vector<VkQueueFamilyProperties> &get_queue_families() const;
};
}
} // namespace vulkan
} // namespace celerygame
/* vim: set ts=2 sw=2 et: */
