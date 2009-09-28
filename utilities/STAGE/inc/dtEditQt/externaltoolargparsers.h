#ifndef externaltoolargparsers_h__
#define externaltoolargparsers_h__

#include <dtEditQt/externaltoolargparser.h>
#include <dtEditQt/editordata.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>

namespace dtEditQt
{
   /**
    * Replace "${CurrentContext}" with the current context path
    */
   class CurrentContextArgParser : public ExternalToolArgParser
   {
   public:
      CurrentContextArgParser() {}
      ~CurrentContextArgParser() {}

      virtual QString ExpandArguments(const QString& args) const
      {
         const QString context = QString::fromStdString(EditorData::GetInstance().getCurrentProjectContext());
         return QString(args).replace(GetVariableText(), context);
      }

      virtual QString GetVariableText() const {return "${CurrentContext}";}

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
      CurrentMapNameArgParser() {}
      ~CurrentMapNameArgParser() {}

      virtual QString ExpandArguments(const QString& args) const
      {
         dtDAL::Map* currentMap = EditorData::GetInstance().getCurrentMap();
         QString mapFilename;
         if (currentMap)
         {
            mapFilename = QString::fromStdString(currentMap->GetFileName());
         }

         return QString(args).replace(GetVariableText(), mapFilename);
      }

      virtual QString GetVariableText() const {return "${CurrentMapFilename}";}

      virtual QString GetDescription() const
      {
         return "The filename of the currently loaded map.";
      }
   };

   /**
    * Replace "${CurrentMesh}" with the currently selected mesh filename.
    */
   class CurrentMeshArgParser : public ExternalToolArgParser
   {
   public:
      CurrentMeshArgParser() {}
      ~CurrentMeshArgParser() {}

      virtual QString ExpandArguments(const QString& args) const
      {
         dtDAL::ResourceDescriptor meshDescr = EditorData::GetInstance().getCurrentMeshResource();
         std::string path;
         if (!meshDescr.GetResourceIdentifier().empty())
         {
            path = dtDAL::Project::GetInstance().GetResourcePath(meshDescr);
         }

         return QString(args).replace(GetVariableText(), QString::fromStdString(path));
      }

      virtual QString GetVariableText() const { return "${CurrentMesh}"; }

      virtual QString GetDescription() const
      {
         return "The Context-relative path and filename of currently selected mesh.";
      }
   };

   /**
    * Replace "${CurrentParticle}" with the currently selected mesh filename.
    */
   class CurrentParticleSystemArgParser : public ExternalToolArgParser
   {
   public:
      CurrentParticleSystemArgParser() {}
      ~CurrentParticleSystemArgParser() {}

      virtual QString ExpandArguments(const QString& args) const
      {
         dtDAL::ResourceDescriptor descr = EditorData::GetInstance().getCurrentParticleResource();
         std::string path;
         if (!descr.GetResourceIdentifier().empty())
         {
            path = dtDAL::Project::GetInstance().GetResourcePath(descr);
         }

         return QString(args).replace(GetVariableText(), QString::fromStdString(path));
      }

      virtual QString GetVariableText() const { return "${CurrentParticle}"; }

      virtual QString GetDescription() const
      {
         return "The Context-relative path and filename of currently selected particle system.";
      }
   };

   /**
    * Replace "${CurrentPaCurrentSkeletonrticle}" with the currently selected mesh filename.
    */
   class CurrentSkeletonArgParser : public ExternalToolArgParser
   {
   public:
      CurrentSkeletonArgParser() {}
      ~CurrentSkeletonArgParser() {}

      virtual QString ExpandArguments(const QString& args) const
      {
         dtDAL::ResourceDescriptor descr = EditorData::GetInstance().getCurrentSkeletalModelResource();
         std::string path;
         if (!descr.GetResourceIdentifier().empty())
         {
            path = dtDAL::Project::GetInstance().GetResourcePath(descr);
         }

         return QString(args).replace(GetVariableText(), QString::fromStdString(path));
      }

      virtual QString GetVariableText() const {return "${CurrentSkeleton}";}

      virtual QString GetDescription() const
      {
         return "The Context-relative path and filename of currently selected skeletal model.";
      }
   };

} // namespace dtEditQt

#endif // externaltoolargparsers_h__
