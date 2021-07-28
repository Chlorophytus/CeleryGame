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
#include "../include/celerygame_lua.hpp"
#include "../include/celerygame_runloop.hpp"
#include "../include/celerygame_vulkan_getset.hpp"
#include "../include/celerygame_vulkan_instance.hpp"
#include "../include/celerygame_vulkan_utils.hpp"
#include "../include/celerygame_vulkan_window.hpp"

int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  celerygame::console::init();

  celerygame::console::set_priority(celerygame::console::priority::debug);

  celerygame::console::listeners()->emplace_back(
      new celerygame::console::terminal_listener{});
  celerygame::console::listeners()->emplace_back(
      new celerygame::console::file_listener{"console.log"});

  celerygame::console::log(celerygame::console::priority::notice, "Celerygame ",
                           celerygame_VSTRING_FULL, "\n");

  auto status = EXIT_FAILURE;
  try {
    celerygame::runloop::init();
    celerygame::runloop::tasks()->emplace_front(
        dynamic_cast<celerygame::runloop::task *>(
            new celerygame::lua::scripted_task));
    celerygame::lua::init(std::filesystem::path{"priv"} / "init.lua");
    // celerygame::vulkan::init();
    // celerygame::vulkan::window::init(
    //     APP_NAME +
    //         (" " + celerygame::vulkan::utils::stringify_version_info(APP_VERS)),
    //     {1280, 720}, false);
    // celerygame::vulkan::instance::init(APP_NAME, APP_VERS, true, {}, {});

    while (celerygame::runloop::tick()) {
      SDL_Delay(10);
    }

    status = EXIT_SUCCESS;
  } catch (const std::exception &e) {
    celerygame::console::log(celerygame::console::priority::alert,
                             "Fatal error: ", e.what(), "\n");
  }

  // celerygame::vulkan::instance::deinit();
  // celerygame::vulkan::window::deinit();
  // celerygame::vulkan::deinit();
  celerygame::lua::deinit();
  celerygame::runloop::deinit();
  celerygame::console::deinit();
  SDL_Quit();
  return status;
}
