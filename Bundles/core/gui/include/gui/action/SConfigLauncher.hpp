/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_SCONFIGLAUNCHER_HPP_
#define _GUI_ACTION_SCONFIGLAUNCHER_HPP_

#include <fwTools/Failed.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwServices/AppConfigManager.hpp>
#include <fwServices/helper/SigSlotConnection.hpp>
#include <fwActivities/registry/Activities.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace action
{


/**
 * @class   SConfigLauncher
 * @brief   To manage configuration file defines in xml extension.
 * @date    2013.
 *
 * This action works on a ::fwData::Composite. It action starts/stops an AppConfig given by its identifier in this action configuration.
 *  - You can specified pattern to replace in the template configuration by the tag 'replace'.
 *  - You can specified pattern to replace by the uid of the object contained by the composite with the given key
 *  - The pattern GENERIC_UID is replaced by a generated unique identifier when the configuration is launch.
 *  This assure that the created object and services in the configuration have a unique uid even if this
 *  configuration is launch several times.
 *
 * Example of this service configuration
 * @verbatim
   <service impl="::gui::action::SConfigLauncher" type="::fwGui::IActionSrv">
        <config>
            <appConfig id="Visu2DID" >
                <parameters>
                    <parameter replace="SERIESDB" by="medicalData"  />
                    <parameter replace="IMAGE" by="@values.image"  />
                </parameters>
            </appConfig>
        </config>
   </service>
   @endverbatim
 */
class GUI_CLASS_API SConfigLauncher : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SConfigLauncher)(::fwGui::IActionSrv) ) ;

    /// Constructor. Do nothing.
    GUI_API SConfigLauncher() throw() ;

    /// Destructor. Do nothing.
    GUI_API virtual ~SConfigLauncher() throw() ;

    /// Set the action service is activated/disable.
    GUI_API virtual void setIsActive(bool isActive);

protected:

    ///This method launches the IAction::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IAction::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief Management of observations ( overrides ).
     *
     * Stop configuration when it receives "WINDOW_CLOSED" event (ie. close the param view).
     */
    virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    /**
     * @brief Starts the view and initialize the operator.
     */
    virtual void updating() throw(::fwTools::Failed);


    /**
     * @brief Declare the configuration to associate with an operator type and a view config
     *
     * Call the IAction::configuring()
     *
     * Example of this service configuration
     * @verbatim
       <service impl="::gui::action::SConfigLauncher" type="::fwGui::IActionSrv">
           <config>
                <appConfig id="Visu2DID" >
                    <parameters>
                        <parameter replace="SERIESDB" by="medicalData"  />
                        <parameter replace="IMAGE" by="@values.image"  />
                    </parameters>
                </appConfig>
            </config>
       </service>
        @endverbatim
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

    /**
     * @brief Read the configuration and show the parameters view.
     */
    virtual void startConfig();

    /**
     * @brief Close the parameters view.
     */
    virtual void stopConfig();


    /// helper to connect tp config root
    void connectToConfigRoot();

    /// helper to disconnect tp config root
    void disconnectToConfigRoot();

    /// Update the action service executable state.
    void updateExecutableState();

    /// To manage connection to the config root
    ::fwServices::helper::SigSlotConnection::sptr m_connections;

    ::fwActivities::registry::ActivityAppConfig m_appConfig;

    /// to know if AppConfig is running
    bool m_configIsRunning;

    // config manager
    ::fwServices::AppConfigManager::sptr m_appConfigManager;
};

} //action
} // GUI


#endif // _GUI_ACTION_SCONFIGLAUNCHER_HPP_

