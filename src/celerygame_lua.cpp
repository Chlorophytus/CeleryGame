// Celerygame Lua handling
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
#include "../include/celerygame_lua.hpp"
#include "../include/celerygame_console.hpp"
#include "../include/celerygame_vulkan_instance.hpp"
#include "../include/celerygame_vulkan_utils.hpp"
#include "../include/celerygame_vulkan_window.hpp"
using namespace celerygame;

static lua_State *L = nullptr;

// =============================================================================
// Lua <-> C calls
// =============================================================================

static int init_vulkan(lua_State *L0) {
  // access "celerygame.app"
  lua_pushstring(L0, "app");
  lua_rawget(L0, 1);
  auto &&app = lua_tostring(L0, -1);
  lua_pop(L0, 1);
  // access "celerygame.vsn"
  lua_pushstring(L0, "vsn");
  lua_rawget(L0, 1);
  auto &&vsn = static_cast<U32>(lua_tointeger(L0, -1));
  lua_pop(L0, 2);

  // init all we need
  celerygame::vulkan::init();
  celerygame::vulkan::window::init(
      app + (" " + celerygame::vulkan::utils::stringify_version_info(vsn)),
      {1280, 720}, false);
  celerygame::vulkan::instance::init(app, vsn, true, {}, {});
  return 0;
}

static int deinit_vulkan(lua_State *L0) {
  lua_pop(L0, 1);
  celerygame::vulkan::instance::deinit();
  celerygame::vulkan::window::deinit();
  celerygame::vulkan::deinit();
  return 0;
}

static int poll_event(lua_State *L0) {
  lua_pop(L0, 1);
  SDL_Event event;
  if(SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      lua_newtable(L0);
      lua_pushstring(L0, "quit");
      lua_setfield(L0, -2, "type");
      return 1;
    default:
      lua_newtable(L0);
      lua_pushnil(L0);
      lua_setfield(L0, -2, "type");
      return 1;
    }
  } else {
    lua_pushnil(L0);
    return 1;
  }
}

// =============================================================================
// Lua state handling
// =============================================================================

void lua::init(std::filesystem::path &&init_file) {
  console::log(console::priority::notice, "Starting Lua runtime.\n");
  L = luaL_newstate();
  luaL_openlibs(L);
  lua_newtable(L);
  lua_setglobal(L, "celerygame");

  // add methods
  lua_getglobal(L, "celerygame");

  lua_pushcfunction(L, &init_vulkan);
  lua_setfield(L, -2, "init_vulkan");
  lua_pushcfunction(L, &deinit_vulkan);
  lua_setfield(L, -2, "deinit_vulkan");
  lua_pushcfunction(L, &poll_event);
  lua_setfield(L, -2, "poll_event");

  auto error_code = luaL_dofile(L, init_file.string().c_str());
  if (error_code != 0) {
    console::log(console::priority::error,
                 "Can't initialize Lua runloop. Error is ",
                 std::to_string(error_code), "\n");
  }
  // call constructor
  lua_pushstring(L, "init_callback");
  lua_rawget(L, 1);
  lua_pcall(L, 0, 0, 0);
}

void lua::scripted_task::perform() {
  // don't exec when we want to quit
  if (!_shall_quit) {
    lua_getglobal(L, "celerygame");
    lua_pushstring(L, "runloop_callback");
    lua_rawget(L, 1);
    if (!lua_pcall(L, 0, 1, 0)) {
      // unsafe when we get an error (lua_pcall != 0) so we wrap
      _shall_quit = lua_toboolean(L, -1);
      lua_pop(L, 1);
    } else {
      console::log(console::priority::error,
                    "The Lua runloop encountered an error.\n");
      _shall_quit = true;
    }
    lua_pop(L, 1);
  }
}

void lua::deinit() {
  console::log(console::priority::notice, "Closing Lua runtime.\n");

  // call destructor
  lua_getglobal(L, "celerygame");
  lua_pushstring(L, "deinit_callback");
  lua_rawget(L, 1);
  lua_pcall(L, 0, 0, 0);

  lua_close(L);
}