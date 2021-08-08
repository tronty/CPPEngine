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
import growing_tree
import grid

def save(grid, filename):
	grid.saveImg(filename)
	ln="saved to %s" % filename
	print ln

grid = grid.Grid(20, 20)
# growing_tree_demo.rb: growing_tree.GrowingTree.on(grid) { |list_| list_.sample }
list_=list_.sample
growing_tree.GrowingTree.on(grid)
save(grid, "growing-tree-random.png")

grid = grid.Grid(20, 20)
# growing_tree_demo.rb: growing_tree.GrowingTree.on(grid) { |list_| list_.last }
list_=list_[len(list_)-1]
growing_tree.GrowingTree.on(grid)
save(grid, "growing-tree-last.png")

grid = grid.Grid(20, 20)
# growing_tree_demo.rb: growing_tree.GrowingTree.on(grid) { |list_| (randint(0, 2) == 0) ? list_.last : list_.sample }
ri=random.randint(1,3)
if ri==1:
	list_=list_[len(list_)-1]
else:
	list_=list_.sample
save(grid, "growing-tree-mix.png")

