/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SClientSender.hpp"

#include <core/com/signal.hxx>
#include <core/tools/failed.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/Preferences.hpp>

namespace sight::module::io::igtl
{

//-----------------------------------------------------------------------------

SClientSender::SClientSender()
= default;

//-----------------------------------------------------------------------------

SClientSender::~SClientSender()
= default;

//-----------------------------------------------------------------------------

void SClientSender::configuring()
{
    service::config_t config = this->getConfiguration();

    const ConfigType configIn = config.get_child("in");

    SIGHT_ASSERT(
        "configured group must be 'objects'",
        configIn.get<std::string>("<xmlattr>.group", "") == "objects"
    );

    const auto keyCfg = configIn.equal_range("key");
    for(auto itCfg = keyCfg.first ; itCfg != keyCfg.second ; ++itCfg)
    {
        const service::config_t& attr = itCfg->second.get_child("<xmlattr>");
        const std::string name        = attr.get("deviceName", "Sight");
        m_deviceNames.push_back(name);
    }

    const std::string serverInfo = config.get("server", "");
    if(!serverInfo.empty())
    {
        const std::string::size_type splitPosition = serverInfo.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_hostnameConfig = serverInfo.substr(0, splitPosition);
        m_portConfig     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw core::tools::failed("Server element not found");
    }
}

//-----------------------------------------------------------------------------

void SClientSender::starting()
{
    if(!m_client.isConnected())
    {
        try
        {
            ui::Preferences preferences;
            const auto port     = preferences.delimited_get<std::uint16_t>(m_portConfig);
            const auto hostname = preferences.delimited_get<std::string>(m_hostnameConfig);

            m_client.connect(hostname, port);
            m_sigConnected->async_emit();
        }
        catch(core::exception& ex)
        {
            sight::ui::dialog::message::show("Connection error", ex.what());
            SIGHT_ERROR(ex.what());
            this->slot(service::slots::STOP)->async_run();
        }
    }
}

//-----------------------------------------------------------------------------

void SClientSender::stopping()
{
    try
    {
        if(m_client.isConnected())
        {
            m_client.disconnect();
        }

        m_sigDisconnected->async_emit();
    }
    catch(core::exception& e)
    {
        sight::ui::dialog::message::show("Error", e.what());
        SIGHT_ERROR(e.what());
    }
}

//-----------------------------------------------------------------------------

void SClientSender::sendObject(const data::Object::csptr& obj, const std::size_t index)
{
    SIGHT_ASSERT("No device name associated with object index " << index, index < m_deviceNames.size());

    if(m_client.isConnected())
    {
        m_client.setDeviceNameOut(m_deviceNames[index]);
        m_client.sendObject(obj);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
