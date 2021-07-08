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
import cell

class CubeCell(cell.Cell):
	def GetFace(self):
		return m_face

	def __init__(self, face, row, column):
		m_face = face
		super(row, column)
	


class CubeGrid(grid.Grid):
	#dim=rows # ???

	def __init__(self, adim):
		super(adim, adim)
	

	def prepare_grid(self):
		for face in [0 for x in range(6)]: 
			for row in [0 for x in range(dim)]:
				for column in [0 for x in range(dim)]:
					CubeCell(face, row, column)
				
			
		
	

	def each_face(self):
		 for face in m_grid.each:
			yield face
		
	

	def each_row(self):
		for face in each_face:
			for row in face:# ???
				yield row
			
		
	

	def random_cell(self):
		face	 = randint(0, 6)
		row		= randint(0, dim)
		column = randint(0, dim)

		m_grid[face][row][column]
	

	def size(self):
		return 6 * dim * dim
	

	def configure_cells(self):
		for cell in each_cell: 
			face, row, column = cell.face, cell.row, cell.column

			cell.west	= self[face, row, column-1]
			cell.east	= self[face, row, column+1]
			cell.north = self[face, row-1, column]
			cell.south = self[face, row+1, column]
		
	

	def GetElem(self, face, row, column):
		if face < 0 or face >= 6:
			return None
		face, row, column = wrap(face, row, column)
		return m_grid[face][row][column]
	

	def wrap(self, face, row, column):
		n = dim-1

		if row < 0:
			if face == 0:
				return [4, column, 0]
			if face == 1:
				return [4, n, column]
			if face == 2:
				return [4, n-column, n]
			if face == 3:
				return [4, 0, n-column]
			if face == 4:
				return [3, 0, n-column]
			if face == 5:
				return [1, n, column]
		elif row >= dim:
			if face == 0:
				return [5, n-column, 0]
			if face == 1:
				return [5, 0, column]
			if face == 2:
				return [5, column, n]
			if face == 3:
				return [5, n, n-column]
			if face == 4:
				return [1, 0, column]
			if face == 5:
				return [3, n, n-column]
		elif column < 0:
			if face == 0:
				return [3, row, n]
			if face == 1:
				return [0, row, n]
			if face == 2:
				return [1, row, n]
			if face == 3:
				return [2, row, n]
			if face == 4:
				return [0, 0, row]
			if face == 5:
				return [0, n, n-row]
		elif column >= dim :
			if face == 0:
				return [1, row, 0]
			if face == 1:
				return [2, row, 0]
			if face == 2: 
				return [3, row, 0]
			if face == 3:
				return [0, row, 0]
			if face == 4:
				return [2, 0, n-row]
			if face == 5:
				return [2, n, row]
		

		return [face, row, column] 
	

	def to_png(self, cell_size=10, inset=0):
    		#inset = (cell_size * inset).to_i
		inset = (cell_size * inset)# ??? .to_i

		face_width = cell_size * dim 
		face_height = cell_size * dim 

		img_width = 4 * face_width 
		img_height = 3 * face_height 

		offsets = [[0, 1], [1, 1], [2, 1], [3, 1], [1, 0], [1, 2]] 

		background = (255,255,255)
		wall = (0,0,0)
		outline = (0xd0, 0xd0, 0xd0)

		img = ChunkyPy.Image(img_width + 1, img_height + 1, background)
		
		draw_outlines(img, face_width, face_height, outline) 

		for backgrounds, walls in mode:
			for cell in each_cell:
				x = offsets[cell.face][0] * face_width + cell.column * cell_size 
				y = offsets[cell.face][1] * face_height + cell.row * cell_size 

				if inset > 0:
					to_png_with_inset(img, cell, mode, cell_size, wall, x, y, inset)
				else:
					to_png_without_inset(img, cell, mode, cell_size, wall, x, y)
				
			
		

		return img

	def draw_outlines(self, img, height, width, outline):
		# face #0
		img.rect(0, height, width, height*2, outline)
		
		# faces #2 & #3
		img.rect(width*2, height, width*4, height*2, outline)
		# line between faces #2 & #3
		img.line(width*3, height, width*3, height*2, outline)

		# face #4
		img.rect(width, 0, width*2, height, outline)

		# face #5
		img.rect(width, height*2, width*2, height*3, outline)
	

	def to_png_without_inset(self, img, cell, mode, cell_size, wall, x, y):
		x1, y1 = x, y
		x2 = x1 + cell_size
		y2 = y1 + cell_size

		if mode == backgrounds:
			color = background_color_for(cell)
			if color:
				img.rect(x, y, x2, y2, color, color) 
		else:
			if (not cell.north.face == cell.face) and (not cell.linked==cell.north):
				img.line(x1, y1, x2, y1, wall)
			if (not cell.west.face == cell.face) and (not cell.linked==cell.west):
				img.line(x1, y1, x1, y2, wall)
			if not cell.linked==cell.east:
				img.line(x2, y1, x2, y2, wall)
			if not cell.linked==cell.south:
				img.line(x1, y2, x2, y2, wall)
		
