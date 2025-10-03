void header_compress(int &clientsocket,string &header,string &path,string &request)
{
	string echo_find="/echo/";
	int spos=path.find(echo_find);
        int start=spos+echo_find.length();
	int epos=request.find(" ",start);
	string filename=path.substr(start,epos-start);
	
	ifstream file(filename,ios::ate);
	int filesize=file.tellg();
	cout<<filesize<<endl;
	
	istringstream ish(header);
	string accept_encoding,compression_scheme;
	ish>>accept_encoding>>compression_scheme;

	if(compression_scheme!="")
	{
		string response="HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Encoding: "+compression_scheme+"\r\n"
				"Content-Length: "+to_string(filesize)
				+"\r\n\r\n";
		send(clientsocket,response.data(),response.size(),0);
	}
	else
	{	
		string response="HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: "+to_string(filesize)
				+"\r\n\r\n";
	
		
		send(clientsocket,response.data(),response.size(),0);
	}	
}

