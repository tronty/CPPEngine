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

class PolarCell(cell.Cell):

	def __init__(self, row, column):
		super
		m_outward = []
		cw=False
		ccw=False
		inward=False 
	

	def neighbors(self):
		list_ = []
		if cw:
			list_.append(cw)
		if ccw:
			list_.append(ccw) 
		if inward:
			list_.append(inward) 
		list_ += m_outward
		return list_
	

