// Copyright (C) 2024-2025 Aleksey Loginov
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Home page: https://github.com/Just-Queue-it/JustQueueIt/

#include "rest_core.hpp"

#include <libraries/utils/utils.hpp>
#include <rfl/enums.hpp>

namespace rest
{
    std::string_view ParseContentType(rest::ContentType content_type)
    {
        switch (content_type)
        {
        case rest::ContentType::TextPlain: return "text/plain";
        case rest::ContentType::ApplicationJson: return "application/json";
        }
        ENSURE_MSG(false, "Invalid content type");
    }

    std::optional<rest::ContentType> ParseContentType(std::string_view content_type)
    {
        for (auto [_, type] : rfl::get_enumerator_array<rest::ContentType>())
            if (ParseContentType(type) == content_type)
                return type;
        return {};
    }
} // namespace rest
