/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_FILE_FINDER_H__
#define __DELTA_DTDIS_FILE_FINDER_H__

#include <osgDB/FileUtils>
#include <dtUtil/fileutils.h>
#include <osgDB/FileNameUtils>             // for file extension code

namespace dtDIS
{
   ///@cond DOXYGEN_SHOULD_SKIP_THIS
   namespace details
   {
      /// appends a container with file names if the extension matches
      template<class AppendT>
      struct AppendIfHasExtension
      {
         AppendIfHasExtension(const std::string& ext, AppendT* container)
            : mExt(ext)
            , mContainer(container)
         {
         }

         void operator ()(const std::string& file)
         {
            if( mExt == osgDB::getFileExtension( file ) )
            {
               mContainer->push_back( file );
            }
         }

      private:
         std::string mExt;
         AppendT* mContainer;
      };

      /// appends a container with file names for DLLs.
      struct DLLFinder
      {
         template<class AppendT>
            void operator ()(const std::string& dir, AppendT& container)
         {
            dtUtil::FileUtils& filefinder = dtUtil::FileUtils::GetInstance();

            // search the plugin directory for potential plugins
            if( filefinder.DirExists(dir) )
            {
               osgDB::DirectoryContents files = filefinder.DirGetFiles( dir );

               // filter out based on extension
#ifdef WIN32
               std::string ext("dll");
#else
               std::string ext("so");
#endif

               dtDIS::details::AppendIfHasExtension<AppendT> predicate( ext , &container );
               std::for_each( files.begin(), files.end(), predicate );
            }
         }
      };
   } // end namespace details
   ///@endcond
}

#endif   // __DELTA_DTDIS_FILE_FINDER_H__
