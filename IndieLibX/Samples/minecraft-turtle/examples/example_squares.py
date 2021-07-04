# Minecraft Turtle Example
from mcturtle import minecraftturtle
from mcpi import minecraft

# create connection to minecraft
mc = minecraft.Minecraft.create()

# get players position
pos = mc.player.getPos()

# create minecraft turtle
steve = minecraftturtle.MinecraftTurtle(mc, pos)

steve.speed(10)

# draw a square
steve.forward(10)
steve.right(90)
steve.forward(10)
steve.right(90)
steve.forward(10)
steve.right(90)
steve.forward(10)

# draw a square on the floor
steve.walk()
steve.forward(11)
steve.right(90)
steve.forward(10)
steve.right(90)
steve.forward(10)
steve.right(90)
steve.forward(10)
