--
-- Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
-- Licensed under The GNU Lesser General Public License, version 2.1:
--     http://opensource.org/licenses/LGPL-2.1
--
require("IndieLib")

[[
AppPy = {}
AppPy.__index = AppPy

function AppPy:create(balance)
   local acnt = {}             -- our new object
   setmetatable(acnt,AppPy)  -- make AppPy handle lookup
   acnt.balance = balance      -- initialize our object
   return acnt
end

function AppPy:withdraw(amount)
   self.balance = self.balance - amount
end

-- create and use an AppPy
acc = AppPy:create(1000)
acc:withdraw(100)
]]

AppPy = {}
AppPy.__index = AppPy

	function init(width, height)
		my_init=loadlib("example.so","luaopen_example") -- for Unix/Linux
		--my_init=loadlib("example.dll","luaopen_example") -- for Windows
		assert(my_init) -- name sure its not nil
		IndieLib:CIndieLib:Instance()
		IndieLib:CIndieLib:Instance():Init("a_01_Installing")
	end

	function render()
		IndieLib:CIndieLib:Instance():Input:Update ()
		IndieLib:CIndieLib:Instance():Render:ClearViewPort (60, 60, 60)
		IndieLib:CIndieLib:Instance():Render:BeginScene ()
		IndieLib:CIndieLib:Instance():Render:EndScene ()
		--IndieLib:CIndieLib:Instance():Render:ShowFpsInWindowTitle()
		return 0
	end

	function run()
		self:init(0,0)
		while True:
			if IndieLib:CIndieLib:Instance():Input:IsKeyUp(IndieLib:STX_KEY_ESCAPE)
				return
			self:render()
	end

app=AppPy()
app:run()

