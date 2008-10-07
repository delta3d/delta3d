#ifndef externaltoolargparsers_h__
#define externaltoolargparsers_h__

#include <dtEditQt/externaltoolargparser.h>
#include <dtEditQt/editordata.h>

namespace dtEditQt
{
   /**
    * Replace "${CurrentContext}" with the current context path
   */
   class CurrentContextArgParser : public ExternalToolArgParser
   {
   public:
      CurrentContextArgParser() {};
      ~CurrentContextArgParser() {};

      virtual QString ExpandArguments(const QString& args) const
      {
         const QString context = QString::fromStdString(EditorData::GetInstance().getCurrentProjectContext());
         return QString(args).replace("${CurrentContext}", context);
      }

      virtual QString GetVariableText() const
      {
         return "${CurrentContext}";
      }

      virtual QString GetDescription() const
      {
         return "The path describing the currently loaded context";
      }
   };

   /** 
    * Replace "${CurrentMapFilename}" with the currently loaded map filename
   */
   class CurrentMapNameArgParser : public ExternalToolArgParser
   {
   public:
      CurrentMapNameArgParser() {};
      ~CurrentMapNameArgParser() {};
   	
      virtual QString ExpandArguments(const QString& args) const
      {
         dtDAL::Map* currentMap =EditorData::GetInstance().getCurrentMap();
         QString mapFilename;
         if (currentMap)
         {
            mapFilename = QString::fromStdString(currentMap->GetFileName());
         }

         return QString(args).replace("${CurrentMapFilename}", mapFilename);
      }

      virtual QString GetVariableText() const
      {
         return "${CurrentMapFilename}";
      }

      virtual QString GetDescription() const
      {
         return "The filename of the currently loaded map.";
      }
   };
}
#endif // externaltoolargparsers_h__
