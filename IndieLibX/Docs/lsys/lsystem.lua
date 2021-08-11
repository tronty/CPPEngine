local gl = require "opengl"
local GL = gl

local sketch = require "opengl.sketch"

local ctx = "L"
win = Window(ctx)

local list = { "F" }

local rules = {
	
	F = { "F", "+", "F", "-", "-", "F", "+", "F", },
	
	--["-"] = { "+" },
	--["+"] = { "-", "-" },
}

local steps = 0

function apply_rules()
	while #list < 3200 do
		local newlist = {}
		for i = 1, #list do
			local symbol = list[i]		
			local rule = rules[symbol]
			if rule then
				for j = 1, #rule do
					local v = rule[j]
					table.insert(newlist, v)
				end
			else
				table.insert(newlist, symbol)
			end
		end
		list = newlist		
		steps = steps + 1
		wait(1)
	end
	print("done, with size", #list)
end


go(1, apply_rules)


function win:draw()
	
	sketch.enter_ortho(0, 0, 1, 1)
	
	gl.Translate(0, 0.3, 0)
	local scale = 1/(3^steps)
	gl.Scale(scale, scale, scale)
	
	-- turtle-interpret list:
	for i = 1, #list do
		local symbol = list[i]	
		if symbol == "F" then
			gl.Begin(GL.LINES)
				gl.Vertex(0, 0, 0)
				gl.Vertex(1, 0, 0)
			gl.End()
			gl.Translate(1, 0, 0)
		elseif symbol == "+" then
			gl.Rotate(60, 0, 0, 1)
		elseif symbol == "-" then
			gl.Rotate(-60, 0, 0, 1)
		end	
	end
	
	sketch.leave_ortho()
end

function win:key(e, key)
	if key == 27 and e == "down" then
		win.fullscreen = not win.fullscreen
	end
end





