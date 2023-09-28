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

#include "core/runtime/detail/io/profile_reader.hpp"

#include "core/runtime/detail/io/validator.hpp"
#include "core/runtime/detail/profile/activater.hpp"
#include "core/runtime/detail/profile/profile.hpp"
#include "core/runtime/path.hpp"
#include "core/runtime/profile/profile.hpp"
#include "core/runtime/runtime_exception.hpp"

#include <libxml/parser.h>

#include <filesystem>
#include <sstream>
#include <string>

namespace sight::core::runtime
{

namespace detail::io
{

std::string profile_reader::ID("id");
std::string profile_reader::NAME("name");
std::string profile_reader::VALUE("value");
std::string profile_reader::VERSION("version");
std::string profile_reader::CHECK_SINGLE_INSTANCE("check-single-instance");
std::string profile_reader::ACTIVATE("activate");
std::string profile_reader::START("start");
std::string profile_reader::PARAM("param");
std::string profile_reader::DIS_EXT_PT("disable-extension-point");
std::string profile_reader::DIS_EXT("disable-extension");

//------------------------------------------------------------------------------

namespace profile = core::runtime::detail::profile;

//------------------------------------------------------------------------------

std::shared_ptr<profile::profile> profile_reader::create_profile(const std::filesystem::path& path)
{
    // Normalizes the path.
    std::filesystem::path normalized_path(std::filesystem::weakly_canonical(path));

    // Asserts that the repository is a valid directory path.
    if(!std::filesystem::exists(normalized_path) || std::filesystem::is_directory(normalized_path))
    {
        throw runtime_exception("'" + normalized_path.string() + "': not a a file.");
    }

    // Validation
    auto profile_xsdlocation = core::runtime::get_library_resource_file_path("core/profile.xsd");

    validator validator(profile_xsdlocation);

    if(!validator.validate(normalized_path))
    {
        throw runtime_exception(validator.get_error_log());
    }

    // Get the document.
    xmlDocPtr document = xmlParseFile(normalized_path.string().c_str());
    if(document == nullptr)
    {
        throw runtime_exception("Unable to read the profile file.");
    }

    try
    {
        // Get the root node.
        xmlNodePtr root_node = xmlDocGetRootElement(document);

        char* p_name =
            reinterpret_cast<char*>(xmlGetProp(root_node, reinterpret_cast<const xmlChar*>(NAME.c_str())));
        char* p_version =
            reinterpret_cast<char*>(xmlGetProp(root_node, reinterpret_cast<const xmlChar*>(VERSION.c_str())));
        char* p_chk_inst =
            reinterpret_cast<char*>(xmlGetProp(
                                        root_node,
                                        reinterpret_cast<const xmlChar*>(CHECK_SINGLE_INSTANCE.c_str())
            ));

        SIGHT_ASSERT("Application profile MUST have a name attribute", p_name);
        SIGHT_ASSERT("Application profile MUST have a version attribute", p_version);

        std::string s_name(p_name);
        std::string s_version(p_version);
        bool check_single_instance = (p_chk_inst != nullptr) && std::string(p_chk_inst) == "true";

        xmlFree(p_name);
        xmlFree(p_version);
        xmlFree(p_chk_inst);

        // Creates and process the profile element.
        std::shared_ptr<profile::profile> profile = process_profile(root_node);

        profile->set_file_path(normalized_path);
        profile->set_name(s_name);
        profile->set_version(s_version);
        profile->set_check_single_instance(check_single_instance);

        detail::profile::set_current_profile(profile);

        // Job's done!
        xmlFreeDoc(document);
        return profile;
    }
    catch(std::exception& /*exception*/)
    {
        xmlFreeDoc(document);
        throw;
    }
}

//------------------------------------------------------------------------------

std::shared_ptr<profile::profile> profile_reader::process_profile(xmlNodePtr node)
{
    auto profile = std::dynamic_pointer_cast<detail::profile::profile>(core::runtime::get_current_profile());

    // Process child nodes.
    for(xmlNodePtr cur_child = node->children ; cur_child != nullptr ; cur_child = cur_child->next)
    {
        if(xmlStrcmp(cur_child->name, reinterpret_cast<const xmlChar*>(ACTIVATE.c_str())) == 0)
        {
            profile->add(profile_reader::process_activater(cur_child));
            continue;
        }

        if(xmlStrcmp(cur_child->name, reinterpret_cast<const xmlChar*>(START.c_str())) == 0)
        {
            profile->add_starter(process_starter(cur_child));
            continue;
        }
    }

    return profile;
}

//------------------------------------------------------------------------------

std::string profile_reader::process_starter(xmlNodePtr node)
{
    // Processes all attributes.
    xmlAttrPtr cur_attr = nullptr;
    std::string identifier;
    std::string version;
    for(cur_attr = node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }

        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(VERSION.c_str())) == 0)
        {
            version = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }
    }

    return identifier;
}

