#include "Parser.hpp"

/**
 * @brief	Parser Default Constructor.
*/
Parser::Parser( void )
{
}

/**
 * @brief	Parser Parametraise Constructor.
*/
Parser::Parser(const String& _fileName) : fileName(_fileName)
{
	/** @attention
	 * 		Read ^ top of the file for more detials about parsing part.
	 */
	getFileContent(); // Reading Content From File and put it in File Content String.
	checkSyntax(); // Checking syntax error and throwing an Exception when error exists.
	splitContentIntoServers();	// Split File Content into vector of vector of String.
	getFinalResualt();	// convert vector of vector String into vector of ServerPattern.
	checkServerKeys(); // check is there is unknow Keys.
	checkingInfos();
	includeMimeTypes();
}

/**
 * @brief	Parser Copy Constructor.
*/
Parser::Parser(const Parser& copy)
{
	*this = copy;
}

/**
 * @brief	Parser Desctructor.
*/
Parser::~Parser( void ) throw()
{
	// clear servers vector.
	servers.clear();
}

/**
 * @brief	Parser assignment operator.
 * @param	target the target you wanna to assign to current object.
 * @return	refrance to current object.
 * @exception	maybe throwing a bad_alloc exception.
*/
Parser& Parser::operator=(const Parser& target)
{
	if (this != &target)
	{
		fileName = target.fileName;
		servers = target.servers;
		fileContent = target.fileContent;
		serversContents = target.serversContents;
	}
	return (*this);
}

/**
 * @brief	Reading File content char by char.
 * @return	String Content
*/
String	Parser::readFile()
{
	String content;
	char	buffer[2];
	bool	isComment = false;

	int fd = open(fileName.c_str(), O_RDONLY);
	// if open faild.
	if (fd < 0)
		throw (ParsingException("Failed to open File."));
	while (1)
	{
		// set buffer to zeroes.
		memset(buffer, 0, 2);
		ssize_t reader = read(fd, buffer, 1);
		if (reader != 1)
			break ;
		if (isComment == true && buffer[0] == '\n')
		{
			// end of comment.
			isComment = false;
			content.append(buffer);
		}
		else if (buffer[0] == '#')
		{
			// begin of comment
			isComment = true;
			content.append(buffer);
		}
		else if (isComment == false && (buffer[0] == '}' || buffer[0] == '{'))
		{
			// "location / {" == become ==> "location / \n{\n".
			content.append("\n");
			content.append(buffer);
			content.append("\n");
		}
		else if (isComment == false && (buffer[0] == ';'))
			content.append(";\n"); // go back to new line after a semicolon.
		else
			content.append(buffer); // append buffer.
	}
	close(fd); // close file descriptor.
	return (content);
}

/**
 * @brief	Read file content unsing ifstream and store it in vector container (fileContent).
 * @param	(none)
 * @return	(none)
 * @exception	throw ParsingException when failed to open file.
*/
void	Parser::getFileContent( void )
{
	String	tmp;
	size_t	pos;
	String	str = readFile();
	if (str.empty() == true)
		throw (ParsingException("Empty File."));
	istringstream outfile(str);
	// read data from file and store it in vector of String.
	while (!outfile.eof())
	{
		getline(outfile, tmp, '\n'); // read line by line.
		tmp.trim(" \t");
		if (tmp.size() == 0 || *tmp.begin() == '#')
			continue ;
		// this part for remove comment inside strings
		{
			pos = tmp.find('#');
			if (pos != String::npos)
				tmp.erase(pos);
		}
		// pushing data into vector
		fileContent.push_back(tmp);
	}
	if (fileContent.empty() == true)
		throw (ParsingException("Invalid Content."));
}

