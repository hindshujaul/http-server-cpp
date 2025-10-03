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
using namespace std;
void header_compress(int &clientsocket,string &header,string &path,string &request)
{
	string echo_find="/echo/";
	int spos=path.find(echo_find);
        int start=spos+echo_find.length();
	int epos=request.find(" ",start);
	string filename=path.substr(start,epos-start);
	
      /*  ifstream file(filename,ios::ate);
	int filesize=file.tellg();
	cout<<filesize<<endl;*/
        
	
	istringstream ish(header);
	string accept_encoding,compression_scheme;
	ish>>accept_encoding>>compression_scheme;

	if(compression_scheme!="")
	{
		string response="HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Encoding: "+compression_scheme+"\r\n"
				"Content-Length: "+to_string(filename.length())
				+"\r\n\r\n";
		send(clientsocket,response.data(),response.size(),0);

		//Body ko separately bhej rahe hain
		send(clientsocket,filename.data(),filename.size(),0);
	}
	else
	{	
		string response="HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: "+to_string(filename.length())
				+"\r\n\r\n";
	
		
		send(clientsocket,response.data(),response.size(),0);

		
		send(clientsocket,filename.data(),filename.size(),0);
	}	
}

