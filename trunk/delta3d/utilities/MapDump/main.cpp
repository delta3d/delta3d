/* -*-c++-*-
 * MapDump - main (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2010, MOVES Institute
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Erik Johnson
 */

///Utility to load a Map and save out the resulting Scene geometry
///into a single geometry file.
/// Examples
///     MapDump.exe "c:/DemoMap" MyCoolMap output.osg
///            will write MyCoolMap into output.osg
///     MapDump.exe "c:/DemoMap" MyCoolMap 
///            will write MyCoolMap into MyCoolMap.ive

#include <dtUtil/log.h>
#include <dtCore/project.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/scene.h>
#include <dtCore/map.h>
#include <osgDB/ReaderWriter>
#include <osgDB/Registry>

void usage(const std::string& progName)
{
   LOG_ALWAYS("usage:" + progName + " <Project Context Path> <Map Name> [outputFile.ext]");
}

int main(int argc, char** argv)
{
   if (argc<3)
   {
      usage(std::string(argv[0]));
      return 1;
   }

   const std::string contextPath(argv[1]);
   const std::string mapName(argv[2]);
   std::string outputFilename(mapName + ".ive");

   if (argc > 3)
   {
      outputFilename = std::string(argv[3]);
   }

   try
   {
      dtCore::Project::GetInstance().SetContext(contextPath, true);
   }
   catch (dtCore::ProjectInvalidContextException& e)
   {
      LOG_ERROR("Could not load project context");
      e.LogException();
      return 1;
   }

   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
   try
   {
      dtCore::Project::GetInstance().LoadMapIntoScene(mapName, *scene);
   }
   catch (const dtCore::ProjectFileNotFoundException& e)
   {
      e.LogException();
      return 1;
   }

   osgDB::ReaderWriter::WriteResult result = osgDB::Registry::instance()->writeNode(*scene->GetSceneNode(),
                                                                                    outputFilename,
                                                                                    osgDB::Registry::instance()->getOptions());
   if (result.success())
   {
      LOG_ALWAYS("Scene written to: " + outputFilename);
   }
   else if  (result.message().empty())
   {
      LOG_WARNING("File write to '" + outputFilename + "' no supported.");
   }
   else
   {
      LOG_ALWAYS(result.message());
   }

   dtCore::Project::GetInstance().CloseAllMaps(true);
   return 0;
}
