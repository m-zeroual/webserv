/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDepandency.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouakhro <nouakhro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 19:51:28 by nouakhro          #+#    #+#             */
/*   Updated: 2023/12/11 18:09:59 by nouakhro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEDEPANDENCY_HPP
#define FILEDEPANDENCY_HPP
#include<iostream>
#include<poll.h>

#define PUTINFILE 1
#define DEFAULT 0
#define PUTINSTRING 2

class   FileDependency
{
private:

	std::string	request;
	std::string	boundary;
	size_t		lenght;
	size_t		contenlenght;
	int			fd;
	int 		method;
	pollfd		fdpoll;

public:

	FileDependency();
	~FileDependency();

	void			setFdPoll(const pollfd &);
	void			setFdPoll(const int &, const short &);
	// void			setRequist(const std::string &, const size_t &);
	void			setRequist(const std::string &);
	void 			setMethod(const int &);
	void			setLenght(const size_t &);
	void			setContenlenght(const size_t &);
	void			setBoundary(const std::string	&);
	void			setFd(const int &);

	const std::string&	getRequist() const;
	const std::string&	getBoundary() const;
	const pollfd&		getFdPoll() const;
	const size_t &		getLenght() const;
	const size_t &		getContenlenght() const;
	const int &			getMethod() const;
	const int &			getFd() const;
	
	int			status;
	std::string	rest;
};

#endif