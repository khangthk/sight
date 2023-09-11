/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "SReader.hpp"

#include <core/com/Signal.hxx>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/System.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <boost/algorithm/string.hpp>

// cspell:ignore sreader xmlattr NVJPEG LIBJPEG OPENJPEG

namespace sight::module::io::bitmap
{

using sight::io::bitmap::Reader;

// Retrieve the backend from the extension
sight::io::bitmap::Backend SReader::findBackend(const std::string& extension) const
{
    const auto& it = std::find_if(
        m_backends.cbegin(),
        m_backends.cend(),
        [&](const auto& backend)
        {
            const auto& backend_extensions = sight::io::bitmap::extensions(backend);

            return std::any_of(
                backend_extensions.cbegin(),
                backend_extensions.cend(),
                [&](const auto& backend_extension)
            {
                return extension.ends_with(backend_extension);
            });
        });

    SIGHT_THROW_IF(
        "No backend found for file extension '" << extension << "'.",
        it == m_backends.cend()
    );

    // Return the backend if found...
    return *it;
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SReader::getIOPathType() const
{
    return sight::io::base::service::IOPathType::FILE;
}

//------------------------------------------------------------------------------

void SReader::openLocationDialog()
{
    static auto defaultLocation = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog locationDialog;

    // Set window title
    if(!m_windowTitle.empty())
    {
        locationDialog.setTitle(m_windowTitle);
    }
    else
    {
        locationDialog.setTitle("Enter file name");
    }

    locationDialog.setDefaultLocation(defaultLocation);
    locationDialog.setOption(ui::base::dialog::ILocationDialog::READ);
    locationDialog.setType(ui::base::dialog::ILocationDialog::SINGLE_FILE);

    // Will be used later to check if "All supported images" is selected
    std::string all_wildcards;

    // If there is at least two backends, add "all" filter
    if(m_backends.size() >= 2)
    {
        for(const auto& backend : m_backends)
        {
            all_wildcards.append(sight::io::bitmap::wildcardFilter(backend).second);
            all_wildcards.append(" ");
        }

        boost::trim(all_wildcards);

        if(!all_wildcards.empty())
        {
            locationDialog.addFilter("All supported images", all_wildcards);
        }
    }

    // Add other filters
    for(const auto& backend : m_backends)
    {
        const auto& [label, wildcard] = sight::io::bitmap::wildcardFilter(backend);
        locationDialog.addFilter(label, wildcard);
    }

    // Show the dialog
    const auto result = std::dynamic_pointer_cast<core::location::SingleFile>(locationDialog.show());

    if(result)
    {
        const auto& file_path = result->getFile();
        setFile(file_path);

        // Get the selected filter
        const auto& current_selection = boost::trim_copy(locationDialog.getCurrentSelection());

        // If "All supported images" is selected, try to guess the backend from the file extension
        if(!all_wildcards.empty() && all_wildcards == current_selection)
        {
            // Use file extension to guess the backend later
            m_selected_backend = sight::io::bitmap::Backend::ANY;
        }
        else
        {
            try
            {
                // Find backend from selected filter
                m_selected_backend = findBackend(locationDialog.getSelectedExtensions().front());
            }
            catch(...)
            {
                // Should normally not happen, but just in case, use file extension as fallback
                m_selected_backend = sight::io::bitmap::Backend::ANY;
            }
        }

        // Save default location for later use
        defaultLocation->setFolder(file_path.parent_path());
        locationDialog.saveDefaultLocation(defaultLocation);

        m_dialog_shown = true;
    }
    else
    {
        clearLocations();
    }
}

//------------------------------------------------------------------------------

void SReader::starting()
{
}

//------------------------------------------------------------------------------

void SReader::stopping()
{
}

//------------------------------------------------------------------------------

void SReader::configuring()
{
    sight::io::base::service::IReader::configuring();

    if(hasLocationDefined())
    {
        m_selected_backend = sight::io::bitmap::Backend::ANY;
    }

    const auto& tree = getConfiguration();

    // Dialog configuration
    if(const auto& dialog_tree = tree.get_child_optional("dialog.<xmlattr>"); dialog_tree.is_initialized())
    {
        m_dialog_policy = stringToDialogPolicy(dialog_tree->get<std::string>("policy", "default"));

        SIGHT_THROW_IF(
            "Cannot read dialog policy.",
            m_dialog_policy == DialogPolicy::INVALID
        );
    }

    // Backend configuration
    if(const auto& backends_tree = tree.get_child_optional("backends"); backends_tree.is_initialized())
    {
        // Remove default configuration
        m_backends.clear();

        if(const auto& enabled = backends_tree->get_optional<std::string>("<xmlattr>.enable");
           enabled.is_initialized() && *enabled == "all")
        {
            // We add everything. Use GPU backend if available
            m_backends.emplace(sight::io::bitmap::Backend::LIBPNG);
            m_backends.emplace(sight::io::bitmap::Backend::LIBTIFF);

#if defined(SIGHT_ENABLE_NVJPEG)
            if(sight::io::bitmap::nvJPEG())
            {
                m_backends.emplace(sight::io::bitmap::Backend::NVJPEG);
            }
            else
 #endif
            {
                m_backends.emplace(sight::io::bitmap::Backend::LIBJPEG);
            }

#if defined(SIGHT_ENABLE_NVJPEG2K)
            if(sight::io::bitmap::nvJPEG2K())
            {
                m_backends.emplace(sight::io::bitmap::Backend::NVJPEG2K);
            }
            else
 #endif
            {
                m_backends.emplace(sight::io::bitmap::Backend::OPENJPEG);
            }
        }
        else if(enabled.is_initialized() && *enabled == "cpu")
        {
            // We add only cpu backends
            m_backends.emplace(sight::io::bitmap::Backend::LIBJPEG);
            m_backends.emplace(sight::io::bitmap::Backend::LIBPNG);
            m_backends.emplace(sight::io::bitmap::Backend::LIBTIFF);
            m_backends.emplace(sight::io::bitmap::Backend::OPENJPEG);
        }
        else if(enabled.is_initialized() && *enabled == "gpu")
        {
            // We add only gpu backends, if possible
#if defined(SIGHT_ENABLE_NVJPEG)
            if(sight::io::bitmap::nvJPEG())
            {
                m_backends.emplace(sight::io::bitmap::Backend::NVJPEG);
            }
#endif

#if defined(SIGHT_ENABLE_NVJPEG2K)
            if(sight::io::bitmap::nvJPEG2K())
            {
                m_backends.emplace(sight::io::bitmap::Backend::NVJPEG2K);
            }
#endif

            SIGHT_THROW_IF("No GPU backend available.", !sight::io::bitmap::nvJPEG() && !sight::io::bitmap::nvJPEG2K());
        }

        // Add hand selected backends
        if(const auto& node = backends_tree->get_child_optional("default"); node.is_initialized())
        {
            m_backends.emplace(sight::io::bitmap::Backend::LIBTIFF);
        }

        if(const auto& node = backends_tree->get_child_optional("libjpeg");
           node.is_initialized()
#if defined(SIGHT_ENABLE_NVJPEG)
           && !m_backends.contains(sight::io::bitmap::Backend::NVJPEG)
#endif
        )
        {
            m_backends.emplace(sight::io::bitmap::Backend::LIBJPEG);
        }

        if(const auto& node = backends_tree->get_child_optional("libpng"); node.is_initialized())
        {
            m_backends.emplace(sight::io::bitmap::Backend::LIBPNG);
        }

        if(const auto& node = backends_tree->get_child_optional("libtiff"); node.is_initialized())
        {
            m_backends.emplace(sight::io::bitmap::Backend::LIBTIFF);
        }

        if(const auto& node = backends_tree->get_child_optional("openjpeg");
           node.is_initialized()
#if defined(SIGHT_ENABLE_NVJPEG2K)
           && !m_backends.contains(sight::io::bitmap::Backend::NVJPEG2K)
#endif
        )
        {
            m_backends.emplace(sight::io::bitmap::Backend::OPENJPEG);
        }

        if(const auto& node = backends_tree->get_child_optional("nvjpeg"); node.is_initialized())
        {
#if defined(SIGHT_ENABLE_NVJPEG)
            if(sight::io::bitmap::nvJPEG())
            {
                m_backends.emplace(sight::io::bitmap::Backend::NVJPEG);

                // Remove libjpeg since we have a better alternative...
                m_backends.erase(sight::io::bitmap::Backend::LIBJPEG);
            }
            else
#endif
            {
                SIGHT_WARN("nvjpeg GPU backend is not available. It will be replaced by libjpeg.");

                if(!m_backends.contains(sight::io::bitmap::Backend::LIBJPEG))
                {
                    m_backends.emplace(sight::io::bitmap::Backend::LIBJPEG);
                }
            }
        }

        if(const auto& node = backends_tree->get_child_optional("nvjpeg2k"); node.is_initialized())
        {
#if defined(SIGHT_ENABLE_NVJPEG2K)
            if(sight::io::bitmap::nvJPEG2K())
            {
                m_backends.emplace(sight::io::bitmap::Backend::NVJPEG2K);

                // Remove openjpeg since we have a better alternative...
                m_backends.erase(sight::io::bitmap::Backend::OPENJPEG);
            }
            else
#endif
            {
                SIGHT_WARN("nvjpeg2k GPU backend is not available. It will be replaced by openJPEG.");

                if(!m_backends.contains(sight::io::bitmap::Backend::OPENJPEG))
                {
                    m_backends.emplace(sight::io::bitmap::Backend::OPENJPEG);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void SReader::updating()
{
    // Set to failed until successful
    m_readFailed = true;

    // Show the save dialog if the path is empty
    if((!hasLocationDefined() && m_dialog_policy != DialogPolicy::NEVER)
       || (!m_dialog_shown && m_dialog_policy == DialogPolicy::ALWAYS))
    {
        openLocationDialog();
    }

    // If the user did not choose a file, we stop here
    if(!hasLocationDefined())
    {
        return;
    }

    // Check if we must add an extension or not
    auto file_path                = getFile();
    const auto& current_extension = file_path.extension().string();

    try
    {
        // Find the backend to use for the current file extension in the list of enabled backends
        const auto& backend_from_extension = findBackend(current_extension);

        // If we can use any backend, we shall use the one found
        if(m_selected_backend == sight::io::bitmap::Backend::ANY)
        {
            m_selected_backend = backend_from_extension;
        }
        else if(m_selected_backend != backend_from_extension)
        {
            // If the user selected a specific backend, it must match the one given by the file extension.
            SIGHT_THROW(
                "Backend mismatch: "
                << std::uint8_t(m_selected_backend)
                << " != "
                << std::uint8_t(backend_from_extension)
            );
        }
    }
    catch(...)
    {
        // If there is no selected backend, use the first available one
        // This could happen if the extension is not supported by any enabled backends
        if(m_selected_backend == sight::io::bitmap::Backend::ANY)
        {
            m_selected_backend = *std::next(m_backends.begin());
        }

        // If we are there, it means that the file extension is not supported or there is no extension.
        // Warn the user and force the extension to be something valid
        const std::string& message =
            (
                current_extension.empty()
                ? "There is no filename extension set"
                : "The selected filename extension '" + current_extension + "' is not valid"
            )
            + " for the selected format '"
            + sight::io::bitmap::wildcardFilter(m_selected_backend).first
            + "' and will be read as '"
            + sight::io::bitmap::extensions(m_selected_backend).front()
            + "'.";

        if(m_dialog_policy != DialogPolicy::NEVER || m_dialog_policy == DialogPolicy::ALWAYS)
        {
            ui::base::dialog::MessageDialog critical_dialog;
            critical_dialog.setIcon(ui::base::dialog::IMessageDialog::CRITICAL);
            critical_dialog.setTitle("Missing or Wrong file extension");
            critical_dialog.setMessage(message + "\n\nWould you like to continue ?");
            critical_dialog.addButton(ui::base::dialog::IMessageDialog::OK);
            critical_dialog.addButton(ui::base::dialog::IMessageDialog::CANCEL);
            critical_dialog.setDefaultButton(ui::base::dialog::IMessageDialog::CANCEL);

            if(critical_dialog.show() == ui::base::dialog::IMessageDialog::CANCEL)
            {
                return;
            }
        }
        else
        {
            SIGHT_WARN(message);
        }
    }

    SIGHT_THROW_IF("The file '" << file_path << "' is an existing folder.", std::filesystem::is_directory(file_path));

    const auto write_job = core::jobs::Job::New(
        "Writing '" + file_path.string() + "' file",
        [&](core::jobs::Job& running_job)
        {
            running_job.doneWork(10);

            // Create the session reader
            auto reader = Reader::New();
            {
                // The object must be unlocked since it will be locked again when writing
                auto data = m_data.lock();
                reader->setObject(data.get_shared());
                reader->setFile(file_path);
            }

            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::base::BusyCursor busyCursor;

            // Read the file
            reader->read(m_selected_backend);

            running_job.done();
        },
        this->worker()
    );

    core::jobs::Aggregator::sptr jobs = core::jobs::Aggregator::New(file_path.string() + " reader");
    jobs->add(write_job);
    jobs->setCancelable(false);

    m_job_created_signal->emit(jobs);

    try
    {
        jobs->run().get();
        m_readFailed = false;
    }
    catch(std::exception& _e)
    {
        // Handle the error.
        SIGHT_ERROR(_e.what());
        sight::ui::base::dialog::MessageDialog::show(
            "Bitmap reader failed",
            _e.what(),
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::base::dialog::MessageDialog::show(
            "Bitmap reader aborted",
            "Reading process aborted",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }

    m_dialog_shown = false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::bitmap