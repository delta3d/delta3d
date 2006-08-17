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
 * @author Bradley Anderegg 08/11/2006
 */

#include <dtAI/npcparser.h>
#include <dtAI/worldstate.h>
#include <dtAI/basenpcutils.h>

#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>

#include <osgDB/FileUtils>

#include <iostream>

namespace dtAI
{

   //////////////////////////////////////////////////////////////////////////
   const std::string NPCParser::TOKEN_NPC("NPC");
   const std::string NPCParser::TOKEN_WORLD_STATE("WorldState");
   const std::string NPCParser::TOKEN_OPERATOR("operator");
   const std::string NPCParser::TOKEN_PRECONDS("preconds");
   const std::string NPCParser::TOKEN_INTERRUPT("interrupts");
   const std::string NPCParser::TOKEN_EFFECTS("effects");
   const std::string NPCParser::TOKEN_COST("cost");
   const std::string NPCParser::TOKEN_GOALS("Goals");
   const std::string NPCParser::TOKEN_TRUE("true");
   const std::string NPCParser::TOKEN_FALSE("false");
   const std::string NPCParser::TOKEN_OPEN_BRACKET("[");
   const std::string NPCParser::TOKEN_CLOSE_BRACKET("]");
   const std::string NPCParser::TOKEN_DELIMETER(",()[]");
   //////////////////////////////////////////////////////////////////////////


   NPCParser::NPCParser()
   {
   }
   
   NPCParser::~NPCParser()
   {
   }


   BaseNPC* NPCParser::LoadScript(const std::string& pFilename)
   {
      BaseNPC* pNPC = 0;

      std::string file = osgDB::findDataFile(pFilename);
      
      if(file.empty() )
      {
         LOG_WARNING("The file, " + pFilename + " was not found.")
         return 0;
      }

      mFileStream.open(file.c_str());

      if(mFileStream.is_open())
      {
         try
         {

            ParseToken(TOKEN_NPC);

            std::string pNPCName;
            ParseString(pNPCName);

            pNPC = new BaseNPC(pNPCName);
           
            if(mLastDelimeter != TOKEN_OPEN_BRACKET[0]) throw CreateError("Syntax Error: token '[' expected.");

            //world state
            ParseToken(TOKEN_WORLD_STATE);
           
            Expression pWorldState;
            ParseExpression(pWorldState);
            CreateWorldState(pNPC, pWorldState);

            //operators
            std::string pToken;
            ParseString(pToken);

            while(pToken == TOKEN_OPERATOR && mFileStream.good())
            {
               std::string OpName;
               ParseString(OpName);

               ParseToken(TOKEN_PRECONDS);
               Expression preConds;
               ParseExpression(preConds);

               ParseToken(TOKEN_INTERRUPT);
               Expression interrupt;
               ParseExpression(interrupt);

               ParseToken(TOKEN_EFFECTS);
               Expression effect;
               ParseExpression(effect);

               ParseToken(TOKEN_COST);
               int cost = ParseInt();  

               CreateOperator(pNPC, OpName, preConds, interrupt, effect, cost);
               
               pToken.clear();
               ParseString(pToken);
            }            


            //goals
            if(pToken != TOKEN_GOALS) throw CreateError("Token Expected, 'Goals'");

            Expression pGoals;
            ParseExpression(pGoals);
            CreateGoals(pNPC, pGoals);

            if(ParseDelimeter() != TOKEN_CLOSE_BRACKET[0]) throw CreateError("Syntax Error: token ']' expected,");
         }
         catch (NPCParserError& pError)
         {
            PrintError(pError);
            return 0;
         }
      }

      mFileStream.close();
      return pNPC;
   }


   void NPCParser::CreateWorldState(BaseNPC* pNPC, Expression& pExr)
   {
      WorldState pWS;
      Expression::iterator iter = pExr.begin();
      Expression::iterator endOfList = pExr.end();
      while(iter != endOfList)
      {       
         //error checking
         if(iter->second.size() != 1)
         {
            throw CreateError("Syntax Error, WorldState variable: " + iter->first + " expects one argument");
         }
         else if(iter->second[0].mType != PARAM_BOOL)
         {
            throw CreateError("Syntax Error, WorldState variable: " + iter->first + " expects argument to be of type bool");
         }

         pWS.AddState(iter->first, new StateVariable(iter->second[0].mBool));
         
         ++iter;
      }      

      pNPC->SetWSTemplate(pWS);
   }

   void NPCParser::CreateGoals(BaseNPC* pNPC, Expression& pExr)
   {      
      Expression::iterator iter = pExr.begin();
      Expression::iterator endOfList = pExr.end();
      while(iter != endOfList)
      {         
         //error checking
         if(iter->second.size() != 2)
         {
            throw CreateError("Syntax Error, Goal: " + iter->first + " expecting two arguments");
         }
         else if(iter->second[0].mType != PARAM_INT)
         {
            throw CreateError("Syntax Error, Goal: " + iter->first + " first argument expects type int");
         }
         else if(iter->second[1].mType != PARAM_BOOL)
         {
            throw CreateError("Syntax Error, Goal: " + iter->first + " second argument expects type bool");
         }

         pNPC->AddGoal(iter->first, new Goal(iter->first, iter->second[1].mBool, iter->second[0].mInt));

         ++iter;
      }      
   }

