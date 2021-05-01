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
#include "../include/celerygame_console.hpp"
#include "../include/celerygame_runloop.hpp"
int main(int argc, const char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  celerygame::console::init();

  celerygame::console::listeners()->emplace_back(
      new celerygame::console::terminal_listener{});

  celerygame::runloop::init();

  celerygame::runloop::tasks()->emplace_front(new celerygame::runloop::task{});

  celerygame::console::log(celerygame::console::priority::informational,
                           "Hello world!\n");

  while (celerygame::runloop::tick())
    SDL_Delay(10);

  celerygame::runloop::deinit();
  celerygame::console::deinit();
  SDL_Quit();
  return EXIT_SUCCESS;
}
/* vim: set ts=2 sw=2 et: */
