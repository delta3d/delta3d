from PyDtUtil import *
from PyDtCore import *

from math import *
from time import *

helo = Object.GetInstance("UH-1N")

transform = Transform()

angle = 0.0
noise = Noise1f()

def radians(v):
   return v * pi/180
   
system = System.GetInstance(0)

while system.IsRunning():
   transform.Set(40*cos(radians(angle)),
                 100 + 40*sin(radians(angle)), 
                 0.0,
                 angle,
                 #add some noise to make it more realistic
                 15.0 * noise.GetNoise(clock() * 0.70332423),
                 -45.0 + 35.0 * noise.GetNoise(clock() * 0.5958992))
   helo.SetTransform(transform)
   sleep(0.01)
   angle += 0.45
