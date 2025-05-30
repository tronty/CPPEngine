[[

Procedural World

Following one man's task of building a virtual world from the comfort of his pajamas. Discusses Procedural Terrain, Vegetation and Architecture generation. Also OpenCL, Voxels and Computer Graphics in general.

 Cantor City
The Cantor set (or how you obtain it) is probably one of the simplest fractals out there. It was one of the first examples I encountered and it helped me greatly understanding whatever came after. Here is how I like to explain it:

1. We start with a horizontal segment.
2. We divide the segment into three equally sized segments.
3. We remove the segment in the middle
4. Repeat from step (2) for the two remaining segments



This produces a set of segments that puzzled mathematicians for a while. I am more interested in the fractal properties of the whole construct. It shows how a simple rule operating at multiple scales can generate complex structures. This set is too primitive to be used for realistic content, but it is the seed of many techniques you will require to achieve that.

This is how you would code the Cantor set using our grammars:
]]
cantordust
{
	instance "box"
	move [0, 120%, 0]
	divide x
		[width/3] cantordust
		[width/3] {}
		[width/3] cantordust
}

cantordust : width < 0.5 {}
[[
This is the real world so we need to make sure it stops. An easy way would be to add another definition of "cantordust" for when the size is small enough. This definition would do nothing, thus stopping any further subdivision:


Here you can see the output:


Let's make it more interesting. First let's add depth to it. Instead of segments we will use 3D boxes:
]]
volume {
	box
	select sides wall
}

wall {
	scale [100%, 100%, 0.2]
	defer wallpanels
}

wallpanels {
	repeat x [3] panel
}

panel {
	instance "wall_1m"
}

angletest {
	module {
		center xz
		move [20%, 0, 0]
		module volume
	}
	module {
		center xz
		move [50%, 0, -40%]
		rotate y 60
		module volume
	}
	module {
		center xz
		move [0%, 0, -40%]
		rotate y 110
		module volume
	}
}

ramps {
	center xz
	loft ngon 32
	[100%, 100%]
	select sides {
		move [0, -1.5, 0]
		scale [100%, 1, 1]
		module arch
	}
}

tower {
	divide y
	[100%] {
		material brick
		module basicturret
	}
	[0.5] {
		center xz
		scale [100%-1, 100%, 100%-1]
		material stone
		module beam
	}
	[3] {
		module {
			center xz
			scale [93%, 2, 100%]
			material stone
			module ramps
		}
		material stone
		module turret
	}
	[3] {
		material stone
		module battlement
	}
}
[[]]
volume {
	box
	select sides wall
	occludes
}

wall {
	scale [100%, 100%, 0.2]
	defer wallpanels
}

wallpanels {
	repeat x [3] panel
}

panel : occluded {
	scale [100%, 0.5, 100%]
	instance "wall_1m"
}

panel {
	instance "wall_1m"
}

angletest {
	module {
		center xz
		move [20%, 0, 0]
		module volume
	}
	module {
		center xz
		move [50%, 0, -40%]
		rotate y 60
		module volume
	}
	module {
		center xz
		move [0%, 0, -40%]
		rotate y 110
		module volume
	}
}
[[]]
volume {
	box
	select sides wall
	occludes
}

wall {
	scale [100%, 100%, 0.2]
	defer wallpanels
	snap wall z 0.1
}

wallpanels {
	repeat x [3|wall] panel
}

panel : occluded {
}

panel {
	instance "wall_1m"
}

angletest {
	module {
		center xz
		move [20%, 0, 0]
		module volume
	}
	module {
		center xz
		move [50%, 0, -40%]
		rotate y 60
		module volume
	}
	module {
		center xz
		move [0%, 0, -40%]
		rotate y 110
		module volume
	}
}
[[]]
testrooms {
	module room
}

room : width > depth, width > 5 {
	divide x [width*0.5] room [width*0.5] room
}

room : depth >= width, depth > 5 {
	divide z [depth*0.5] room [depth*0.5] room
}

room {
	box
	select sides {
		scale[100%, 100%, 0.1]
		instance "wall_1m"
	}
}
[[]]
testrooms {
	const ratio 0.61
	const minroom 5
	const density 0.8
	module room
}

room : width > depth, depth > minroom, rnd < density {
	divide x [width*ratio] room [width*(1 - ratio)] room
}

