/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_PATIENTDB_HPP_
#define _FWDATA_PATIENTDB_HPP_

#include <vector>
#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Patient.hpp"

fwCampAutoDeclareDataMacro((fwData)(PatientDB), FWDATA_API);

namespace fwData
{
/**
 * @class   PatientDB
 * @brief   This class defines a patients container.
 *
 * @see     ::fwData::Patient
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API PatientDB : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (PatientDB)(::fwData::Object), (()), ::fwData::factory::New< PatientDB >) ;
    fwCampMakeFriendDataMacro((fwData)(PatientDB));


    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API PatientDB(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~PatientDB();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    typedef std::vector< ::fwData::Patient::sptr > PatientContainerType;

    /**
     * @brief Get/Set patients
     */
    fwDataGetSetCRefMacro(Patients, PatientContainerType);

    /**
     * @brief Get the number of patients
     */
    FWDATA_API PatientContainerType::size_type getNumberOfPatients() const;

    /**
     * @brief add patient
     */
    FWDATA_API void addPatient( ::fwData::Patient::sptr _patient );

protected:

    PatientContainerType m_attrPatients;
};

}//end namespace fwData

#endif // _FWDATA_PATIENTDB_HPP_

