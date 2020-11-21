#define _POSIX_C_SOURCE 201112L

#include <cstring>
#include <unistd.h>
#include <iostream>
#include <utility>
#include <sstream>

#include "Socket.h"
#include "AddrinfoWrapper.h"

#define BUFFER_SIZE 64

Socket::Socket() {
  this->fd = -1;
}

Socket::~Socket() {
  this->shutdown();
  this->close();
}

Socket::Socket(Socket&& other) {
  this->fd = std::move(other.fd);
  other.fd = -1;
}

Socket& Socket::operator=(Socket&& other) {
  this->fd = std::move(other.fd);
  other.fd = -1;
  return *this;
}

int Socket::bind(const char *service) {
  AddrinfoWrapper addrinfo_wrapper(NULL, service, AI_PASSIVE);

  struct addrinfo *addr;
  for (addr = addrinfo_wrapper.addresses; addr != NULL; addr = addr->ai_next) {
    this->fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (this->fd == -1) continue;

    if (::bind(this->fd, addr->ai_addr, addr->ai_addrlen) == 0)
      break;
    else
      throw std::runtime_error("SOCKET ERROR: Error in bind");
  }

  if (addr == NULL) 
    throw std::runtime_error("Could not find suitable address to bind to");
  return 0;
}

int Socket::connect(const char *host, const char *service) {
  AddrinfoWrapper addrinfo_wrapper(host, service, 0);

  struct addrinfo *addr;
  for (addr = addrinfo_wrapper.addresses; addr != NULL; addr = addr->ai_next) {
    this->fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (this->fd == -1) continue;

    if (::connect(this->fd, addr->ai_addr, addr->ai_addrlen) == 0)
      break;
    else
      throw std::runtime_error("SOCKET ERROR: Error in connect");
  }

  if (addr == NULL)
    throw std::runtime_error("Could not find suitable address to connect to");
  return 0;
}

int Socket::listen(int queue_size) const {
  if (::listen(this->fd, queue_size) != 0)
    throw std::runtime_error("SOCKET ERROR: Failed to start listening");
  return 0;
}

Socket Socket::accept() const {
  Socket peer;
  peer.fd = ::accept(this->fd, NULL, NULL);
  if (peer.fd == -1) 
    throw std::runtime_error("SOCKET ERROR: Failed to accept");
  return peer;
}

int Socket::send(const char *data, size_t data_size) const {
  size_t total_bytes_sent = 0;
  while (total_bytes_sent < data_size) {
    int bytes_sent = ::send(this->fd,
                            &data[total_bytes_sent],
                            data_size - total_bytes_sent,
                            MSG_NOSIGNAL);

    switch (bytes_sent) {
      case -1 : throw std::runtime_error("SOCKET ERROR: error in send");
      case 0 : return total_bytes_sent;
      default: total_bytes_sent += bytes_sent;
    }
  }
  return total_bytes_sent;
}

int Socket::receive(char *buffer, size_t n_bytes) const {
  size_t total_bytes_received = 0;
  while (total_bytes_received < n_bytes) {
    int bytes_received = ::recv(this->fd, &buffer[total_bytes_received],
                                n_bytes - total_bytes_received, 0);

    switch (bytes_received) {
      case -1 : throw std::runtime_error("SOCKET ERROR: error in receive");
      case 0 : return total_bytes_received;
      default : total_bytes_received += bytes_received;
    }
  }
  return total_bytes_received;
}

void Socket::shutdown() {
  ::shutdown(this->fd, SHUT_RDWR);
}

void Socket::shutdown_write() {
  ::shutdown(this->fd, SHUT_WR);
}

void Socket::close() {
  if (this->fd != -1) ::close(this->fd);
}
