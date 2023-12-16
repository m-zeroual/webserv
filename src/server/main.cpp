/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouakhro <nouakhro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 16:41:57 by nouakhro          #+#    #+#             */
/*   Updated: 2023/11/28 16:48:52 by nouakhro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include"ManageServers.hpp"
void	socketHaveEvent(ManageServers &Manageservers)
{
	for (size_t i = 0; i < Manageservers.WorkingSocketsSize(); i++)
	{
		if(Manageservers.WorkingRevents(i) & POLLIN)
		{
			try
			{
				Manageservers.readyToRead(i);
				std::cout << Manageservers.getRequest(i) << std::endl;
				Manageservers.setRespond("HTTP/1.1 200 OK\r\n\r\n <h1> hello </h1>", i);
			}
			catch(std::runtime_error &e){}
		}
		else if(Manageservers.WorkingRevents(i) & POLLOUT)
			Manageservers.readyToWrite(i);
	}
}
int main(int ac, char **av)
{
	static_cast<void>(ac);
	Parser parser(av[1]);
	ServerData serv(parser.getServers());
	ManageServers Manageservers(serv);

	Manageservers.runAllServers();
	Manageservers.acceptConection();
	while (true)
	{
		try{
			Manageservers.setWorkingSockets(Manageservers.isSocketsAreReady());
			socketHaveEvent(Manageservers);
		}
		catch(const ManageServers::PollException& e)
		{
			Logger::warn(std::cout, e.what(), "");
			continue;
		}
	}
}




