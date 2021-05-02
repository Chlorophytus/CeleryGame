// Celerygame SDL2 window/event management
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
#include "../include/celerygame_sdl2.hpp"
#include "../include/celerygame_cfg.hpp"
#include "../include/celerygame_console.hpp"
using namespace celerygame;

const static std::vector<const char *> layers_validation = {
    "VK_LAYER_KHRONOS_validation"};
#ifdef NDEBUG
const static bool layers_enable_validation = false;
#else
const static bool layers_enable_validation = true;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
               VkDebugUtilsMessageTypeFlagsEXT type,
               const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
               void *user_data) {
  auto message_type = std::string{};
  if ((type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) != 0) {
    message_type += 'G';
  } else {
    message_type += ' ';
  }
  if ((type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) != 0) {
    message_type += 'V';
  } else {
    message_type += ' ';
  }
  if ((type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) != 0) {
    message_type += 'P';
  } else {
    message_type += ' ';
  }
  if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0) {
    console::log(console::priority::error, "Vulkan [", message_type,
                 "]: ", callback_data->pMessage, "\n");
  } else if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) !=
             0) {
    console::log(console::priority::warning, "Vulkan [", message_type,
                 "]: ", callback_data->pMessage, "\n");
  } else if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) != 0) {
    console::log(console::priority::informational, "Vulkan [", message_type,
                 "]: ", callback_data->pMessage, "\n");
  } else if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) !=
             0) {
    console::log(console::priority::debug, "Vulkan [", message_type,
                 "]: ", callback_data->pMessage, "\n");
  }
  return VK_FALSE;
}

/// Get a listing of all physical Vulkan devices
const std::vector<VkPhysicalDevice> &sdl2::window_task::get_physical_devices() const {
  return physical_devices;
}

/// Set the Vulkan device. Returns false if not capable of rendering.
bool sdl2::window_task::set_physical_device(VkPhysicalDevice &device_ref) {
  auto queue_family_count = U32{0};
  vkGetPhysicalDeviceQueueFamilyProperties(device_ref, &queue_family_count, nullptr);

  auto queue_families = std::vector<VkQueueFamilyProperties>{};
  queue_families.resize(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device_ref, &queue_family_count, queue_families.data());

  console::log(console::priority::debug, "Got device queue families, partitioning them...\n");

  auto true_queue_families_offset = std::stable_partition(queue_families.begin(), queue_families.end(), [](const VkQueueFamilyProperties &queue_family){
    return (queue_family.queueCount > 0) && ((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0);
  });
  
  console::log(console::priority::debug, "Partitioned into ", std::distance(queue_families.begin(), true_queue_families_offset), " good queue families and ", std::distance(true_queue_families_offset, queue_families.end()), " bogus ones.\n");

  device_properties = VkPhysicalDeviceProperties{};
  vkGetPhysicalDeviceProperties(device_ref, &device_properties);

  if(true_queue_families_offset == queue_families.begin()) {
    console::log(console::priority::notice, "Can't use Vulkan device '", device_properties.deviceName, "' because it does not have necessary queue type(s)\n");
    return false;
  }

  console::log(console::priority::notice, "Using Vulkan device '", device_properties.deviceName, "'\n");
  return true;
}

/// Check for Vulkan validation layer support
bool sdl2::window_task::is_validation_capable() const {
  auto layer_count = U32{0};
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  auto layers = std::vector<VkLayerProperties>{};
  layers.resize(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, layers.data());

  return std::all_of(layers_validation.cbegin(), layers_validation.cend(),
                     [&layers](const char *validation_layer) {
                       return std::any_of(
                           layers.cbegin(), layers.cend(),
                           [&validation_layer](const auto &layer) {
                             return strcmp(validation_layer, layer.layerName) ==
                                    0;
                           });
                     });
}

void sdl2::window_task::perform() {
  // use goat task for now, but we'll change it
  runloop::task::perform();
}

sdl2::window_task::window_task(const std::string &title, U16 w, U16 h, bool f) {
  width = w;
  height = h;
  fullscreen = f;

  // Vulkan stuff
  instance = VkInstance{};

  application = VkApplicationInfo{};
  application.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application.pApplicationName = title.c_str();
  application.pEngineName = "Celerygame";
  application.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
  application.engineVersion =
      VK_MAKE_VERSION(celerygame_VMAJOR, celerygame_VMINOR, celerygame_VPATCH);
  application.apiVersion = VK_API_VERSION_1_0;

  creator = VkInstanceCreateInfo{};
  creator.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  creator.pApplicationInfo = &application;

  // Back to window creation
  window = SDL_CreateWindow(
      title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width,
      height, SDL_WINDOW_VULKAN | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
  assert(window != nullptr);

  // Vulkan extension grabbing
  auto extension_count = unsigned{0};
  auto extension_names = std::vector<const char *>{};
  assert(SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr));

  console::log(console::priority::debug, "Need ", extension_count,
               " Vulkan extensions by default.\n");
  extension_names.resize(extension_count);
  assert(SDL_Vulkan_GetInstanceExtensions(window, &extension_count,
                                          extension_names.data()));
  if (layers_enable_validation) {
    console::log(console::priority::informational,
                 "Enabling Vulkan debug facilities\n");
    assert(is_validation_capable());
    extension_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    extension_count++;
  }
  std::for_each(
      extension_names.cbegin(), extension_names.cend(), [](const char *name) {
        console::log(console::priority::debug, "Vulkan EXT: ", name, "\n");
      });
  creator.enabledExtensionCount = extension_count;
  creator.ppEnabledExtensionNames = extension_names.data();

  // Vulkan layers
  if (layers_enable_validation) {
    creator.enabledLayerCount = static_cast<U32>(layers_validation.size());
    creator.ppEnabledLayerNames = layers_validation.data();

    debug = VkDebugUtilsMessengerEXT{};
    debug_creator = VkDebugUtilsMessengerCreateInfoEXT{};
    debug_creator.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_creator.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_creator.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_creator.pfnUserCallback = debug_callback;
    debug_creator.pUserData = nullptr;

    creator.pNext =
        reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT *>(&debug_creator);
  } else {
    creator.enabledLayerCount = 0;
  }

  // Instance creation
  assert(vkCreateInstance(&creator, nullptr, &instance) == VK_SUCCESS);

  // Vulkan debug messenger creation
  if (layers_enable_validation) {
    // Vulkan function pointer
    auto createDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    assert(createDebugUtilsMessengerEXT != nullptr);
    createDebugUtilsMessengerEXT(instance, &debug_creator, nullptr, &debug);
  }

  /// Lazily enumerate Vulkan physical devices
  auto physical_device_count = U32{0};
  vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);
  physical_devices.resize(physical_device_count);
  vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data()) ;

  assert(set_physical_device(*physical_devices.begin()));

  console::log(console::priority::debug, "Created a window.\n");
}

/// Deletes the Vulkan window and its layers/instance/etc.
sdl2::window_task::~window_task() {
  if (layers_enable_validation) {
    // Vulkan function pointer
    auto destroyDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    assert(destroyDebugUtilsMessengerEXT != nullptr);
    destroyDebugUtilsMessengerEXT(instance, debug, nullptr);
  }

  vkDestroyInstance(instance, nullptr);
  SDL_DestroyWindow(window);

  console::log(console::priority::debug, "Destroyed a window.\n");
}
/* vim: set ts=2 sw=2 et: */
