// Celerygame Vulkan instance singleton
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
#include "../include/celerygame_vulkan_instance.hpp"
#include "../include/celerygame_cfg.hpp"
#include "../include/celerygame_console.hpp"
#include "../include/celerygame_vulkan_utils.hpp"
using namespace celerygame;

static auto debug_messenger_ptr =
    std::unique_ptr<VkDebugUtilsMessengerEXT>{nullptr};

/// The debug messenger callback for logging Vulkan information to stdout.
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

void vulkan::instance::init(
    std::string &&app_name, U32 app_version, bool debug,
    std::vector<const char *> &&layers_requested = {},
    std::vector<const char *> &&extensions_requested = {}) {
  console::log_namespace("vulkan::instance::init", [&app_name, &app_version,
                                                    &debug, &layers_requested,
                                                    &extensions_requested](
                                                       auto &name) {
    if (vulkan::instance::get() == nullptr) {
      // APPLICATION INFO
      auto application_info = VkApplicationInfo{};
      application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      application_info.pNext = nullptr;
      application_info.pApplicationName = app_name.c_str();
      application_info.applicationVersion = app_version;
      application_info.pEngineName = "Celerygame";
      application_info.engineVersion = VK_MAKE_API_VERSION(
          0, celerygame_VMAJOR, celerygame_VMINOR, celerygame_VPATCH);
      application_info.apiVersion = VK_API_VERSION_1_2;

      // INSTANCE CREATE INFO
      auto instance_create_info = VkInstanceCreateInfo{};
      instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      instance_create_info.pNext = nullptr;
      instance_create_info.flags = 0;
      instance_create_info.pApplicationInfo = &application_info;

      auto extensions_count = U32{0};
      auto extensions = std::vector<const char *>{};
      auto layers = std::vector<const char *>{};

      if (!SDL_Vulkan_GetInstanceExtensions(vulkan::window::get(),
                                            &extensions_count, nullptr)) {
        console::log(console::priority::alert, SDL_GetError(), "\n");
      }
      extensions.resize(extensions_count);
      if (!SDL_Vulkan_GetInstanceExtensions(
              vulkan::window::get(), &extensions_count, extensions.data())) {
        console::log(console::priority::alert, SDL_GetError(), "\n");
      }

      for (auto &&extension_req : extensions_requested) {
        extensions.emplace_back(extension_req);
      }
      for (auto &&layer_req : layers_requested) {
        layers.emplace_back(layer_req);
      }
      console::log(console::priority::debug, name,
                   ": Debug facilities: ", debug, "\n");
      if (debug) {
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        layers.emplace_back("VK_LAYER_KHRONOS_validation");
      }
      instance_create_info.ppEnabledExtensionNames = extensions.data();
      instance_create_info.enabledExtensionCount = extensions.size();
      instance_create_info.ppEnabledLayerNames = layers.data();
      instance_create_info.enabledLayerCount = layers.size();
      if (debug) {
        if (debug_messenger_ptr == nullptr) {
          auto debug_create_info = VkDebugUtilsMessengerCreateInfoEXT{};
          debug_messenger_ptr = std::make_unique<VkDebugUtilsMessengerEXT>();

          debug_create_info.sType =
              VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
          debug_create_info.messageSeverity =
              VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
          debug_create_info.messageType =
              VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
          debug_create_info.pfnUserCallback = debug_callback;
          debug_create_info.pUserData = nullptr;
          instance_create_info.pNext = &debug_create_info;

          auto instance_ptr = new VkInstance;
          vkCreateInstance(&instance_create_info, nullptr, instance_ptr);
          vulkan::instance::set(instance_ptr);

          auto vkCreateDebugUtilsMessengerEXT =
              vulkan::utils::procaddr_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                  "vkCreateDebugUtilsMessengerEXT");

          vkCreateDebugUtilsMessengerEXT(*vulkan::instance::get(),
                                         &debug_create_info, nullptr,
                                         debug_messenger_ptr.get());
        } else {
          console::log(
              console::priority::warning, name,
              ": Not going to create more than one Vulkan debug messenger.\n");
        }
      } else {
        auto instance_ptr = new VkInstance;

        vkCreateInstance(&instance_create_info, nullptr, instance_ptr);
        vulkan::instance::set(instance_ptr);
      }
    } else {
      console::log(console::priority::warning, name,
                   ": singleton already exists.\n");
    }
  });
}

void vulkan::instance::deinit() {
  console::log_namespace("vulkan::instance::deinit", [](auto &name) {
    if (debug_messenger_ptr) {
      auto vkDestroyDebugUtilsMessengerEXT =
          vulkan::utils::procaddr_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
              "vkDestroyDebugUtilsMessengerEXT");
      vkDestroyDebugUtilsMessengerEXT(*vulkan::instance::get(),
                                      *debug_messenger_ptr, nullptr);
      debug_messenger_ptr = nullptr;
      console::log(console::priority::debug, name,
                   ": freed the debug messenger\n");
    }
    if (vulkan::instance::get() != nullptr) {
      vkDestroyInstance(*vulkan::instance::get(), nullptr);
      vulkan::instance::set(nullptr);
    } else {
      console::log(console::priority::warning, name,
                   ": can't doublefree a singleton.\n");
    }
  });
}
