// Celerygame Vulkan device management
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
#include "../include/celerygame_vulkan_device.hpp"
using namespace celerygame;

vulkan::device::physical_device::physical_device(VkPhysicalDevice &&reference /**< [in] reference to the physical device that this resource represents */) : _reference{reference} {
  auto queue_family_count = U32{0};
  vkGetPhysicalDeviceQueueFamilyProperties(_reference, &queue_family_count, nullptr);

  _queue_families.resize(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(_reference, &queue_family_count, _queue_families.data());

  vkGetPhysicalDeviceProperties(_reference, &_properties);
  console::log(console::priority::debug, "Enumerated Vulkan device '", _properties.deviceName, "'\n");
}

const std::vector<VkQueueFamilyProperties> &vulkan::device::physical_device::get_queue_families() /**< [out] all queue families implemented on this device */ const {
  return _queue_families;
}
/* vim: set ts=2 sw=2 et: */