room : depth >= width, width > minroom, rnd < density {
	divide z [depth*ratio] room [depth*(1 - ratio)] room
}

room {
	box
	select sides {
		scale[100%, 100%, 0.1]
		instance "wall_1m"
	}
}
[[]]
testrooms {
	const ratio 0.61
	const minroom 5
	const density 0.8
	const margin 0.5
	module room
}

room : width > depth, depth > minroom, rnd < density {
	divide x [width*ratio] room [width*(1 - ratio)] room
}

room : depth >= width, width > minroom, rnd < density {
	divide z [depth*ratio] room [depth*(1 - ratio)] room
}

room {
	scale[100%, 100%, 100%]
	box
	select sides {
		scale[100% - 2*margin, 100%, 0.1]
		move [margin, 0, 0]
		module wall
	}
}

wall : occluded {
	scale [100%, 2, 400%]
	instance "wall_1m"
}

wall {
	instance "wall_1m"
	occludes
}
[[]]
testrooms {
	const ratio 0.61
	const minroom 5
	const density 0.8
	const margin 0.5
	module room
}

room : width > depth, depth > minroom, rnd < density {
	divide x [width*ratio] room [width*(1 - ratio)] room
}

room : depth >= width, width > minroom, rnd < density {
	divide z [depth*ratio] room [depth*(1 - ratio)] room
}

room {
	scale[100%, 100%, 100%]
	box
	select sides {
		scale[100%, 100%, 0.1]
		module wall
	}
}

wall : occluded {
	divide x
		[50%] {instance "wall_1m"}
		[2]   {divide y [100%] {} [1] {instance "wall_1m"}}
		[50%] {instance "wall_1m"}
}

wall {
	scale[100% - 2*margin, 100%, 100%]
	move [margin, 0, 0.0]
	occludes
}



testrooms {
	const ratio 0.61
	const minroom 5
	const density 0.8
	const margin 0.5
	module room
	module outerwalls
}

outerwalls {
	box
	select sides outerwall
}

outerwall {
	scale [100%, 100%, 0.2]
	defer wallpanels
	repeat x [5] {
		instance "romanesque_smallwindow1_1m"
	}
}
[[]]
testrooms {
	const ratio 0.61
	const minroom 5
	const density 0.8
	const margin 0.5
	module room
	defer outerwalls
}

outerwalls {
	box
	select sides outerwall
}

outerwall {
	scale [100%, 100%, 0.2]
	repeat x [5|wall] {
		instance "romanesque_smallwindow1_1m"
	}
}

room : width > depth, depth > minroom, rnd < density {
	divide x [width*ratio] room [width*(1 - ratio)] room
}

room : depth >= width, width > minroom, rnd < density {
	divide z [depth*ratio] room [depth*(1 - ratio)] room
}

room {
	scale[100%, 100%, 100%]
	box
	select sides {
		scale[100%, 100%, 0.1]
		module wall
	}
}

wall : occluded {
	divide x
		[50%] {instance "wall_1m"}
		[2]   {divide y [100%] {} [1] {instance "wall_1m"}}
		[50%] {instance "wall_1m"}
	snap wall z 0.5
}

wall {
	scale[100% - 2*margin, 100%, 100%]
	move [margin, 0, 0.0]
	occludes
}
[[]]
ramps {
	center xz
	loft ngon 32
	[100%, 100%]
	select sides {
		move [0, -1.5, 0]
		scale [100%, 1, 1]
		module arch
	}
}

