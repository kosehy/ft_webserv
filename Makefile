# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jjosephi <jjosephi@student.42.us.org>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/06/01 14:58:30 by jjosephi          #+#    #+#              #
#    Updated: 2020/06/01 14:58:30 by jjosephi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = serv


all: $(NAME)

$(NAME) :
	clang++ -std=c++11 http_serv.cpp init.cpp Data.cpp -o serv

re :
	rm -f serv
	clang++ -std=c++11 http_serv.cpp init.cpp slct_loop.cpp Data.cpp -o serv