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

/// Vulkan queue support list
using queue_support_set_t = std::bitset<3>;

namespace device {
/// Stores a Vulkan physical device, its queue families, and its properties.
class physical_device {
  VkPhysicalDevice _reference;
  VkPhysicalDeviceProperties _properties{};
  VkPhysicalDeviceFeatures _features{};
  std::vector<VkQueueFamilyProperties> _queue_families{};

public:
  /// Creates a physical device representation and initializes its queue family
  /// storage.
  physical_device(VkPhysicalDevice &&);

  /// Gets all Vulkan queue families implemented on this device.
  const std::vector<VkQueueFamilyProperties> &get_queue_families() const;

  /// Gets all Vulkan properties of this device.
  const VkPhysicalDeviceProperties &get_properties() const;

  /// Gets all Vulkan features of this device.
  const VkPhysicalDeviceFeatures &get_features() const;

  /// Gets the reference of this device.
  const VkPhysicalDevice &get_device() const;
};
class logical_device {
  VkDevice _reference;

public:
  /// Creates a logical device.
  logical_device(VkDevice &&);

  /// Attempts to RAII destroy the logical device.
  ~logical_device();
};
} // namespace device

/// Gets the Vulkan instance
VkInstance *const get_instance();

/// Gets the Vulkan surface
VkSurfaceKHR *const get_surface();

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

/// Get a list of physical devices
std::vector<device::physical_device> *const get_physical_devices();

/// Pick a physical device as the logical device.
bool try_use_device(U16, extension_list_t &&, layer_list_t &&, bool);

/// Cleanup Vulkan window singleton's component(s)
void deinit();
} // namespace vulkan
} // namespace celerygame
/* vim: set ts=2 sw=2 et: */
