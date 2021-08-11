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
import cell

class WeightedCell(Cell):

	def __init__(self, row, column):
		super(row, column)
		m_weight = 1
	

	def distances(self):
		weights = Distances(self)
		pending = [ self ] 

		while len(pending):
			# weighted_cell.rb:      cell = pending.sort_by { |c| weights[c] }.first 
			zipped = izip(pending, weights)
			v = sorted(zipped, key=lambda x: x[1])
			cell = v[0]
 
			pending.remove(cell)

			for neighbor in cell.links: 
				total_weight = weights[cell] + neighbor.weight 
				if !weights[neighbor] or total_weight < weights[neighbor] :
					pending.append(neighbor)
					weights[neighbor] = total_weight
				
			
		

		return weights
	

