#ifndef COMPRESSION_HEADER_H
#define COMPRESSION_HEADER_H

#include <string>
// Use std::string explicitly or `using namespace std;`
void header_compress(int &clientsocket, std::string &header, std::string &path, std::string &request);

#endif

