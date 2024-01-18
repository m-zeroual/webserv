#ifndef	DATA_HPP
#define DATA_HPP
#include <iostream>
#include "String.hpp"
#include "Logger.hpp"

using namespace std;
class Data
{
	String	key;
	String	value;

	public :
		Data(){}
		Data(const String& _key, const String& _value);
		void	setKey(const String& Key);
		void	setValue(const String& Value);
		String	getKey( void ) const;
		String	getValue( void ) const;
		
		static	void	printData(const Data& data, String&);
		static	void	printData(const Data& data, const String&);
};

#endif
