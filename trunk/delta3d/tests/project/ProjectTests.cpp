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
#include <vector>
#include <set>
#include <string>

#include <stdio.h>
#include <time.h>

#include <osgDB/FileUtils>

#include <dtCore/dt.h>
#include <dtCore/scene.h>
#include <dtABC/application.h>

#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/log.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/exception.h>
#include <dtDAL/fileutils.h>
#include <dtDAL/datatype.h>

#include <dtDAL/stringtokenizer.h>

#include "ProjectTests.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ProjectTests );


void ProjectTests::setUp() {
    dtDAL::Project::GetInstance();
    std::string logName("projectTest");

    logger = &dtDAL::Log::GetInstance("Project.cpp");
    logger->SetLogLevel(dtDAL::Log::LOG_DEBUG);
    logger = &dtDAL::Log::GetInstance("FileUtils.cpp");
    logger->SetLogLevel(dtDAL::Log::LOG_DEBUG);
    logger = &dtDAL::Log::GetInstance("MapXML.cpp");
    logger->SetLogLevel(dtDAL::Log::LOG_DEBUG);

    logger = &dtDAL::Log::GetInstance(logName);

    logger->SetLogLevel(dtDAL::Log::LOG_DEBUG);
    logger->LogMessage(dtDAL::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Log initialized.\n");
    dtDAL::FileUtils& fileUtils = dtDAL::FileUtils::GetInstance();
    fileUtils.PushDirectory("project");

    fileUtils.PushDirectory("WorkingProject");
    fileUtils.DirDelete(dtDAL::DataType::STATIC_MESH.GetName(), true);
    fileUtils.PopDirectory();

}


void ProjectTests::tearDown() {
    dtDAL::FileUtils& fileUtils = dtDAL::FileUtils::GetInstance();
    fileUtils.PopDirectory();
}

void ProjectTests::testFileIO() {
    dtDAL::FileUtils& fileUtils = dtDAL::FileUtils::GetInstance();

    std::string Dir1("Testing");
    std::string Dir2(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + "Testing1");

    //cleanup
    try {
        fileUtils.DirDelete(Dir1, true);
    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_ASSERT_MESSAGE((ex.What() + ": Error deleting Directory, but file exists.").c_str(),
            ex.TypeEnum() == dtDAL::ExceptionEnum::ProjectFileNotFound);
    }

    osgDB::makeDirectory(Dir1);
    osgDB::makeDirectory(Dir2);

    std::string file1("dirt.ive");
    std::string file2("flatdirt.ive");

    struct dtDAL::FileInfo file1Info = fileUtils.GetFileInfo(file1);
    struct dtDAL::FileInfo file2Info = fileUtils.GetFileInfo(file2);

    try {
        fileUtils.GetFileInfo(file2 + "euaoeuaiao.ao.u");
    } catch (const dtDAL::Exception& ex) {
        //this should throw a file not found.
        CPPUNIT_ASSERT_MESSAGE(ex.What().c_str(), ex.TypeEnum() == dtDAL::ExceptionEnum::ProjectFileNotFound);
        //correct
    }

    CPPUNIT_ASSERT_MESSAGE("dirt.ive should exist.", fileUtils.FileExists(file1));

    fileUtils.FileCopy(file1, Dir1, false);

    CPPUNIT_ASSERT_MESSAGE("The original dirt.ive should exist.", fileUtils.FileExists(file1));

    CPPUNIT_ASSERT_MESSAGE("The new dirt.ive should exist.", fileUtils.FileExists(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1));

    fileUtils.FileCopy(file1, Dir1, true);

    CPPUNIT_ASSERT_MESSAGE("The original dirt.ive should exist.", fileUtils.FileExists(file1));

    CPPUNIT_ASSERT_MESSAGE("The new dirt.ive should exist.", fileUtils.FileExists(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1));

    try {
        fileUtils.FileCopy(file2, Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1, false);
        CPPUNIT_FAIL("The file copy should have failed since it was attempting to overwrite the file and overwriting was disabled.");
    } catch (const dtDAL::Exception& ex) {
        //correct
    }

    CPPUNIT_ASSERT_MESSAGE("The original flatdirt.ive should exist.", fileUtils.FileExists(file2));

    CPPUNIT_ASSERT_MESSAGE("The new dirt.ive should still exist.", fileUtils.FileExists(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1));

    struct dtDAL::FileInfo fi = fileUtils.GetFileInfo(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1);
    CPPUNIT_ASSERT_MESSAGE("dirt.ive should be the same size as the original", fi.size == file1Info.size);

    try {
        fileUtils.FileCopy(file2, Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1, true);
    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_FAIL(ex.What().c_str());
    }

    CPPUNIT_ASSERT_MESSAGE("The original flatdirt.ive should exist.", fileUtils.FileExists(file2));

    CPPUNIT_ASSERT_MESSAGE("The new dirt.ive, copied from flatdirt.ive, should exist.", fileUtils.FileExists(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1));

    fi = fileUtils.GetFileInfo(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1);
    CPPUNIT_ASSERT_MESSAGE("dirt.ive should be the same size as flatdirt.ive", fi.size == file2Info.size);

    try {
        fileUtils.FileMove(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1, Dir2 + dtDAL::FileUtils::PATH_SEPARATOR + file1, false);
    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_FAIL(ex.What().c_str());
    }

    CPPUNIT_ASSERT_MESSAGE("dirt.ive should not exist.",!fileUtils.FileExists(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1));

    CPPUNIT_ASSERT_MESSAGE("The new dirt.ive, copied from dirt.ive, should exist.", fileUtils.FileExists(Dir2 + dtDAL::FileUtils::PATH_SEPARATOR + file1));

    fi = fileUtils.GetFileInfo(Dir2 + dtDAL::FileUtils::PATH_SEPARATOR + file1);
    CPPUNIT_ASSERT_MESSAGE("dirt.ive should be the same size as flatdirt.ive", fi.size == file2Info.size);

    //copy the file back so we can try to move it again with overwriting.
    try {
        fileUtils.FileCopy(Dir2 + dtDAL::FileUtils::PATH_SEPARATOR + file1, Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1, false);
    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_FAIL(ex.What().c_str());
    }

    try {
        fileUtils.FileMove(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1, Dir2 + dtDAL::FileUtils::PATH_SEPARATOR + file1, false);
        CPPUNIT_FAIL("Moving the file should have failed since overwriting was turned off.");
    } catch (const dtDAL::Exception& ex) {
        //correct
    }
    try {
        fileUtils.FileMove(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1, Dir2 + dtDAL::FileUtils::PATH_SEPARATOR + file1, true);
    } catch (const dtDAL::Exception& ex) {
        //correct
        CPPUNIT_FAIL(ex.What().c_str());
    }

    CPPUNIT_ASSERT_MESSAGE("dirt.ive should not exist.", !fileUtils.FileExists(Dir1 + dtDAL::FileUtils::PATH_SEPARATOR + file1));

    CPPUNIT_ASSERT_MESSAGE("The new dirt.ive, copied from dirt.ive, should exist.", fileUtils.FileExists(Dir2 + dtDAL::FileUtils::PATH_SEPARATOR + file1));

    fi = fileUtils.GetFileInfo(Dir2 + dtDAL::FileUtils::PATH_SEPARATOR + file1);
    CPPUNIT_ASSERT_MESSAGE("dirt.ive should be the same size as flatdirt.ive", fi.size == file2Info.size);

    osgDB::DirectoryContents dc = fileUtils.DirGetSubs(Dir1);

    CPPUNIT_ASSERT_MESSAGE((Dir1 + " Should only contain 1 entry.").c_str(), dc.size() == 1);

    for (osgDB::DirectoryContents::const_iterator i = dc.begin(); i != dc.end(); i++) {
        const std::string& s = *i;
        CPPUNIT_ASSERT_MESSAGE((Dir1 + " Should only contain 1 entry and it should be \"Testing1\".").c_str(), s == "Testing1");
    }

    //Testing the delete functionality tests DirGetFiles
    try {
        CPPUNIT_ASSERT_MESSAGE("Deleting an nonexisten Directory should be ok.", fileUtils.DirDelete("gobbletygook", false) == true);
    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_FAIL("Deleting an nonexisten Directory should be ok.");
    }

    //Testing the delete functionality tests DirGetFiles
    try {
        fileUtils.DirDelete(Dir1, false);
    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_FAIL("Deleting non-empty Directory with a non-recursive call should have returned false.");
    }
    CPPUNIT_ASSERT_MESSAGE(Dir1 + " should still exist.", fileUtils.DirExists(Dir1));
    try {
        fileUtils.DirDelete(Dir1, true);
    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_FAIL((ex.What() + ": Deleting non-empty Directory with a non-recursive call should not have generated an Exception.").c_str());
    }
    CPPUNIT_ASSERT_MESSAGE(Dir1 + " should not still exist.", !fileUtils.DirExists(Dir1));
}



core::tree<dtDAL::ResourceTreeNode>::const_iterator ProjectTests::findTreeNodeFromCategory(
    const core::tree<dtDAL::ResourceTreeNode>& currentTree,
    const dtDAL::DataType* dt, const std::string& category) const {

    if (dt != NULL && !dt->IsResource())
        return currentTree.end();

    std::vector<std::string> tokens;
    dtDAL::StringTokenizer<dtDAL::IsCategorySeparator>::tokenize(tokens, category);
    //if dt == NULL, assume that the datatype name is at the front of the category.
    if (dt != NULL)
        //Push the name of the datetype because it's the top level of the tree.
        tokens.insert(tokens.begin(), dt->GetName());

    std::string currentCategory;

    core::tree<dtDAL::ResourceTreeNode>::const_iterator ti = currentTree.tree_iterator();

    for (std::vector<std::string>::const_iterator i = tokens.begin(); i != tokens.end(); ++i) {
        if (ti == currentTree.end())
            return currentTree.end();

        //std::cout << *i << std::endl;

        //keep a full category string running
        //to create an accurate tree node to compare against.
        //Skip the first token because it is the datatype, not the category.
        if (i != tokens.begin())
            if (currentCategory == "")
                currentCategory += *i;
            else
                currentCategory += dtDAL::ResourceDescriptor::DESCRIPTOR_SEPARATOR + *i;

        ti = ti.tree_ref().find(dtDAL::ResourceTreeNode(*i, currentCategory));
    }
    return ti;
}

void ProjectTests::printTree(const core::tree<dtDAL::ResourceTreeNode>::const_iterator& iter) {
    for (unsigned tabs = 0; tabs < iter.level(); ++tabs)
        std::cout << "\t";

    std::cout << iter->getNodeText();
    if (!iter->isCategory())
        std::cout << " -- " << iter->getResource().GetResourceIdentifier();
    else
        std::cout << " -> " << iter->getFullCategory();

    std::cout << std::endl;

    for (core::tree<dtDAL::ResourceTreeNode>::const_iterator i = iter.tree_ref().in();
        i != iter.tree_ref().end();
        ++i) {
        printTree(i);

    }
}

void ProjectTests::testReadonlyFailure() {
    try {
        dtDAL::Project& p = dtDAL::Project::GetInstance();

        std::string projectDir("TestProject");

        try {
            p.SetContext(projectDir);
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_FAIL(std::string("Project should have been able to set context. Exception: ") + e.What());
        }

        try {
            p.SetContext(projectDir, true);
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_FAIL(std::string("Project should have been able to set context. Exception: ") + e.What());
        }

        CPPUNIT_ASSERT_MESSAGE("context should be valid", p.IsContextValid());
        CPPUNIT_ASSERT_MESSAGE("context should be valid", p.IsReadOnly());

        try {
            p.Refresh();
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_FAIL(std::string("Project should have been able to call refresh: ") + e.What());
        }

        try {
            core::tree<dtDAL::ResourceTreeNode> toFill;
            p.GetResourcesOfType(dtDAL::DataType::STATIC_MESH, toFill);
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_FAIL(std::string("Project should have been able to call GetResourcesOfType: ") + e.What());
        }

        try {
            p.getAllResources();
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_FAIL(std::string("Project should have been able to call GetResourcesOfType: ") + e.What());
        }

        try {
            p.DeleteMap("mojo");
            CPPUNIT_FAIL("deleteMap should not be allowed on a readoly context.");
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
                e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
        }

        try {
            p.SaveMap("mojo");
            CPPUNIT_FAIL("deleteMap should not be allowed on a readoly context.");
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
                e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
        }

        try {
            p.SaveMapAs("mojo", "a", "b");
            CPPUNIT_FAIL("deleteMap should not be allowed on a readoly context.");
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
                e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
        }


        try {
            p.AddResource("mojo", std::string("../jojo.ive"),
                std::string("fun:bigmamajama"), dtDAL::DataType::STATIC_MESH);
            CPPUNIT_FAIL("addResource should not be allowed on a readoly context.");
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
                e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
        }

        try {
            p.RemoveResource(dtDAL::ResourceDescriptor("",""));
            CPPUNIT_FAIL("removeResource should not be allowed on a readoly context.");
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
                e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
        }

        try {
            p.CreateResourceCategory("name-o", dtDAL::DataType::STRING);
            CPPUNIT_FAIL("createResourceCategory should not be allowed on a readoly context.");
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
                e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
        }

        try {
            p.RemoveResourceCategory("name-o", dtDAL::DataType::SOUND, true);
            CPPUNIT_FAIL("removeResourceCategory should not be allowed on a readoly context.");
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
                e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
        }

        try {
            p.CreateMap("name-o", "testFile");
            CPPUNIT_FAIL("createMap should not be allowed on a readoly context.");
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
                e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
        }

        try {
            p.ClearBackup("name-o");
            CPPUNIT_FAIL("clearBackup should not be allowed on a readoly context.");
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
                e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
        }
    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_FAIL(ex.What());
    }
}

void ProjectTests::testCategories() {
    try {
        dtDAL::Project& p = dtDAL::Project::GetInstance();

        dtDAL::FileUtils& fileUtils = dtDAL::FileUtils::GetInstance();

        std::string projectDir("TestProject");

        try {
            p.SetContext(projectDir);
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_FAIL(std::string(std::string("Project should have been able to set context. Exception: ") + e.What()).c_str());
        }

        for (std::vector<dtUtil::Enumeration*>::const_iterator i = dtDAL::DataType::Enumerate().begin();
            i != dtDAL::DataType::Enumerate().end(); ++i) {
            dtDAL::DataType& d = *static_cast<dtDAL::DataType*>(*i);

            //don't index the first time so it will be tested both ways.
            if (i != dtDAL::DataType::Enumerate().begin())
                p.getAllResources();

            if (!d.IsResource()) {
                try {
                    p.CreateResourceCategory("littleFoot", d);
                    CPPUNIT_FAIL("Project should not be able to create a category for a primitive type.");
                } catch (const dtDAL::Exception&) {
                    //correct
                }
            } else {
                p.CreateResourceCategory("abomb", d);

                CPPUNIT_ASSERT_MESSAGE(
                    "attempting to remove a simple category should succeed.",
                    p.RemoveResourceCategory("abomb", d, false));

                p.CreateResourceCategory("abomb:hbomb", d);

                CPPUNIT_ASSERT_MESSAGE(
                    "attempting to remove a simple category should succeed.",
                    p.RemoveResourceCategory("abomb:hbomb", d, false));

                std::string catPath(p.GetContext() + dtDAL::FileUtils::PATH_SEPARATOR
                    + d.GetName() + dtDAL::FileUtils::PATH_SEPARATOR + "abomb");

                CPPUNIT_ASSERT_MESSAGE("Static mesh category abomb should exist.",
                    fileUtils.DirExists(catPath));

                p.CreateResourceCategory("abomb:jojo:eats:hummus", d);

                std::string longPath(catPath + dtDAL::FileUtils::PATH_SEPARATOR + "jojo"
                    + dtDAL::FileUtils::PATH_SEPARATOR + "eats"
                    + dtDAL::FileUtils::PATH_SEPARATOR + "hummus");

                CPPUNIT_ASSERT_MESSAGE(std::string("Static mesh category ") + longPath + " should exist.",
                    fileUtils.DirExists(longPath));
                //printTree(p.GetAllResources());
                CPPUNIT_ASSERT(p.RemoveResourceCategory("abomb:jojo:eats:hummus", d, false));
                CPPUNIT_ASSERT_MESSAGE(std::string("Static mesh category ") + longPath + " should NOT exist.",
                    !fileUtils.DirExists(longPath));

                CPPUNIT_ASSERT_MESSAGE(
                    "Attempting to non-recursivly remove a category with contents should return false.",
                    !p.RemoveResourceCategory("abomb:jojo", d, false));

                CPPUNIT_ASSERT_MESSAGE("Static mesh category abomb should exist.",
                    fileUtils.DirExists(catPath + dtDAL::FileUtils::PATH_SEPARATOR + "jojo"
                    + dtDAL::FileUtils::PATH_SEPARATOR + "eats"));

                CPPUNIT_ASSERT(p.RemoveResourceCategory("abomb", d, true));
                CPPUNIT_ASSERT_MESSAGE("Static mesh category abomb should not exist.",
                    !fileUtils.DirExists(catPath));
            }
        }



    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_FAIL(ex.What());
    }

}

void ProjectTests::testResources() {
    try {
        dtDAL::Project& p = dtDAL::Project::GetInstance();

        dtDAL::FileUtils& fileUtils = dtDAL::FileUtils::GetInstance();

        //Open an existing project.
        std::string projectDir = "WorkingProject";

        try {
            p.SetContext(projectDir);
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_FAIL((std::string("Project should have been able to Set context. Exception: ")
                + e.What()).c_str());
        }

        CPPUNIT_ASSERT_MESSAGE("Project should not be read only.", !p.IsReadOnly());

        const std::set<std::string>& mapNames = p.GetMapNames();

        std::vector<osg::ref_ptr<dtDAL::Map> > maps;

        time_t currentTime;
        time(&currentTime);
        const std::string& utcTime = dtDAL::MapWriter::TimeAsUTC(currentTime);

        logger->LogMessage(dtDAL::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
            "Current time as UTC is %s", utcTime.c_str());

        std::string testResult;

        try {
            p.AddResource("mojo", std::string("../jojo.ive"),
                std::string("fun:bigmamajama"), dtDAL::DataType::STATIC_MESH);
            CPPUNIT_FAIL("The add resource call to add a non-existent file should have failed.");
        } catch (const dtDAL::Exception& ex) {
            CPPUNIT_ASSERT_MESSAGE(ex.What().c_str(), ex.TypeEnum() == dtDAL::ExceptionEnum::ProjectFileNotFound);
            //correct otherwise
        }

        try {
            p.AddResource("dirt", std::string("../dirt.ive"),
                std::string("fun:bigmamajama"), dtDAL::DataType::BOOLEAN);
            CPPUNIT_FAIL("The add resource call to add boolean should have failed.");
        } catch (const dtDAL::Exception& ex) {
            //should not allow a boolean resource to be added.
            CPPUNIT_ASSERT_MESSAGE(ex.What().c_str(), ex.TypeEnum() == dtDAL::ExceptionEnum::ProjectResourceError);
            //correct otherwise
        }

        std::string dirtCategory = "fun:bigmamajama";

        //add one marine before indexing
        dtDAL::ResourceDescriptor marineRD = p.AddResource("marine", "../marine.rbody", "",
            dtDAL::DataType::CHARACTER);

        //force resources to be indexed.
        p.getAllResources();

        //add one marine after indexing
        dtDAL::ResourceDescriptor marine2RD = p.AddResource("marine2", "../marine.rbody", "marine",
            dtDAL::DataType::CHARACTER);


        core::tree<dtDAL::ResourceTreeNode> toFill;
        p.GetResourcesOfType(dtDAL::DataType::CHARACTER, toFill);
        core::tree<dtDAL::ResourceTreeNode>::const_iterator marineCategory =
            findTreeNodeFromCategory(toFill, NULL, "");

        CPPUNIT_ASSERT_MESSAGE(std::string("the category \"marine")
            + "\" should have been found in the resource tree", marineCategory != p.getAllResources().end());

        core::tree<dtDAL::ResourceTreeNode>::const_iterator marineResource =
            marineCategory.tree_ref().find(dtDAL::ResourceTreeNode("marine.rbody", marineCategory->getFullCategory(), &marineRD));


        marineCategory =
            findTreeNodeFromCategory(toFill, NULL, "marine");

        core::tree<dtDAL::ResourceTreeNode>::const_iterator marine2Resource =
            marineCategory.tree_ref().find(dtDAL::ResourceTreeNode("marine2.rbody", marineCategory->getFullCategory(), &marine2RD));

        CPPUNIT_ASSERT_MESSAGE("The marine resource should have been found.", marineResource != p.getAllResources().end());
        CPPUNIT_ASSERT_MESSAGE("The marine resource should have been found.", marine2Resource != p.getAllResources().end());

        //Done with the marines

        dtDAL::ResourceDescriptor rd = p.AddResource("dirt", std::string("../flatdirt.ive"),
            dirtCategory, dtDAL::DataType::STATIC_MESH);

        CPPUNIT_ASSERT_MESSAGE("Descriptor id should not be empty.", !rd.GetResourceIdentifier().empty());

        testResult = p.GetResourcePath(rd);

        CPPUNIT_ASSERT_MESSAGE("Getting the resource path returned the wrong value: " + testResult,
            testResult == dtDAL::DataType::STATIC_MESH.GetName() + "/fun/bigmamajama/dirt.ive");


        for (std::set<std::string>::const_iterator i = mapNames.begin(); i != mapNames.end(); i++) {
            logger->LogMessage(dtDAL::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found map named %s.", i->c_str());
            //dtDAL::Map& m = p.GetMap(*i);

            //maps.Push_back(osg::ref_ptr<dtDAL::Map>(&m));
        }

        p.GetResourcesOfType(dtDAL::DataType::STATIC_MESH, toFill);

        CPPUNIT_ASSERT_MESSAGE("The head of the tree should be static mesh",
            toFill.data().getNodeText() == dtDAL::DataType::STATIC_MESH.GetName());

        core::tree<dtDAL::ResourceTreeNode>::const_iterator treeResult =
            findTreeNodeFromCategory(p.getAllResources(),
            &dtDAL::DataType::STATIC_MESH, dirtCategory);

        printTree(toFill.tree_iterator());

        CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + dirtCategory
            + "\" should have been found in the resource tree", treeResult != p.getAllResources().end());


        CPPUNIT_ASSERT_MESSAGE(std::string("the resource \"") + rd.GetResourceIdentifier()
            + "\" should have been found in the resource tree",
            treeResult.tree_ref().find(dtDAL::ResourceTreeNode(rd.GetDisplayName(), dirtCategory, &rd))
            != p.getAllResources().end());

        p.RemoveResource(rd);

        treeResult = findTreeNodeFromCategory(p.getAllResources(),
            &dtDAL::DataType::STATIC_MESH, dirtCategory);

        CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + dirtCategory
            + "\" should have been found in the resource tree", treeResult != p.getAllResources().end());

        CPPUNIT_ASSERT_MESSAGE(std::string("the resource \"") + rd.GetResourceIdentifier()
            + "\" should have NOT been found in the resource tree",
                 treeResult.tree_ref().find(dtDAL::ResourceTreeNode(rd.GetDisplayName(), dirtCategory,  &rd))
                    == p.getAllResources().end());

        CPPUNIT_ASSERT(!p.RemoveResourceCategory("fun", dtDAL::DataType::STATIC_MESH, false));
        CPPUNIT_ASSERT(p.RemoveResourceCategory("fun", dtDAL::DataType::STATIC_MESH, true));

        treeResult = findTreeNodeFromCategory(p.getAllResources(),
            &dtDAL::DataType::STATIC_MESH, dirtCategory);
        CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + dirtCategory
            + "\" should not have been found in the resource tree", treeResult == p.getAllResources().end());

        treeResult = findTreeNodeFromCategory(p.getAllResources(),
            &dtDAL::DataType::STATIC_MESH, "fun");
        CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + "fun"
            + "\" should not have been found in the resource tree", treeResult == p.getAllResources().end());

        rd = p.AddResource("pow", std::string("../pow.wav"), std::string("tea:money"), dtDAL::DataType::SOUND);
        testResult = p.GetResourcePath(rd);

        CPPUNIT_ASSERT_MESSAGE("Getting the resource path returned the wrong value: " + testResult ,
            testResult == dtDAL::DataType::SOUND.GetName() + "/tea/money/pow.wav");

        dtDAL::ResourceDescriptor rd1 = p.AddResource("bang", std::string("../bang.wav"),
            std::string("tee:cash"), dtDAL::DataType::SOUND);
        testResult = p.GetResourcePath(rd1);

        CPPUNIT_ASSERT_MESSAGE("Getting the resource path returned the wrong value: " + testResult,
            testResult == dtDAL::DataType::SOUND.GetName() + "/tee/cash/bang.wav");

        p.Refresh();

        //const core::tree<dtDAL::ResourceTreeNode>& allTree = p.GetAllResources();

        //printTree(allTree.tree_iterator());

        p.RemoveResource(rd);
        p.RemoveResource(rd1);
        p.RemoveResource(marineRD);
        p.RemoveResource(marine2RD);

        fileUtils.PushDirectory(projectDir);
        CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtDAL::DataType::STATIC_MESH.GetName() + std::string("/fun/bigmamajama/dirt.ive")));
        CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtDAL::DataType::SOUND.GetName() + std::string("/tea/money/pow.wav")));
        CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtDAL::DataType::SOUND.GetName() + std::string("/tee/cash/bang.wav")));
        CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtDAL::DataType::CHARACTER.GetName() + std::string("/marine.rbody")));
        CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtDAL::DataType::CHARACTER.GetName() + std::string("/marine/marine2.rbody")));
        fileUtils.PopDirectory();

        //this should work fine even if the file is deleted.
        p.RemoveResource(rd);

    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_FAIL(ex.What());
    }

}


