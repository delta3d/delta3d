/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 10:06
 *
 * @file package_builder.h
 * @version 1.0
 * @brief Main class that kicks-off all the batch jobs.  There should only be
 * one instance of this class and it should contain all the jobs as members or
 * start them dynamically.
 */
#ifndef PACKAGER_PACKAGE_BUILDER
#define PACKAGER_PACKAGE_BUILDER

// ansi
#include <string>
#include <vector>


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;
class JobBase;


//======================================
// CLASS
//======================================

class PackageBuilder
{
   public:
      PackageBuilder();
      ~PackageBuilder();

      /**
       * Initializes the builder.
       * @note Should only be called once.
       */
      void Initialize();

      /**
       * Shuts down the builder.
       * @note Should only be called once.
       */
      void Shutdown();
   
      /**
       * Kicks off the building process.
       * @param inputfile The package profile to load.
       * @param output The final resulting output directory or file
       */
      void Build(const std::string &inputfile, const std::string &output);

   private:
      PackageProfile        *mProfile;
      std::vector<JobBase*>  mJobs;
};

#endif
