plane = P51.GetObjectInstance('cessna')

transform = P51.Transform()

angle = 0

t = os.clock()

while true do
   transform:SetTranslation(40*math.cos(math.rad(angle)),
                            100 + 40*math.sin(math.rad(angle)), 
                            0)
   transform:SetRotation(angle, 0, -45)
   plane:SetTransform(transform)
   nt = os.clock()
   angle = angle - 45*(nt-t)
   t = nt
end