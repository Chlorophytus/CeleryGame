// Celerygame Vulkan singleton window management
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
#include "../include/celerygame_vulkan.hpp"
#include "../include/celerygame_vulkan_device.hpp"
#include "../include/celerygame_cfg.hpp"
using namespace celerygame;

static auto instance = std::unique_ptr<VkInstance>{nullptr};
static SDL_Window *window = nullptr;
static auto debug_messenger =
    std::unique_ptr<VkDebugUtilsMessengerEXT>{nullptr};
static auto physical_devices = std::unique_ptr<std::vector<vulkan::device::physical_device>>{nullptr};

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

VkInstance *const vulkan::get_instance() /**< [out] The Vulkan instance */ {
  if (instance == nullptr) {
    console::log(console::priority::warning,
                 "The Vulkan instance is currently null. Are you sure you want "
                 "to do this?\n");
  }
  return instance.get();
}

std::string vulkan::stringify_version_info(
    U32 version /**< [in] A version encoded per Vulkan spec */) {
  return std::string{"v"} + std::to_string(VK_API_VERSION_MAJOR(version)) +
         "." + std::to_string(VK_API_VERSION_MINOR(version)) + "." +
         std::to_string(VK_API_VERSION_PATCH(version));
}

void vulkan::init(
    const char
        *application_name,   /**< [in] Application name to display and encode */
    U32 application_version, /**< [in] Application version encoded per Vulkan
                               spec */
    vulkan::extension_list_t
        &&extensions, /**< [in] list of extensions to attempt to enable */
    vulkan::layer_list_t
        &&layers, /**< [in] list of layers to attempt to enable */
    U16 width, /**< [in] width of window */ U16 height,
    /**< [in] height of window */
    bool fullscreen, /**< [in] window fullscreen if true */
    bool debug /**< [in] enable Vulkan debugging facilities */) {
  if (instance != nullptr) {
    throw std::runtime_error{
        "Can't initialize Vulkan instance twice."};
  }
  if (window != nullptr) {
    throw std::runtime_error{
        "Can't initialize Vulkan window twice."};
  }
  // CREATE A WINDOW
  window = SDL_CreateWindow(
      (std::string{application_name} + std::string{" "} +
       vulkan::stringify_version_info(application_version))
          .c_str(),
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
      SDL_WINDOW_VULKAN | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
  if (window == nullptr) {
    console::log(console::priority::alert, SDL_GetError(), "\n");
    throw std::runtime_error{
        "Can't initialize Vulkan window."};
  }

  // APPLICATION INFO
  auto application_info = VkApplicationInfo{};
  application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application_info.pNext = nullptr;
  application_info.pApplicationName = application_name;
  application_info.applicationVersion = application_version;
  application_info.pEngineName = "Celerygame";
  application_info.engineVersion = VK_MAKE_API_VERSION(0, celerygame_VMAJOR, celerygame_VMINOR, celerygame_VPATCH);
  application_info.apiVersion = VK_API_VERSION_1_2;

  // INSTANCE CREATE INFO
  auto instance_create_info = VkInstanceCreateInfo{};
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pNext = nullptr;
  instance_create_info.flags = 0;
  instance_create_info.pApplicationInfo = &application_info;
  auto layers_passed = std::vector<const char *>{};
  for (auto &&layer : layers) {
    layers_passed.emplace_back(layer);
  }
  auto extensions_required = std::vector<const char *>{};
  auto extensions_required_count = U32{0};
  if (!SDL_Vulkan_GetInstanceExtensions(window,
                                        &extensions_required_count, nullptr)) {
    console::log(console::priority::alert, SDL_GetError(), "\n");
    throw std::runtime_error{
        "Can't gather required Vulkan instance extensions count."};
  }
  extensions_required.resize(extensions_required_count);
  if (!SDL_Vulkan_GetInstanceExtensions(window,
                                        &extensions_required_count,
                                        extensions_required.data())) {
    console::log(console::priority::alert, SDL_GetError(), "\n");
    throw std::runtime_error{
        "Can't gather required Vulkan instance extensions."};
  }
  auto extensions_passed = std::vector<const char *>{};
  // auto extensions_enumerated = std::vector<const char *>{};

  for (auto &&extension : extensions) {
    extensions_passed.emplace_back(extension);
  }
  if (debug) {
    console::log(console::priority::informational,
                 "Enabling Vulkan debug layers and extensions.\n");
    layers_passed.emplace_back("VK_LAYER_KHRONOS_validation");
    extensions_passed.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  for (auto &&extension_required : extensions_required) {
    if (std::none_of(extensions_passed.cbegin(), extensions_passed.cend(),
                     [&extension_required](const char *extension_passed) {
                       return std::strcmp(extension_passed,
                                          extension_required) == 0;
                     })) {
      extensions_passed.emplace_back(extension_required);
    }
  }

#if 0
  if (!std::all_of(extensions_passed.cbegin(), extensions_passed.cend(),
                   [&extensions_enumerated](const char *extension_passed) {
                     return std::any_of(
                         extensions_enumerated.cbegin(),
                         extensions_enumerated.cend(),
                         [&extension_passed](const char *extension_enumerated) {
                           return std::strcmp(extension_passed,
                                              extension_enumerated) == 0;
                         });
                   })) {
    // we couldn't find it
    throw std::runtime_error{
        "Runtime incapable of providing Vulkan extension(s) needed for the "
        "window"};
  }
#endif
  instance_create_info.enabledExtensionCount =
      static_cast<U32>(extensions_passed.size());
  instance_create_info.ppEnabledExtensionNames = extensions_passed.data();
  instance_create_info.enabledLayerCount =
      static_cast<U32>(layers_passed.size());
  instance_create_info.ppEnabledLayerNames = layers_passed.data();
  auto debug_messenger_create = VkDebugUtilsMessengerCreateInfoEXT{};
  if (debug) {
    if (debug_messenger != nullptr) {
      throw std::runtime_error{
          "Vulkan Debug Messenger may not be allocated twice."};
    }
    debug_messenger = std::make_unique<VkDebugUtilsMessengerEXT>();
    debug_messenger_create.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_create.pfnUserCallback = debug_callback;
    debug_messenger_create.pUserData = nullptr;
    instance_create_info.pNext = &debug_messenger_create;
  }

  // Time to create the instance
  instance = std::make_unique<VkInstance>();

  if (vkCreateInstance(&instance_create_info, nullptr, instance.get()) !=
      VK_SUCCESS) {
    throw std::runtime_error{
        "Vulkan instance creation failed."};
  }

  if (debug) {
    auto vkCreateDebugUtilsMessengerEXT =
        vulkan::procaddr_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            "vkCreateDebugUtilsMessengerEXT");
    vkCreateDebugUtilsMessengerEXT(*instance.get(), &debug_messenger_create,
                                   nullptr, debug_messenger.get());
  }

  // Make physical devices
  console::log(console::priority::debug, "Enumerating Vulkan devices...\n");
  auto raw_devices = std::vector<VkPhysicalDevice>{};

  auto raw_device_count = U32{0};
  vkEnumeratePhysicalDevices(*instance.get(), &raw_device_count, nullptr);

  raw_devices.resize(raw_device_count);
  vkEnumeratePhysicalDevices(*instance.get(), &raw_device_count, raw_devices.data());

  physical_devices = std::make_unique<std::vector<vulkan::device::physical_device>>();

  for(auto &&raw_device : raw_devices) {
    physical_devices->emplace_back(std::move(raw_device));
  }
}

void vulkan::deinit() {
  if (instance == nullptr) {
    throw std::runtime_error{
        "Can't destroy Vulkan instance twice."};
  }
  if(window == nullptr) {
    throw std::runtime_error{"Can't destroy Vulkan window twice."};
  }

  if (debug_messenger != nullptr) {
    auto vkDestroyDebugUtilsMessengerEXT =
        vulkan::procaddr_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            "vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(*instance.get(), *debug_messenger.get(), nullptr);
  }
  vkDestroyInstance(*instance.get(), nullptr);
  SDL_DestroyWindow(window);

  instance = nullptr;
}
/* vim: set ts=2 sw=2 et: */
