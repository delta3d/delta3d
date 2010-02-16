/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 10:16
 *
 * @file package_profile.h
 * @version 1.0
 * @brief Wraps up the package profile configuration file.  Does so by loading
 * the package profile ( an XML file ) and stores its DOM hierarchy as a member
 * variable.  The class also contains wrapper functions for extracting useful
 * chunks of information out of the stored XML file.
 * @note Uses Xerces DOM api for parsing the XML files.
 */
#ifndef PACKAGER_PACKAGE_PROFILE
#define PACKAGER_PACKAGE_PROFILE

// xerces
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XercesVersion.hpp>
// ansi
#include <string>
#include <vector>

XERCES_CPP_NAMESPACE_USE


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class Options;
XERCES_CPP_NAMESPACE_BEGIN
class DOMBuilder;
class DOMDocument;
XERCES_CPP_NAMESPACE_END

#ifndef XMLNode
   typedef DOMNode XMLNode;
#endif


//======================================
// CLASS
//======================================

class PackageProfile
{
   public:
      enum TargetPlatform
      {
	TP_WINDOWS,
	TP_LINUX
      };

      enum LibraryType
      {
         LIB_DEPENDENCY,
         LIB_LAUNCHER,
         LIB_GAME
      };

      enum JARFileType
      {
         JAR_DATA,
         JAR_LIBRARY
      };

      enum PackageProfileFlag
      {
         FLAG_SCORM_COMPLIANT    = 0x01,
         FLAG_PAD_1              = 0x02,
         FLAG_PAD_2              = 0x04,
         FLAG_PAD_3              = 0x08,
         FLAG_PAD_4              = 0x10,
         FLAG_PAD_5              = 0x20,
         FLAG_PLATFORM_WINDOWS   = 0x40,
         FLAG_PLATFORM_LINUX     = 0x80
      };      

   public:
      PackageProfile();
      ~PackageProfile();

      //-------------------------------
      // STARTUP FUNCTIONS
      //-------------------------------

      /**
       * Loads the package profile configuration (XML file).
       * @param filename The package profile to load.
       */
      bool LoadPackageProfile(const std::string &filename );

      /**
       * Cleans up any used memory.
       */
      void ReleasePackageProfile();

      //-------------------------------
      // COMMON ACCESSORS
      //-------------------------------

      /**
       * Set the output string.
       * @param output The output directory/file string.  Relative or absolute.
       */
      void SetOutput(const std::string &output );

      /**
       * Gets the directory path of the output.
       * @return A clean, valid directory string.
       */
      std::string GetOutputDirectory() const;

      /**
       * Query whether or not the output should be SCORM compliant.
       * @return True if SCORM info specified in profile.
       */
      bool IsSCORMCompliant() const;

      /**
       * Query whether or not the app supports the specified platform.
       * @param x One of the platforms defined above.
       * @return True if the specified platform is supported.
       */
      bool IncludePlatform( TargetPlatform x ) const;

      /**
       * Query the URL of the web package.
       * @return The URL specified in the package profile.
       */
      std::string GetURL();

      /**
       * Query the keystore file.
       * @return The keystore file used for signing JAR files.
       */
      std::string GetKeystoreFile();

      /**
       * Query the keystore key.
       * @return The keystore key used for signing JAR files.
       */
      std::string GetKeystoreKey();

      /**
       * Query the keystore password;
       * @return The keystore password used for signing JAR files.
       */
      std::string GetKeystorePassword();

      /**
       * Sets the signing password.
       * @param password Used for signing JAR files.
       */
      void SetKeystorePassword(const std::string &password);

      /**
       * Query the main application name.
       * @return The application name specified in the package profile.
       */
      std::string GetApplicationName();

      /**
       * Query the main application vendor.
       * @return The application vendor specified in the package profile.
       */
      std::string GetApplicationVendor();

      /**
       * Query the general launch parameters for the application.
       * @return String representing general application launch parameters,
       * exactly as they would be fed into a system prompt.
       * @note As of right now these parameters are stored in the package
       * profile, and not actually called from the system prompt.
       */
      std::string GetApplicationLaunchParams();

      //-------------------------------
      // JOB UTILITY FUNCTIONS
      //-------------------------------
      
      /**
       * Query the input directory defined under the specified chunk.
       * @return The input directory as a string.
       */
      std::string GetDeploymentDirectory();

      /**
       * Query the filename of the Java Messaging Applet.
       * @return The filename of the Java Applet.
       */
      std::string GetJavaApplet();

      /**
       * Query the filename of the Java launching program.
       * @return The name of the launch program.
       */
      std::string GetJavaLaunchProgram();      

      /**
       * Query the filename of the Java launching program's config file.
       * @return The config filename for the Java launching program.
       */
      std::string GetJavaLaunchConfig();

      /**
       * Query the main class name inside of the Java launching program.
       * @return The full class name including package string.
       */
      std::string GetJavaLaunchMainClass();

      /**
       * Query the filename of the html Launch Page.
       * @return The filename of the launch page.
       */
      std::string GetLaunchPage();

      /**
       * Query for all of the profile InputDirectory nodes within the Deployment node.
       * @param elements A collection of InputDirectory nodes within the Deployment node.
       */
      void GetDeploymentInputDirectoryElements( std::vector<const XMLNode*> &elements);

