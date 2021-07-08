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
import kruskals
import weave_grid

class SimpleOverCell(OverCell):
  def neighbors(self):
    list_ = []
    if north:
    	list_.append(north) 
    if south:
    	list_.append(south) 
    if east:
    	list_.append(east)  
    if west:
    	list_.append(west)  
    list_
  


class PreconfiguredGrid(WeaveGrid):
  def prepare_grid(self):
    for row in rows:
      for column in columns:
        SimpleOverCell(row, column, self)
      
    
  


grid = grid.PreconfiguredGrid(20, 20)
state = kruskals.Kruskals.State(grid)

for i in range(0, grid.size.times):
  row = 1 + randint(0, grid.rows - 2)
  column = 1 + randint(0, grid.columns - 2)
  state.add_crossing(grid[row, column])


kruskals.Kruskals.on(grid, state)

filename = "kruskals.png"
grid.saveImg(filename)
ln="saved to %s" % filename
print ln