   void NPCParser::CreateOperator(BaseNPC* pNPC, const std::string& pOpName, Expression& pPreConds, Expression& pInterrupts, Expression& pEffects, int pCost)
   {
      NPCOperator* pOperator = new NPCOperator(pOpName);
      
      //parse preconditions
      Expression::iterator iter = pPreConds.begin();
      Expression::iterator endOfList = pPreConds.end();
      while(iter != endOfList)
      {
         pOperator->AddPreCondition(CreatePrecondition(*iter));
         ++iter;
      }

      iter = pInterrupts.begin();
      endOfList = pInterrupts.end();
      while(iter != endOfList)
      {
         pOperator->AddInterrupt(CreatePrecondition(*iter));
         ++iter;
      }

      iter = pEffects.begin();
      endOfList = pEffects.end();
      while(iter != endOfList)
      {  
         pOperator->AddEffect(CreateEffect(*iter));
         ++iter;
      }

      pOperator->SetCost(pCost);

      pNPC->AddOperator(pOperator);
   }

   Precondition* NPCParser::CreatePrecondition(ExpressionPair& expr)
   {
      if(expr.second.size() != 1)
      {
         throw CreateError("Syntax Error, Conditional: " + expr.first + " expects one argument");
      }
      else if(expr.second[0].mType != PARAM_BOOL)
      {
         throw CreateError("Syntax Error, Conditional: " + expr.first + " expects argument to be of type bool");
      }

      return new Precondition(expr.first, expr.second[0].mBool);      
   }

   Effect* NPCParser::CreateEffect(ExpressionPair& expr)
   {
      if(expr.second.size() != 1)
      {
         throw CreateError("Syntax Error, Effect: " + expr.first + " expects one argument");
      }
      else if(expr.second[0].mType != PARAM_BOOL)
      {
         throw CreateError("Syntax Error, Effect: " + expr.first + " expects argument to be of type bool");
      }

      return new Effect(expr.first, expr.second[0].mBool);      
   }

   void NPCParser::ParseExpression(Expression& pExp)
   {
      if(ParseDelimeter() != TOKEN_OPEN_BRACKET[0]) throw CreateError("NPCParser, Syntax Error: token '[' expected,");
      
      std::string pToken;
      do
      {
         ExpressionPair ep;
         if(!ParseString(pToken)) break;
         ep.first = pToken;

         do 
         {
            ep.second.push_back(ParseParam());
         } 
         while(mLastDelimeter == ',');

         pExp.push_back(ep);
         pToken.clear();

         ParseDelimeter();
      }
      while(mLastDelimeter == ',');
            
      if(mLastDelimeter != TOKEN_CLOSE_BRACKET[0])
      {
         throw CreateError("Syntax Error, expected token:" + TOKEN_CLOSE_BRACKET);
      }
   }

   NPCParser::Param NPCParser::ParseParam()
   {
      std::string pStr;
      ParseString(pStr);
      if(!pStr.empty() && IsDigit(pStr[0]))
      {
         return Param(dtUtil::ToType<int>(pStr));
      }
      else
      {
         if(pStr == TOKEN_TRUE)
         {
            return Param(true);
         }
         else if(pStr == TOKEN_FALSE)
         {
            return Param(false);
         }
         else
         {
            throw CreateError("Unable to parse parameter: " + pStr);
         }
      }
   }

   bool NPCParser::IsDigit(const char pChar) const
   {
      return (pChar >= 48 && pChar <= 57); 
   }

   void NPCParser::ParseToken(const std::string& pToken)
   {
      std::string pStr;
      ParseString(pStr);
      if(pStr != pToken)
      {
         throw CreateError("Syntax error, token " + pToken + " expected.");
      }
   }


   bool NPCParser::ParseString(std::string& pStr)
   {
      char c = ' ';
      while(IsWhiteSpace(c) && !IsDelimeter(c)) mFileStream.get(c);

      if(IsDelimeter(c))
      {
         mLastDelimeter = c;
         return false;
      }

      while(mFileStream.good() && !IsDelimeter(c) && c != ' ')
      {
         if(!IsWhiteSpace(c)) pStr.push_back(c);

         mFileStream.get(c);
      }
      
      mLastDelimeter = c;
      return true;
   }

   char NPCParser::ParseDelimeter()
   {
      char pDel = ' ';
      while(IsWhiteSpace(pDel)) mFileStream >> pDel;
      mLastDelimeter = pDel;
      return mLastDelimeter;
   }

   bool NPCParser::IsDelimeter(char c) const
   {
      std::size_t pSize = TOKEN_DELIMETER.find(c);
      return (pSize != std::string::npos);
   }

   bool NPCParser::IsWhiteSpace(char c) const
   {
      return(c == '\n' || c == '\t' || c == ' ' || c == '\r');
   }

   int NPCParser::ParseInt()
   {
      int i;
      mFileStream >> i;
      return i;
   }

   NPCParser::NPCParserError NPCParser::CreateError(const std::string& pError)
   {
      NPCParserError error;
      error.pError = pError;
      error.pLineNumber = 0;
      return error; 
   }

   void NPCParser::PrintError(NPCParserError& pError)
   {
      LOG_WARNING( "NPCParser Error: " + pError.pError);//  std::endl;// << "   Line: " << pError.pLineNumber << std::endl;
   }


}//namespace dtAI
