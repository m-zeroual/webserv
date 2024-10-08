#include "LocationPattern.hpp"

LocationPattern::LocationPattern( void ) : GeneralPattern()
{
}

LocationPattern::LocationPattern(const GeneralPattern& model, const String& Path, const vector<LocationPattern>& _innerLocation) :
	GeneralPattern(model),
	innerLocation(_innerLocation)
{
	path = Path;
}

LocationPattern::LocationPattern(const LocationPattern& copy) : GeneralPattern(copy)
{
	*this = copy;
}

LocationPattern::~LocationPattern( void ) throw()
{
	innerLocation.clear();
}

LocationPattern& LocationPattern::operator=(const LocationPattern& target)
{
	if (this != &target)
	{
		GeneralPattern::operator=(target);
		path = target.getPath();
		innerLocation = target.innerLocation;
	}
	return (*this);
}

void	LocationPattern::setPath(const String& Path)
{
	path = Path;
	path.trim(" \t\n\r");
}

String	LocationPattern::getPath( void ) const
{
	return (path);
}

const vector<LocationPattern>&	LocationPattern::getInnerLocation( void ) const
{
	return (innerLocation);
}

vector<LocationPattern>&	LocationPattern::getInnerLocation( void )
{
	return (innerLocation);
}

void	LocationPattern::printAllLocations(const vector<LocationPattern>& locations, String& str)
{
	vector<LocationPattern>::const_iterator ibegin = locations.begin();
	vector<LocationPattern>::const_iterator iend = locations.end();
	while (ibegin < iend)
	{
		cout << str << "PATH ==> " << ibegin->getPath() << endl;
		str.append("\t");
		printGeneralPattern(*ibegin, str);
		if (ibegin->getInnerLocation().empty() == false)
		{
			str.append("\t");
			printAllLocations(ibegin->getInnerLocation(), str);
			str.erase(str.length() - 1);
		}
		str.erase(str.length() - 1);
		ibegin++;
	}
}

bool	LocationPattern::empty( void ) const
{
	if (GeneralPattern::empty() && innerLocation.empty() && path.empty())
		return (true);
	return (false);
}