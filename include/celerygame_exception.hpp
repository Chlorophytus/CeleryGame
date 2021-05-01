// Celerygame include for exception handling
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
namespace celerygame {
namespace exception {
/// Thrown upon entering an unrecoverable runtime assertion.
class runtime_assertion_failed : public std::runtime_error {};
} // namespace exception
} // namespace celerygame
/* vim: set ts=2 sw=2 et: */
