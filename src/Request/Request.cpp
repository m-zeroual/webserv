#include "Request.hpp"

#define READ_SIZE 1000

string removeSlash(const string &path) {
	size_t i = 0;
	string str;
	while (path[i])
	{
		if ((path[i] == '/' && path[i + 1] != '/') || path[i] != '/')
			str += path[i];

		i++;
	}
	return (str);
}


Request::Request(void) {
}
Request::~Request(void) {
}

// ----------------------------------- parsing request -----------------------------------
void    Request::parseRequest( const string &request )
{
	size_t pos = request.find("\r\n\r\n");
	if (pos != string::npos) {
		this->Header = request.substr(0, pos + 2);
		this->Body = request.substr(pos + 4, request.length() - pos);
	}

	size_t start = request.find("\r\n");
	if (start != string::npos)
		parseRequestLine(this->Header.substr(0, start));
	this->Header = this->Header.substr(start + 2, this->Header.length() - start + 2);

	if (!this->Header.empty())
		parseHeader();
	if (!this->Body.empty())
		parseBody();
}
// void Request::unchunked() {
// 	size_t hex_value = 0;
// 	while (true)
// 	{
// 		size_t pos = Body.find("\r\n", hex_value);
// 		string hex = Body.substr(hex_value, pos - hex_value);
// 		Body.erase(hex_value, pos - hex_value + 2);
// 		size_t decimal = static_cast<size_t>(strtol(hex.c_str(), NULL, 16));
// 		hex_value += decimal + 2;
// 		if (decimal == 0)
// 			break ;
// 	}
// }
void Request::parseRequestLine( const string& requestLine )
{
	if (!requestLine.empty())
	{
		vector<string> sp = split(requestLine, " ");
		if (sp.size() == 3) {
			this->_method = sp[0];
			parseUrl(sp[1]);
			this->_version = sp[2];
		}
	}
}
void Request::parseUrl( const string& url ) {
    string path;
    size_t pos = 0;
    if ((pos = url.find("?")) != string::npos)
	{
    	this->_path = removeSlash(url.substr(0, pos));
        this->_query = url.substr(pos + 1, url.length());
	}
	else
    	this->_path = removeSlash(url.substr(0, url.length()));
}
void Request::parseHeader() {
    pair<string, string>			keyValue;
	string							line;
	size_t							newlinePos = 0;
	size_t							pos = 0;
	size_t							step = 0;

	this->_header.clear();
	while ((newlinePos = this->Header.find("\r\n", step)) != string::npos)
	{
		line = this->Header.substr(step, newlinePos - step);
		if ((pos = line.find(": ")) != string::npos)
		{
			keyValue.first = line.substr(0, pos);
			keyValue.second = line.substr(pos + 2, line.length() - pos + 2);
			this->_header[keyValue.first] = keyValue.second;
		}
		step = newlinePos + 2;
	}
	string content = this->header("Content-Type");
	if (!content.empty()) {
		vector<string> sp = split(content, "; ");
		if (sp.size() > 1) {
			_contentType = sp[0];
			size_t pos = 0;
			if (!sp[1].empty() && (pos = sp[1].find("boundary")) != string::npos) {
				_boundary = sp[1].substr(size_t(9), size_t(pos - 9));
			}
		}
	}
}




void Request::parseMultipartFormData(const string& body, const string& boundary) {
    vector<string>	parts;

    // Split body into parts based on boundary
    parts = split(body, boundary + "\r\n");
	for (size_t i = 0; i < parts.size(); i++)
	{
		string key;
		string content;
		parts[i] = parts[i].substr(0, parts[i].length() - 2);

		vector<string>	lines;
		if (!parts[i].empty())
    		lines = split(parts[i], "\r\n\r\n");

		for (size_t line = 0; line < lines.size(); line++) {
			if (line == 0) {
				vector<string> splitLine = split(lines[line], "; ");
				for (size_t j = 0; j < splitLine.size(); j++) {
					if (splitLine[j].find("=") != string::npos) {
						vector<string> keyValue = split(splitLine[j], "=");
						if (keyValue[0] == "filename") {
							vector<string> tmp = split(keyValue[1], "\r\n");
							key = tmp[0].substr(1, tmp[0].length() - 2);
						}
						else
							keyValue[1] = keyValue[1].substr(1, keyValue[1].length() - 2);
					}
				}
			}
			else {
				size_t lastBoundaryPos = lines[line].rfind(boundary);
				if (lastBoundaryPos != string::npos)
					lines[line].erase(lines[line].begin() + (int)lastBoundaryPos, lines[line].end());
				content = lines[line].substr(0, lines[line].size() - 2);
			}
		}
		_upload.push_back(make_pair(key, content));
	}	
}
void Request::parseBody() {
	parseMultipartFormData(this->Body, "--" + getBoundary());
}
const vector<pair<string, string> > Request::getUploads() const {
	return (_upload);
}

string Request::extention( const string &path) const {
	size_t pos = path.rfind(".");
	string extention;
	if (pos != string::npos)
		extention = path.substr(pos + 1, path.length() - pos);
	return (extention);
}
// ---------------------------------------------------------------------------------------------
string Request::header(const string &key) const {
	try {
		return (_header.at(key));
	}
	catch(...){
		return ("");
	}
}
const string &Request::getMethod() const {
    return (_method);
}
const string &Request::getVersion() const {
    return (_version);
}
const string &Request::getPath() const {
    return (_path);
}
const string &Request::getQuery() const {
    return (_query);
}
const string &Request::getBoundary() const {
    return (_boundary);
}
const string &Request::getContentType() const {
    return (_contentType);
}
const string &Request::getHeader() const {
    return (Header);
}
const string &Request::getBody() const {
    return (Body);
}


const map<string, string>   &Request::getHeaders( void ) const {
	return (_header);
}
