/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 Bradley Anderegg
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Bradley Anderegg 08/11/2006
 */

#ifndef __DELTA_NPCPARSER_H__
#define __DELTA_NPCPARSER_H__

#include <dtAI/export.h>
#include <dtAI/basenpc.h>
#include <osgDB/fstream>

#include <string>
#include <vector>

namespace dtAI
{
   /**
    * The class used to parse NPCScript
    */
   class DT_AI_EXPORT NPCParser
   {
   public:
      NPCParser();
      virtual ~NPCParser();

      bool LoadScript(const std::string& pFilename, BaseNPC* pNPC);

   private:
      // internal datatypes

      enum ParamType{ PARAM_BOOL = 0, PARAM_INT};

      struct Param
      {
         Param(bool b): mType(PARAM_BOOL) {mBool = b;}
         Param(int i): mType(PARAM_INT) {mInt = i;}

         Param& operator=(const Param& pIn)
         {
            if (pIn.mType == PARAM_BOOL)
            {
               mType = PARAM_BOOL;
               mBool = pIn.mBool;
            }
            else if (pIn.mType == PARAM_INT)
            {
               mType = PARAM_INT;
               mInt = pIn.mInt;
            }

            return *this;
         }

         ParamType mType;

         union
         {
            bool mBool;
            int mInt;
         };

      };

      struct NPCParserError
      {
         std::string pError;
         unsigned pLineNumber;
      };

      typedef std::pair<std::string, std::vector<Param> > ExpressionPair;
      typedef std::vector<ExpressionPair> Expression;

      static const std::string TOKEN_NPC;
      static const std::string TOKEN_WORLD_STATE;
      static const std::string TOKEN_OPERATOR;
      static const std::string TOKEN_PRECONDS;
      static const std::string TOKEN_INTERRUPT;
      static const std::string TOKEN_EFFECTS;
      static const std::string TOKEN_COST;
      static const std::string TOKEN_GOALS;
      static const std::string TOKEN_TRUE;
      static const std::string TOKEN_FALSE;
      static const std::string TOKEN_OPEN_BRACKET;
      static const std::string TOKEN_CLOSE_BRACKET;
      static const std::string TOKEN_OPEN_PAREN;
      static const std::string TOKEN_CLOSE_PAREN;
      static const std::string TOKEN_DELIMETER;

      void CreateWorldState(BaseNPC* pNPC, Expression& pExr);
      void CreateGoals(BaseNPC* pNPC, Expression& pExr);
      void CreateOperator(BaseNPC* pNPC, const std::string& pOpName, Expression& pPreConds, Expression& pInterrupts, Expression& pEffects, int pCost);
      Precondition* CreatePrecondition(ExpressionPair& expr);
      Effect* CreateEffect(ExpressionPair& expr);

      void ParseExpression(Expression& pExp);
      Param ParseParam();
      void ParseToken(const std::string& pToken);
      bool ParseString(std::string& pStr);
      bool IsDigit(const char pChar) const;
      int ParseInt();
      char ParseDelimeter();
      bool IsDelimeter(char c) const;
      bool IsWhiteSpace(char c) const;
      NPCParserError CreateError(const std::string& pError);
      void PrintError(NPCParserError& pError);

      mutable unsigned mLineNumber;
      std::string mCurrentTask;
      osgDB::ifstream mFileStream;
   };
} // namespace dtAI

#endif // __DELTA_NPCPARSER_H__
