/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_APPLICATION_HPP__
#define __FWGUIQT_APPLICATION_HPP__

#include <fwCore/base.hpp>

#include <fwGui/Application.hpp>

#include "fwGuiQt/config.hpp"

namespace fwGuiQt
{

/**
 * @brief   Gives access to the qt application part
 * @class   Application
 *
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API Application : public ::fwGui::Application
{

public:

    Application(::fwGui::GuiBaseObject::Key key)
    {
    }

    virtual ~Application()
    {
    }

    /// Tells the application to exit with a returncode
    FWGUIQT_API virtual void exit( int returncode = 0);

};

} // namespace fwGuiQt

#endif /*__FWGUIQT_APPLICATION_HPP__*/


