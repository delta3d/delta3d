/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 13:52
 *
 * @file job_launchpage.h
 * @version 1.0
 */
#ifndef PACKAGER_JOB_LAUNCHPAGE
#define PACKAGER_JOB_LAUNCHPAGE

// local
#include "job_html.h"


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;


//======================================
// CLASS
//======================================

class JobLaunchPage : public JobHTML
{
   public:
      JobLaunchPage();
      ~JobLaunchPage();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      void Execute( const PackageProfile *profile );

   private:
};

#endif
