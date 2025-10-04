#ifndef COMPRESSION_HEADER_H
#define COMPRESSION_HEADER_H
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <sstream>
#include <thread>
#include <fstream>
#include <filesystem>
#include <string>
// Use std::string explicitly or `using namespace std;`
void header_compress(int &clientsocket, std::string &header, std::string &path, std::string &request);

#endif

