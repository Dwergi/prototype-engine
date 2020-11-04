#pragma once

namespace dd
{
	template <typename T>
	struct float_value
	{
		float_value() {}
		float_value( float f ) : Value( f ) {}

		T& operator=( T v )
		{
			Value = v.Value;
			return *static_cast<T*>(this);
		}

		T& operator+=( T v )
		{
			Value += v.Value;
			return *static_cast<T*>(this);
		}

		T& operator-=( T v )
		{
			Value -= v.Value;
			return *static_cast<T*>(this);
		}

		bool operator==( T t ) const
		{
			return Value == t.Value;
		}

		bool operator<( T t ) const
		{
			return Value < t.Value;
		}

		bool operator>( T t ) const
		{
			return Value > t.Value;
		}

		bool operator>=( T t ) const
		{
			return Value >= t.Value;
		}

		bool operator<=( T t ) const
		{
			return Value <= t.Value;
		}

		bool operator!=( T t ) const
		{
			return Value != t.Value;
		}

		T operator/( float f ) const
		{
			return T( Value / f );
		}

		float operator/( T t ) const
		{
			return Value / t.Value;
		}

		T operator*( float f ) const
		{
			return T( Value / f );
		}

		T operator-( T t )
		{
			return T( Value - t.Value );
		}

		T operator+( T t )
		{
			return T( Value + t.Value );
		}

		float Value { 0 };
	};

	struct metres;
	struct seconds;

	struct speed : float_value<speed>
	{
		speed() {}
		speed( float f ) : float_value<speed>( f ) {}
		speed( const speed& s ) : float_value<speed>( s.Value ) {}

		metres operator*( seconds s ) const;
	};

	struct seconds : float_value<seconds>
	{
		seconds() {}
		seconds( float f ) : float_value<seconds>( f ) {}
		seconds( const seconds& s ) : float_value<seconds>( s.Value ) {}
	};

	struct metres : float_value<metres>
	{
		metres() {}
		metres( float f ) : float_value<metres>( f ) {}
		metres( const metres& m ) : float_value<metres>( m.Value ) {}

		speed operator/( seconds s ) const;
	};

	metres operator"" _m( long double d );
	seconds operator"" _s( long double d );
	speed operator"" _ms( long double d );
}