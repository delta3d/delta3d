/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/13 - 9:40
 *
 * @file job_deployment.h
 * @version 1.0
 */
#ifndef PACKAGER_JOB_DEPLOYMENT
#define PACKAGER_JOB_DEPLOYMENT

// local
#include "job_base.h"
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

class JobDeployment : public JobBase
{
   public:
      JobDeployment();
      ~JobDeployment();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      virtual void Execute( PackageProfile *profile );

   private:
      void copyAllFiles(const std::string &srcDir, const std::string &destDir, const std::string &ignore);
};

#endif
