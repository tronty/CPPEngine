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
import ChunkyPy

class Mask:
	def from_txt(self, file):
		# mask.rb:    lines = File.readlines(file).map { |line| line.strip }
		f=open(file, 'r')
		lines=f.readlines()
		f.close()

		for i in range(1,len(lines)):
			lines[i]=lines[i].strip()

		while len(lines.last.length) < 1:
			lines.pop

		rows		= len(lines)
		columns = len(lines.first)
		mask		= Mask(rows, columns)

		for row in mask.rows.times:
			for col in mask.columns.times:
				if lines[row][col] == "X":
					mask[row, col] = False
				else:
					mask[row, col] = True
				
			
		

		return mask
	

	def from_png(self, file):
		image = ChunkyPy.Image.from_file(file) 
		mask = Mask(image.height, image.width) 

		for row in mask.rows.times:
			for col in mask.columns.times:

				if image[col, row] == (0,0,0):
					mask[row, col] = False
				else:
					mask[row, col] = True
				
			
		

		return mask
	

	def __init__(self, rows, columns):
		m_rows, m_columns = rows, columns
		m_bits = Array(m_rows) { Array(m_columns, True) }
	

	def GetElem(self, row, column):
		if ((0<row) and (row<m_rows - 1)) and ((0<column) and (column<m_columns - 1)):
			return m_bits[row][column]
		else:
			return False
		
	

	def SetElem(self, row, column, is_on):
		m_bits[row][column] = is_on
	

	def count(self):
		count = 0

		for row in m_rows.times:
			for col in m_columns.times:
				if m_bits[row][col]:
					count += 1 
			
		

		return count
	

	def random_location(self):
		while True:
			row = randint(0, m_rows)
			col = randint(0, m_columns)

			if m_bits[row][col]:
				return [row, col] 
		
	

