/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 13:55
 *
 * @file job_launchconfig.h
 * @version 1.0 
 */
#ifndef PACKAGER_JOB_LAUNCH_CONFIG
#define PACKAGER_JOB_LAUNCH_CONFIG

// local
#include <job_xml.h>


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;


//======================================
// CLASS
//======================================

class JobLaunchConfig : public JobXML
{
   public:
      JobLaunchConfig();
      ~JobLaunchConfig();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      void Execute( PackageProfile *profile );

   private:
      std::string removeDebugCharFromFilename( const std::string &filename );
      std::string removeLibAndExtension( const std::string &filename );
};

#endif
