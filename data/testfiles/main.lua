print("Hello Lua from gbin!")

function _init()
	print("On Init!")
	colour = 0;
	x = 64
	y = 32
	xs = 3
	ys = 2 
end

function _update()
 --	print("On Update!")
  colour = colour + 1
	if colour == 16 then colour = 0 end
	x = x + xs;
	y = y + ys;
	if x<10 or x>118 then xs = -xs end
	if y<10 or y>118 then ys = -ys end
end

function _draw()
	cls()
	circfill(x,y,15,colour)

	-- print("On Draw!")
end