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

class OverCell(cell.Cell):
	def __init__(self, row, column, grid):
		super(row, column)
		m_grid = grid
	

	def neighbors(self):
		list_ = super 
		if can_tunnel_north:
			list_.append(north.north)  
		if can_tunnel_south:
			list_.append(south.south)
		if can_tunnel_east:
			list_.append(east.east)
		if can_tunnel_west:
			list_.append(west.west) 

		return list_
	

	def can_tunnel_north(self):
		return north and north.north and north.horizontal_passage
	

	def can_tunnel_south(self):
		return south and south.south and south.horizontal_passage
	

	def can_tunnel_east(self):
		return east and east.east and east.vertical_passage
	

	def can_tunnel_west(self):
		return west and west.west and west.vertical_passage
	

	def horizontal_passage(self):
		return (linked==east) and linked==west and not linked==north and not linked==south
	

	def vertical_passage(self):
		return (linked==north) and linked==south and not linked==east and not linked==west
	

	def link(cell, bidi=True):
		if north and north == cell.south:
			neighbor = north
		elif south and south == cell.north:
			neighbor = south
		elif east and east == cell.west:
			neighbor = east
		elif west and west == cell.east:
			neighbor = west
		 

		if neighbor :
			return m_grid.tunnel_under(neighbor) 
		else:
			return super
		
	


class UnderCell(cell.Cell):
	def __init__(self, over_cell):
		super(over_cell.row, over_cell.column) 

		if over_cell.horizontal_passage:
			self.north = over_cell.north 
			over_cell.north.south = self
			self.south = over_cell.south
			over_cell.south.north = self 

			link(north)
			link(south)
		else:
			self.east = over_cell.east 
			over_cell.east.west = self
			self.west = over_cell.west
			over_cell.west.east = self 

			link(east)
			link(west)
		
	

	def horizontal_passage(self):
		return east or west
	

	def vertical_passage(self):
		return north or south
	

