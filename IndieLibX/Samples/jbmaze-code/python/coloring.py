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
import colored_grid
import binary_tree

grid = ColoredGrid(25, 25)
BinaryTree.on(grid)

start = grid[grid.rows / 2, grid.columns / 2] 

grid.distances = start.distances

filename = "colorized.png"
grid.saveImg(filename)
ln="saved image to %s" % filename
print ln
