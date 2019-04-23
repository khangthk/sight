/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQml/dialog/PulseProgressDialog.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <QFutureWatcher>
#include <QGuiApplication>
#include <QString>
#include <QtConcurrent>
#include <QtCore>

fwGuiRegisterMacro(::fwGuiQml::dialog::PulseProgressDialog, ::fwGui::dialog::IPulseProgressDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{
//------------------------------------------------------------------------------

PulseProgressDialog::PulseProgressDialog(::fwGui::GuiBaseObject::Key key)
{
    m_dialog = nullptr;
}

//------------------------------------------------------------------------------

PulseProgressDialog::~PulseProgressDialog()
{
    delete m_dialog;
}

//------------------------------------------------------------------------------

void PulseProgressDialog::setTitle(const std::string& title)
{
    m_title = QString::fromStdString(title);
}

//------------------------------------------------------------------------------

void PulseProgressDialog::setMessage(const std::string& msg)
{
    m_message = QString::fromStdString(msg);
}

//------------------------------------------------------------------------------

void PulseProgressDialog::show()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/dialog/PulseProgressDialog.qml");

    // load the qml ui component
    engine->getRootContext()->setContextProperty("pulseProgressDialog", this);
    m_dialog = engine->createComponent(dialogPath);
    // Create a QFutureWatcher and connect signals and slots.
    QFutureWatcher<void> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(finished()), this, SLOT(onFinished()));
    QObject::connect(this, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));

    Q_EMIT titleChanged();
    Q_EMIT messageChanged();
    // Start the computation.
    QMetaObject::invokeMethod(m_dialog, "open");
    futureWatcher.setFuture(QtConcurrent::run(m_stuff));
    while (!m_isClicked && m_visible)
    {
        qGuiApp->processEvents();
    }
    if (futureWatcher.isRunning())
    {
        Q_EMIT canceled();
    }
}

//------------------------------------------------------------------------------

void PulseProgressDialog::onFinished()
{
    m_visible = false;
    Q_EMIT visibleChanged();
}

//------------------------------------------------------------------------------

void PulseProgressDialog::onCanceled()
{
    m_visible = false;
    Q_EMIT canceled();
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
