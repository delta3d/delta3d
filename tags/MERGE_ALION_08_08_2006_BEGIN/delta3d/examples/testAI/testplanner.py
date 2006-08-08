from PyDtUtil import *
from PyDtCore import *
from PyDtAI import *

import sys


class Recipe(IStateVariable):

   def __init__(self):
      IStateVariable.__init__(self)
      self.mRecipe = 0

   def Copy(self):
      print "Copy"
      rec = Recipe()
      rec.HasRecipe(self.GetRecipe())
      return rec

   def HasRecipe(self, b):
      print "HasRecipe"
      self.mRecipe = b

   def GetRecipe(self):
      print "GetRecipe"
      return self.mRecipe
   

class HasRecipe(IConditional):

   def __init__(self):
      IConditional.__init__(self)      

   def GetName(self):
      print "GetName"
      return "Recipe"

   def Evaluate(self, ws):
      print "Evaluate"
      recipe = ws.GetState("Recipe")
      return recipe.GetRecipe()
      

class CallGrandma(ApplyOperatorFunctor):

   def Apply(self, wsFrom, wsTo):
      print "CallGrandma"
      recipe = wsTo.GetState("Recipe")
      recipe.HasRecipe(1)
      wsTo.AddCost(1)


class RemainingCost(RemainingCostFunctor):
   def RemainingCost(self, ws):
      print "RemainingCost, Python"
      return 1


class DesiredState(DesiredStateFunctor):
   def IsDesiredState(self, ws):
      print "IsDesiredState, Python"
      return 1
      #recipe = ws.GetState("Recipe")
      #return recipe.HasRecipe()      



class PlannerNPC:

   def __init__(self):
      remainingCost = RemainingCost()
      desiredState = DesiredState()
      self.mHelper = PlannerHelper(remainingCost, desiredState)

   def Init(self):
      self.mGrandmaFunctor = CallGrandma()
      self.mGrandmaOperator = NPCOperator("CallGrandma", self.mGrandmaFunctor)
      self.mHelper.AddOperator(self.mGrandmaOperator)

      self.mRecipe = Recipe()
      self.mHelper.GetCurrentState().AddState("Recipe", self.mRecipe)
      recipe = self.mHelper.GetCurrentState().GetState("Recipe")
      recipe.HasRecipe(0)

   def GeneratePlan(self):
      pPlanner = Planner()
      pPlanner.Reset(self.mHelper)
      pPlanner.GeneratePlan()
      return pPlanner.GetPlan()

   
         
class TestPlanner:

   def Run(self) :
      print "Run"
      pNPC = PlannerNPC()
      pNPC.Init()      
      plan = pNPC.GeneratePlan()
      #print plan[0]      
       



plannerTest = TestPlanner()
plannerTest.Run()
