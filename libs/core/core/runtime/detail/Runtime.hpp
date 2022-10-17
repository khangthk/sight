/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include "core/base.hpp"
#include "core/config.hpp"
#include "core/runtime/detail/Extension.hpp"
#include "core/runtime/ExecutableFactory.hpp"
#include "core/runtime/IPlugin.hpp"

#include <regex>
#include <set>

namespace sight::core::runtime::detail
{

class ExtensionPoint;
class Module;

/**
 * @brief   Defines the runtime class.
 *
 */
class Runtime
{
public:

    /**
     * @name    Type Definitions
     */
    //@{
    /// Defines the module container type.
    using ModuleContainer = std::set<std::shared_ptr<core::runtime::Module> >;
    /// Defines the extension container type.
    using ExtensionContainer = std::set<std::shared_ptr<Extension> >;
    /// Defines the extension container type.
    using ExtensionIterator = ExtensionContainer::iterator;
    //@}

    /// Regex used in different places to find the module part in a path
    static const std::regex s_MATCH_MODULE_PATH;

    /// Retrieves the singleton
    static Runtime& get();

    /**
     * @brief   Constructor.
     */
    Runtime();
    /**
     * @brief   Destructor : does nothing.
     */
    ~Runtime();

    /**
     * @name    Public API implementation
     * @{
     */

    /**
     * @brief       Adds all module found in the given path.
     *
     * @param[in]   repository  a path that may containing modules
     */
    void addModules(const std::filesystem::path& repository);

    /**
     * @brief       Retrieves the module for the specified identifier.
     *
     * @param[in]   identifier  a string containing a module identifier
     * @param[in]   version     the version of the module (undefined by default)
     *
     * @return      a shared pointer to the found module or null if none
     */
    [[nodiscard]] SPTR(core::runtime::Module) findModule(const std::string& identifier) const;

    /**
     * @brief   Create an instance of the given executable object type.
     *
     * An attempt is made to retrieve a registered executable factory. If none
     * is found, the creation will fail.
     *
     * @remark      This method will not try to load any module.
     *
     * @param[in]   type    a string containing an executable type
     *
     * @return      a pointer to the created executable instance
     */
    [[nodiscard]] IExecutable* createExecutableInstance(const std::string& type) const;

    /**
     * @brief   Create an instance of the given executable object type and configuration element.
     *
     * An attempt is made to find a registered executable factory. If none
     * is found, the module of the given configuration element is started in the
     * hope it will register a executable factory for the given type. Then an
     * executable factory for the given type is searched once again and the
     * instantiation procedure goes further.
     *
     * @param[in]   type                    a string containing an executable type
     * @param[in]   configurationElement    a shared pointer to the configuration element to use for the executable
     * initialization
     *
     * @return  a pointer to the created executable instance
     */
    [[nodiscard]] IExecutable* createExecutableInstance(
        const std::string& type,
        SPTR(ConfigurationElement) configurationElement
    ) const;

    /**
     * @brief       Retrieves the extension instance matching the specified identifier.
     *
     * @param[in]   identifier  a string containing an extension identifier
     *
     * @return      a shared pointer to the found extension instance or null if none
     */
    [[nodiscard]] std::shared_ptr<core::runtime::Extension> findExtension(const std::string& identifier) const;

    /// Return all modules known by the runtime
    ModuleContainer getModules() const;

    /// Get the path where libraries, modules and share folder are located.
    [[nodiscard]] std::filesystem::path getWorkingPath() const;

    /// Get the path where libraries, modules and share folder are located, for each known location
    [[nodiscard]] std::vector<std::pair<std::filesystem::path,
                                        std::filesystem::path> > getRepositoriesPath() const;
    //@}

    /**
     * @name    Modules
     *
     * @{
     */

    /**
     * @brief       Adds a new module instance to the runtime system.
     *
     * @remark      The given module state will be altered according to the current configuration rules.
     * @param[in]   module  a shared pointer to the module instance to add
     */
    void addModule(std::shared_ptr<detail::Module> module);

    /**
     * @brief       Unregister a module instance to the runtime system.
     *
     * @param[in]   module  a shared pointer to the module instance to unregister
     */
    void unregisterModule(std::shared_ptr<detail::Module> module);

