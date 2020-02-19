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

#include <string_view>

namespace oead::util {

template <typename Callback>
inline void SplitStringByLine(std::string_view data, Callback cb) {
  size_t line_start_pos = 0;
  while (line_start_pos < data.size()) {
    const auto line_end_pos = data.find('\n', line_start_pos);
    if (line_start_pos != line_end_pos) {
      auto line = data.substr(line_start_pos, line_end_pos - line_start_pos);
      if (!line.empty() && line.back() == '\r')
        line.remove_suffix(1);
      cb(line);
    }

    if (line_end_pos == std::string_view::npos)
      break;
    line_start_pos = line_end_pos + 1;
  }
}

}  // namespace oead::util
