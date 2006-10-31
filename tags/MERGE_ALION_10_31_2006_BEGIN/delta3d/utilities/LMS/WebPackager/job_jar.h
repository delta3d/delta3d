/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 13:35
 *
 * @file job_jar.h
 * @version 1.0
 */
#ifndef PACKAGER_JOB_JAR
#define PACKAGER_JOB_JAR

// local
#include "job_base.h"


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;


//======================================
// CLASS
//======================================

class JobJAR : public JobBase
{
   public:
      JobJAR();
      ~JobJAR();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      virtual void Execute( PackageProfile *profile );

      /**
       * Sign the JAR file for security and permissions.
       */
      void Sign();

   private:
};

#endif
