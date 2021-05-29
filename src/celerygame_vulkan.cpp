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
#include "../include/celerygame_cfg.hpp"
using namespace celerygame;

static auto instance = std::unique_ptr<VkInstance>{nullptr};
static SDL_Window *window = nullptr;
static VkSurfaceKHR surface;

static auto debug_messenger =
    std::unique_ptr<VkDebugUtilsMessengerEXT>{nullptr};

static auto physical_devices =
    std::unique_ptr<std::vector<vulkan::device::physical_device>>{nullptr};

static auto logical_device =
    std::unique_ptr<vulkan::device::logical_device>{nullptr};

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

VkSurfaceKHR *const vulkan::get_surface() /**< [out] The Vulkan surface */ {
  return &surface;
}

vulkan::device::logical_device::logical_device(
    VkDevice &&reference
    /**< [in] reference to the logical device that this resource represents */)
    : _reference{reference} {}

vulkan::device::physical_device::physical_device(
    VkPhysicalDevice &&reference
    /**< [in] reference to the physical device that this resource represents */)
    : _reference{reference} {
  auto queue_family_count = U32{0};
  vkGetPhysicalDeviceQueueFamilyProperties(_reference, &queue_family_count,
                                           nullptr);

  _queue_families.resize(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(_reference, &queue_family_count,
                                           _queue_families.data());

  vkGetPhysicalDeviceProperties(_reference, &_properties);
  vkGetPhysicalDeviceFeatures(_reference, &_features);

  console::log(console::priority::debug, "Enumerated Vulkan device '",
               _properties.deviceName, "'\n");
}

vulkan::device::logical_device::~logical_device() {
  vkDestroyDevice(_reference, nullptr);
}

const std::vector<VkQueueFamilyProperties> &
vulkan::device::physical_device::get_queue_families() /**< [out] all queue
                                                         families implemented on
                                                         this device */
    const {
  return _queue_families;
}

const VkPhysicalDeviceProperties &
vulkan::device::physical_device::get_properties() /**< [out] all properties of
                                                         this device */
    const {
  return _properties;
}

const VkPhysicalDeviceFeatures &
vulkan::device::physical_device::get_features() /**< [out] all features of
                                                         this device */
    const {
  return _features;
}

const VkPhysicalDevice &
vulkan::device::physical_device::get_device() /**< [out] the device */
    const {
  return _reference;
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
    throw std::runtime_error{"Can't initialize Vulkan instance twice."};
  }
  if (window != nullptr) {
    throw std::runtime_error{"Can't initialize Vulkan window twice."};
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
    throw std::runtime_error{"Can't initialize Vulkan window."};
  }

  // APPLICATION INFO
  auto application_info = VkApplicationInfo{};
  application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application_info.pNext = nullptr;
  application_info.pApplicationName = application_name;
  application_info.applicationVersion = application_version;
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
  auto layers_passed = std::vector<const char *>{};
  for (auto &&layer : layers) {
    layers_passed.emplace_back(layer);
  }
  auto extensions_required = std::vector<const char *>{};
  auto extensions_required_count = U32{0};
  if (!SDL_Vulkan_GetInstanceExtensions(window, &extensions_required_count,
                                        nullptr)) {
    console::log(console::priority::alert, SDL_GetError(), "\n");
    throw std::runtime_error{
        "Can't gather required Vulkan instance extensions count."};
  }
  extensions_required.resize(extensions_required_count);
  if (!SDL_Vulkan_GetInstanceExtensions(window, &extensions_required_count,
                                        extensions_required.data())) {
    console::log(console::priority::alert, SDL_GetError(), "\n");
    throw std::runtime_error{
        "Can't gather required Vulkan instance extensions."};
  }
  auto extensions_passed = std::vector<const char *>{};

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
    throw std::runtime_error{"Vulkan instance creation failed."};
  }

  if (debug) {
    auto vkCreateDebugUtilsMessengerEXT =
        vulkan::procaddr_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            "vkCreateDebugUtilsMessengerEXT");
    vkCreateDebugUtilsMessengerEXT(*instance.get(), &debug_messenger_create,
                                   nullptr, debug_messenger.get());
  }

  // Create surface
  if (!SDL_Vulkan_CreateSurface(window, *instance.get(), &surface)) {
    console::log(console::priority::alert, SDL_GetError(), "\n");
    throw std::runtime_error{"Can't create Vulkan surface."};
  }

  // Make physical devices
  console::log(console::priority::debug, "Enumerating Vulkan devices...\n");
  auto raw_devices = std::vector<VkPhysicalDevice>{};

  auto raw_device_count = U32{0};
  vkEnumeratePhysicalDevices(*instance.get(), &raw_device_count, nullptr);

  raw_devices.resize(raw_device_count);
  vkEnumeratePhysicalDevices(*instance.get(), &raw_device_count,
                             raw_devices.data());

  physical_devices =
      std::make_unique<std::vector<vulkan::device::physical_device>>();

  for (auto &&raw_device : raw_devices) {
    physical_devices->emplace_back(std::move(raw_device));
  }

  console::log(console::priority::informational, "Engine is started.\n");
}