    /**
     * @brief       Retrieves the enabled module for the specified identifier.
     *
     * @param[in]   identifier  a string containing a module identifier
     * @param[in]   version     the version of the module (undefined by default)
     *
     * @return      a shared pointer to the found module or null if none
     */
    [[nodiscard]] std::shared_ptr<core::runtime::Module> findEnabledModule(const std::string& identifier) const;

    //@}

    /**
     * @name    Executable, Executable Factories & Dynamic Executable Instantiation
     *
     * @{
     */

    /**
     * @brief       Adds a new executable factory instance to the runtime system.
     *
     * @param[in]   factory a shared pointer to an executable factory
     */
    void addExecutableFactory(std::shared_ptr<ExecutableFactory> factory);

    /**
     * @brief       Unregister a new executable factory instance to the runtime system.
     *
     * @param[in]   factory a shared pointer to an executable factory
     */
    void unregisterExecutableFactory(std::shared_ptr<ExecutableFactory> factory);

    /**
     * @brief       Retrieves the executable factory for the given identifier.
     *
     * @param[in]   type    a string containing a type identifier
     *
     * @return      a sgared pointer to the found executable factory or null of none
     */
    [[nodiscard]] std::shared_ptr<ExecutableFactory> findExecutableFactory(const std::string& type) const;

    /**
     * @brief       Retrieves the plugin instance for the specified module.
     *
     * @param[in]   module  a shared pointer to a module instance
     *
     * @return      a shared pointer to a plugin instance or null if none
     */
    [[nodiscard]] std::shared_ptr<IPlugin> getPlugin(std::shared_ptr<Module> module) const;

    //@}

    /**
     * @name    Extensions
     *
     * @{
     */

    /**
     * @brief       Registers a new extension.
     *
     * @param[in]   extension   a shared pointer to the extension to register
     */
    void addExtension(std::shared_ptr<detail::Extension> extension);

    /**
     * @brief       Unregister a new extension.
     *
     * @param[in]   extension   a shared pointer to the extension to register
     */
    void unregisterExtension(std::shared_ptr<detail::Extension> extension);

    /// Retrieves the extension collection.
    ExtensionContainer getExtensions() const;
    ExtensionIterator extensionsBegin() const;
    ExtensionIterator extensionsEnd() const;
    //@}

    /**
     * @name    Extension Points
     */
    //@{
    /**
     * @brief       Registers a new extension point.
     *
     * @param[in]   point   a pointer to the extension point to register
     */
    void addExtensionPoint(std::shared_ptr<ExtensionPoint> point);

    /**
     * @brief       Unregister a new extension point.
     *
     * @param[in]   point   a pointer to the extension point to register
     */
    void unregisterExtensionPoint(std::shared_ptr<ExtensionPoint> point);

    /**
     * @brief       Retrieves the extension point instance matching the specified identifier.
     *
     * @param[in]   identifier  a string containing an extension point identifier
     *
     * @return      a shared pointer to the found extension point instance or null if none
     */
    [[nodiscard]] std::shared_ptr<ExtensionPoint> findExtensionPoint(const std::string& identifier) const;
    //@}

private:

    ///< Defines the executable factory container type.
    using ExecutableFactoryContainer = std::set<std::shared_ptr<ExecutableFactory> >;

    ///< Defines the extension point container type.
    using ExtensionPointContainer = std::set<std::shared_ptr<ExtensionPoint> >;

    ///< Defines the plugin container type.
    using PluginContainer = std::vector<std::shared_ptr<IPlugin> >;

    static std::shared_ptr<Runtime> m_instance; ///< The runtime instance.

    ExecutableFactoryContainer m_executableFactories; ///< Contains all executable factories.
    ExtensionContainer m_extensions;                  ///< Contains all registered extensions.
    ExtensionPointContainer m_extensionPoints;        ///< Contains all registered extension points.
    ModuleContainer m_modules;                        ///< Contains all modules.
    PluginContainer m_plugins;                        ///< Contains all plugins.

    std::filesystem::path m_workingPath; ///< Main path where Modules and share folder are located.

    /// All paths as a pair of <binary path/resource path> containing modules, added from the CLI
    std::vector<std::pair<std::filesystem::path, std::filesystem::path> > m_repositories;
};

} // namespace sight::core::runtime::detail
