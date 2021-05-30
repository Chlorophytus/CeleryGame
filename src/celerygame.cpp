// Celerygame entry point
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
#include "../include/celerygame.hpp"
#include "../include/celerygame_cfg.hpp"
#include "../include/celerygame_console.hpp"
#include "../include/celerygame_runloop.hpp"
#include "../include/celerygame_vulkan.hpp"
int main(int argc, const char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  celerygame::console::init();

  celerygame::console::set_priority(celerygame::console::priority::informational);

  celerygame::console::listeners()->emplace_back(
      new celerygame::console::terminal_listener{});
  celerygame::console::listeners()->emplace_back(
      new celerygame::console::file_listener{"console.log"});

  celerygame::console::log(celerygame::console::priority::notice, "Celerygame ",
                           celerygame_VSTRING_FULL, "\n");

  auto status = EXIT_FAILURE;
  try {
    celerygame::runloop::init();
    celerygame::runloop::tasks()->emplace_front(new celerygame::runloop::task);
    celerygame::vulkan::init("whatever", VK_MAKE_API_VERSION(0, 0, 1, 0), {},
                             {}, 1280, 720, false, true);

    // You must select a swap chain configuration for now.
    auto config = celerygame::vulkan::device::swap_chain_config{};
    config.format = VK_FORMAT_B8G8R8A8_SRGB;
    config.color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    config.present_mode = VK_PRESENT_MODE_FIFO_KHR;
    config.use_globally();

    celerygame::vulkan::try_use_device(0, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, {},
                                       true);

    while (celerygame::runloop::tick()) {
      SDL_Delay(10);
    }

    status = EXIT_SUCCESS;
  } catch (const std::exception &e) {
    celerygame::console::log(celerygame::console::priority::alert,
                             "Fatal error: ", e.what(), "\n");
  }

  celerygame::vulkan::deinit();
  celerygame::runloop::deinit();
  celerygame::console::deinit();
  SDL_Quit();
  return status;
}
/* vim: set ts=2 sw=2 et: */
