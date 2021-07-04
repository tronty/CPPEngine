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
class Sidewinder:

	def on(self, grid):
		for row in grid.each_row: 
			run = []
			for cell in row:
				run.append(cell)

				at_eastern_boundary	= (cell.east == None) 
				at_northern_boundary = (cell.north == None) 

				should_close_out = at_eastern_boundary or (not at_northern_boundary and randint(0, 2) == 0) 

				if should_close_out:
					member = run.sample
					if member.north:
						member.link(member.north) 
					run.clear
				else:
					cell.link(cell.east)
				
			
		

		return grid
	


