/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/ui/qt/config.hpp"

#include <data/camera.hpp>

#include <boost/bimap/bimap.hpp>

#include <QVideoFrame>

namespace sight::module::ui::qt::video
{

//-----------------------------------------------------------------------------

using pixel_format_translator_t = boost::bimaps::bimap<QVideoFrame::PixelFormat, enum data::camera::pixel_format>;
extern MODULE_UI_QT_API pixel_format_translator_t pixel_format_translator;

} // namespace sight::module::ui::qt::video
