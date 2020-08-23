-- This script loads the level

local sponza = loadModel("./models/glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf", "Sponza")

if sponza >= 0 then
	setModelScale(sponza, 0.1, 0.1, 0.1)
end

function setupLighting()
	local lightPos = { x = -50.0, y = 10.0, z = 0.0 }
	local lightColor = { r = 1.0, g = 1.0, b = 0.8 }
	local lightIntensity = 1000.0

	local pointLight = addPointLight("My Awesome Point Light", lightPos, lightColor, lightIntensity)

	return pointLight
end

local pointLight = setupLighting()
