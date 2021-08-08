#---
# Excerpted from "Mazes for Programmers",
# published by The Pragmatic Bookshelf.
# Copyrights apply to this code. It may not be used to create training material, 
# courses, books, articles, and the like. Contact us if you are in doubt.
# We make no guarantees that this code is fit for any purpose. 
# Visit http://www.pragmaticprogrammer.com/titles/jbmaze for more book information.
#---
require './grid'
require './recursive_backtracker'

grid = Grid.new(20, 20)
RecursiveBacktracker.recursively_on(grid)

filename = "recursive_backtracker.png"
grid.to_png.save(filename)
puts "saved to #{filename}"
