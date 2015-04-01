/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/editor/SSlider.hpp"

#include <fwCore/base.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Object.hpp>

#include <fwServices/Base.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QString>
#include <QTime>

#include <math.h>

namespace videoQt
{
namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::videoQt::editor::SSlider, ::fwData::Object );


const ::fwCom::Signals::SignalKeyType SSlider::s_POSITION_CHANGED_SIG = "positionChanged";

const ::fwCom::Slots::SlotKeyType SSlider::s_SET_POSITION_SLIDER_SLOT = "setPositionSlider";
const ::fwCom::Slots::SlotKeyType SSlider::s_SET_DURATION_SLIDER_SLOT = "setDurationSlider";


QString convertMSecToHHMMSS(long long milliseconds)
{
    double hours   = 0;
    double minutes = 0;
    double seconds = 0;

    if (milliseconds > 3600000)
    {
        hours         = floor(milliseconds/3600000.);
        milliseconds -= (hours*3600000);
    }
    if (milliseconds > 60000)
    {
        minutes       = floor(milliseconds/60000.);
        milliseconds -= (minutes*60000);
    }
    if (milliseconds > 1000)
    {
        seconds       = floor(milliseconds/1000.);
        milliseconds -= (seconds*1000);
    }

    QTime time(hours, minutes, seconds, milliseconds);
    return time.toString("hh:mm:ss");

}
//------------------------------------------------------------------------------

SSlider::SSlider() throw() :
    m_sliderPressed(false)
{
    m_slotChangePosition = ::fwCom::newSlot( &SSlider::setPosition, this );
    m_slotChangeDuration = ::fwCom::newSlot( &SSlider::setDuration, this );

    m_sigPositionChanged = PositionChangedSignalType::New();

    ::fwCom::HasSignals::m_signals(s_POSITION_CHANGED_SIG, m_sigPositionChanged);

    ::fwCom::HasSlots::m_slots( s_SET_POSITION_SLIDER_SLOT, m_slotChangePosition)
        ( s_SET_DURATION_SLIDER_SLOT, m_slotChangeDuration);



    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );

}

//------------------------------------------------------------------------------

SSlider::~SSlider() throw()
{
}

//------------------------------------------------------------------------------

void SSlider::starting() throw(::fwTools::Failed)
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QPointer<QHBoxLayout> layout = new QHBoxLayout();
    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);

    QObject::connect(m_positionSlider, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
    QObject::connect(m_positionSlider, SIGNAL(sliderReleased()), this, SLOT(changePosition()));

    m_currentPosition = new QLabel();
    m_currentPosition->setText("--:--:--");

    m_totalDuration = new QLabel();
    m_totalDuration->setText("--:--:--");

    layout->addWidget(m_currentPosition);
    layout->addWidget(m_positionSlider);
    layout->addWidget(m_totalDuration);

    container->setLayout(layout);
    this->updating();
}

//------------------------------------------------------------------------------

void SSlider::stopping() throw(::fwTools::Failed)
{
    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SSlider::configuring() throw(fwTools::Failed)
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SSlider::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SSlider::swapping() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void SSlider::changePosition()
{
    int newPos = m_positionSlider->sliderPosition();
    m_positionSlider->setSliderPosition(newPos);
    m_currentPosition->setText(convertMSecToHHMMSS(newPos));

    // Notify the new position
    ::videoQt::editor::SSlider::PositionChangedSignalType::sptr sig;
    sig = this->signal< ::videoQt::editor::SSlider::PositionChangedSignalType >
              ( ::videoQt::editor::SSlider::s_POSITION_CHANGED_SIG );

    sig->asyncEmit(newPos);

    m_sliderPressed = false;
}

//------------------------------------------------------------------------------

void SSlider::sliderPressed()
{
    m_sliderPressed = true;
}
//------------------------------------------------------------------------------

void SSlider::setPosition(const long long newPos)
{
    if(!m_sliderPressed)
    {
        m_positionSlider->setValue(newPos);
        m_currentPosition->setText(convertMSecToHHMMSS(newPos));
    }
}

//------------------------------------------------------------------------------

void SSlider::setDuration(const long long duration)
{
    m_positionSlider->setRange(0, duration);
    m_totalDuration->setText(convertMSecToHHMMSS(duration));
}

//------------------------------------------------------------------------------

} //namespace editor
} //namespace videoQt

