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
class Distances:
	def __init__(self, root):
		m_root = root
		m_cells = {}
		m_cells[m_root] = 0
	

	def GetElem(self, cell):
		return m_cells[cell]
	

	def SetElem(self, cell, distance):
		m_cells[cell] = distance
	

	def cells(self):
		return m_cells.keys
	

	def path_to(self, goal):
		current = goal 

		breadcrumbs = Distances(m_root) 
		breadcrumbs[current] = m_cells[current]

		while not current == m_root: 
			for neighbor in current.links.each: 
				if m_cells[neighbor] < m_cells[current] :
					breadcrumbs[neighbor] = m_cells[neighbor] 
					current = neighbor
					break
				
			
		

		return breadcrumbs
	

	def max(self):
		max_distance = 0
		max_cell = m_root

		for cell, distance in m_cells.each:
			if distance > max_distance:
				max_cell = cell
				max_distance = distance
			
		

		return [max_cell, max_distance]
	

