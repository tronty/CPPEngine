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
import prims
import grid

grid = grid.Grid(20, 20)
prims.SimplifiedPrims.on(grid)

filename = "prims-simple.png"
grid.saveImg(filename)
ln="saved to %s" % filename
print ln

grid = grid.Grid(20, 20)
prims.TruePrims.on(grid)

filename = "prims-True.png"
grid.saveImg(filename)
ln="saved to %s" % filename
print ln
