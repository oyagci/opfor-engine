#pragma once

#include "lazy.hpp"
#include <exception>

class TextureAutoBind
{
private:
	GLuint _unit;
	GLenum _type;
	GLuint _id;
	bool _bIsBound;

public:
	TextureAutoBind()
	{
		_unit = 0;
		_type = 0;
		_id = 0;
		_bIsBound = false;
	}

	TextureAutoBind(unsigned int unit, GLenum type, unsigned int id)
	{
		_unit = unit;
		_type = type;
		_id = id;
		_bIsBound = false;

		Bind();
	}

	~TextureAutoBind()
	{
		if (_bIsBound) {
			Unbind();
		}
	}

	TextureAutoBind(TextureAutoBind &&other)
	{
		_unit = other._unit;
		_type = other._type;
		_id = other._id;
		_bIsBound = other._bIsBound;

		other._unit = 0;
		other._type = 0;
		other._id = 0;
		other._bIsBound = false;
	}

	TextureAutoBind &operator=(TextureAutoBind &&other)
	{
		if (this != &other) {

			if (_bIsBound) {
				Unbind();
			}

			_id = other._id;
			_type = other._type;
			_unit = other._unit;
			_bIsBound = other._bIsBound;

			other._unit = 0;
			other._type = 0;
			other._id = 0;

			if (!_bIsBound) {
				Bind();
			}
		}
		return *this;
	}

	void Unbind()
	{
		if (_bIsBound) {
			_bIsBound = false;
			glActiveTexture(_unit);
			glBindTexture(_type, 0);
		}
		else throw std::runtime_error("Tried to unbind an unbound texture");
	}

	void Bind()
	{
		if (!_bIsBound) {
			_bIsBound = true;
			glActiveTexture(_unit);
			glBindTexture(_type, _id);
		}
		else throw std::runtime_error("Tried to bind a bound texture");
	}

	bool IsBound() const
	{
		return _bIsBound;
	}
};
