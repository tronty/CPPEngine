from pro import *

@rule
def Begin():
	# leave only one split uncommented
	split(z).into(repeat(2>>Segment()))
	split(x).into(
		flt(1)>>Wall(),
		2>>split(y).into(1>>Wall(), 1.5>>Window(), flt(1)>>Wall()),
		flt(1)>>Wall()
	)
	split(x).into(
		repeat(
			flt(5)>>X(2.5),
			repeat(
				flt(4)>>Y(1.5),
				repeat(
					flt(3)>>Y(),
					flt(2)>>Z(1.25),
					10>>Z()
				)
			)
		)
	)

@rule
def Segment():
	print("in Segment")

@rule
def X(width):
	print("X", width)
	
@rule	
def Y(width=1):
	print("Y", width)

@rule
def Z(width):
	print("Y", width)