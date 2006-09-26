/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 13:41
 *
 * @file job_xml.h
 * @version 1.0  
 * @brief This class wraps the functionality for creating, writing &
 * serializing an XML document.
 */
#ifndef PACKAGER_JOB_XML
#define PACKAGER_JOB_XML

// local
#include "job_sgml.h"
// xerces
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
// ansi
#include <string>

XERCES_CPP_NAMESPACE_USE


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;
XERCES_CPP_NAMESPACE_BEGIN
class DOMDocument;
XERCES_CPP_NAMESPACE_END

#ifndef XMLNode
   typedef DOMNode XMLNode;
#endif


//======================================
// CLASS
//======================================

class JobXML : public JobSGML
{
   public:
      JobXML();
      ~JobXML();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      virtual void Execute( PackageProfile *profile );

      //-------------------------------
      // JOB FUNCTIONS
      //-------------------------------

      /**
       * Create a new XML DOM document in memory.  We can edit this DOM in
       * memory and then save it out.
       * @param rootName Name of the root element.
       * @return The root node element.
       * @note There can only be one document at a time.
       */
      XMLNode *CreateDocument( std::string rootName );

      /**
       * Shutdown the document to release used memory.
       */
      void CloseDocument();

      /**
       * Save a copy of the document out to a file by the specified name.
       * @param strFile The name of the file we are saving to.
       */
      void SaveToFile( std::string strFile );

      /**
       * Create new element and attach it to the specified parent node, whether
       * that be the root of the document or another element.
       * @param parent The element we are adding this new element to.
       * @param element What to name the newly created element.
       * @param content Optional text content inside the newly created element.
       * @return The element node we just created.
       */
      XMLNode *AddElement( XMLNode *parent, std::string element, std::string content = "" );

      /**
       * Add a copy of an element to another element.
       * @param parent The parent element we are adding this element to.
       * @param element The element we are adding.
       * @return The element we just added.
       */
      XMLNode *AddElement( XMLNode *parent, const XMLNode *element );

      /**
       * Create new comment and attach it to the specified parent node.
       * @param parent The element we are adding this comment to.
       * @param comment A string representing the contents of the comment.
       */
      void AddComment( XMLNode *parent, std::string comment );

      /**
       * Set the text content of a particular element.
       * @param element The element we are setting.
       * @param content The text content we want to fill in.
       */
      void SetContent( XMLNode *element, std::string content );

      /**
       * Set a particular attribute of an element.
       * @param element The element we are setting.
       * @param attribute The name of the attribute we are setting.
       * @param value The value to set the specified attribute.
       */
      void SetAttribute( XMLNode *element, std::string attribute, std::string value );
      
   private:
      void unload();
      XMLNode *deepCopyElement( XMLNode **copy, const XMLNode *original );

   private:
      DOMImplementation *mDOMImplementor;
      DOMDocument       *mDocument;
};

#endif
