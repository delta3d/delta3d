/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 13:43
 *
 * @file job_jnlp.h
 * @version 1.0
 * @brief Description..
 */
#ifndef PACKAGER_JOB_JNLP
#define PACKAGER_JOB_JNLP

// local
#include "job_xml.h"
// ansi
#include <string>


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;

// constants
#define JNLP_FILENAME   "ApplicationLaunch.jnlp"


//======================================
// CLASS
//======================================

class JobJNLP : public JobXML
{
   public:
      JobJNLP();
      ~JobJNLP();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      void Execute( PackageProfile *profile );

   private:
      void removeSpaces( std::string &str );
};

#endif
