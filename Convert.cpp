#include "Convert.h"
template <class T> static string Convert<T>::toString(T number)
{
	ostringstream oOStrStream;
	oOStrStream << number;
	return oOStrStream.str();
}

