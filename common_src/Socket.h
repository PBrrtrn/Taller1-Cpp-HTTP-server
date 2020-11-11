#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <vector>

class Socket {
private:
	int fd;
	struct addrinfo* initializeAddrinfo(const char *host, 
																		  const char* service,
																		  int flags);
public:
	Socket();
	~Socket();
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
	Socket(Socket&& other);
	Socket& operator=(Socket&& other);
	int bind(const char *service);
	int connect(const char *host, const char *service);
	int listen(int queue_size);
	Socket accept();
	int send(const char *data, size_t data_size);
	std::vector<char> receive();
};

#endif