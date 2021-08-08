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
import mask
import masked_grid
import recursive_backtracker

mask = Mask(5, 5) 

mask[0, 0] = False 
mask[2, 2] = False
mask[4, 4] = False 

grid = MaskedGrid(mask) 
RecursiveBacktracker.on(grid)

print grid
