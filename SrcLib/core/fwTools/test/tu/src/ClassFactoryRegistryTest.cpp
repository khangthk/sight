/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/shared_ptr.hpp>

#include "fwTools/ClassFactoryRegistry.hpp"
#include "fwTools/ClassFactory.hpp"
#include "fwTools/Object.hpp"

#include "ClassFactoryRegistryTest.hpp"

// Registers the fixture into the 'registry'
namespace fwTools
{

CPPUNIT_TEST_SUITE_REGISTRATION( ClassFactoryRegistryTest );

void ClassFactoryRegistryTest::setUp()
{
	// Set up context before running a test.
}

void ClassFactoryRegistryTest::tearDown()
{
	// Clean up after the test run.
}

void ClassFactoryRegistryTest::methode1()
{
	const std::string STR = "toto";

	::boost::shared_ptr< ::fwTools::ClassFactory< ::fwTools::Object, ::fwTools::Field, std::string > > cf(new ::fwTools::ClassFactory< ::fwTools::Object, ::fwTools::Field, std::string >(STR));

	::fwTools::ClassFactoryRegistry::addFactory(cf);

	std::list< ::boost::shared_ptr< ::fwTools::Object > > list =  ::fwTools::ClassFactoryRegistry::subClasses< ::fwTools::Object >(STR);

	::fwTools::Object::sptr obj = ::fwTools::ClassFactoryRegistry::create< ::fwTools::Object >(STR);

	CPPUNIT_ASSERT(list.begin() != list.end());
	CPPUNIT_ASSERT(obj);
}


}
