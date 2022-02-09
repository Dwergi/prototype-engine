#pragma once

namespace ddc
{
	enum class Tag : uint
	{
		None = 0,
		Visible = 1 << 0,
		Focused = 1 << 1,
		Selected = 1 << 2,
		Static = 1 << 3,
		Dynamic = 1 << 4
	};
}