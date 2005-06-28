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

#ifndef DELTA_PROJECTTESTS_H
#define DELTA_PROJECTTESTS_H


#include <tree.h>
#include <dtDAL/log.h>

#include <cppunit/extensions/HelperMacros.h>

namespace dtDAL {
    class ResourceTreeNode;
    class DataType;
}

class ProjectTests : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ProjectTests );
    CPPUNIT_TEST( testReadonlyFailure );
    CPPUNIT_TEST( testProject );
    CPPUNIT_TEST( testFileIO );
    CPPUNIT_TEST( testCategories );
    CPPUNIT_TEST( testResources );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testProject();
    void testFileIO();
    void testCategories();
    void testReadonlyFailure();
    void testResources();
private:
    dtDAL::Log* logger;
    void printTree(const core::tree<dtDAL::ResourceTreeNode>::const_iterator& iter);
    core::tree<dtDAL::ResourceTreeNode>::const_iterator findTreeNodeFromCategory(
        const core::tree<dtDAL::ResourceTreeNode>& currentTree,
        const dtDAL::DataType* dt, const std::string& category) const;
};

#endif  // DELTA_PROJECTTESTS_H

