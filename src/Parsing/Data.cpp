#include "Data.hpp"

Data::Data(const String& _key, const String& _value) : key(_key), value(_value)
{
}

void	Data::setKey(const String& Key)
{
	key = Key;
}

void	Data::setValue(const String& Value)
{
	value = Value;
}

String Data::getKey( void ) const
{
	return (key);
}

String Data::getValue( void ) const
{
	return (value);
}

void	Data::printData(const Data& data, String& str)
{
	cout << str << "Key	==>	" << data.getKey() << "	| Value	==>	" << data.getValue() << endl;
}

void	Data::printData(const Data& data, const String& str)
{
	cout << str << "Key	==>	" << data.getKey() << "	| Value	==>	" << data.getValue() << endl;
}
