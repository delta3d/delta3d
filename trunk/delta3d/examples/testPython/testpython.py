from dtCore import *
from math import *
from time import *

window = Window()

scene = Scene()

camera = Camera()

camera.SetScene(scene)
camera.SetWindow(window)

System.GetSystem().Run()
