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

class TriangleCell(cell.Cell):
	def upright(self):
		return (row + column).even
	

	def neighbors(self):
		list_ = []
		if west:
			list_.append(west) 
		if east:
			list_.append(east) 
		if (not upright) and north:
			list_.append(north) 
		if (not upright) and south:
			list_.append(south) 
		return list_
	

