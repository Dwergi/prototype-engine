#pragma once

inline const Scalar& x() const 
{ 
	return this->operator()( 0 );
}

inline const Scalar& y() const 
{ 
	return this->operator()( 1 );
}

inline const Scalar& z() const 
{ 
	return this->operator()( 2 );
}

inline const Scalar& w() const 
{ 
	return this->operator()( 3 );
}

inline const Scalar& u() const 
{ 
	return this->operator()( 0 );
}

inline const Scalar& v() const 
{ 
	return this->operator()( 1 );
}

inline Scalar& x() 
{ 
	return this->operator()( 0 );
}

inline Scalar& y() 
{ 
	return this->operator()( 1 );
}

inline Scalar& z() 
{ 
	return this->operator()( 2 );
}

inline Scalar& w() 
{ 
	return this->operator()( 3 );
}

inline Scalar& u() 
{ 
	return this->operator()( 0 );
}

inline Scalar& v() 
{ 
	return this->operator()( 1 );
}