std::vector<vulkan::device::physical_device> *const
vulkan::get_physical_devices() {
  if (physical_devices == nullptr) {
    console::log(
        console::priority::warning,
        "The physical device list is currently null. Are you sure you want "
        "to do this?\n");
  }
  return physical_devices.get();
}

bool vulkan::try_use_device(
    U16 offset,
    /**< [in] Offset from the first device in the physical devices list */
    vulkan::extension_list_t &&extensions, /**< [in] device extensions */
    vulkan::layer_list_t &&layers /**< [in] device layers **/,
    bool debug /**< [in] enable vulkan debugging facilities */)
/**< [out] True if device will be used, otherwise false. */ {
  auto &&device = physical_devices->at(offset);
  auto &&device_ref = device.get_device();

  auto &&device_properties = device.get_properties();
  auto &&device_features = device.get_features();
  auto &&device_queue_families = device.get_queue_families();
  auto &&device_queues_map = std::map<U16, queue_support_set_t>{};

  auto usable_extensions_count = U32{0};
  auto usable_extensions = std::vector<VkExtensionProperties>{};
  // auto usable_extensions_text = std::vector<const char *>{};
  auto extension_test = std::vector<bool>{};

  auto wanted_layers = std::vector<const char *>{};
  for (auto &&layer : layers) {
    wanted_layers.emplace_back(layer);
  }
  auto wanted_extensions = std::vector<const char *>{};
  for (auto &&extension : extensions) {
    wanted_extensions.emplace_back(extension);
  }

  if (debug) {
    console::log(console::priority::informational,
                 "Enabling Vulkan device debug layers and extensions.\n");
    wanted_layers.emplace_back("VK_LAYER_KHRONOS_validation");
  }

  console::log(console::priority::debug, "Trying to pick '",
               device_properties.deviceName, "' at devices[", offset, "]\n");

  if (vkEnumerateDeviceExtensionProperties(device_ref, nullptr,
                                           &usable_extensions_count,
                                           nullptr) == VK_SUCCESS) {
    usable_extensions.resize(usable_extensions_count);
    // for (auto &&usable_extension : usable_extensions) {
    //   usable_extensions_text.emplace_back(usable_extension.extensionName);
    // }
    if (vkEnumerateDeviceExtensionProperties(
            device_ref, nullptr, &usable_extensions_count,
            usable_extensions.data()) == VK_SUCCESS) {
      for (auto &wanted : extensions) {
        extension_test.emplace_back(
            std::find_if(usable_extensions.cbegin(), usable_extensions.cend(),
                         [&wanted](const auto &usable) {
                           return std::strcmp(usable.extensionName, wanted) ==
                                  0;
                         }) != usable_extensions.cend());
        console::log(console::priority::debug, "Device extension usable? '",
                     wanted, "': ", extension_test.back(), "\n");
      }
    } else {
      console::log(console::priority::warning,
                   "Can't get device extensions, continuing\n");
    }
  } else {
    console::log(console::priority::warning,
                 "Can't get device extension count, continuing\n");
  }

  auto iter = U16{0};
  for (auto &&device_queue_family : device_queue_families) {
    console::log(console::priority::debug,
                 "Checking for support at queue index ", iter, "...\n");
    auto queue_support_set = queue_support_set_t{};

    queue_support_set.set(0, device_queue_family.queueCount > 0);
    auto presenter = VkBool32{false};
    vkGetPhysicalDeviceSurfaceSupportKHR(device_ref, iter, surface, &presenter);
    queue_support_set.set(1, presenter);
    queue_support_set.set(
        2, (device_queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0);

    device_queues_map.emplace(iter, queue_support_set);
    iter++;
  }

  auto graphics_queues = std::set<U16>{};
  auto presentation_queues = std::set<U16>{};
  // bitfoolery
  for (auto &&candidate : device_queues_map) {
    auto &&bitset = candidate.second;
    if (bitset[0]) {
      if (bitset[2]) {
        graphics_queues.emplace(candidate.first);
      }
      if (bitset[1]) {
        presentation_queues.emplace(candidate.first);
      }
    }
  }

  // HACK: assume the devices are the same.
  auto queues = std::vector<U16>{};
  std::set_intersection(graphics_queues.begin(), graphics_queues.end(),
                        presentation_queues.begin(), presentation_queues.end(),
                        std::inserter(queues, queues.begin()));
  console::log(console::priority::debug, "Checked for set intersection...\n");

  if (!queues.empty() &&
      std::all_of(extension_test.begin(), extension_test.end(),
                  [](const auto &v) { return v; })) {
    VkDevice logical_device_temp;

    VkDeviceQueueCreateInfo queue_info;
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.queueFamilyIndex = queue_info.queueCount = 1;
    auto &&queue_info_priorities = std::vector<F32>{1.0f};
    queue_info.pQueuePriorities = queue_info_priorities.data();
    queue_info.pNext = nullptr;

    VkDeviceCreateInfo device_info;
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledLayerCount = static_cast<U32>(wanted_layers.size());
    device_info.ppEnabledLayerNames = wanted_layers.data();
    device_info.enabledExtensionCount =
        static_cast<U32>(wanted_extensions.size());
    device_info.ppEnabledExtensionNames = wanted_extensions.data();
    device_info.pNext = nullptr;
    device_info.pEnabledFeatures = nullptr;

    vkCreateDevice(device_ref, &device_info, nullptr, &logical_device_temp);

    console::log(console::priority::notice, "Using Vulkan device '",
                 device_properties.deviceName, "'.\n");
    logical_device = std::make_unique<vulkan::device::logical_device>(
        std::move(logical_device_temp));
    return true;
  }

  // Our assumption was incorrect.
  console::log(console::priority::warning, "Couldn't pick physical device ",
               offset, "\n");
  return false;
}

void vulkan::deinit() {
  if (surface == nullptr) {
    throw std::runtime_error{"Can't destroy Vulkan surface twice."};
  }
  if (instance == nullptr) {
    throw std::runtime_error{"Can't destroy Vulkan instance twice."};
  }
  if (window == nullptr) {
    throw std::runtime_error{"Can't destroy Vulkan window twice."};
  }
  
  logical_device = nullptr;
  vkDestroySurfaceKHR(*instance.get(), surface, nullptr);

  if (debug_messenger != nullptr) {
    auto vkDestroyDebugUtilsMessengerEXT =
        vulkan::procaddr_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            "vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(*instance.get(), *debug_messenger.get(),
                                    nullptr);
  }

  vkDestroyInstance(*instance.get(), nullptr);
  SDL_DestroyWindow(window);

  instance = nullptr;
}
/* vim: set ts=2 sw=2 et: */
