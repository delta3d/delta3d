/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 13:47
 *
 * @file job_jardata.h
 * @version 1.0
 */
#ifndef PACKAGER_JOB_JAR_DATA
#define PACKAGER_JOB_JAR_DATA

// local
#include "job_jar.h"
// ansi
#include <string>


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;


//======================================
// CLASS
//======================================

class JobJARData : public JobJAR
{
   public:
      JobJARData();
      ~JobJARData();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      void Execute( PackageProfile *profile );

   private:
      void appendAllFiles(const std::string &inputDir, 
                          const std::string &srcDir,
                          std::string &listOfFiles, 
                          const std::string &ignore );
};

#endif