/**
 * @brief	checking syntax errors like unclosed brackets or semicolon.
 * @param	(none)
 * @return 	(none)
 * @exception	throw ParsingException if there is a syntax error.
*/
void	Parser::checkSyntax( void )
{
	int	openBrackets = 0;
	vector<String> server;
	bool	insideServer = false;
	if (fileContent.empty() == true)
		throw (ParsingException("Empty file."));
	vector<String>::iterator iterBegin = fileContent.begin();
	const vector<String>::iterator iterEnd = fileContent.end();

	while (iterBegin < iterEnd)
	{
		// Counting Brackets.
		openBrackets += iterBegin->countRepeating('{');
		openBrackets -= iterBegin->countRepeating('}');

		// Skip Server Line.
		if (!iterBegin->compare("server"))
		{
			insideServer = !insideServer;
			iterBegin++;
			// check if server followed by "{"
			if (iterBegin < iterEnd)
				if (*iterBegin != "{")
					throw (ParsingException("Syntax error."));
			continue ;
		}
		if (insideServer == true)
		{
			// Skip Location Line.
			if (iterBegin->contains("location ") == true || iterBegin->countRepeating('{') || iterBegin->countRepeating('}'))
			{
				// check if location followed by a "{"
				if (iterBegin->contains("location ") == true && (iterBegin + 1) < iterEnd)
					if (*(iterBegin + 1) != "{")
						throw (ParsingException("Syntax error."));
				iterBegin++;
				continue ;
			}
			// Check semicolon in the end of line.
			if (*(iterBegin->end() - 1) != ';')
			{
				String message;
				message.append("[emerg] invalid parameter \"").append(*iterBegin).append("\" in ").append(fileName);
				message.append(" test failed.");
				throw (ParsingException(message));
			}
		}
		iterBegin++;
	}
	// Check Brackets is different a zero.
	if (openBrackets)
	{
		String message;
		message.append("webserv: [emerg] unexpected end of file, expecting \"");
		message.append((openBrackets < 0) ? "{" : "}").append("\" in ").append(fileName);
		message.append("\nwebserv: configuration file ").append(fileName).append(" test failed.");
		throw (ParsingException(message));
	}
}


/**
 * @brief	Extract Server Configuration one by one from vector of File Content vector.
 * @param	iterBegin	The begin of File Content vector.
 * @param	iterEnd		The End of File Content vector.
 * @return	new vector that's contains one Server Configuration.
*/
vector<String>	Parser::getServerConfig(vector<String>::iterator& iterBegin, const vector<String>::iterator& iterEnd)
{
	int	openBrackets = 0; // Numebr of Brackets.
	vector<String> server; // Return Value;
	bool	insideServer = false;
	bool	isServer = false;

	while (iterBegin < iterEnd)
	{
		isServer = false;
		if (!iterBegin->compare("server"))
		{
			insideServer = true;
			isServer = true;
			iterBegin++;
		}
		if (isServer && iterBegin->compare("{"))
			throw (ParsingException("Syntax Error."));
		openBrackets += iterBegin->countRepeating('{');
		if (iterBegin->countRepeating('{'))
			iterBegin++;
		openBrackets -= iterBegin->countRepeating('}');
		if (insideServer == true)
		{
			if (server.empty())
				server.push_back("servercheck valid");
			if (!openBrackets)
				break ;
			String s = iterBegin->trim(" \t");
			server.push_back(s);
			iterBegin++;
			continue;
		}
		if (!openBrackets)
			break ;
		iterBegin++;
	}
	return server;
}

/**
 * @brief	Split	File Content into vector of Servers.
 * 			for more detials, go top.
*/
void	Parser::splitContentIntoServers( void )
{
	if (fileContent.empty())
		throw (ParsingException("Empty file."));
	vector<String>::iterator begin = fileContent.begin();
	const vector<String>::iterator end = fileContent.end();
	while (begin < end)
	{
		vector<String> srv = getServerConfig(begin, end);
		if (srv.empty() == false)
			serversContents.push_back(srv);
		begin++;
	}
	if (serversContents.empty() == true)
		throw (ParsingException("No Server to run."));
	// clear file content vector.
	fileContent.clear();
}

/**
 * @brief	Split String into part Key and Value.
 * @param	line	String to split.
 * @return 	Data class that contains key and value.
 * @exception	no-throw exception.
*/
Data	Parser::extractDataFromString(String& line)
{
	vector<String> vec = line.split();
	String	key = vec.at(0).trim(" \t;");
	String	value("");
	for (vector<String>::size_type i = 1; i < vec.size(); i++)
		value.append(" ").append(vec.at(i).trim(" \t;"));
	return (Data(key, value.trim(" \t")));
}

