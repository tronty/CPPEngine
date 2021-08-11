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
import binary_tree
import sidewinder
import aldous_broder
import wilsons
import hunt_and_kill

algorithms = [BinaryTree, Sidewinder, AldousBroder, Wilsons, HuntAndKill] 

tries = 100 
size = 20 

averages = {}
for algorithm in algorithms:
	ln="running %14s..." % algorithm
	print ln

	deadend_counts = []
	for i in range(1,tries.times):
		grid = Grid(size, size) 
		algorithm.on(grid) 
		deadend_counts.append(grid.deadends.count) 

	# deadend_counts.rb:  total_deadends = deadend_counts.inject(0) { |s,a| s + a }
	total_deadends=0
	for a in deadend_counts:
		total_deadends+=a
	averages[algorithm] = total_deadends / deadend_counts.length 


total_cells = size * size
print
ln="Average dead-ends per %dx%d maze (%d cells):" % (size,size,total_cells)
print ln
print

# deadend_counts.rb:sorted_algorithms = algorithms.sort_by { |algorithm| -averages[algorithm] } 
zipped = izip(-averages,algorithms)
sorted_algorithms = sorted(zipped, key=lambda x: x[1])

for algorithm in sorted_algorithms:
	percentage = averages[algorithm] * 100.0 / (size * size)
	print "%14s : %3d/%d (%d%%)" % [algorithm, averages[algorithm], total_cells, percentage]

