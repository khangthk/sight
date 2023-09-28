/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "SScreenSelector.hpp"

#include <core/com/signal.hxx>

#include <service/macros.hpp>

#include <QApplication>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QInputDialog>
#include <QPointer>
#include <QScreen>

namespace sight::module::ui::viz
{

static const core::com::signals::key_t SCREEN_SELECTED_SIG = "screenSelected";

//------------------------------------------------------------------------------

SScreenSelector::SScreenSelector() :
    m_screenSelectedSig(new_signal<ScreenSelectedSignalType>(SCREEN_SELECTED_SIG))
{
}

//------------------------------------------------------------------------------

SScreenSelector::~SScreenSelector()
= default;

//------------------------------------------------------------------------------

void SScreenSelector::configuring()
{
    this->initialize();

    const auto configTree = this->getConfiguration();

    m_mode = configTree.get("config.<xmlattr>.mode", m_mode);

    SIGHT_ERROR_IF(
        "Unknown selection mode '" + m_mode + "'.",
        m_mode != "current" && m_mode != "neighbor" && m_mode != "select"
    );
}

//------------------------------------------------------------------------------

void SScreenSelector::starting()
{
    this->actionServiceStarting();
}

//------------------------------------------------------------------------------

void SScreenSelector::updating()
{
    int screenNum = -1;
    if(m_mode == "select")
    {
        screenNum = sight::module::ui::viz::SScreenSelector::selectScreen();
    }
    else
    {
        const QDesktopWidget* desktop = QApplication::desktop();

        screenNum = desktop->screenNumber(qApp->activeWindow());

        if(m_mode == "neighbor")
        {
            screenNum++;
        }

        const auto screens = QGuiApplication::screens();
        if(screenNum >= QGuiApplication::screens().count())
        {
            QScreen* screen = QGuiApplication::primaryScreen();
            auto it         = std::ranges::find(screens, screen);
            screenNum = it - screens.begin();
        }
    }

    if(screenNum >= 0)
    {
        m_screenSelectedSig->async_emit(screenNum);
    }
}

//------------------------------------------------------------------------------

void SScreenSelector::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

int SScreenSelector::selectScreen()
{
    QStringList screenNames;
    int screenNumber = 0;

    if(QGuiApplication::screens().size() <= 1)
    {
        return 0;
    }

    for(QScreen* screen : QGuiApplication::screens())
    {
        const QString numberStr = QString::number(screenNumber++) + ".";

        // Compute the screen's diagonal length in inches.
        constexpr qreal inchesPerMillimeter = 0.03937008;
        const auto screenSize               = screen->physicalSize();
        const qreal diagonalLengthMm        = std::sqrt(
            screenSize.width() * screenSize.width()
            + screenSize.height() * screenSize.height()
        );
        const qreal diagonalLengthInches = diagonalLengthMm * inchesPerMillimeter;

        const QString diagonal = QString::number(diagonalLengthInches, 'f', 1) + "\"";

        const auto geom          = screen->geometry();
        const QString resolution = "[" + QString::number(geom.width()) + "x" + QString::number(geom.height()) + "]";

        QString displayName = screen->manufacturer() + " " + screen->model();
        if(displayName.size() == 1)
        {
            displayName = screen->name();
        }

        screenNames << numberStr + " " + diagonal + " " + resolution + " " + displayName;
    }

    bool okClicked       = false;
    QString selectedItem = QInputDialog::getItem(
        nullptr,
        "Select a screen.",
        "Screen:",
        screenNames,
        0,
        false,
        &okClicked
    );

    std::int64_t retScreen = -1;
    if(okClicked)
    {
        auto nameIt = std::find(screenNames.cbegin(), screenNames.cend(), selectedItem);

        if(nameIt != screenNames.cend())
        {
            retScreen = std::distance(screenNames.cbegin(), nameIt);
        }
    }

    return static_cast<int>(retScreen);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
