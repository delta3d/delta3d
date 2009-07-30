#ifndef _STAGE_CONFIGURATION_MANAGER_H__
#define _STAGE_CONFIGURATION_MANAGER_H__

#include <map>
#include <string>

#include <osg/Vec2>

#include <xercesc/dom/DOMElement.hpp>

#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/framework/XMLFormatter.hpp>

namespace dtEditQt
{

class ConfigurationManager : public xercesc::DefaultHandler
{
public:
   enum SectionType
   {
      GENERAL,
      LAYOUT,
      MENU,
      PLUGINS
   }; 
   
   static ConfigurationManager& GetInstance();  

   void ReadXML(const std::string& fileName);
   void WriteXML(const std::string& fileName);

   std::string GetVariable(SectionType section, const std::string& name);
   void SetVariable(SectionType section, const std::string& name,
                    const std::string& value);
   void SetVariable(SectionType section, const std::string& name, int value);

private:   
   ConfigurationManager();

   void ToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc) const;   

   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* 
      WriteSectionToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc,
                         SectionType section) const;

   void startDocument();
   void startElement(const XMLCh* const uri, const XMLCh* const localname,
                     const XMLCh* const qname, const xercesc::Attributes& attributes);

   // -----------------------------------------------------------------------
   //  Implementations of the SAX ErrorHandler interface
   // -----------------------------------------------------------------------
   void warning(const xercesc::SAXParseException& exc);
   void error(const xercesc::SAXParseException& exc);
   void fatalError(const xercesc::SAXParseException& exc);

   void SetDefaultConfigValues();

   //Singleton instance of this class.
   static ConfigurationManager* mInstance;

   std::map<std::string, std::string> mGeneralVariables;    // name/value pairs
   std::map<std::string, std::string> mLayoutVariables;     // name/value pairs
   std::map<std::string, std::string> mMenuVariables;       // name/value pairs
   std::map<std::string, std::string> mPluginVariables;     // name/value pairs
   //To add more sections to the config file, make some more maps here, add a 
   //value to the SectionType enum, and then modify the XML readers/writers   
   
};

} //namespace dtEditQt

#endif //_STAGE_CONFIGURATION_MANAGER_H__
