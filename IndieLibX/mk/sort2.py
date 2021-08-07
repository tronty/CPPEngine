from itertools import izip
names = list('datx')
vals  = reversed(list(xrange(len(names))))
zipped = izip(names, vals)
print sorted(zipped, key=lambda x: x[1])
