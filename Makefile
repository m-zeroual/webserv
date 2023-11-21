NAME	=	webserv


SRCS	=	src/Server/main.cpp \
		src/Parser/Parser.cpp \
		src/Parser/Checker.cpp \
		src/Parser/Location.cpp \
		src/Models/GlobalModel.cpp \
		src/Models/ServerModel.cpp \
		src/Models/Data.cpp \
		src/Utils/String.cpp \
		src/Exception/ParsingException.cpp \
		src/Exception/ServerException.cpp \
		src/Server/server.cpp \
		src/Server/Request.cpp \
		src/Utils/ServerData.cpp \
		src/Utils/Poll.cpp
#		src/Utils/Select.cpp

TEMPLATES =	src/Templates/*.tpp
HEADERS	=	src/Includes/*.hpp src/Server/server.hpp src/Server/Request.hpp


CPP	=	c++
CPPFLAGS	=	-Wall -Wextra -Werror -std=c++98 -O3 -I./src/Includes# -fsanitize=address

BIN		=	bin

OBJS	=	${SRCS:src/%.cpp=${BIN}/%.o}


all	:	${NAME}

${NAME}	: ${OBJS}
	${CPP} ${CPPFLAGS} $^ -o $@
	@echo "finish !!"

${BIN}/%.o	:	src/%.cpp ${HEADERS} ${TEMPLATES}
	@mkdir -p $(dir $@)
	${CPP} ${CPPFLAGS} -c $< -o $@

clean	:
	rm -rf ${BIN}

fclean	:	clean
	rm -rf ${NAME}

re	: fclean all