/**
 * @brief	Extract Location part from config file using recuresion.
 * @param	begin	The start of iterator.
 * @param	end		The end of iterator.
 * @return	new Location that extact from the config.
 * @exception	no-throw exception.
*/
LocationPattern	Parser::getLocations(vector<String>::iterator& begin, const vector<String>::iterator& end, String	path)
{
	vector<LocationPattern> newLocation; // Return Value.
	LocationPattern model; // For Data vector.
	while (begin < end)
	{
		// If *begin equal } : the end of Location, skip this line and break.
		if (!begin->compare("}"))
		{
			begin++;
			break ;
		}
		// If *begin location } : new Location, skip this line and go in to recursion, else add data to Data vector (AGeneralPattern).
		if (!begin->compare(0, 9, "location "))
		{
			String _path = extractDataFromString(*begin).getValue();
			// maybe push_back throw an exception !!
			newLocation.push_back(getLocations(++begin, end, _path.trim(" \t")));
		}
		else
			model.addData(extractDataFromString(*(begin++)));
	}
	return (LocationPattern(model, path, newLocation));
}

/**
 * @brief	Separate between Location and Data, and store it in ServerPattern.
 * @param	content The Server Part.
*/
void	Parser::parsingFile(vector<String> content)
{
	ServerPattern server;
	vector<String>::iterator iBegin = content.begin();
	vector<String>::iterator iEnd = content.end();
	while (iBegin < iEnd)
	{
		if (iBegin->compare(0, 9, "location ")) // Data Part.
		{
			server.addData(extractDataFromString(*iBegin));
			iBegin++;
		}
		else if (!iBegin->compare(0, 9, "location "))
		{
			String _path = extractDataFromString(*iBegin).getValue();
			server.addLocation(getLocations(++iBegin, iEnd, _path.trim(" \t")));
		}
	}
	servers.push_back(server);
}

/**
 * @brief return the final result of parsing file into server struct.
*/
void	Parser::getFinalResualt( void )
{
	vector<vector<String> >::iterator begin = serversContents.begin();
	vector<vector<String> >::iterator end = serversContents.end();
	while (begin < end)
	{
		parsingFile(*begin);
		begin++;
	}
	serversContents.clear();
}


/**
 * @brief	Prant Server Model Infos.
 * @param	server	Server to print
*/
void	Parser::printServerPattern(const ServerPattern& server)
{
	vector<LocationPattern>::const_iterator b = server.getLocations().begin();
	vector<LocationPattern>::const_iterator e = server.getLocations().end();
	while (b < e)
	{
		printLocations(*b);
		b++;
	}
}

/**
 * @brief	printing all Locations stored in Parser class.
 * @param	locs	const Location class.
 * @return	(none).
 * @exception	no-throw exception.
*/
void	Parser::printLocations(const LocationPattern& locs)
{
	static String s;
	if (locs.getAllData().empty() == true)
		return ;
	// printing Path of Location.
	cout << s << "Path : " << locs.getPath() << endl;

	// printing All data from AGeneralPattern (Key, Value).
	vector<Data> vec = locs.getAllData();
	for (vector<LocationPattern>::size_type i = 0; i < vec.size(); i++)
		Data::printData(vec.at(i), s);

	// printing All Location from AGeneralPattern (Key, Value).
	vector<LocationPattern> innerLoc = locs.getInnerLocation();
	for (vector<LocationPattern>::size_type i = 0; i < innerLoc.size(); i++)
	{
		s.append("\t");
		printLocations(innerLoc.at(i));
		s.erase(s.length() - 1);
	}
}

/**
 * @brief	Check All Location Keys is valid or not.
*/
void    Parser::checkLocationKeys(const vector<LocationPattern>& loca, const vector<String>& keys)
{
    for (size_t i = 0; i < loca.size(); i++)
    {
		vector<Data> _data = loca.at((unsigned int)i).getAllData();
		for (unsigned int j = 0; j < (unsigned int)_data.size(); j++)
			if (find(keys.begin(), keys.end(), _data.at(j).getKey()) == keys.end())
			{
				String message("Invalid Key '");
				message.append(_data.at(j).getKey()).append("'.");
				throw (ParsingException(message));
			}
        if (loca.at(i).getInnerLocation().empty() == false)
            checkLocationKeys(loca.at(i).getInnerLocation(), keys);  
    }
}

