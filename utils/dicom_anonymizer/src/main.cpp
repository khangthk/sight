/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <io/dicom/helper/dicom_anonymizer.hpp>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

/** \file dicom_anonymizer/src/main
 *
 *********************
 * Software : dicom_anonymizer
 *********************
 * Allows to anonymize a Dicom folder
 * HELP  : dicom_anonymizer.exe --help
 * USE :   dicom_anonymizer.exe <options>
 * Allowed options:
 *   -h [ --help ]           produce help message
 *   -i [ --input ] arg      set the input folder
 *   -o [ --output ] arg     set the output folder
 */
int main(int argc, char** argv)
{
    // Declare the supported options.
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "produce help message")
    ("input,i", boost::program_options::value<std::string>(), "set input folder")
    ("output,o", boost::program_options::value<std::string>(), "set output folder")
    ;

    // Manage the options
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if(vm.count("help") != 0U)
    {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    if(vm.count("input") == 0U)
    {
        std::cout << "You must specify an input file." << std::endl << std::endl;
        std::cout << desc << std::endl;
        return EXIT_FAILURE;
    }

    if(vm.count("output") == 0U)
    {
        std::cout << "You must specify an output file." << std::endl << std::endl;
        std::cout << desc << std::endl;
        return EXIT_FAILURE;
    }

    if(vm["input"].as<std::string>() == vm["output"].as<std::string>())
    {
        std::cout << "The output folder can not be the input folder." << std::endl;
        return EXIT_FAILURE;
    }

    // Get paths
    std::filesystem::path input(vm["input"].as<std::string>());
    std::filesystem::path output(vm["output"].as<std::string>());

    if(!std::filesystem::exists(input) || !std::filesystem::is_directory(input))
    {
        std::cout << "The specified input folder " << input << " is not a directory." << "\n";
        return EXIT_FAILURE;
    }

    if(std::filesystem::exists(output))
    {
        std::cout << "The specified output folder " << output << " already exists." << "\n";
        return EXIT_FAILURE;
    }

    // Copy and anonymize
    sight::io::dicom::helper::dicom_anonymizer::copy_directory(input, output);
    sight::io::dicom::helper::dicom_anonymizer anonymizer;
    anonymizer.anonymize(output);

    return EXIT_SUCCESS;
}
