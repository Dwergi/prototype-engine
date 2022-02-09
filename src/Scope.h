//
// Scope.h - A scope guard to prevent re-entrancy.
// Copyright (C) Sebastian Nordgren 
// November 11th 2021
//

#pragma once

struct Inside
{
	Inside(bool& flag) : m_flag(flag)
	{
		if (m_flag)
		{
			return; // already entered
		}

		m_flag = true;
		m_entered = true;
	}

	~Inside()
	{
		if (m_entered)
		{
			m_flag = false;
		}
	}

	operator bool() const
	{
		return m_entered;
	}

private:
	bool& m_flag;
	bool m_entered { false };
};

struct Scope
{
	Inside Enter()
	{
		return Inside(m_inside);
	}

private:
	bool m_inside { false };
};