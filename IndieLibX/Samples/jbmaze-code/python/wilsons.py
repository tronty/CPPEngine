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
# We make no guarantees that this code is fit for any purpose. 
# Visit http://www.pragmaticprogrammer.com/titles/jbmaze for more book information.
#---
class Wilsons:

	def on(self, grid):
		unvisited = [] 
		# wilsons.rb:    grid.each_cell { |cell| unvisited << cell }
		for cell in grid.each_cell():
			cell.unvisited.append(cell)

		first = unvisited.sample 
		unvisited.remove(first) 

		while len(unvisited):
			cell = unvisited.sample 
			path = [cell]

			while unvisited.include==cell:
				cell = cell.m_neighbors.sample 
				position = path.index(cell) 
				if position:
					path = path[0..position] 
				else:
					path.append(cell)
				
			 

			for index in range(0,path.length-2):
				path[index].link(path[index + 1]) 
				unvisited.remove(path[index]) 
			 
		

		return grid
	


