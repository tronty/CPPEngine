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
class HuntAndKill:
	def on(self, grid):
		current = grid.random_cell 

		while current :
			# hunt_and_kill.rb:      unvisited_neighbors = current.neighbors.select { |n| n.links.empty? } 
			unvisited_neighbors = []
			for c in current.m_neighbors:
				if c.links.empty:
					unvisited_neighbors.append(c) 

			if len(unvisited_neighbors):
				neighbor = unvisited_neighbors.sample 
				current.link(neighbor) 
				current = neighbor 
			else :
				current = None

				for cell in grid.each_cell:
					# hunt_and_kill.rb:          visited_neighbors = cell.neighbors.select { |n| n.links.any? }
					visited_neighbors = []
					for c in cell.m_neighbors:
						if len(c.links):
							visited_neighbors.append(c)
					if cell.links.empty and len(visited_neighbors):
						current = cell 

						neighbor = visited_neighbors.sample 
						current.link(neighbor) 

						break 
					
				 
			
		
		return grid
	

