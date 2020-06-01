/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_serv.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjosephi <jjosephi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/21 15:40:51 by jjosephi          #+#    #+#             */
/*   Updated: 2020/05/31 16:20:12 by jjosephi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORT 80
int main(int argc, char const *argv[])
{
    int *server_fd, new_socket;
	long valread;
    struct sockaddr_in *address;
    int client[30];
    int i, check_socket, max_sd, sd;
    int max_client = 30;
    int addrlen = sizeof(address);
    char buffer[1025];
	std::list<Data> servers;
    std::string ss;

	servers = init();
    // set of socket descriptors
    fd_set read_fd;
    
    // Only this line has been changed. Everything is same.
    char hello[] = {"HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!"};
    
    // initialise all client arrays ot 0
    for (i = 0; i < max_client; ++i)
    {
        client[i] = 0;
    }
    // Creating socket file descriptor
	server_fd = malloc(sizeof(int) * servers.size);
    // if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    // {
    //     perror("In socket");
    //     exit(EXIT_FAILURE);
    // }

	for (int i = 0; i < servers.size(); i++)
	{
		address[i].sin_family = AF_INET;
		address[i].sin_addr.s_addr = INADDR_ANY;
		address[i].sin_port = htons(servers[i]);
		memset(address[i].sin_zero, '\0', sizeof address.sin_zero);
		if ((server_fd[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    	{
        	perror("In socket");
        	exit(EXIT_FAILURE);
   		}
		if (bind(server_fd[i], (struct sockaddr *)&address[i], sizeof(address[i]))<0)
		{
			perror("In bind");
			exit(EXIT_FAILURE);
		}
	}
	
	
    // if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    // {
    //     perror("In bind");
    //     exit(EXIT_FAILURE);
    // }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    // accept the incoming connection
    std::cout << "Waiting for connections ..." << std::endl;

    while(1)
    {
        // clear the socket set
        FD_ZERO(&read_fd);

        // add socket to set
        FD_SET(server_fd, &read_fd);
        max_sd = server_fd;

        // add child sockets to set
        for (i = 0; i < max_client; ++i)
        {
            // socket descriptor
            sd = client[i];

            // if socket descriptor is valid, then add it to read list
            if (sd > 0)
                FD_SET(sd, &read_fd);

            // If the descriptor number is over the max, set the max_sd as the number of descriptor
            if (sd > max_sd)
                max_sd = sd;
        }

        // wait for an check_socket on one of the sockets, timeout is NULL,
        // so wait indefinitely
        check_socket = select(max_sd + 1, &read_fd, NULL, NULL, NULL);

        if ((check_socket < 0) && (errno != EINTR))
        {
            std::cout<< "select error" << std::endl;
        }
        //If something happened on the new_socket, then it's an incoming connection
        if (FD_ISSET(server_fd, &read_fd))
        {
            if ((new_socket = accept(server_fd,
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform the user of socket number which used in send and receive commands
            std::cout << "New connection , socket fd is " << std::to_string(new_socket);
            std::cout << " , ip is : " << inet_ntoa(address.sin_addr);
            std::cout << " , port : " << std::to_string(ntohs(address.sin_port)) << std::endl;
            //send new connection message
            if( send(new_socket, hello, strlen(hello), 0) != strlen(hello) )
            {
                perror("send");
            }

            std::cout << "Welcome message sent successfully" << std::endl;

            //add the new socket into the array of sockets
            for (i = 0; i < max_client; i++)
            {
                //if position is empty
                if( client[i] == 0 )
                {
                    client[i] = new_socket;
                    std::cout << "Adding to list of sockets as " << std::to_string(i) << std::endl;
                    break;
                }
            }
        }

        //else it's some IO operation on some other socket 
        for (i = 0; i < max_client; i++)
        {
            sd = client[i];

            if (FD_ISSET( sd , &read_fd))
            {
                //Check if it was for closing , and also read the incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //if socket is disconnected, get the details and print them
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    std::cout << "Host disconnected , ip " << inet_ntoa(address.sin_addr);
                    std::cout << " , port " << std::to_string(ntohs(address.sin_port)) << std::endl;

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client[i] = 0;
                }

                //Echo back the message that came in
                else 
                {
                    //set the string terminating NULL byte on the end of the data read
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
    return 0;
}
