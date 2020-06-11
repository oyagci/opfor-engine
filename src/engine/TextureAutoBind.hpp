#pragma once

#include "lazy.hpp"
#include <exception>

class TextureAutoBind
{
private:
	GLenum _unit;
	GLenum _target;
	GLuint _texture;
	bool _bIsBound;

public:
	TextureAutoBind()
	{
		_unit = 0;
		_target = 0;
		_texture = 0;
		_bIsBound = false;
	}

	TextureAutoBind(GLenum unit, GLenum target, GLuint id)
	{
		_unit = unit;
		_target = target;
		_texture = id;
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
		_target = other._target;
		_texture = other._texture;
		_bIsBound = other._bIsBound;

		other._texture = 0;
		other._bIsBound = false;
	}

	TextureAutoBind &operator=(TextureAutoBind &&other)
	{
		if (this != &other) {

			if (_bIsBound) {
				Unbind();
			}

			_texture = other._texture;
			_target = other._target;
			_unit = other._unit;
			_bIsBound = other._bIsBound;

			other._texture = 0;

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
			glBindTexture(_target, 0);
		}
		else throw std::runtime_error("Tried to unbind an unbound texture");
	}

	void Bind()
	{
		if (_target == 0 || _unit == 0 || _texture == 0) {
			throw std::runtime_error("Tried to bind a null texture");
		}
		if (!_bIsBound) {
			_bIsBound = true;
			glActiveTexture(_unit);
			glBindTexture(_target, _texture);
		}
		else throw std::runtime_error("Tried to bind a bound texture");
	}

	bool IsBound() const
	{
		return _bIsBound;
	}
};