      /**
       * Query for the list of files that need to be deployed.
       * @param inputElement The InputDirectory node to get the deployment contents from.
       * @param files A collection of filenames returned.
       * @param options A collection of options per file.
       * @note Both vectors are the same size.
       */
      void GetDeploymentContents( const XMLNode *inputElement,
                                  std::vector<std::string> &files,
                                  std::vector<Options> &options );

      /**
       * Query for a list of libraries specified by type.
       * @param libType Type of libraries we are looking for.
       * @param files A collection of filenames returned.
       * @param options A collection of options per file.
       * @note Both returned vectors are the same size.
       */
      void GetLibraries( LibraryType libType, std::vector<std::string> &files,
                                              std::vector<Options> &options,
                                              TargetPlatform targetPlatform);

      /**
       * Retrieve the main entry point library for the type of libs we want.
       * @param libType Type of libraries we are looking for.
       * @param lib The returned library filename.
       * @param options The returned options for the entry point lib.
       */
      void GetEntryPointLibrary( LibraryType libType, std::string &lib,
                                                      Options &options,
                                                      TargetPlatform targetPlatform);

      /**
       * Query for the list of library JAR files. This is merely the names of
       * the JAR files themselves and any options specified per JAR file.
       * @param files A collection of JAR filenames.
       * @param options A collection of options per JAR file.
       * @note Both vectors are the same size.
       */
      void GetJARFiles( JARFileType jarType, std::vector<std::string> &files,
                                             std::vector<Options> &options );

      /**
       * Query for the contents of a particular JAR file.
       * @param jarFilename The JAR file we are interested in.
       * @param inputDir Returned string. The input directory of JAR contents.
       * @param files A collection of filenames, the contents of the JAR file.
       * @param options A collection of options per file in the JAR file.
       * @note Both vectors are the same size.
       */
      void GetJARFileContents( JARFileType jartype, std::string jarFilename,
                               std::string &inputDir,
                               std::vector<std::string> &files,
                               std::vector<Options> &options );

      /**
       * Query for the operating system this jar is meant for.
       * @param jarType The type of jar file to look for (JAR_DATA or JAR_LIBRARY).
       * @param jarFilename The JAR file we are interested in.
       * @return A string containing the operating system ("windows" or "linux").
       */
      std::string GetJarFilePlatform( JARFileType jarType, std::string jarFilename );

      /**
       * Gets all elements inside the SCORM chunk.
       * @param elements List of elements inside the SCORM chunk.
       * @note This is so the SCORM section of the profile can support more than
       * one element.
       */
      void GetSCORMContents( std::vector<const XMLNode*> &elements );
      
      /**
       * Gets the number of Sharable Content Objects in the SCORM section.
       * @return 0 if no SCOs found, positive number otherwise.
       */
      unsigned int GetSCORM_SCO_Count();

      /**
       * Gets the attributes for the specified SCORM SCO.
       * @param scoIdx The SCORM Sharable Content Object we are interested in.
       * @param options The attributes specified in the 'sco' element.
       */
      void GetSCORM_SCO_Options( unsigned int scoIdx, Options &options );

      /**
       * Gets the name of the launch page for the specified SCORM SCO.
       * @param scoIdx The SCORM Sharable Content Object we are interested in.
       * @param launchPage The returned launch page name.
       */
      void GetSCORM_SCO_LaunchPage( unsigned int scoIdx, std::string &launchPage );

      /**
       * Gets a string with all the launch parameters for the specified SCORM SCO.
       * @param scoIdx The SCORM Sharable Content Object we are interested in.
       * @param launchParams A string representing the SCO's parameters.
       */
      void GetSCORM_SCO_LaunchParams( unsigned int scoIdx, std::string &launchParams );

      /**
       * Gets all the 'objective' elements for the specified SCORM SCO.
       * @param scoIdx The SCORM Sharable Content Object we are interested in.
       * @param objectives The objectives themselves.
       * @param options The attributes specified per objective.
       */
      void GetSCORM_SCO_Objectives( unsigned int scoIdx,
                                    std::vector<std::string> &objectives,
                                    std::vector<Options> &options );

      /**
       * Retrieve a chunk of the package profile by element name.
       * @param elementName Name of root element of desired chunk.
       * @return The returned chunk if found, NULL otherwise.
       */
      const XMLNode *GetChunk(const std::string &elementName);

      std::string GetElementAttribute( const XMLNode *element, std::string attrName ) const;

   private:
      void unload();
      void setFlag( PackageProfileFlag flag, bool on = true );
      bool getFlag( PackageProfileFlag flag ) const;      
      const XMLNode *getRoot() const;
      const XMLNode *getElement( const XMLNode *parent, std::string name );
      const XMLNode *getElementBFS( const XMLNode *parent, std::string name );
      std::string getElementContent( const XMLNode *element ) const;
      //std::string getElementAttribute( const XMLNode *element, std::string attrName ) const;
      const XMLNode *findSCORMSCO( unsigned int scoIdx );
      void findSCORMSCOObjectives( const XMLNode *scoElement,
                                   std::vector<std::string> &objectives,
                                   std::vector<Options> &options );

   private:
#if XERCES_VERSION_MAJOR >= 3
      DOMLSParser*                 mParser;
#else
      DOMBuilder*                  mParser;
#endif
      DOMDocument*                 mDocument;
      std::vector<const XMLNode*>  mXmlSearchQueue;
      std::string                  mOutput;
      std::string                  mPassword;
      unsigned char                mFlags;      
};

#endif