tower {
	divide y
	[100%] {
		material brick
		module basicturret
	}
	[0.5] {
		center xz
		scale [100%-1, 100%, 100%-1]
		material stone
		module beam
	}
	[3] {
		module {
			center xz
			scale [93%, 2, 100%]
			material stone
			module ramps
		}
		material stone
		module turret
	}
	[3] {
		material stone
		module battlement
	}
}
[[

Procedural World

Following one man's task of building a virtual world from the comfort of his pajamas. Discusses Procedural Terrain, Vegetation and Architecture generation. Also OpenCL, Voxels and Computer Graphics in general.
You can get Voxel Farm now. For more information click here.
Showing posts with label Procedural Architecture. Show all posts
Saturday, November 15, 2014
Cantor City
The Cantor set (or how you obtain it) is probably one of the simplest fractals out there. It was one of the first examples I encountered and it helped me greatly understanding whatever came after. Here is how I like to explain it:

1. We start with a horizontal segment.
2. We divide the segment into three equally sized segments.
3. We remove the segment in the middle
4. Repeat from step (2) for the two remaining segments



This produces a set of segments that puzzled mathematicians for a while. I am more interested in the fractal properties of the whole construct. It shows how a simple rule operating at multiple scales can generate complex structures. This set is too primitive to be used for realistic content, but it is the seed of many techniques you will require to achieve that.

This is how you would code the Cantor set using our grammars:

This is the real world so we need to make sure it stops. An easy way would be to add another definition of "cantordust" for when the size is small enough. This definition would do nothing, thus stopping any further subdivision:


Here you can see the output:


Let's make it more interesting. First let's add depth to it. Instead of segments we will use 3D boxes:


This already looks like we were growing little towers. We can make the towers taller if there is enough room:



Let's add a random element. We will not split into three equal spaces anymore, instead we will pick a random value for each iteration:


For a microsecond this image can fool you into thinking you have seen a city. I think that is pretty cool if you consider it is only a few lines of grammar code.

Of course this is not a replacement for a city or buildings. The cantor grammars are probably the simplest you can have. You will need a lot of more code to produce somethings that can pass as a city. But odds are it will be mostly variations of the Cantor dust.

3 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Cantor dust, City, fractal, L-System, Procedural Architecture, Procedural City
Sunday, November 9, 2014
Life without a debugger
Some programmers love the more literary aspects of coding. They enjoy learning new languages and the new structures of thought they bring. You will often hear words like "terse" and "art" from them. Some programmers like learning different frameworks, while others die by the not-invented-here rule. Some see themselves more like civil engineers: they have little regard for the nuances of coding, frameworks are just bureaucracy to them, whatever they build must stand on its own merits.

I have been a member of all these camps in one moment or another, so really no judgment here. But what probably unites all coders is that if you write a program, you will likely have to debug it.

If you have not stepped through your code at least once, you probably do not know what it is doing. Yes you can unit test all you want, but even then you should also step through your test code.

Karma dictates if you put a new language out there, you must also give a way for people to trace and debug it. That is what we just did with our LSystem and grammar language:




It turned to be quite surprising. We are very used to tracing imperative languages like C++ and Java, but tracing the execution of a L-System is unlike anything I had tried before.

I get the power of L-Systems and context sensitive grammars better now. This system feels like it had the ability to foresee, to plan features ahead. You see it happening in the video: often empty boxes appear in many different spots, like if the program was testing for different alternatives. That is in fact what it is happening.

It looks amazing that end features like the tip of a tower may appear even before than the base. In reality the program has already decided there will be a base, but its specific details are still a bit fuzzy so they come up much later. But then once they do appear, everything connects as it should, all predictions line up properly.

All that did not require planning from the programmer. In this case the code is not a series of instructions, it is more like a declaration of what the structure should be. The blocks fall into their pieces, the debugger just shows how.



It reminds me of another type of code at work: DNA. Now that would be one really cool debugger.

17 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Grammar, L-System, Procedural Architecture, Voxel Studio
Monday, May 5, 2014
Grammar Time
Yes, grammars. The last update had a hint of this, but here you can see them in more detail. As you probably know, we are building a repertoire of architectural grammars. The following video shows a few of them:



This set is geared towards medieval, fantasy settings, but I suspect most of these grammars would hold for a different theme.

This system becomes very interesting once you consider ending building blocks could be replaced by elements that tie closely to your project's vision. At any time you could replace the arches, columns, ornaments, even bricks, by custom components you may have sculpted earlier. What the grammar gives you is the order and structure of these prefabs, but the final look and feel can be pretty much up to you.

As you can see for the moment we are focusing on smaller grammars. You can think of these as smart brushes that will allow to lay walls, floors, bridges, even towers in the locations you choose.

While grammars are able to express entire buildings (even cities), I believe we need to start small and allow you to place these smaller elements following your imagination. Also there is little point in generating an entire castle if the building elements are not interesting enough. So we are making sure we have a solid repertoire of grammars before we take on larger things. Even something as simple as a basic stair tool can save you a lot of time.


As usual let me know what you think. I'm in particular interested in how many of these do you think we would need for a particular theme, how generic or specific they should be, what kind of parameters would you like to have as inputs.


27 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Grammar, L-System, Procedural Architecture
Friday, April 18, 2014
Video Update for April 2014
Wondering what happened in the last few months? Here is an update:


There are several things we did that were not covered in this update. You will notice a river in the background but there is no mention about water.


It is not that we are hydrophobic or that we want to tease you about this feature, we just want to spend more time improving the rendering.

I also go on in this update about how clean and sharp our new tools are. There is indeed a big difference in the new toolset, but still there are serious issues with aliasing when you bring detail beyond what the voxels can encode. For instance, the line tool now can do much better lines, but we still cannot do a one-voxel thick line that goes in any angle. This is because in order to fix the aliasing in this line would need sub-voxel resolution. So it OK to expect cleaner lines, but they can still break due to aliasing.
55 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Grammar, Physics, Procedural Architecture, video update, Voxelization
Wednesday, February 12, 2014
Vacansopapurosophobia
The terror of staring at a blank page. We all have felt it one time or another. Even seasoned artists can feel anxiety when starting a project from scratch. If someone is watching over your shoulder it can become unbearable. At that point you may just give up and save yourself all the stress.

Sandbox games put the player in a similar situation. You now have the ability to shape the world and create beautiful things, but again there is the game world as a blank canvas looking back at you, wondering if you are going to suck as a creator again.

If you share this sandbox world with others, you may have already been exposed to all sort of wonderful creations. Maybe you have seen them on YouTube or Twitch. These people know how to build perfect columns, archways and vaults for their creations.

So you start placing a few blocks, but it gets you nowhere close to that grandiose design you had in mind. It is not that you lack the imagination. What you don't have is the technique to produce the elements you want.

We have designed a tool that can level the playing feel.

For a long time now we had architecture grammars. These grammars take a block of space and create something inside. I have shown many examples so far. Very often they were used to generate entire buildings. The trick is, you do not have to target full buildings for these grammars to be very useful.

You can realize your vision by combining simpler elements like roofs, barandas, columns, arches, vaults, etc. And these elements, instead of being labor intense, could be just the output of smart architecture grammars.

The fact that these are full fledged programs means they get to adapt to different configurations. For instance look at the output of this architecture program:



The program was clever enough to reinforce the corners. Yes, you could do this by hand, but imagine how many corners like this one you will get in your massive castle. Also note how the program has introduced a random element to the stone placement, making the walls more interesting.

Here is a video showing a different example:


As you can see the program adds more columns as they are needed. No distortion in the columns occurs as you make the box wider or taller.

At this point your are no longer dealing with the masonry. It is like you had hired one of the talented builders from YouTube to do the low level work for you. Knowing you have a repertoire of interesting prefabs you can combine in imaginative ways will surely help you get over your vacansopapurosophobia.


73 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Grammar, Procedural Architecture, Sandbox
Monday, February 4, 2013
Voxel Studio Videos
Here are three videos I captured from Voxel Studio to help with the AiGameDev.com interview:

12 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Procedural Architecture, Voxel Studio, Voxel Tree
Monday, December 3, 2012
Videos of Caves and Buildings
Here is a series of videos I captured over the weekend.

The first one shows a bit of the architecture, also how a couple of useful underground passages can save you time while travelling:



The second is a trip I did to get close to a tower I saw. It seemed simple at the beginning, but the terrain had some severe accidents so I had to figure out my way. I found some caves that helped me get there, also saw I needed to do a quick bridge over a chasm. It runs at double the speed so you would not get bored by all the wandering. Also note that at the end of the video I carve some holes into the tower walls.



The next video also runs at double the speed. It shares the same beginning as the first video posted here, but goes a lot farther.



And the last video shows how the caves can sometimes go really deep underground. You often see cave openings into the surface, but also they could be right under your feet and you would not know. I cheated in this video by showing the wireframe, otherwise it would have taken an awful lot of time to find these caves.

26 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Caves, Procedural Architecture, VoxelFarm Realtime
Architecture On-The-Fly
Boy this was a tough one. I always knew it was coming, tried to postpone it for as long as I could.

In the past I had succeeded creating complex buildings out of L-Systems. While it was fast, it was for a different approach to world generation. It was for static game worlds like Skyrim's. I had the luxury of time.

In a world that generates as the viewer moves, most of the time I had for generation was already taken by the terrain and trees.

Architecture is something you cannot just instance. You want buildings to be different, even if they belong to the same class. I needed the actual grammar programs to run in real time, its polygonal output  voxelized and then merged with the rest of the voxel data.

You may think going from polygons to voxels then back to polygons is an awful waste of time. It had been, except for the fact I needed changes made Minecraft-style to also work over buildings. When you are wielding that pickaxe you don't really care if that rock in front is a natural formation or a cathedral wall.

At the end I did manage to make it work. As usual most of the solution went into creating indices to accelerate the actual work.

I had to make compromises. For starters, the voxel resolution in the realtime engine is much coarser than what I had for the pre-computed worlds. This meant a lot of the detail in buildings had to go. This also applied to the following levels of detail. Detail had to be removed in all levels.

I also saw I needed simpler grammars just so the system could keep up in lower-end hardware. I found it  common to have thousands of different buildings in a single scene. The solution I found was to pass the LOD parameter to the building grammar at evaluation time. It made it possible, but now the building programmer has to worry about creating different views for the same building.

I am still very happy with the results of something I feared almost impossible. There is room for improvement, and this is a good thing. The hard part is over.

I leave you with some screenshots of a single tower grammar that appears scattered everywhere. Please do not mind the lack of actual cities, or the fact it is the same tower over and over again. This is rather a test for the system. I will be posting some videos later.













34 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Grammar, L-System, Procedural Architecture, VoxelFarm Realtime
Friday, August 17, 2012
More Grammar Screenshots
Here is another series of screenshots. These are also from the Canyon City grammar.






I really need to improve the lighting in this preview. I normally export the final mesh to 3ds max and render it with nice global illumination. The meshes I get out of this are so large 3ds max cannot deal with them anymore.

The mesh complexity is a problem only at this stage of the grammar edition. Once this is voxelized and optimized back to polygons, it will be fast enough even for realtime rendering.

Over the weekend I will see if I can load this into 3ds max to produce better previews and share them with you.


17 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Canyon City, Grammar, Procedural Architecture, Procedural City, Voxel Studio
Friday, August 10, 2012
Producing complex shapes
Architecture L-Systems deal with fairly simple shapes: boxes, tubes, cylinders, prisms, some times loft and revolution operations. They define the bounding volumes where you can instance actual geometry samples, like columns, windows, doors, bricks and tiles.

These basic shapes alone can get you far. The problem arises when you need these constructs to intersect each other. In most cases they produce complex shapes for which we don't even have names, let alone a unique mathematical formula. For instance, you can create a nice dome by doing a loft operation on a tube or a cylinder. Now, if you needed to blend two domes, you would be facing an increasingly difficult problem.

In other fields this is usually solved by "Constructive Solid Geometry" (CSG), which is about combining multiple shapes into bigger or smaller shapes by subtraction, addition, intersection and other similar operations. The problem with CSG is that it outputs a soup of triangles. The little boxes, cylinders, prisms we used to have disappear in this soup. This matters a lot for  architecture grammars. The rules you write are designed for these basic elements, not flat triangles.

I encountered this problem while building grammars for a Mosque. The base of the building is square, the top of the building is a dome which has a circular base. There is some kind of roof that transitions from the square into the circle. This is an element also found in Roman and Byzantine buildings.

In the next image you can see a red arrow pointing to it:


If you isolate this element, this is how it looks like:


It is some kind of curved square with a round hole in it. Not only this element is special. The side walls of the square base must be properly cut so it fits. Here you can see them:


These images, as usual, are the output of my architecture system. Which means these shapes entirely possible. They are quite easy to produce as well.

The trick again is in the occlusion rules. This time they can be combined with the recursive nature of L-Systems to great advantage.

The starting point is to define these shapes ignoring the weird cuts we will make later. This is what you get from a square base and a cone-shaped tube, both basic shapes:


Here comes the first trick. In reality, these shapes are made of smaller ones. I have reduced them just so you can see:


Using occlusion rules, we can hide the elements that are outside the box and above the roof. This produces a very coarse approximation to the shape we want:

Now comes the really important bit. The elements that are completely occluded will remain hidden, but those elements that are partially occluded we will subdivide them into smaller elements instead of just hiding them.

This produces a new set of elements. To this set we apply the same rule as before. Some will be entirely visible, some will be entirely occluded and some will be partially occluded. Once again, those that are partially occluded will be subdivided into a smaller set. Eventually the elements will be small to a point where it is not practical to subdivide again. This is where the process ends.

Here you can see the output of this method. Note that all the elements are boxes:



Obviously the cuts are not completely smooth. You still get these microscopic boxes. In my case this is not a problem at all. These volumes are sent to a voxelization phase. The tiny boxes are smaller than the voxel resolution so they are blended into straight features which later optimize very well.

I thought this was a nice technique. For sure you could use it to produce really bizarre objects. We may see some of that later.



19 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Grammar, L-System, Procedural Architecture
Tuesday, July 31, 2012
Building Unimaginable Shapes
This is a very nice TED talk by architect Michael Hansmeyer. It goes straight into procedural generation. Here is some of his work:



I like what he says about "start thinking about designing the process, not the object". Here is the video. You can find a direct link here.



3 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Procedural Architecture
Tuesday, June 5, 2012
Dude where is my castle?
I have been writing some new grammars lately. I realized that if my system was any good, it had to be able to do romantic castles like Neuschwanstein, Disneyland, etc. I don't really like this style, but there was no way to avoid it.

So here are a few screens of procedural kitsch:







Note there are no materials or texturing. This should help you see the grammar output clearly.

This is the same castle from different angles. It sits on top of a series of boxes which were produced by an earlier stage, when city blocks are created. It is not required for the boxes to be aligned, although they are in this particular instance.

The interior is properly connected by a system of doors and stairs. Even the round towers have spiral stairs. You could go in and fool around inside this structure.

Most of what you see here is parametric, which means very different castles can arise from the same grammar rules. The basic components that you find instanced many times over, like windows, doorways, ledges, can also be replaced.

If you have been following this blog for a while now you will realize that this castle is built out the same elements of the church I posted more than a year ago. As you can see it takes little effort on the modeling side. I have been very lazy about creating new basic components.

As usual there is a lot of room for improvement. I want to add little windows in the rooftops, gargoyles, better doors and windows, balconies, etc. I will probably revisit this style later, for now I will move to the next cliche, maybe a Mayan temple or a Japanese Castle. If you have one type of architecture that you think will prove unsolvable, please drop me a comment.
55 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Castle, Grammar, L-System, Neuschwanstein, Procedural Architecture
Wednesday, April 4, 2012
A room with an angle
This is a continuation of the previous post. Like the one before, it is mostly about occlusion tests and snap planes in architecture generation systems.

It is possible to create richer architectural volumes by combining several simple volumes like boxes. The resulting volumes can be quite complex, which is in part what we wanted but it also brings some complications.

In the following image you can see the result of three boxes intersecting each other:

If you concentrate on the walls going out you will see they would make an interesting building. At the same time there is a lot of nastiness in the inside. There are several triangular spaces and at least one non-convex polygonal space. Those would not translate well into rooms. Ideally we would like to keep the outer walls and remove all the nonsense spaces. This can be done with occlusion tests and snap planes.

To show this I will use the same grammar language as in the earlier post. If you feel lost in the code there are some comments in that post that may help.

This is the code that produced these three boxes. It will be our starting point:































The module "angletest" is the one that produces the three rotated box volumes. Each box has its sides covered by wall panels.

The first half of the trick is to hide those panels that appear inside a volume. The occlusion query can do that. It would take only a few lines to add it:



































The "volume" module now occludes. Also there is a new definition for the "panel" module that should be picked when the panel is occluded. Just to visualize which ones should be hidden I chose to render them as lines on the floor. You can see the results here:


This is already better, but there are still some holes in the corners. This is because an entire panel is hidden even if only one part of it is occluded.

Snap planes can help with this. If each volume defines a set of planes, the intersecting panels will match their size to the near volume. This is how you would write it:


































Now each wall introduces a snap plane, then the panel repetition is snapped to those planes. Panels are aligned to wall intersections. They can be entirely removed and no holes will appear:



It wasn't so bad. Again, this alone won't be enough to create interesting buildings, but it is a technique you may want to remember.

Here is how it would look if you replaced the panels by windows. I also added some random rotation to the three boxes.




8 comments: Email ThisBlogThis!Share to TwitterShare to FacebookShare to Pinterest
Labels: Grammar, L-System, Procedural Architecture
Thursday, March 22, 2012
Building Rooms
There are many papers and demos about creating building volumes and their facades. There is not much about how to build their interiors. Today I want to share a simple technique I found. It allows to break a space into multiple rooms. This alone is not enough to build interesting interiors, but it can be successfully combined with halls, staircases and more dramatic spaces.

This is what we are going for:


The idea is to divide a space into rooms of random sizes. The rooms should be connected by doors. The outer walls should have windows.

As you will see, the real challenge is to make sure the doors and windows will appear in the right places. A door should not appear in the middle of a wall intersection. Windows must properly align with room sizes.

To show this technique I will use the L-System langue I have developed for Voxel Studio. I hope the code is simple enough to convey the idea behind it.

We will start by breaking the space into even rooms:

















You can see the output of the "testrooms" module here:

It is a simple, neat idea. The "room" module will divide recursively while the room size remains bigger than 5 (meters). Once the room cannot be subdivided anymore, the third definition for "room" kicks in and instances some walls.

Let's add some variety. Instead of splitting rooms in half we can choose a more interesting ratio. We can also make the subdivision random. Rooms could stop dividing even if they were bigger than 5 meters, which will lead to bigger rooms:

Not much changed since the previous version. The key addition was a third constraint in the "room" rules, selecting the rule only if (rnd < 0.8). This means 20% of the time a room, no matter how big, could stop subdividing.

The results are already more interesting. Also, multiple runs of the "testrooms" module produce different layouts. Here is one example:

This looks close to what we wanted, it should be just adding doors to the inner walls and windows to the outer ones. Well, not so fast. While the rooms are properly identified, there is something going on with the walls. If we contract each room a little bit the problem becomes apparent:


As you can see, each room has its own set of four walls. When a large room appears besides a series of smaller rooms, we get a large wall running over the smaller walls. If we had doors there, they would be hidden behind the large wall.

The simplest solution would be just to hide the large wall when we detect it has been occluded by a smaller wall. This can be done by adding an "occlusion" conditional to the walls. 

Occlusion tests work by intersecting volumes. Even the slightest intersection will result in a positive occlusion. For this reason, we will shrink our wall occluders a little bit. This way wall intersections in the corners will not result in positive occlusion. We will not worry about the little gaps this will create for now.

Here is the modified code:

There are now two rules for the walls. One for a wall that is occluded, and a second for a wall that does not intersect any other wall. To spot which walls were occluded, I chose to render them as shorter, wider walls. They appear in red below:
As you can see, we could start punching door holes in the remaining walls and they would not be hidden by a parallel wall coming from a neighbor cell. Still, finding the proper location for these holes would be tricky. The hole must not appear in the intersection of two perpendicular walls.

Instead finding a solution for this, let's look closely at the walls marked in red. These were the walls  removed by the occlusion test. You will see they never intersect each other. We could open a hole anywhere in the wall and it would never be in front of another wall. What if we reverse our logic, that is, we hide the non occluded walls and use the occluded ones for our rooms?

The following code does that, it also adds a door opening in the middle of the walls:

Here is a render of the new rules:


You will notice the outer walls are gone. This is because they are not occluded. This actually helps, since we are adding windows there anyway. 

There is another advantage. Remember the gaps we had to create in the walls so they would not occlude by the corners? Only occluding walls need to leave gaps, and since now they are not even visible, the gaps are no longer a problem.

Let's add outer walls with some window openings:

The previous code adds a call to the "outerwalls" module, which will target the sides of the scope box and repeat a series of windows at constant intervals. You can see the results here:

The red arrow points to a new problem. Since the windows repeat regularly, they often appear in the middle of walls. The solution is to use "snap" planes. Snap planes, just like occlusion rules, are recurrent techniques in procedural architecture. A snap plane is an imaginary line that influences the subdivision and repetition of modules. 

We can make the repeating windows snap to the walls. This way they will still repeat, but the repetition intervals will be constrained by the snap lines.

This is how you would specify them:

The lines that matter are highlighted in red. Starting at the bottom, the "snap" statement declares a new snap plane and names it "wall". Then the repeat statement includes the "wall" snap plane. It means it will repeat every 5 meters or a "wall" snap plane is found, whichever comes first. And last, the "module" statement to call the "outerwalls" module was replaced by "defer". Defer makes sure all the other rules have run before the module is executed. This is necessary because all the walls need to be in place before adding any windows.

As a result, the repetition now is now properly constrained within the rooms:
	
 




]]

