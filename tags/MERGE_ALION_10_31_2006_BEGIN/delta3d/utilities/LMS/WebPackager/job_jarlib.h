/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 13:44
 *
 * @file job_jarlib.h
 * @version 1.0
 */
#ifndef PACKAGER_JOB_JAR_LIBRARY
#define PACKAGER_JOB_JAR_LIBRARY

// local
#include "job_jar.h"


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;


//======================================
// CLASS
//======================================

class JobJARLibrary : public JobJAR
{
   public:
      JobJARLibrary();
      ~JobJARLibrary();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      void Execute( PackageProfile *profile );

   private:
};

#endif
