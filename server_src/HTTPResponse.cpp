#include "HTTPResponse.h"

HTTPResponse::HTTPResponse() { }

HTTPResponse::~HTTPResponse() { }

OKResponse::OKResponse(const std::string& body) : body(body) { }

std::string OKResponse::getResponse() {
  std::string response("HTTP/1.1 200 OK\n");
  response += this->body;
  return response;
}

std::string ForbiddenResponse::getResponse() {
  return std::string("HTTP/1.1 403 FORBIDDEN\n\n");
}

std::string NotFoundResponse::getResponse() {
  return std::string("HTTP/1.1 404 NOT FOUND\n");
}

NotAllowedResponse::NotAllowedResponse(const std::string& method_called) 
  : method_called(method_called) { }

std::string NotAllowedResponse::getResponse() {
  std::string response("HTTP/1.1 405 METHOD NOT ALLOWED\n\n");
  response += this->method_called + " es un comando desconocido";

  return response;
}
