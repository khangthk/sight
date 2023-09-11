/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "ui/qt/config.hpp"
#include "ui/qt/container/QtContainer.hpp"

#include <ui/base/layoutManager/OverlayLayoutManager.hpp>

namespace sight::ui::qt
{

/**
 * @brief   Defines the overlay layout manager.
 */
class UI_QT_CLASS_API OverlayLayoutManager : public ui::base::layoutManager::OverlayLayoutManager
{
public:

    SIGHT_DECLARE_CLASS(
        OverlayLayoutManager,
        ui::base::layoutManager::OverlayLayoutManager
    )

    UI_QT_API OverlayLayoutManager(ui::base::GuiBaseObject::Key key);

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent containers must be instanced.
     */
    UI_QT_API void createLayout(ui::base::container::fwContainer::sptr parent, const std::string& id) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_QT_API void destroyLayout() override;

private:

    ui::qt::container::QtContainer::sptr m_parentContainer;
};

} // namespace sight::ui::qt