/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author David Guthrie
 */
#ifndef DELTA_MAPTESTS_H
#define DELTA_MAPTESTS_H

#include <dtDAL/log.h>
#include <dtDAL/map.h>

#include <cppunit/extensions/HelperMacros.h>

class MapTests : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE( MapTests );
    CPPUNIT_TEST( testMapAddRemoveProxies );
    CPPUNIT_TEST( testMapProxySearch );
    CPPUNIT_TEST( testMapLibraryHandling );
    CPPUNIT_TEST( testLoadMapIntoScene );
    CPPUNIT_TEST( testLoadErrorHandling );
    CPPUNIT_TEST( testMapSaveAndLoad );
    CPPUNIT_TEST( testLibraryMethods );
    CPPUNIT_TEST( testWildCard );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testMapAddRemoveProxies();
    void testMapProxySearch();
    void testMapLibraryHandling();
    void testMapSaveAndLoad();
    void testLoadMapIntoScene();
    void testLoadErrorHandling();
    void testLibraryMethods();
    void testWildCard();
private:
    void createActors(dtDAL::Map& map);
        dtDAL::Log* logger;
    dtDAL::ActorProperty* getActorProperty(dtDAL::Map& map,
        const std::string& propName, dtDAL::DataType& type, unsigned which = 0);
};

#endif  // DELTA_MAPTESTS_H

