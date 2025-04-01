#
# Copyright (c) 2018 Tuomas Roenty   http://www.tuomasronty.fi/
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
import polar_cell

class PolarGrid(grid.Grid):
	def __init__(self, arows):
		super(arows, 1)
	def __init__(self, arows, acol):
		super(arows, acol)

	def prepare_grid(self):
		rows = [0 for x in range(len(super.rows))] 

		row_height = 1.0 / len(rows) 
		rows[0] = [ PolarCell(0, 0) ] 

		for row in range(1,len(rows)):
			radius = row.to_f / len(rows) 
			circumference = 2 * math.pi * radius 

			previous_count = len(rows[row - 1])
			estimated_cell_width = circumference / previous_count 
			ratio = (estimated_cell_width / row_height).round 

			cells = previous_count * ratio 
			# polar_grid.rb:      rows[row] = Array.new(cells) { |col| PolarCell.new(row, col) } 
			rows[row] = [0 for x in range(len(cells))] 
			for col in range(1,cells):
				rows[row][col]=PolarCell(row, col)
		return rows
	

	def configure_cells(self):
		for cell in each_cell:
			row, col = cell.row, cell.column

			if row > 0 :
				cell.cw	= self[row, col + 1] 
				cell.ccw = self[row, col - 1] 

				ratio = m_grid[row].length / m_grid[row - 1].length 
				parent = m_grid[row - 1][col / ratio] 
				parent.m_outward.append(cell) 
				cell.inward = parent 
			
		
	

	def GetElem(self, row, column):
		if (row.between<0) or (row.between>len(rows)):
			return None
		return m_grid[row][column % m_grid[row].count]
	

	def random_cell(self):
		row = randint(0, len(rows))
		col = randint(0, m_grid[row].length)
		return m_grid[row][col]
	

	def to_png(self, cell_size=10):
		to_png_v2(self, cell_size)
	

	def to_png_v1(self, cell_size=10):
		img_size = 2 * len(rows) * cell_size 

		background = (255,255,255)
		wall = (0,0,0)

		img = ChunkyPy.Image(img_size + 1, img_size + 1, background)
		center = img_size / 2 

		for cell in each_cell:
			theta				= 2 * math.pi / m_grid[cell.row].length 
			inner_radius = cell.row * cell_size
			outer_radius = (cell.row + 1) * cell_size
			theta_ccw		= cell.column * theta
			theta_cw		 = (cell.column + 1) * theta 

			ax = center + (inner_radius * Math.cos(theta_ccw)).to_i 
			ay = center + (inner_radius * Math.sin(theta_ccw)).to_i
			bx = center + (outer_radius * Math.cos(theta_ccw)).to_i
			by = center + (outer_radius * Math.sin(theta_ccw)).to_i
			cx = center + (inner_radius * Math.cos(theta_cw)).to_i
			cy = center + (inner_radius * Math.sin(theta_cw)).to_i
			dx = center + (outer_radius * Math.cos(theta_cw)).to_i
			dy = center + (outer_radius * Math.sin(theta_cw)).to_i 

			if not cell.linked==cell.north: 
				img.line(ax, ay, cx, cy, wall)
			if not cell.linked==cell.east:
				img.line(cx, cy, dx, dy, wall)		

		img.circle(center, center, len(rows) * cell_size, wall) 
		return img
	

	def to_png_v2(self, cell_size=10):
		img_size = 2 * len(rows) * cell_size

		background = (255,255,255)
		wall = (0,0,0)

		img = ChunkyPy.Image(img_size + 1, img_size + 1, background)
		center = img_size / 2

		for cell in each_cell:
			if cell.row == 0:
				continue

			# ...


			theta				= 2 * math.pi / m_grid[cell.row].length
			inner_radius = cell.row * cell_size
			outer_radius = (cell.row + 1) * cell_size
			theta_ccw		= cell.column * theta
			theta_cw		 = (cell.column + 1) * theta

			ax = center + (inner_radius * Math.cos(theta_ccw)).to_i
			ay = center + (inner_radius * Math.sin(theta_ccw)).to_i
			bx = center + (outer_radius * Math.cos(theta_ccw)).to_i
			by = center + (outer_radius * Math.sin(theta_ccw)).to_i
			cx = center + (inner_radius * Math.cos(theta_cw)).to_i
			cy = center + (inner_radius * Math.sin(theta_cw)).to_i
			dx = center + (outer_radius * Math.cos(theta_cw)).to_i
			dy = center + (outer_radius * Math.sin(theta_cw)).to_i

			if not cell.linked==cell.inward:
				img.line(ax, ay, cx, cy, wall)
			if not cell.linked==cell.cw:
				img.line(cx, cy, dx, dy, wall)		

		img.circle(center, center, len(rows) * cell_size, wall)
		return img

'''
if __name__ == "__main__":
    source = sys.argv[1]
    td = PolarGrid()
    needed_stuff = td.convert(source)
    print(needed_stuff)
 '''
 
