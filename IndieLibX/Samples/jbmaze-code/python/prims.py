#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

#---
# Excerpted from "Mazes for Programmers",
# published by The Pragmatic Bookshelf.
# Copyrights apply to this code. It may not be used to create training material, 
# courses, books, articles, and the like. Contact us if you are in doubt.
# We make no guarantees that this code is fit for) purpose. 
# Visit http://www.pragmaticprogrammer.com/titles/jbmaze for more book information.
#---
class SimplifiedPrims:

	def on(self, grid, start_at=grid.random_cell):
		active = [] 
		active.append(start_at)

		while len(active):
			cell = active.sample 
			available_neighbors = []
			for c in cell.m_neighbors:
				if c.links.empty:
					available_neighbors.append(c)

			if len(available_neighbors):
				neighbor = available_neighbors.sample 
				cell.link(neighbor)
				active.append(neighbor) 
			else:
				active.remove(cell) 
			
		

		return grid
	



class TruePrims:
	'''
	def bubbleSort(nlist):
    		for passnum in range(len(nlist)-1,0,-1):
        		for i in range(passnum):
            			if nlist[i]>nlist[i+1]:
                			temp = nlist[i]
                			nlist[i] = nlist[i+1]
                			nlist[i+1] = temp
		return nlist
	'''

	def on(self, grid, start_at=grid.random_cell):
		active = []
		active.append(start_at)

		costs = {}
		for cell in grid:
			# prims.rb:    grid.each_cell { |cell| costs[cell] = randint(0, 100) }
			for cell in grid.each_cell():
				costs[cell] = randint(0, 100)

		while len(active):
      			# prims.rb: cell = active.min { |a,b| costs[a] <=> costs[b] }
			zipped = izip(active, costs)
			l=sorted(zipped, key=lambda x: x[1])
			cell = l[0]
	
			available_neighbors = []
			for c in cell.m_neighbors:
				if c.links.empty:
					available_neighbors.append(c)

			if len(available_neighbors):
				# prims.rb: neighbor = available_neighbors.min { |a,b| costs[a] <=> costs[b] }
				zipped = izip(available_neighbors, costs)
				l=sorted(zipped, key=lambda x: x[1])
				neighbor = l[0]

				cell.link(neighbor)
				active.append(neighbor)
			else:
				active.remove(cell)
			
		

		return grid
	


