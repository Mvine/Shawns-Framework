#include "Texture2D.h"
#include "Logging.h"
#include <stb_image.h>
#include <GLM/gtc/integer.hpp>
#include <GLM/gtc/type_ptr.hpp>

Texture2D::Texture2D(const Texture2DDescription& desc) {
	myDescription = desc;
	
	myTextureHandle  = 0;
	__SetupTexture();
}

Texture2D::~Texture2D() {
	glDeleteTextures(1, &myTextureHandle);
}

void Texture2D::__SetupTexture() {
	if (myDescription.MipLevels == -1)
		myDescription.MipLevels = glm::max(
			glm::log2(glm::min(myDescription.Width, myDescription.Height)), 1u);
	
	glCreateTextures(GL_TEXTURE_2D, 1, &myTextureHandle);
	glTextureStorage2D(myTextureHandle,
		myDescription.EnableMip ? myDescription.MipLevels : 1,
		(GLenum)myDescription.Format, myDescription.Width, myDescription.Height);
	
	glTextureParameteri(myTextureHandle, GL_TEXTURE_WRAP_S, (GLenum)myDescription.Sampler.WrapS);
	glTextureParameteri(myTextureHandle, GL_TEXTURE_WRAP_T, (GLenum)myDescription.Sampler.WrapT);
	glTextureParameteri(myTextureHandle, GL_TEXTURE_MIN_FILTER, (GLenum)myDescription.Sampler.MinFilter);
	glTextureParameteri(myTextureHandle, GL_TEXTURE_MAG_FILTER, (GLenum)myDescription.Sampler.MagFilter);
	glTextureParameterfv(myTextureHandle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(myDescription.Sampler.BorderColor));

	if (myDescription.Sampler.AnisotropicEnabled)
		glTextureParameterf(myTextureHandle, GL_TEXTURE_MAX_ANISOTROPY, myDescription.Sampler.MaxAnisotropy);
}

void Texture2D::Bind(int slot) const {
	// Bind to the given texture slot, OpenGL 4 guarantees that we have at least 80 texture slots
	// Note that this is part of Direct State Access added in 4.5, replacing the old glActiveTexture and glBindTexture calls
	glBindTextureUnit(slot, myTextureHandle);
}

void Texture2D::UnBind(int slot) {
	// Binding zero to a texture slot will unbind the texture
	glBindTextureUnit(slot, 0);
}


void Texture2D::LoadData(void* data, size_t width, size_t height, PixelFormat format, PixelType type) {
	// TODO: Re-create texture if our data is a different size
	
	LOG_ASSERT(width == myDescription.Width, "Width of data does not match the width of this texture!");
	LOG_ASSERT(height == myDescription.Height, "Height of data does not match the width of this texture!");
	
	glTextureSubImage2D(myTextureHandle, 0, 0, 0, myDescription.Width, myDescription.Height, (GLenum)format, (GLenum)type, data);

	if (myDescription.EnableMip)
		glGenerateTextureMipmap(myTextureHandle);
}

Texture2D::Sptr Texture2D::LoadFromFile(const std::string& fileName, bool loadAlpha) {

	int width, height, numChannels;
	void* data = stbi_load(fileName.c_str(), &width, &height, &numChannels, loadAlpha ? 4 : 3);

	if (data != nullptr && width != 0 && height != 0 && numChannels != 0) {
		Texture2DDescription desc = Texture2DDescription();
		desc.Width = width;
		desc.Height = height;
		desc.Format = loadAlpha ? InternalFormat::RGBA8 : InternalFormat::RGB8;
		
		Sptr result = std::make_shared<Texture2D>(desc);
		result->LoadData(data, width, height, loadAlpha? PixelFormat::Rgba : PixelFormat::Rgb, PixelType::UByte);
		stbi_image_free(data);
		return result;
	} else {
		LOG_WARN("Failed to load image from \"{}\"", fileName);
		return nullptr;
	}
}

