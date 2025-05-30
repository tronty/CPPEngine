#
# Copyright (c) 2018 Tuomas Roenty   http://www.tuomasronty.fi/
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
class AldousBroder:

	def on(grid):
		cell = grid.random_cell 
		unvisited = grid.size - 1 

		while unvisited > 0:
			neighbor = cell.m_neighbors.sample 

			if neighbor.links.empty:
				cell.link(neighbor) 
				unvisited -= 1 
			

			cell = neighbor 
		

		return grid
	


