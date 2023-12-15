/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageServers.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouakhro <nouakhro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 16:41:52 by nouakhro          #+#    #+#             */
/*   Updated: 2023/12/11 17:42:09 by nouakhro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MANAGESERVERS
#define MANAGESERVERS
#include"Server.hpp"
#include"FileDependency.hpp"
#include<poll.h>
#include<fstream>
#include<chrono>

#define NPOS  std::string::npos 
#define GET 0
#define POST 1
#define DELET 2

#ifndef READ_NUMBER
#define READ_NUMBER 2025
#endif
typedef struct sockaddr_in S_address;
class	ManageServers
{
private:
	ServerData	servers;
	std::vector<int> fdSockets;
public:
	ManageServers(ServerData	srvers);
	~ManageServers();
	class PollException : std::exception
	{
		std::string exception_msg;

		public :
			PollException();
			PollException(const std::string &);
			const char * what() const throw();
			~PollException() throw();
	};

	void	runAllServers(void);
	void	initSockets(std::vector<int> &);
	void	initSocketPort80(void);
	void	acceptConection(void);
	void	handler(std::vector<FileDependency>&, std::vector<FileDependency> &, size_t);
	void	socketHaveEvent(std::vector<FileDependency>	&, std::vector<FileDependency> &);
	void	readyToRead(std::vector<FileDependency>	&working, std::vector<FileDependency> &master, size_t i);

	std::vector<int>	getAllPorts() const;
	std::vector<FileDependency>		ifSocketsAreReady(std::vector<FileDependency> &);
};

#endif