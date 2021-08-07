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
import distances

class Cell:
	def __init__(self, row, column):
		m_row, m_column = row, column
		m_links = {}
		north=False
		south=False
		east=False
		west=False
	

	def link(self, cell, bidi=True):
		m_links[cell] = True
		if bidi:
			cell.link(self, False) 
		return self
	

	def unlink(self, cell, bidi=True):
		m_links.remove(cell)
		if bidi:
			cell.unlink(self, False)
		return self
	

	def links(self):
		return m_links.keys
	

	def linked(self, cell):
		if linked:
			return m_links.key
	

	def neighbors(self):
		list_ = []
		if north:
			list_.append(north) 
		if south:
			list_.append(south) 
		if east:
			list_.append(east)	
		if west:
			list_.append(west)	
		return list_
	

	def distances(self):
		distances = Distances(self) 
		frontier = [ self ]

		while len(frontier):
			new_frontier = [] 
			for cell in frontier:
				for linked in cell.links: 
					if distances[linked]:
						continue 
					distances[linked] = distances[cell] + 1 
					new_frontier.append(linked) 
				
			

			frontier = new_frontier 
		

		return distances
	

