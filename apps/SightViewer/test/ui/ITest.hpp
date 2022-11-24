/************************************************************************
 *
 * Copyright (C) 2022-2022 IRCAD France
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

#include <ui/testCore/ITest.hpp>

namespace sight::sightviewer::test::ui
{

class ITest : public sight::ui::testCore::ITest
{
public:

    std::filesystem::path getProfilePath() override;

    static void openFile(
        sight::ui::testCore::Tester& tester,
        const std::string& format,
        const std::filesystem::path& path
    );
    static void saveSnapshot(sight::ui::testCore::Tester& tester, const std::filesystem::path& path);
    static void resetNegatos(sight::ui::testCore::Tester& tester);
};

} // namespace sight::sightviewer::test::ui
