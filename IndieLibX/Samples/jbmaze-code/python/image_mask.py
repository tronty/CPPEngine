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

if len(sys.argv)<2:
	abort "Please specify a PNG image to use as a template"
	print abort
mask = Mask.from_png(sys.argv[0])
grid = MaskedGrid(mask)
RecursiveBacktracker.on(grid)

filename = "masked.png"
grid.to_png(cell_size: 5).save(filename)
ln="saved image to %s" % filename
print ln

