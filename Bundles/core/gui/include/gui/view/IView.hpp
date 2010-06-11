/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_VIEW_IVIEW_HPP_
#define _GUI_VIEW_IVIEW_HPP_

#include <fwServices/IService.hpp>

#include <fwWX/IGuiContainer.hpp>

#include "gui/export.hpp"

namespace gui
{

/**
 * @namespace ::gui::view
 * @brief This namespace contains the classes which define container.
 */
namespace view
{

/**
 * @brief   Defines the service interface managing a view placed in main frame.
 * @class   IView.
 * @author  IRCAD (Research and Development Team).

 * @date    2009.
 *
 */
class GUI_CLASS_API IView : public ::fwWX::IGuiContainer
{

public :

    fwCoreNonInstanciableClassDefinitionsMacro ( (IView)( ::fwServices::IService) ) ;

protected:

    /**
    * @brief Constructor. Do nothing (Just initialize parameters).
    */
    GUI_API IView() throw() ;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~IView() throw() ;

};



} // namespace view
} // namespace gui


#endif /*_GUI_VIEW_IVIEW_HPP_*/
