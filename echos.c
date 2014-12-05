r
r("socket error");
		return 1;
	}   
	memset((void *)&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNUM);
	
	if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind error");
		return 1;
	}   
	if(listen(listen_fd, 5) == -1)
	{
		perror("listen error");
		return 1;
	}   
	
	FD_ZERO(&readfds);
	FD_SET(listen_fd, &readfds);

	maxfd = listen_fd;
	while(1)
	{
		allfds = readfds;
		printf("Select Wait %d\n", maxfd);
		fd_num = select(maxfd + 1 , &allfds, (fd_set *)0,
					  (fd_set *)0, NULL);

		if (FD_ISSET(listen_fd, &allfds))
		{
			addrlen = sizeof(client_addr);
			client_fd = accept(listen_fd,
					(struct sockaddr *)&client_addr, &addrlen);

			FD_SET(client_fd,&readfds);

			if (client_fd > maxfd)
				maxfd = client_fd;
			printf("Accept OK\n");
			continue;
		}

		for (i = 0; i <= maxfd; i++)
		{
			sockfd = i;
			if (FD_ISSET(sockfd, &allfds))
			{
				if( (readn = read(sockfd, buf, MAXLINE-1)) == 0)
				{
					printf("close\n");
					close(sockfd);
					FD_CLR(sockfd, &readfds);
				}
				else
				{
					buf[readn] = '\0';
					write(sockfd, buf, strlen(buf));
				}
				if (--fd_num <= 0)
					break;
			}
		}
	}
}
