/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "SSeriesPuller.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/tools/System.hpp>

#include <data/DicomSeries.hpp>

#include <io/http/exceptions/Base.hpp>
#include <io/http/helper/Series.hpp>
#include <io/http/Request.hpp>

#include <service/base.hpp>
#include <service/extension/Config.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>
#include <ui/base/Preferences.hpp>

#include <filesystem>

namespace sight::module::io::dicomweb
{

//------------------------------------------------------------------------------

SSeriesPuller::SSeriesPuller() noexcept =
    default;

//------------------------------------------------------------------------------

SSeriesPuller::~SSeriesPuller() noexcept =
    default;

//------------------------------------------------------------------------------

void SSeriesPuller::configuring()
{
    core::runtime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SIGHT_ASSERT("The service module::io::dicomweb::SSeriesPuller must have a \"config\" element.", config);

    bool success = false;

    // Dicom Reader
    std::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SIGHT_ASSERT("It should be a \"dicomReader\" in the module::io::dicomweb::SSeriesPuller config element.", success);

    // Dicom Reader Config
    std::tie(success, m_dicomReaderSrvConfig) = config->getSafeAttributeValue("readerConfig");

    service::IService::ConfigType configuration = this->getConfigTree();
    //Parse server port and hostname
    if(configuration.count("server") != 0U)
    {
        const std::string serverInfo               = configuration.get("server", "");
        const std::string::size_type splitPosition = serverInfo.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_serverHostnameKey = serverInfo.substr(0, splitPosition);
        m_serverPortKey     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw core::tools::Failed("'server' element not found");
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::starting()
{
    // Create temporary SeriesSet
    m_tmp_series_set = data::SeriesSet::New();

    // Create reader
    m_dicomReader = service::add<sight::io::base::service::IReader>(m_dicomReaderType);
    SIGHT_ASSERT(
        "Unable to create a reader of type: \"" + m_dicomReaderType + "\" in module::io::dicomweb::SSeriesPuller.",
        m_dicomReader
    );
    m_dicomReader->setInOut(m_tmp_series_set, sight::io::base::service::s_DATA_KEY);

    if(!m_dicomReaderSrvConfig.empty())
    {
        // Get the config
        core::runtime::ConfigurationElement::csptr readerConfig =
            service::extension::Config::getDefault()->getServiceConfig(
                m_dicomReaderSrvConfig,
                "sight::io::base::service::IReader"
            );

        SIGHT_ASSERT(
            "Sorry, there is no service configuration "
            << m_dicomReaderSrvConfig
            << " for sight::io::base::service::IReader",
            readerConfig
        );

        m_dicomReader->setConfiguration(core::runtime::ConfigurationElement::constCast(readerConfig));
    }

    m_dicomReader->configure();
    m_dicomReader->start();
}

//------------------------------------------------------------------------------

void SSeriesPuller::stopping()
{
    // Stop reader service
    m_dicomReader->stop();
    service::remove(m_dicomReader);
}

//------------------------------------------------------------------------------

void SSeriesPuller::updating()
{
    try
    {
        ui::base::Preferences preferences;
        m_serverPort     = preferences.delimited_get(m_serverPortKey, m_serverPort);
        m_serverHostname = preferences.delimited_get(m_serverHostnameKey, m_serverHostname);
    }
    catch(...)
    {
        // Do nothing
    }

    if(m_isPulling)
    {
        // Display a message to inform the user that the service is already pulling data.
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pulling Series");
        messageBox.setMessage(
            "The service is already pulling data. Please wait until the pulling is done "
            "before sending a new pull request."
        );
        messageBox.setIcon(ui::base::dialog::IMessageDialog::INFO);
        messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
    else
    {
        const auto selectedSeries = m_selectedSeries.lock();
        if(selectedSeries->empty())
        {
            // Display a message to inform the user that there is no series selected.
            sight::ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Pulling Series");
            messageBox.setMessage("Unable to pull series, there is no series selected. ");
            messageBox.setIcon(ui::base::dialog::IMessageDialog::INFO);
            messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        else
        {
            this->pullSeries();
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::pullSeries()
{
    // Catch any errors
    try
    {
        // Clear map of Dicom series being pulled
        m_pullingDicomSeriesMap.clear();

        // Set pulling boolean to true
        m_isPulling = true;

        // Reset Counters
        m_seriesIndex   = 0;
        m_instanceCount = 0;

        const auto selectedSeries = m_selectedSeries.lock();

        // Find which selected series must be pulled
        DicomSeriesContainerType pullSeriesVector;
        DicomSeriesContainerType selectedSeriesVector;

        auto it = selectedSeries->cbegin();
        for( ; it != selectedSeries->cend() ; ++it)
        {
            data::DicomSeries::sptr series = data::DicomSeries::dynamicCast(*it);

            // Check if the series must be pulled
            if(series
               && std::find(
                   m_localSeries.begin(),
                   m_localSeries.end(),
                   series->getSeriesInstanceUID()
               ) == m_localSeries.end())
            {
                // Add series in the pulling series map
                m_pullingDicomSeriesMap[series->getSeriesInstanceUID()] = series;

                pullSeriesVector.push_back(series);
                m_instanceCount += series->numInstances();
            }

            selectedSeriesVector.push_back(series);
        }

        // Pull series
        if(!pullSeriesVector.empty())
        {
            /// GET
            const InstanceUIDContainerType& seriesInstancesUIDs =
                sight::io::http::helper::Series::toSeriesInstanceUIDContainer(pullSeriesVector);
            for(const std::string& seriesInstancesUID : seriesInstancesUIDs)
            {
                // Find Series according to SeriesInstanceUID
                QJsonObject query;
                query.insert("SeriesInstanceUID", seriesInstancesUID.c_str());

                QJsonObject body;
                body.insert("Level", "Series");
                body.insert("Query", query);
                body.insert("Limit", 0);

                /// Url PACS
                const std::string pacsServer("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));

                /// Orthanc "/tools/find" route. POST a JSON to get all Series corresponding to the SeriesInstanceUID.
                sight::io::http::Request::sptr request = sight::io::http::Request::New(
                    pacsServer + "/tools/find"
                );
                QByteArray seriesAnswer;
                try
                {
                    seriesAnswer = m_clientQt.post(request, QJsonDocument(body).toJson());
                }
                catch(sight::io::http::exceptions::HostNotFound& exception)
                {
                    std::stringstream ss;
                    ss << "Host not found:\n"
                    << " Please check your configuration: \n"
                    << "Pacs host name: " << m_serverHostname << "\n"
                    << "Pacs port: " << m_serverPort << "\n";

                    sight::module::io::dicomweb::SSeriesPuller::displayErrorMessage(ss.str());
                    SIGHT_WARN(exception.what());
                }

                QJsonDocument jsonResponse    = QJsonDocument::fromJson(seriesAnswer);
                const QJsonArray& seriesArray = jsonResponse.array();

                const int seriesArraySize = seriesArray.count();
                for(int i = 0 ; i < seriesArraySize ; ++i)
                {
                    const std::string& seriesUID = seriesArray.at(i).toString().toStdString();

                    /// GET all Instances by Series.
                    const std::string& instancesUrl(std::string(pacsServer).append("/series/").append(seriesUID));
                    const QByteArray& instancesAnswer =
                        m_clientQt.get(sight::io::http::Request::New(instancesUrl));
                    jsonResponse = QJsonDocument::fromJson(instancesAnswer);
                    const QJsonObject& jsonObj       = jsonResponse.object();
                    const QJsonArray& instancesArray = jsonObj["Instances"].toArray();

                    const int instancesArraySize = instancesArray.count();
                    for(int j = 0 ; j < instancesArraySize ; ++j)
                    {
                        const std::string& instanceUID = instancesArray.at(j).toString().toStdString();

                        /// GET DICOM Instance file.
                        const std::string instanceUrl(std::string(pacsServer).append("/instances/").append(instanceUID).
                                                      append("/file"));

                        try
                        {
                            m_path = m_clientQt.getFile(sight::io::http::Request::New(instanceUrl));
                        }
                        catch(sight::io::http::exceptions::ContentNotFound& exception)
                        {
                            std::stringstream ss;
                            ss << "Content not found:  \n"
                            << "Unable download the DICOM instance. \n";

                            sight::module::io::dicomweb::SSeriesPuller::displayErrorMessage(ss.str());
                            SIGHT_WARN(exception.what());
                        }

                        // Create dicom folder
                        std::filesystem::path instancePath = m_path.parent_path() / seriesInstancesUID;
                        QDir().mkpath(instancePath.string().c_str());
                        // Move dicom file to the created dicom folder
                        instancePath /= m_path.filename();
                        QFile::rename(m_path.string().c_str(), instancePath.string().c_str());
                        m_path = m_path.parent_path() / seriesInstancesUID;
                    }
                }
            }
        }

        // Read series if there is no error
        if(m_isPulling)
        {
            this->readLocalSeries(selectedSeriesVector);
        }

        // Set pulling boolean to false
        m_isPulling = false;
    }
    catch(sight::io::http::exceptions::Base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        sight::module::io::dicomweb::SSeriesPuller::displayErrorMessage(ss.str());
        SIGHT_WARN(exception.what());
        m_isPulling = false;
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::readLocalSeries(DicomSeriesContainerType selectedSeries)
{
    const auto dest_series_set = m_series_set.lock();

    const auto scoped_emitter = dest_series_set->scoped_emit();

    // Read only series that are not in the SeriesSet
    const InstanceUIDContainerType& alreadyLoadedSeries =
        sight::io::http::helper::Series::toSeriesInstanceUIDContainer(dest_series_set->get_content());

    for(const auto& series : selectedSeries)
    {
        const std::string& selectedSeriesUID = series->getSeriesInstanceUID();

        // Add the series to the local series vector
        if(std::find(m_localSeries.begin(), m_localSeries.end(), selectedSeriesUID) == m_localSeries.end())
        {
            m_localSeries.push_back(selectedSeriesUID);
        }

        // Check if the series is loaded
        if(std::find(
               alreadyLoadedSeries.cbegin(),
               alreadyLoadedSeries.cend(),
               selectedSeriesUID
           ) == alreadyLoadedSeries.cend())
        {
            // Clear temporary series
            m_tmp_series_set->clear();

            m_dicomReader->setFolder(m_path);
            m_dicomReader->update();

            // Merge series
            std::copy(m_tmp_series_set->cbegin(), m_tmp_series_set->cend(), sight::data::inserter(*dest_series_set));
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::displayErrorMessage(const std::string& message)
{
    SIGHT_WARN("Error: " + message);
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage(message);
    messageBox.setIcon(ui::base::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicomweb
