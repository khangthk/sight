/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_PSAFROMPDBSELECTORUPDATERSRV_HPP_
#define _CTRLSELECTION_PSAFROMPDBSELECTORUPDATERSRV_HPP_

#include <fwData/Acquisition.hpp>
#include <fwData/PatientDB.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{
namespace updater
{

/**
 * @class  PSAFromPDBSelectorUpdaterSrv.
 * @brief  Specific updater to update acquisition from a patientDB.
 * @author IRCAD (Research and Development Team).

 * @date    2007-2009.
 */
class CTRLSELECTION_CLASS_API PSAFromPDBSelectorUpdaterSrv : public ::ctrlSelection::IUpdaterSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (PSAFromPDBSelectorUpdaterSrv)(::ctrlSelection::IUpdaterSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API PSAFromPDBSelectorUpdaterSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~PSAFromPDBSelectorUpdaterSrv() throw() ;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configure the services : declare the event to react.
     * @verbatim
            <service uid="myUpdaterAcq" impl="::ctrlSelection::updater::PSAFromPDBSelectorUpdaterSrv" type="::ctrlSelection::IUpdaterSrv" autoConnect="no">
                <update patientKey="selectedPatient" studyKey="selectedStudy" acquisitionKey="myAcquisition" patientNameKey="PatientName" patientInfoKey="PatientInfo" onEvent="NEW_IMAGE_SELECTED" fromUID="patientDB" actionType="ADD_OR_SWAP" />
                <update patientKey="selectedPatient" studyKey="selectedStudy" acquisitionKey="myAcquisition" patientNameKey="PatientName" patientInfoKey="PatientInfo" onEvent="CLEAR_PATIENT" fromUID="patientDB" actionType="REMOVE" />
            </service>
      @endverbatim
     * @see IUpdaterSrv::configureManagedEvents(::fwRuntime::ConfigurationElement::sptr configuration);
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    CTRLSELECTION_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private:

    typedef ::boost::tuple< std::string, std::string, std::string, std::string, std::string, std::string, std::string, ActionType > PSAManagedEvent;
    typedef std::vector < PSAManagedEvent > PSAManagedEvents;

    /// List of the psa managed event
    PSAManagedEvents m_psaManagedEvents;
};

} // updater
} // ctrlSelection

#endif // _CTRLSELECTION_ACQFROMPDBSELECTORUPDATERSRV_HPP_