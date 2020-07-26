#include "ImageLoader.hpp"

namespace opfor {

ImageLoader::Image ImageLoader::Load(std::string const &path)
{
	Image image{};

	uint8_t *data = stbi_load(path.c_str(), &image.width, &image.height, &image.nchannel, 0);
	image.data = UniquePtr<uint8_t, std::function<void(uint8_t*)>>(data, [] (uint8_t *ptr) { stbi_image_free(ptr); });

	return image;
}

}
