/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "ui/base/Preferences.hpp"

#include "ui/base/dialog/InputDialog.hpp"
#include "ui/base/dialog/MessageDialog.hpp"

#include <core/crypto/PasswordKeeper.hpp>
#include <core/crypto/SHA256.hpp>
#include <core/runtime/profile/Profile.hpp>
#include <core/tools/Os.hpp>

#include <io/zip/ArchiveReader.hpp>
#include <io/zip/ArchiveWriter.hpp>
#include <io/zip/exception/Read.hpp>

#include <boost/property_tree/json_parser.hpp>

namespace sight::ui::base
{

using core::crypto::secure_string;
using core::crypto::PasswordKeeper;

// Generate a pseudo random password. Do not move this function has we use __FILE__ and __LINE__ macros
inline static secure_string pseudo_random_password()
{
    const auto profile = core::runtime::getCurrentProfile();
    const auto& salt   = profile ? profile->getName() : "";
    return SIGHT_PSEUDO_RANDOM_HASH(salt);
}

// The default preference file name
static constexpr auto s_preferences_file = "preferences.json";
static constexpr auto s_encrypted_file   = "preferences.sight";

// Keep a password
static std::unique_ptr<PasswordKeeper> s_password;

// Password policy to use
static PasswordKeeper::PasswordPolicy s_password_policy {PasswordKeeper::PasswordPolicy::DEFAULT};

// Encryption policy to use
static PasswordKeeper::EncryptionPolicy s_encryption_policy {PasswordKeeper::EncryptionPolicy::DEFAULT};

// Guard the preference tree
std::shared_mutex Preferences::s_preferences_mutex;

// Contains the preference tree
std::unique_ptr<boost::property_tree::ptree> Preferences::s_preferences;

// True if the preferences has been modified
bool Preferences::s_is_preferences_modified {false};

// Preferences can be disabled globally
bool Preferences::s_is_enabled {false};

//------------------------------------------------------------------------------

inline static bool must_encrypt()
{
    return (s_password && !s_password->getPassword().empty())
           || s_encryption_policy == PasswordKeeper::EncryptionPolicy::FORCED;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path compute_preferences_filepath()
{
    // Get the profile
    const auto& profile = core::runtime::getCurrentProfile();
    SIGHT_THROW_IF("No current profile set", !profile);

    // Get the current application name
    const auto& name = profile->getName();
    SIGHT_THROW_IF("Unable to determine application name", name.empty());

    // Get the application data directory
    const auto& data_directory = core::tools::os::getUserDataDir("sight", name, true);
    SIGHT_THROW_IF("Unable to define user data directory", data_directory.empty());
    SIGHT_THROW_IF(
        "Data directory '" + data_directory + "' doesn't exist or is not a directory",
        !std::filesystem::is_directory(data_directory)
    );

    // Build the preferences filepath
    const auto& preferences_filepath = data_directory + "/" + (must_encrypt() ? s_encrypted_file : s_preferences_file);
    SIGHT_THROW_IF(
        "Preferences file '" + preferences_filepath + "' already exists and is not a regular file",
        std::filesystem::exists(preferences_filepath)
        && !std::filesystem::is_regular_file(preferences_filepath)
    );

    return preferences_filepath;
}

//------------------------------------------------------------------------------

inline static secure_string compute_password()
{
    if(s_encryption_policy == PasswordKeeper::EncryptionPolicy::FORCED
       && (!s_password || s_password->getPassword().empty()))
    {
        return pseudo_random_password();
    }
    else if(s_encryption_policy == PasswordKeeper::EncryptionPolicy::SALTED)
    {
        return s_password->getPassword() + pseudo_random_password();
    }
    else
    {
        return s_password->getPassword();
    }
}

//------------------------------------------------------------------------------

inline static void set_password_nolock(const core::crypto::secure_string& password)
{
    if(password.empty())
    {
        // No password, we disable encryption
        s_password.reset();
    }
    else
    {
        // If the PasswordKeeper is not there, create it
        if(!s_password)
        {
            s_password = std::make_unique<PasswordKeeper>();
        }

        // Store the password
        s_password->setPassword(password);
    }
}

//------------------------------------------------------------------------------

Preferences::Preferences()
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // Number of retry
    int password_retry {0};

    const std::function<void(void)> load =
        [&]
        {
            // Create and load the preferences file if not already done
            if(s_is_enabled && !s_preferences)
            {
                // Set the password to use
                // NEVER policy means we never ask for a password and only rely on manually set
                if(s_password_policy != PasswordKeeper::PasswordPolicy::NEVER)
                {
                    const secure_string& password =
                        s_password
                        ? s_password->getPassword()
                        : PasswordKeeper::getGlobalPassword();

                    if(password_retry > 0
                       || (s_password_policy == PasswordKeeper::PasswordPolicy::ALWAYS)
                       || (s_password_policy == PasswordKeeper::PasswordPolicy::ONCE
                           && password.empty()))
                    {
                        sight::ui::base::dialog::InputDialog inputDialog;
                        const auto& new_password = secure_string(
                            inputDialog.showInputDialog(
                                "Enter Password",
                                "Password:",
                                password.c_str(),
                                sight::ui::base::dialog::InputDialog::EchoMode::PASSWORD
                            )
                        );

                        set_password_nolock(new_password);
                        PasswordKeeper::setGlobalPassword(new_password);
                    }
                    else if(!s_password)
                    {
                        set_password_nolock(password);
                    }
                }

                try
                {
                    s_preferences = std::make_unique<boost::property_tree::ptree>();

                    const auto& preferences_filepath = compute_preferences_filepath();

                    if(std::filesystem::exists(preferences_filepath))
                    {
                        // If a password has been set or if the encryption is "forced" open as an encrypted file
                        if(must_encrypt())
                        {
                            // Open the archive that holds the property tree
                            auto archive = io::zip::ArchiveReader::get(preferences_filepath);

                            // Create the input stream, with a password, allowing decoding an encrypted file
                            auto istream = archive->openFile(
                                s_preferences_file,
                                compute_password()
                            );

                            // Read the property tree from the archive
                            boost::property_tree::read_json(*istream, *s_preferences);
                        }
                        else
                        {
                            boost::property_tree::read_json(preferences_filepath.string(), *s_preferences);
                        }

                        // Preferences has been read from disk
                        s_is_preferences_modified = false;
                    }
                    else
                    {
                        // Create an empty preferences
                        s_is_preferences_modified = true;
                    }
                }
                catch(const io::zip::exception::BadPassword& e)
                {
                    s_preferences.reset();

                    // NEVER policy means we never ask for a password, so there is no point to retry
                    if(s_password_policy != PasswordKeeper::PasswordPolicy::NEVER)
                    {
                        sight::ui::base::dialog::MessageDialog messageDialog;
                        messageDialog.setTitle("Wrong password");

                        if(password_retry > 5)
                        {
                            messageDialog.setMessage("The provided password is wrong and there were too many tries.");
                            messageDialog.setIcon(ui::base::dialog::IMessageDialog::CRITICAL);
                            messageDialog.addButton(ui::base::dialog::IMessageDialog::OK);
                            messageDialog.show();
                        }
                        else
                        {
                            // Ask if the user want to retry.
                            messageDialog.setMessage(
                                "The file is password protected and the provided password is wrong.\n\nRetry with a"
                                " different password ?"
                            );
                            messageDialog.setIcon(ui::base::dialog::IMessageDialog::QUESTION);
                            messageDialog.addButton(ui::base::dialog::IMessageDialog::RETRY);
                            messageDialog.addButton(ui::base::dialog::IMessageDialog::CANCEL);

                            if(messageDialog.show() == sight::ui::base::dialog::IMessageDialog::RETRY)
                            {
                                // Retry...
                                password_retry++;
                                load();

                                // Exit from lambda
                                return;
                            }
                            else
                            {
                                s_is_enabled = false;
                            }
                        }
                    }

                    SIGHT_WARN("Preferences has been disabled because the password is wrong.");
                    SIGHT_THROW_EXCEPTION(BadPassword(e.what()));
                }
                catch(const std::exception& e)
                {
                    // We simply print an error and disable preferences management
                    s_preferences.reset();
                    SIGHT_ERROR(e.what());
                }
                catch(...)
                {
                    // We disable preferences management
                    s_preferences.reset();
                }
            }
        };

    // Call the lambda !
    load();

    throw_if_disabled();
}

Preferences::~Preferences()
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // Check if we must save the modifications
    if(s_is_enabled && s_is_preferences_modified)
    {
        const auto& preferences_filepath = compute_preferences_filepath();

        if(must_encrypt())
        {
            // Delete the preferences file, since minizip is unable to replace file.
            // Doing otherwise will result with many preferences.json inside the archive, which cannot be read back
            std::filesystem::remove_all(preferences_filepath);

            // Create the archive that will hold the property tree
            auto archive = io::zip::ArchiveWriter::get(preferences_filepath);

            // Create the output stream, with a password, resulting in an encrypted file
            auto ostream = archive->openFile(
                s_preferences_file,
                compute_password()
            );

            // Read the property tree from the archive
            boost::property_tree::write_json(*ostream, *s_preferences, false);
        }
        else
        {
            boost::property_tree::write_json(preferences_filepath.string(), *s_preferences);
        }

        s_is_preferences_modified = false;
    }
}

