#ifndef externaltoolargparser_h__
#define externaltoolargparser_h__

#include <QtCore/QString>

namespace dtEditQt
{
   class ExternalToolArgParser
   {
   public:
      ExternalToolArgParser();
      ~ExternalToolArgParser();

      virtual QString ExpandArguments(const QString& args) const = 0;

      virtual QString GetVariableText() const = 0;
      virtual QString GetDescription() const = 0;
   };

} // namespace dtEditQt

#endif // externaltoolargparser_h__
