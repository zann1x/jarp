#pragma once

union Vec2f
{
	struct
	{
		float x, y;
	};
	struct
	{
		float u, v;
	};
};

typedef union Vec2f Vec2f;

inline Vec2f math_vec2f_add(Vec2f vec1, Vec2f vec2)
{
	Vec2f result;
	result.x = vec1.x + vec2.x;
	result.y = vec1.y + vec2.y;

	return result;
}
