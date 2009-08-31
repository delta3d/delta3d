#include <dtEditQt/configurationmanager.h>

#include <dtCore/refptr.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/xerceswriter.h>

#include <xercesc/parsers/SAX2XMLReaderImpl.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <sstream>

namespace dtEditQt
{
//Singleton global variable for the ConfigurationManager
ConfigurationManager* ConfigurationManager::mInstance(NULL);

////////////////////////////////////////////////////////////////////////////////
ConfigurationManager::ConfigurationManager()
{
   xercesc::XMLPlatformUtils::Initialize();

   SetDefaultConfigValues();
}

////////////////////////////////////////////////////////////////////////////////
ConfigurationManager& ConfigurationManager::GetInstance()
{
   if (ConfigurationManager::mInstance == NULL)
   {
      ConfigurationManager::mInstance = new ConfigurationManager();
   }
   return *(ConfigurationManager::mInstance);
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::ReadXML(const std::string& fileName)
{
   xercesc::SAX2XMLReaderImpl parser;

   parser.setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);
   parser.setFeature(xercesc::XMLUni::fgXercesSchema, true);
   parser.setFeature(xercesc::XMLUni::fgXercesSchemaFullChecking, true);
   parser.setFeature(xercesc::XMLUni::fgSAX2CoreNameSpacePrefixes, false);
   parser.setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
   parser.setFeature(xercesc::XMLUni::fgXercesDynamic, true);

   parser.setContentHandler(this);
   parser.setErrorHandler(this);

   //if the file isn't valid (usually means STAGEConfig.xml is missing,
   // -- then we'll just use the default config so don't log an error
   if (dtUtil::FileUtils::GetInstance().FileExists(fileName))
   {
      parser.parse(fileName.c_str());
   }
   else
   {
      std::string err = "STAGE config file not found: ";
      err += fileName;

      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
         __FUNCTION__, __LINE__, err);
      
   }  
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::WriteXML(const std::string& fileName)
{
   dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();
   writer->CreateDocument("STAGEConfig");

   XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();

   ToXML(*doc);

   writer->WriteFile(fileName);
}

////////////////////////////////////////////////////////////////////////////////
std::string ConfigurationManager::GetVariable(SectionType section,
                                               const std::string& name)
{
   switch(section)
   {
      case GENERAL:
         return mGeneralVariables[name];
         break;
      case LAYOUT:
         if (mLayoutVariables[name].empty())
         {
            mLayoutVariables[name] = "false";
         }
         return mLayoutVariables[name];
         break;      
      case PLUGINS:
         return mPluginVariables[name];
         break;
      default:
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
                   __FUNCTION__, __LINE__, "Unrecognized SectionType");
         break;
   }

   return "false";
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::SetVariable(SectionType section,
                                       const std::string& name, const std::string& value)
{
   std::string errString;

   switch(section)
   {
      case GENERAL:
         mGeneralVariables[name] = value;
         break;
      case LAYOUT:
         mLayoutVariables[name] = value;
         break;
      case PLUGINS:
         mPluginVariables[name] = value;
         break;
      default:
         errString = "Unrecognized SectionType, unable to set variable: ";
         errString += name;
         errString += " = ";
         errString += value;
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
            __FUNCTION__, __LINE__, errString.c_str());
         break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::SetVariable(SectionType section,
                                       const std::string& name, int value)
{
   char val[1024];

   sprintf(val, "%d", value);

   SetVariable(section, name, val);
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::ToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc) const
{
   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* root = doc.getDocumentElement();

   //These attributes are getting set on the root node in order to facilitate validation
   //against the schema (see the .xsd file)
   XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("xmlns");
   XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("http://www.w3schools.com");

   root->setAttribute(name, value);

   XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
   XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

   name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("xmlns:xsi");
   value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("http://www.w3.org/2001/XMLSchema-instance");

   root->setAttribute(name, value);

   XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
   XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

   name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("xsi:schemaLocation");
   value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("http://www.w3schools.com STAGEConfig.xsd");

   root->setAttribute(name, value);

   XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
   XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

   root->appendChild(WriteSectionToXML(doc, GENERAL));
   root->appendChild(WriteSectionToXML(doc, LAYOUT));
   root->appendChild(WriteSectionToXML(doc, PLUGINS));
}

////////////////////////////////////////////////////////////////////////////////
XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*
   ConfigurationManager::WriteSectionToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc,
                                           SectionType section) const
{
   std::map<std::string, std::string>::const_iterator beginIt, endIt, it;
   XMLCh* groupName;

   switch(section)
   {
      case GENERAL:
         beginIt = mGeneralVariables.begin();
         endIt = mGeneralVariables.end();
         groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("General");
         break;
      case LAYOUT:
         beginIt = mLayoutVariables.begin();
         endIt = mLayoutVariables.end();
         groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("Layout");
         break;
      case PLUGINS:
         beginIt = mPluginVariables.begin();
         endIt = mPluginVariables.end();
         groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("Plugins");
         break;
      default:
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
            __FUNCTION__, __LINE__, "Unrecognized Section.");
         return NULL;
         break;
   }

   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc.createElement(groupName);
   XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

