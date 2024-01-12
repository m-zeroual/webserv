#include "ServerData.hpp"

ServerData::ServerData( void )
{
}

ServerData::ServerData(const std::vector<ServerPattern>& _data) : servers(_data)
{
}

ServerData::ServerData(const ServerData& copy)
{
	*this = copy;
}

ServerData&	ServerData::operator=(const ServerData& target)
{
	servers = target.servers;
	return (*this);
}

void	ServerData::setServerData(const std::vector<ServerPattern>& serversData)
{
	servers = serversData;
}

const std::vector<ServerPattern>&	ServerData::getAllServers() const
{
	return (servers);
}

void	ServerData::displayServers( void )
{
	std::vector<ServerPattern>::iterator	iterBegin = servers.begin();
	std::vector<ServerPattern>::iterator	iterEnd = servers.end();
	int i = 0;
	while (iterBegin < iterEnd)
	{
		std::cout << "\n===================== Server " << ++i << " Info =====================\n\n";
		ServerPattern::printServerPatternInfo(*iterBegin);
		iterBegin++;
	}
}

std::vector<ServerPattern>	ServerData::getServersByServerName(const std::vector<ServerPattern>& servers, const String& serverName)
{
	std::vector<ServerPattern>	serv;
	std::vector<ServerPattern>::const_iterator iterBegin = servers.begin();
	std::vector<ServerPattern>::const_iterator iterEnd = servers.end();
	while (iterBegin < iterEnd)
	{
		std::vector<Data> value = iterBegin->getData("server_name");
		for (int i = 0; i < (int)value.size(); i++)
		{
			String str = value.at((std::vector<Data>::size_type)i).getValue();
			std::vector<String> values = str.split();
			if (find(values.begin(), values.end(), serverName) != values.end())
				serv.push_back(*iterBegin);
		}
		iterBegin++;
	}
	if (serv.empty())
		return (servers);
	return (serv);
}

std::vector<ServerPattern>	ServerData::getServersByIpAndPort(const std::vector<ServerPattern>& servers, String strHost)
{
	std::vector<ServerPattern>	serv;
	std::vector<ServerPattern>::const_iterator iterBegin = servers.begin();
	std::vector<ServerPattern>::const_iterator iterEnd = servers.end();

	while (iterBegin < iterEnd)
	{
		std::vector<Data> value = iterBegin->getData("listen");
		for (size_t i = 0; i < value.size(); i++)
			if (!value[i].getValue().compare(strHost))
				serv.push_back(*iterBegin);
		iterBegin++;
	}
	return (serv);
}

const ServerPattern&	ServerData::getDefaultServer(const std::vector<ServerPattern>& servers)
{
	std::vector<ServerPattern>	serv;
	std::vector<ServerPattern>::const_iterator iterBegin = servers.begin();
	std::vector<ServerPattern>::const_iterator iterEnd = servers.end();
	while (iterBegin < iterEnd)
	{
		std::vector<Data> value = iterBegin->getData("listen");
		for (int i = 0; i < (int)value.size(); i++)
		{
			String str = value.at((std::vector<Data>::size_type)i).getValue();
			std::vector<String> values = str.split();
			if (find(values.begin(), values.end(), "default_server") != values.end())
				serv.push_back(*iterBegin);
		}
		iterBegin++;
	}
	if (serv.size() > 1)
		throw (ServerException("Duplicate default server."));
	else if (serv.size() == 0)
		serv.push_back(servers.front());
	return (serv.front());
}

std::vector<ServerPattern>	ServerData::getServer(ServerData& servers,  const String& ClientAddress, String strHost)
{
	std::vector<ServerPattern>	srvs = servers.getAllServers();
	srvs = ServerData::getServersByIpAndPort(srvs, ClientAddress);
	if (strHost.empty())
		strHost = "\"\"";
	srvs = ServerData::getServersByServerName(srvs, strHost);
	if (srvs.size() > 1)
	{
		ServerPattern s = ServerData::getDefaultServer(srvs);
		srvs.clear();
		srvs.push_back(s);
	}
	return (srvs);
}