//------------------------------------------------------------------------------

void Preferences::erase(const std::string& key)
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // If the preferences are disabled or not loaded, throw an exception to disallow loading
    throw_if_disabled();

    s_preferences->erase(key);

    // Mark the tree as modified so it will be saved when Preferences is deleted
    s_is_preferences_modified = true;
}

//------------------------------------------------------------------------------

void Preferences::clear()
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // If the preferences are disabled or not loaded, throw an exception to disallow loading
    throw_if_disabled();

    s_preferences->clear();

    // Mark the tree as modified so it will be saved when Preferences is deleted
    s_is_preferences_modified = true;
}

//------------------------------------------------------------------------------

void Preferences::set_enabled(bool enable)
{
    std::unique_lock guard(s_preferences_mutex);
    s_is_enabled = enable;

    if(!enable)
    {
        s_preferences.reset();
        s_is_preferences_modified = false;
    }
}

//------------------------------------------------------------------------------

void Preferences::throw_if_disabled()
{
    SIGHT_THROW_EXCEPTION_IF(
        PreferencesDisabled("Preferences are disabled"),
        !s_is_enabled || !s_preferences
    );
}

//------------------------------------------------------------------------------

void Preferences::set_password(const core::crypto::secure_string& password)
{
    std::unique_lock guard(s_preferences_mutex);
    set_password_nolock(password);
}

//------------------------------------------------------------------------------

void Preferences::set_password_policy(const core::crypto::PasswordKeeper::PasswordPolicy policy)
{
    std::unique_lock guard(s_preferences_mutex);
    s_password_policy = policy;
}

//------------------------------------------------------------------------------

void Preferences::set_encryption_policy(const core::crypto::PasswordKeeper::EncryptionPolicy policy)
{
    std::unique_lock guard(s_preferences_mutex);
    s_encryption_policy = policy;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base