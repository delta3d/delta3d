#include <prefix/dtqtprefix.h>
#include <dtQt/docbrowserxmlreader.h>

#include <dtCore/refptr.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/xerceswriter.h>

#include <xercesc/parsers/SAX2XMLReaderImpl.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <sstream>

#include <QtCore/QFile>
#include <QtCore/QTextStream>

namespace dtQt
{
   ////////////////////////////////////////////////////////////////////////////////
   DocBrowserXMLReader::DocBrowserXMLReader()
      : mCurrentSection(NULL)
   {
      xercesc::XMLPlatformUtils::Initialize();

      SetDefault();
   }

   ////////////////////////////////////////////////////////////////////////////////
   DocBrowserXMLReader::~DocBrowserXMLReader()
   {
      mCurrentSection = NULL;

      for (int index = 0; index < (int)mSections.size(); index++)
      {
         delete mSections[index];
      }

      mSections.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserXMLReader::ReadXML(const std::string& fileName)
   {
      if (fileName.empty()) return;

      xercesc::SAX2XMLReaderImpl parser;

      parser.setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);
      parser.setFeature(xercesc::XMLUni::fgXercesSchema, false);
      parser.setFeature(xercesc::XMLUni::fgXercesSchemaFullChecking, false);
      parser.setFeature(xercesc::XMLUni::fgSAX2CoreNameSpacePrefixes, false);
      parser.setFeature(xercesc::XMLUni::fgSAX2CoreValidation, false);
      parser.setFeature(xercesc::XMLUni::fgXercesDynamic, true);

      parser.setContentHandler(this);
      parser.setErrorHandler(this);

      // If the filename has a colon in front of it, then consider it a QResource.
      bool isQResource = (fileName[0] == ':')? true : false;

      // Open a QResource file.
      if (isQResource)
      {
         QFile file(fileName.c_str());
         if (file.open(QFile::ReadOnly | QFile::Text))
         {
            QTextStream in(&file);
            QString data = in.readAll();

            char* buffer = new char[data.length() + 1];
            memset(buffer, 0, data.length() + 1);
            strcpy(buffer, data.toStdString().c_str());

            xercesc::MemBufInputSource* memBuf = new xercesc::MemBufInputSource(
               (const XMLByte*)buffer,
               data.length(), "DocBrowserFile", false);

            parser.parse(*memBuf);
            delete[] buffer;
            return;
         }
      }
      // Open a file on the disk.
      else
      {
         if (dtUtil::FileUtils::GetInstance().FileExists(fileName))
         {
            parser.parse(fileName.c_str());
            return;
         }
      }

      // If we get here, we didn't find the file.
      std::string err = "Help File not found: ";
      err += fileName;

      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
         __FUNCTION__, __LINE__, err);
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string DocBrowserXMLReader::getTitleForRef(const std::string& ref)
   {
      return getTitleForRef(ref, mSections);
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string DocBrowserXMLReader::getTitleForRef(const std::string& ref, const std::vector<SectionInfo*>& sections)
   {
      for (int index = 0; index < (int)sections.size(); index++)
      {
         if (sections[index]->link == ref)
         {
            return sections[index]->title;
         }

         getTitleForRef(ref, sections[index]->children);
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserXMLReader::startDocument()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserXMLReader::startElement(const XMLCh* const uri,
                                    const XMLCh* const localname,
                                    const XMLCh* const qname,
                                    const xercesc::Attributes& attributes)
   {
      mElements.push(dtUtil::XMLStringConverter(localname).ToString());
      std::string& topEl = mElements.top();

      if (topEl == HELP_READER_SECTION)
      {
         // Create a new section.
         SectionInfo* section = new SectionInfo();
         section->parent = mCurrentSection;

         for(XMLSize_t i = 0; i < attributes.getLength(); ++i)
         {
            std::string element = dtUtil::XMLStringConverter(attributes.getQName(i)).ToString();
            std::string value   = dtUtil::XMLStringConverter(attributes.getValue(i)).ToString();

            if (element == HELP_READER_SECTION_TITLE)
            {
               section->title = value;
            }
            else if (element == HELP_READER_SECTION_REF)
            {
               section->link = value;
            }
            else if (element == HELP_READER_SECTION_EXPANDED)
            {
               QString val = value.c_str();
               if (val.toLower() == "yes" ||
                  val.toLower() == "true")
               {
                  section->expanded = true;
               }
            }
         }

         // Now add the new section to the tree.
         if (mCurrentSection)
         {
            mCurrentSection->children.push_back(section);
         }
         else
         {
            mSections.push_back(section);
         }

         mCurrentSection = section;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserXMLReader::endElement( const XMLCh* const uri,
                                   const XMLCh* const localname,
                                   const XMLCh* const qname)
   {
      std::string& topEl = mElements.top();

      if (topEl == HELP_READER_SECTION)
      {
         mCurrentSection = mCurrentSection->parent;
      }

      mElements.pop();
   }

   ////////////////////////////////////////////////////////////////////////////////
#if XERCES_VERSION_MAJOR < 3
   void DocBrowserXMLReader::characters(const XMLCh* const chars, const unsigned int length)
#else
   void DocBrowserXMLReader::characters(const XMLCh* const chars, const XMLSize_t length)
#endif
   {
      std::string& topEl = mElements.top();

      if (topEl == HELP_READER_TITLE)
      {
         mTitle = dtUtil::XMLStringConverter(chars).ToString();
      }
      else if (topEl == HELP_READER_HOME)
      {
         mHome = dtUtil::XMLStringConverter(chars).ToString();
      }
      else if (topEl == HELP_READER_RESOURCE)
      {
         mResource = dtUtil::XMLStringConverter(chars).ToString();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserXMLReader::error(const xercesc::SAXParseException& e)
   {
      std::ostringstream errStrm;
      dtUtil::XMLStringConverter sysIDConverter(e.getSystemId());
      dtUtil::XMLStringConverter msgConverter(e.getMessage());

      errStrm << "\nError at file " << sysIDConverter.ToString()
         << ", line " << e.getLineNumber()
         << ", char " << e.getColumnNumber()
         << "\n  Message: " << msgConverter.ToString() << "\n";

      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,
         __LINE__, errStrm.str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserXMLReader::fatalError(const xercesc::SAXParseException& e)
   {
      std::ostringstream errStrm;
      dtUtil::XMLStringConverter sysIDConverter(e.getSystemId());
      dtUtil::XMLStringConverter msgConverter(e.getMessage());

      errStrm << "\nFatal Error at file " << sysIDConverter.ToString()
         << ", line " << e.getLineNumber()
         << ", char " << e.getColumnNumber()
         << "\n  Message: " << msgConverter.ToString() << "\n";

      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,
         __LINE__, errStrm.str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserXMLReader::warning(const xercesc::SAXParseException& e)
   {
      std::ostringstream errStrm;
      dtUtil::XMLStringConverter sysIDConverter(e.getSystemId());
      dtUtil::XMLStringConverter msgConverter(e.getMessage());

      errStrm << "\nWarning at file " << sysIDConverter.ToString()
         << ", line " << e.getLineNumber()
         << ", char " << e.getColumnNumber()
         << "\n  Message: " << msgConverter.ToString() << "\n";

      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,
         __LINE__, errStrm.str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserXMLReader::SetDefault()
   {
      mTitle = "Help";
      mHome = "Index.html";
      mResource = ":/";
      mSections.clear();
   }

} //namespace dtQt
