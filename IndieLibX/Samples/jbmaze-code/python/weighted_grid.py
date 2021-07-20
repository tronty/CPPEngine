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
import ChunkyPy
import grid
import weighted_cell

class WeightedGrid(Grid):

	def distances(self, adistances):
		m_distances = adistances
		farthest, m_maximum = adistances.max
	

	def prepare_grid(self):
		m_grid = [[0 for x in range(grid.Grid.m_columns)] for y in range(grid.Grid.m_rows)] 
		for row in range(grid.Grid.m_rows):
			for column in range(grid.Grid.m_columns):
				WeightedCell(row, column)
			
		
	

	def background_color_for(self, cell):
		if cell.weight > 1:
			return (255, 0, 0) 
		elif m_distances:
			if not distance == m_distances[cell]:
				return None
			intensity = 64 + 191 * (m_maximum - distance) / m_maximum
			return (intensity, intensity, 0) 
		
	

