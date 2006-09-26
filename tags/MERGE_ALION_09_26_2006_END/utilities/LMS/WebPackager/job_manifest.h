/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 13:50
 *
 * @file job_manifest.h
 * @version 1.0
 */
#ifndef PACKAGER_JOB_MANIFEST
#define PACKAGER_JOB_MANIFEST

// local
#include "job_xml.h"
// ansi
#include <string>
#include <vector>


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;


//======================================
// CLASS
//======================================

class JobManifest : public JobXML
{
   public:
      JobManifest();
      ~JobManifest();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      void Execute( PackageProfile *profile );

   private:
      void collectAllFiles( std::string srcDir, std::string relativeDir, std::vector<std::string> &filenames );
};

#endif
