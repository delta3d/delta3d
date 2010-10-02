#ifndef _DOC_BROWSER_XML_MANAGER_H__
#define _DOC_BROWSER_XML_MANAGER_H__

#include <vector>
#include <string>
#include <stack>

#include <dtQt/export.h>

#include <dtUtil/refstring.h>

#include <xercesc/dom/DOMElement.hpp>

#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/framework/XMLFormatter.hpp>

namespace dtQt
{
   static const dtUtil::RefString HELP_READER_VERSION("version");
   static const dtUtil::RefString HELP_READER_TITLE("title");
   static const dtUtil::RefString HELP_READER_HOME("home");
   static const dtUtil::RefString HELP_READER_RESOURCE("resource");
   static const dtUtil::RefString HELP_READER_CONTENTS("contents");
   static const dtUtil::RefString HELP_READER_SECTION("section");
   static const dtUtil::RefString HELP_READER_SECTION_TITLE("title");
   static const dtUtil::RefString HELP_READER_SECTION_REF("ref");
   static const dtUtil::RefString HELP_READER_SECTION_EXPANDED("expanded");


   class DT_QT_EXPORT DocBrowserXMLReader : public xercesc::DefaultHandler
   {
   public:
      struct SectionInfo
      {
         bool        expanded;
         std::string title;
         std::string link;

         SectionInfo* parent;
         std::vector<SectionInfo*> children;

         SectionInfo()
         {
            expanded = false;
            title = "";
            link = "";
            parent = NULL;
         }

         ~SectionInfo()
         {
            for (int index = 0; index < (int)children.size(); index++)
            {
               delete children[index];
            }

            children.clear();
         }
      };

      /**
      * Constructor.
      */
      DocBrowserXMLReader();

      /**
      * Destructor.
      */
      ~DocBrowserXMLReader();

      /**
      * Reads the XML file data.
      */
      void ReadXML(const std::string& fileName);

      /**
      * Retrieves the title of the help documentation.
      */
      std::string getTitle() {return mTitle;}

      /**
      * Retrieves the home page.
      */
      std::string GetHome() {return mHome;}

      /**
      * Retrieves the resource prefix.
      */
      std::string getResource() {return mResource;}

      /**
      * Retrieves all sections found in the table of contents.
      */
      const std::vector<SectionInfo*>& getSections() {return mSections;}

      /**
      * Retrieves a title for a given reference page.
      */
      std::string getTitleForRef(const std::string& ref);

   private:

      std::string getTitleForRef(const std::string& ref, const std::vector<SectionInfo*>& sections);

      // Parsing callbacks.
      void startDocument();
      void startElement(const XMLCh* const uri, const XMLCh* const localname,
                        const XMLCh* const qname, const xercesc::Attributes& attributes);
      virtual void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);

#if XERCES_VERSION_MAJOR < 3
      virtual void characters(const XMLCh* const chars, const unsigned int length);
#else
      virtual void characters(const XMLCh* const chars, const XMLSize_t length);
#endif

      // -----------------------------------------------------------------------
      //  Implementations of the SAX ErrorHandler interface
      // -----------------------------------------------------------------------
      void warning(const xercesc::SAXParseException& exc);
      void error(const xercesc::SAXParseException& exc);
      void fatalError(const xercesc::SAXParseException& exc);

      void SetDefault();

      // Data.
      std::string    mTitle;
      std::string    mHome;
      std::string    mResource;
      std::vector<SectionInfo*> mSections;

      std::stack<std::string> mElements;
      SectionInfo* mCurrentSection;
   };

} //namespace dtQt

#endif //_DOC_BROWSER_XML_MANAGER_H__
