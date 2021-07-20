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
import polar_grid

class HemisphereCell(PolarCell):

	def __init__(self, hemisphere, row, column):
		m_hemisphere = hemisphere
		super(row, column)
	


class HemisphereGrid(PolarGrid):

	def __init__(self, id, arows):
		m_id = id
		super(arows)
	

	def size(self, row) :
		m_grid[row].length
	

	def prepare_grid(self):
		grid = [0 for x in range(len(super.rows))]

		angular_height = math.pi / (2 * m_rows) 

		grid[0] = [ HemisphereCell(id, 0, 0) ]

		for row in range(1,len(rows)):
			theta = (row + 1) * angular_height 
			radius = Math.sin(theta)
			circumference = 2 * math.pi * radius 

			previous_count = grid[row - 1].length
			estimated_cell_width = circumference / previous_count
			ratio = (estimated_cell_width / angular_height).round

			cells = previous_count * ratio
			# sphere_grid.rb:      grid[row] = Array.new(cells) { |col| HemisphereCell.new(id, row, col) }
			grid[row] = [0 for x in range(len(cells))]
			for col in range(1,len(cells)):
				grid[row][col] = HemisphereCell(id, row, col)

		return grid
	


class SphereGrid(Grid):
	def __init__(self, rows):
		if not rows.even? 
			raise ArgumentError, "argument must be an even number"
		

		m_equator = rows / 2 
		super(rows, 1)
	

	def prepare_grid(self):
		# sphere_grid.rb:    Array.new(2) { |id| HemisphereGrid.new(id, @equator) }
		return [ |id|, HemisphereGrid(id, m_equator) ] 
	

	def configure_cells(self):
		belt = m_equator - 1
		size(belt).times do |index| 
			a, b = self[0, belt, index], self[1, belt, index]
			a.m_outward.append(b)
			b.m_outward.append(a)
		 
	

	def GetElem(self, hemi, row, column):
		m_grid[hemi][row, column]
	

	def size(self, row):
		m_grid[0].size(row)
	

	def each_cell(self):
		# sphere_grid.rb:      hemi.each_cell { |cell| yield cell }
		for cell in hemi.each_cell():
			yield cell
		
	

	def random_cell(self):
		m_grid.sample.random_cell
	

	def to_png(self, ideal_size=10) :
		img_height = ideal_size * m_rows 
		img_width = m_grid[0].size(m_equator - 1) * ideal_size 

		background = (255,255,255)
		wall = (0,0,0)

		img = ChunkyPy.Image(img_width + 1, img_height + 1, background)

		for cell in each_cell:
			row_size = size(cell.row)
			cell_width = img_width.to_f / row_size 

			x1 = cell.column * cell_width
			x2 = x1 + cell_width

			y1 = cell.row * ideal_size
			y2 = y1 + ideal_size

			if cell.hemisphere > 0 :
				y1 = img_height - y1
				y2 = img_height - y2
			 

			x1 = x1.round; y1 = y1.round
			x2 = x2.round; y2 = y2.round

			if cell.row > 0:
				img.line(x2, y1, x2, y2, wall) if not cell.linked?(cell.cw)
				img.line(x1, y1, x2, y1, wall) if not cell.linked?(cell.inward)
			

			if cell.hemisphere == 0 and cell.row == m_equator - 1:
				if not cell.linked==cell.m_outward[0]:
					img.line(x1, y2, x2, y2, wall) 
			 
		

		return img
	

