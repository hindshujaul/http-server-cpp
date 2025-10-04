#include "common_header.h"
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
        
	string compression_scheme="";
	istringstream stream(request);
	string line;
	while(getline(stream,line))
	{
		if(line.find("Accept-Encoding:")!=string::npos)
		{
			//accept encoding k baad substring bana lete hain
			int pos=line.find(":")+1;
			string compression_scheme_str=line.substr(pos);
			if(compression_scheme_str.find("gzip")!=string::npos)
			{
				compression_scheme="gzip";
			}
			else if(compression_scheme_str.find("invalid")!=string::npos)
			{	
				compression_scheme="invalid_encoding";
				cout<<"INVALID "<<compression_scheme<<endl;		
			}
			else if(compression_scheme_str.find("encoding-1")!=string::npos)
			{
				compression_scheme="encoding";
				cout<<"ENCODING "<<compression_scheme<<endl;		
			}
				
		}
	}
	if(compression_scheme.compare("")==0)
	{
		string response="HTTP/1.1 404 Not Found\r\n\r\n";
		send(clientsocket,response.data(),response.size(),0);
	}
	else if(compression_scheme.compare("gzip")==0)
	{
		cout<<compression_scheme<<endl;
		string response="HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Encoding: "+compression_scheme+"\r\n"
				"Content-Length: "+to_string(filename.length())
				+"\r\n\r\n";
		cout<<"Inside gzip"<<endl;
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
	
		cout<<"Nahi mila gzip"<<endl;	
		send(clientsocket,response.data(),response.size(),0);

		
		send(clientsocket,filename.data(),filename.size(),0);
	}	
}

