Engine = require("Engine")

function onStart(deltaTime)
	io.write("Start -- Custom Script\n")
end

offset = 0.0

function onUpdate(deltaTime)
	offset = offset + 0.01
	xpos = math.sin(offset) * 10.0
	ypos = math.cos(offset) * 10.0

	Engine.setPosition({ x = xpos, y = 10.0, z = ypos })
end
