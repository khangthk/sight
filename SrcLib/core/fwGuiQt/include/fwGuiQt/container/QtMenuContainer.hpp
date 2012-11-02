/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_CONTAINER_QTMENUCONTAINER_HPP_
#define _FWGUIQT_CONTAINER_QTMENUCONTAINER_HPP_

#include <QPointer>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/container/fwMenu.hpp>

#include "fwGuiQt/config.hpp"


QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE


namespace fwGuiQt
{
namespace container
{

/**
 * @brief   Defines the Qt menu bar container for IHM.
 * @class   QtMenuContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API QtMenuContainer : public ::fwGui::container::fwMenu
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QtMenuContainer)(::fwGui::container::fwMenu),
                                            (()),
                                            ::fwGui::factory::New< QtMenuContainer > );

    FWGUIQT_API QtMenuContainer(::fwGui::GuiBaseObject::Key key) throw() ;

    FWGUIQT_API virtual ~QtMenuContainer() throw() ;


    FWGUIQT_API virtual void clean();
    FWGUIQT_API virtual void destroyContainer();

    FWGUIQT_API virtual void setQtMenu(QMenu *menu);
    FWGUIQT_API virtual QMenu* getQtMenu();

private :

    QPointer<QMenu> m_menu;
};

} // namespace container
} // namespace fwGuiQt

#endif /*_FWGUIQT_CONTAINER_QTMENUCONTAINER_HPP_*/