//------------------------------------------------------------------------------

std::shared_ptr<detail::profile::activater> profile_reader::process_activater(xmlNodePtr node)
{
    // Processes all attributes.
    xmlAttrPtr cur_attr = nullptr;
    std::string identifier;
    std::string version;
    for(cur_attr = node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }

        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(VERSION.c_str())) == 0)
        {
            version = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }
    }

    // Creates the activater object.
    auto activater = std::make_shared<detail::profile::activater>(identifier, version);

    // Processes child node that are the parameters
    for(xmlNodePtr cur_child = node->children ; cur_child != nullptr ; cur_child = cur_child->next)
    {
        if(xmlStrcmp(cur_child->name, reinterpret_cast<const xmlChar*>(PARAM.c_str())) == 0)
        {
            process_activater_param(cur_child, activater);
            continue;
        }

        if(xmlStrcmp(cur_child->name, reinterpret_cast<const xmlChar*>(DIS_EXT_PT.c_str())) == 0)
        {
            process_activater_disable_extension_point(cur_child, activater);
            continue;
        }

        if(xmlStrcmp(cur_child->name, reinterpret_cast<const xmlChar*>(DIS_EXT.c_str())) == 0)
        {
            process_activater_disable_extension(cur_child, activater);
            continue;
        }
    }

    // Job's done.
    return activater;
}

//------------------------------------------------------------------------------

void profile_reader::process_activater_param(xmlNodePtr node, std::shared_ptr<detail::profile::activater> activater)
{
    // Processes all attributes.
    xmlAttrPtr cur_attr = nullptr;
    std::string identifier;
    std::string value;
    for(cur_attr = node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }

        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(VALUE.c_str())) == 0)
        {
            value = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }
    }

    // Stores the parameter into the activater.
    activater->add_parameter(identifier, value);
}

//------------------------------------------------------------------------------

void profile_reader::process_activater_disable_extension_point(
    xmlNodePtr node,
    std::shared_ptr<detail::profile::activater> activater
)
{
    // Processes all attributes.
    xmlAttrPtr cur_attr = nullptr;
    std::string identifier;
    for(cur_attr = node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }
    }

    // Stores the parameter into the activater.
    activater->add_disable_extension_point(identifier);
}

//------------------------------------------------------------------------------

void profile_reader::process_activater_disable_extension(
    xmlNodePtr node,
    std::shared_ptr<profile::activater> activater
)
{
    // Processes all attributes.
    xmlAttrPtr cur_attr = nullptr;
    std::string identifier;
    for(cur_attr = node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }
    }

    // Stores the parameter into the activater.
    activater->add_disable_extension(identifier);
}

//------------------------------------------------------------------------------

} // namespace detail::io

//------------------------------------------------------------------------------

std::shared_ptr<core::runtime::profile> io::profile_reader::create_profile(const std::filesystem::path& path)
{
    return detail::io::profile_reader::create_profile(path);
}

} // namespace sight::core::runtime