/**
 * @brief	Check All Keys inside File config is exists.
*/
void	Parser::checkServerKeys( void )
{
	vector<String> keys = String(KEYS).split(' ');
	for (unsigned int i = 0; i < (unsigned int)servers.size(); i++)
	{
		vector<Data> _data = servers.at((unsigned int)i).getAllData();
		for (unsigned int j = 0; j < (unsigned int)_data.size(); j++)
		{
			if (find(keys.begin(), keys.end(), _data.at(j).getKey()) == keys.end())
			{
				String message("Invalid Key '");
				message.append(_data.at(j).getKey()).append("'.");
				throw (ParsingException(message));
			}
		}
		checkLocationKeys(servers.at(i).getLocations(), keys);
	}
}

void    Parser::checkingInfos( void )
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        vector<Data> data = servers.at(i).getData("root");
		if (data.empty() && servers.at(i).getData("alias").empty())
			servers.at(i).addData(Data("root", "html"));
		data = servers.at(i).getData("index");
		if (data.empty())
			servers.at(i).addData(Data("index", "index.html"));
		data = servers.at(i).getData("client_max_body_size");
		if (data.empty())
			servers.at(i).addData(Data("client_max_body_size", "1m"));
		data = servers.at(i).getData("cgi_time");
		if (data.empty())
			servers.at(i).addData(Data("cgi_time", "3"));
        data = servers.at(i).getData("listen");
		servers.at(i).clearKey("servercheck");
		if (data.empty())
		{
			servers.at(i).clearKey("listen");
			servers.at(i).addData(Data("listen", "0.0.0.0:80"));
			continue;
		}
		vector<Data> newData;
        for (size_t j = 0; j < data.size(); j++)
        {
			servers.at(i).clearKey("listen");
            String characters(".0123456789:");
			vector<String> values = data.at(j).getValue().split();
			if (values.size() < 1 || values.size() > 2 || (values.size() == 2 && values[1].compare("default_server")))
                    throw (ParsingException("check Failed : invalid arguments in listen."));
            String value(values[0]);
            for (size_t z = 0; z < value.size(); z++)
                if (find(characters.begin(), characters.end(), value.at(z)) == characters.end())
                    throw (ParsingException("check Failed : invalid character in listen."));
            vector<String> strs = value.split(':');
            if (strs.size() == 1)
            {
                if (strs.at(0).find('.') == String::npos)
                    newData.push_back(Data("listen", "0.0.0.0:" + strs[0]));
                else
                    newData.push_back(Data("listen", strs[0] + ":80"));
            }
			else
				newData.push_back(data.at(j));
        }
		if (newData.size())
			servers.at(i).addMultipleData(newData);
    }
}

map<string, string> Parser::getMimeTypes(String fileName)
{
	map<string, string> mimeType;
	fstream os(fileName.c_str());
	if (!os.is_open())
		return (mimeType);
	while (!os.eof())
	{
		String tmp;
		getline(os, tmp, '\n');
		if (tmp.empty())
			continue;
		tmp.trim(" \t\n");
		vector<String> values = tmp.split();
		if (values.size() < 2)
			continue ;
		mimeType.insert(make_pair<string, string>(string(values.front()), string(values.back())));
	}
	return (mimeType);
}

void	Parser::includeMimeTypes( void )
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		vector<Data> vls = servers[i].getData("include");
		if (!vls.empty())
			servers[i].mimeTypes = getMimeTypes(vls.front().getValue());
	}
}


/**
 * @brief	Getter of vector of vector of String.
 * @param	(none)
 * @return	constant refrance vector of vector of String.
 * @exception	no-throw exception.
*/
const vector<vector<String> >&	Parser::getServersContents( void ) const
{
	return (serversContents);
}

/**
 * @brief	Getter of vector of Server Models.
 * @param	(none)
 * @return	constant refrance vector of Server Model.
 * @exception	no-throw exception.
*/
const	vector<ServerPattern>&	Parser::getServers( void ) const
{
	return (servers);
}