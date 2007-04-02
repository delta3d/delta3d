#ifndef _dt_util_file_finder_h_
#define _dt_util_file_finder_h_

#include <osgDB/FileUtils>
#include <dtUtil/fileutils.h>
#include <osgDB/FileNameUtils>             // for file extension code

namespace dtDIS
{
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
   }
}

#endif   // _dt_util_file_finder_h_