void ProjectTests::testProject() {

    try {
        dtDAL::Project& p = dtDAL::Project::GetInstance();

        dtDAL::FileUtils& fileUtils = dtDAL::FileUtils::GetInstance();

        try {
            p.SetContext(std::string("/:**/../^^jojo/funky/\\\\/,/,.uchor"));
            CPPUNIT_FAIL("Project should not have been able to Set context.");
        } catch (const dtDAL::Exception&) {
            //correct
        }

        std::string projectDir("TestProject");

        if (osgDB::fileExists(projectDir)) {
            try {
                fileUtils.DirDelete(projectDir, true);
            } catch (const dtDAL::Exception& ex) {
                CPPUNIT_FAIL(ex.What().c_str());
            }

            CPPUNIT_ASSERT_MESSAGE("The project Directory should not yet exist.", !osgDB::fileExists(projectDir));
        }

        fileUtils.MakeDirectory(projectDir);

        try {
            p.SetContext(projectDir, true);
            CPPUNIT_FAIL("Project should not have been able to Set the readonly context because it is empty.");
        } catch (const dtDAL::Exception& e) {
            //correct
        }

        try {
            p.SetContext(projectDir);
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_FAIL(std::string(std::string("Project should have been able to Set context. Exception: ") + e.What()).c_str());
        }

        CPPUNIT_ASSERT_MESSAGE("Project should not be read only.", !p.IsReadOnly());

        try {
            p.SetContext(projectDir, true);
        } catch (const dtDAL::Exception& e) {
            CPPUNIT_FAIL(std::string(std::string("Project should have been able to Set context. Exception: ") + e.What()).c_str());
        }

        CPPUNIT_ASSERT_MESSAGE("Project should be read only.", p.IsReadOnly());


        try {
            fileUtils.DirDelete(projectDir, true);
        } catch (const dtDAL::Exception& ex) {
            CPPUNIT_FAIL(ex.What().c_str());
        }

        CPPUNIT_ASSERT_MESSAGE("The project Directory should have been deleted.", !osgDB::fileExists(projectDir));

    } catch (const dtDAL::Exception& ex) {
        CPPUNIT_FAIL(ex.What());
    }

}



