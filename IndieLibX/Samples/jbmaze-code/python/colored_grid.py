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
import grid
import ChunkyPy

class ColoredGrid(Grid):
	def distances=(self, distances) :
		m_distances = distances
		farthest, m_maximum = distances.max 
	

	def background_color_for(self, cell) :
		distance = m_distances[cell] or return None
		intensity = (m_maximum - distance).to_f / m_maximum 
		dark = (255 * intensity).round
		bright = 128 + (127 * intensity).round 
		return (dark, bright, dark) 
	