   for(it = beginIt; it != endIt; ++it)
   {
      XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(it->first.c_str());
      XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(it->second.c_str());

      //XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* element = doc.createElement(name);
      groupElement->setAttribute(name, value);
      //XERCES_CPP_NAMESPACE_QUALIFIER DOMText* txt = doc.createTextNode(value);

      //element->appendChild(txt);
      //groupElement->appendChild(element);

      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);
   }

   return groupElement;
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::startDocument()
{
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::startElement(const XMLCh* const uri, const XMLCh* const localname,
                                        const XMLCh* const qname,
                                        const xercesc::Attributes& attributes)
{
   std::map<std::string, std::string> *currentMapPtr = NULL;

   dtUtil::XMLStringConverter qnameConv(qname);
   std::string sectionStr = qnameConv.ToString();

   if(sectionStr == "General")
   {
      currentMapPtr = &mGeneralVariables;
   }
   else if(sectionStr == "Layout")
   {
      currentMapPtr = &mLayoutVariables;
   }
   else if(sectionStr == "Plugins")
   {
      currentMapPtr = &mPluginVariables;
   }
   else
   {
      //Unrecognized element; can't do much with it
      return;
   }

   for(XMLSize_t i = 0; i < attributes.getLength(); ++i)
   {
      dtUtil::XMLStringConverter varNameConv(attributes.getQName(i));
      dtUtil::XMLStringConverter varValueConv(attributes.getValue(i));

      (*currentMapPtr)[varNameConv.ToString()] = varValueConv.ToString();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::error(const xercesc::SAXParseException& e)
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
void ConfigurationManager::fatalError(const xercesc::SAXParseException& e)
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
void ConfigurationManager::warning(const xercesc::SAXParseException& e)
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
void ConfigurationManager::SetDefaultConfigValues()
{
   mGeneralVariables[CONF_MGR_MAP_FILE] = "";
   mGeneralVariables[CONF_MGR_PROJECT_CONTEXT] = "";
   mGeneralVariables[CONF_MGR_SAVE_ON_CLOSE] = "false";
   mGeneralVariables[CONF_MGR_STYLESHEET] = "";

   mLayoutVariables[CONF_MGR_SHOW_FRONT_VIEW] = "true";
   mLayoutVariables[CONF_MGR_SHOW_SIDE_VIEW] = "true";
   mLayoutVariables[CONF_MGR_SHOW_TOP_VIEW] = "true";
   mLayoutVariables[CONF_MGR_SHOW_PERSP_VIEW] = "true";
   
   mLayoutVariables[CONF_MGR_SHOW_ACTOR_DOCKWIDGET] = "true";
   mLayoutVariables[CONF_MGR_SHOW_ACTOR_BROWSER] = "true";
   mLayoutVariables[CONF_MGR_SHOW_PREFAB_BROWSER] = "true";
   mLayoutVariables[CONF_MGR_SHOW_SEARCH_DOCKWIDGET] = "true";
   mLayoutVariables[CONF_MGR_SHOW_ACTOR_SEARCH] = "true";
   mLayoutVariables[CONF_MGR_SHOW_GLOBAL_ACTORS] = "true";
   mLayoutVariables[CONF_MGR_SHOW_RESOURCE_BROWSER] = "true";
   mLayoutVariables[CONF_MGR_SHOW_PROPERTY_EDITOR] = "true";

   mPluginVariables[CONF_MGR_PLUGINS_ACTIVATED] = "";
}

} //namespace dtEditQt
