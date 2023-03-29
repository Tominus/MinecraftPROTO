#pragma once

enum class ESide
{
	None = 0,
	Down = 1,
	Up = 2,
	Left = 4,
	Right = 8,
	Back = 16,
	Front = 32,
};

inline ESide operator | (ESide _left, ESide _right)
{
	return static_cast<ESide>(static_cast<int>(_left) | static_cast<int>(_right));
}

inline ESide& operator |=(ESide& _this, ESide _right)
{
	_this = _this | _right;
	return _this;
}

inline bool operator & (ESide _left, ESide _right)
{
	return static_cast<int>(_left) & static_cast<int>(_right);
}