/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>
#include <exception>
#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/SelectorDialog.hpp>

#include "uiMeasurement/action/RemoveDistance.hpp"

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv , ::uiMeasurement::action::RemoveDistance , ::fwData::Image ) ;

//------------------------------------------------------------------------------

RemoveDistance::RemoveDistance( ) throw()
{}

//------------------------------------------------------------------------------

RemoveDistance::~RemoveDistance() throw()
{}

//------------------------------------------------------------------------------

void RemoveDistance::info(std::ostream &_sstream )
{
    _sstream << "Action for remove distance" << std::endl;
}
//------------------------------------------------------------------------------

std::string distanceToStr(double dist)
{
    std::stringstream ss;
    ss.precision(3);
    ss << dist << " mm";
    return ss.str();
}

//------------------------------------------------------------------------------

::fwData::PointList::sptr  getDistanceToRemove(::fwData::Image::sptr image, bool &removeAll)
{
    ::fwData::PointList::sptr distToRemove;
    removeAll = false;
    ::fwData::Vector::sptr vectDist;
    vectDist = image->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);

    if(vectDist)
    {
        std::vector< std::string > selections;
        selections.push_back("ALL");
        std::map< std::string , ::fwData::PointList::sptr > correspondance;

        BOOST_FOREACH(::fwData::Object::sptr obj, *vectDist)
        {
            ::fwData::PointList::sptr pl = ::fwData::PointList::dynamicCast(obj);

            if ( pl->getPoints().size()!=2 ) { continue; } // we skip no paired pointList
            ::fwData::Point::sptr pt1 =  pl->getPoints().front();
            ::fwData::Point::sptr pt2 =  pl->getPoints().back();

            double dist=0;
            double delta = pt1->getCRefCoord()[0] - pt2->getCRefCoord()[0];
            dist += delta*delta;
            delta = pt1->getCRefCoord()[1] - pt2->getCRefCoord()[1];
            dist += delta*delta;
            delta = pt1->getCRefCoord()[2] - pt2->getCRefCoord()[2];
            dist += delta*delta;
            dist = sqrt(dist);

            selections.push_back( distanceToStr(dist) );
            correspondance[ selections.back() ] = pl;
        }

        if ( selections.size() )
        {
            ::fwGui::dialog::SelectorDialog::NewSptr selector;
            selector->setTitle("Select a distance to remove");
            selector->setSelections(selections);
            std::string selection = selector->show();
            if( ! selection.empty() )
            {
                if (selection=="ALL")
                {
                    removeAll=true;
                }
                else
                {
                    removeAll=false;
                    distToRemove = correspondance[selection];
                }
            }
        }
    }
    return distToRemove;
}

//------------------------------------------------------------------------------

void RemoveDistance::notifyDeleteDistance(::fwData::Image::sptr image, ::fwData::Object::sptr distance)
{
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::DELETE_DISTANCE, distance );
    ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
}

//------------------------------------------------------------------------------

void RemoveDistance::notifyNewDistance(::fwData::Image::sptr image, ::fwData::Object::sptr backup)
{
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::DISTANCE, backup );
    ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
}

//------------------------------------------------------------------------------

void RemoveDistance::updating( ) throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::Vector::sptr vectDist;
    vectDist = image->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);

    if (::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image)
        && vectDist)
    {
        bool requestAll;
        ::fwData::PointList::sptr distToRemove = getDistanceToRemove(image, requestAll );

        // perform action only available distance
        if ( distToRemove )
        {
            SLM_ASSERT("No Field ImageDistancesId",vectDist);
            ::fwData::Vector::IteratorType newEnd = std::remove(vectDist->begin(), vectDist->end(), distToRemove);
            vectDist->getContainer().erase(newEnd, vectDist->end());

            this->notifyDeleteDistance(image, distToRemove);
        }
        if ( requestAll )
        {
            // backup
            ::fwData::Object::sptr backupDistance = image->getField( ::fwComEd::Dictionary::m_imageDistancesId );

            image->removeField( ::fwComEd::Dictionary::m_imageDistancesId );
            this->notifyNewDistance(image, backupDistance);
        }
    }
}

//------------------------------------------------------------------------------

void RemoveDistance::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void RemoveDistance::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void RemoveDistance::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void RemoveDistance::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMeasurement
