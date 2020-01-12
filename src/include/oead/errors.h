/**
 * Copyright (C) 2020 leoetlino
 *
 * This file is part of oead.
 *
 * oead is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * oead is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with oead.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdexcept>

namespace oead {

/// Thrown when an operation is applied to an object of the wrong type.
struct TypeError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

/// Thrown when the data that is passed to a function is invalid or otherwise unusable.
struct InvalidDataError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

}  // namespace oead
