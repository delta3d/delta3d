/* -*-c++-*-
* Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "disactorcomponents.h"

#include <dtCore/datatype.h>
#include <dtCore/stringactorproperty.h>

#include <dtDIS/propertyname.h>

#include <dtGame/gameactor.h>

#include <dtUtil/stringutils.h>

////////////////////////////////////////////////////////////////////////////////
EntityTypeActorComponent::EntityTypeActorComponent(int kind, int dom, int country, 
                                                   int cat, int subcat, int spec):
dtGame::ActorComponent("EntityTypeActorComponent")
, mKind(kind)
, mDom(dom)
, mCountry(country)
, mCat(cat)
, mSubcat(subcat)
, mSpec(spec)
{
}

////////////////////////////////////////////////////////////////////////////////
void EntityTypeActorComponent::OnAddedToActor(dtGame::GameActor& actor)
{
   //Add an ActorProptery to the parent actor

   actor.GetGameActorProxy().AddProperty( 
      new dtCore::StringActorProperty(dtDIS::EntityPropertyName::ENTITY_TYPE.Get(),
      "DIS EntityType",
      dtCore::StringActorProperty::SetFuncType(this, &EntityTypeActorComponent::SetText),
      dtCore::StringActorProperty::GetFuncType(this, &EntityTypeActorComponent::GetText),
      "The DIS EntityType string",
      "DIS")
      );
}

////////////////////////////////////////////////////////////////////////////////
void EntityTypeActorComponent::SetText(const std::string& text)
{
   std::vector<std::string> tokens;
   dtUtil::StringTokenizer<dtUtil::IsSlash> tokenizer;
   DTUNREFERENCED_PARAMETER(tokenizer); //to squelch "unreferenced" compiler warning

   tokenizer.tokenize(tokens, text);
   if (tokens.size() == 6)
   {
      mKind = atoi(tokens[0].c_str());
      mDom = atoi(tokens[1].c_str());
      mCountry = atoi(tokens[2].c_str());
      mCat = atoi(tokens[3].c_str());
      mSubcat = atoi(tokens[4].c_str());
      mSpec = atoi(tokens[5].c_str());
   }
}

////////////////////////////////////////////////////////////////////////////////
std::string EntityTypeActorComponent::GetText() const
{
   using namespace dtUtil;

   std::string str;
   str += ToString(mKind) + "/" +
          ToString(mDom) + "/" +
          ToString(mCountry) + "/" +
          ToString(mCat) + "/" +
          ToString(mSubcat) + "/" +
          ToString(mSpec);

   return str;
}
