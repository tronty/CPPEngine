from pro import *

height = param(random(20, 40))

@rule
def Begin():
	extrude(height)