#include "common_header.h"
using namespace std;
void process_request(int clientsocket,string directory,string single_req);
bool writeToFile(string &client_str,string &fullpath) 
{
	ofstream file(fullpath,ios::binary);
	if(!file.is_open())
	{
		cout<<"File nhi khula kutch toh locha hai"<<endl;
		return false;	
	}

	cout<<"CLIENT STR:"<<client_str<<endl;	
	file<<client_str;

	file.close();
	
	if(filesystem::exists(fullpath))
	{
		cout<<"FILE PRESENT"<<endl;
	}
	else	
	{
		cout<<"FILE NOT PRESENT"<<endl;
	}
	cout<<"Kaam ho gaya file aur bandh bhi ho gaya"<<endl;
	
	return true;
	
}
string getSubstr(string &string_find,string &request)
{
	int spos=request.find(string_find);
	int start=spos+string_find.length();
        int epos=request.find(" ",start);
        int end=epos;
        string res=request.substr(start,end-start);
	return res;
}
string extractfilename(string &request,string &to_find)
{
	return getSubstr(to_find,request);
}
string parseDirectory(int argc,char*argv[])
{
	for(int i=0;i<argc;i++)
	{
		if(string(argv[i])=="--directory" && i+1<argc)
		{
			return argv[i+1];
		}
	}
    return "";
}
string find_content_type(string &request)
{
	string content_find="Content-Type:";
	int spos=request.find(content_find);
        int start=spos+content_find.length()+1;
        string end_find="\r";
	int epos=request.find(end_find,start);
        int end=epos-1;
        string res=request.substr(start,end-start);
	return res;
        
}
void handle_client(int clientsocket,string directory)
{
	
	char buffer[1024];
	string request;
	while(1)
	{ 
		int received=recv(clientsocket,buffer,sizeof(buffer)-1,0);
		
		if(received<=0)
		{
			cout<<"Client disconnected"<<endl;
			break;
		}
		request.append(buffer,received);

		while (!request.empty() && (request[0] == '\r' || request[0] == '\n')) {
   				 request.erase(0, 1);
		}
		//Headers khojo aab 

		int headerEnd=request.find("\r\n\r\n");
		if(headerEnd == string::npos)
		{
			//client ne final message ab tak bheja nhi hai 
			continue;
		}

		string single_req=request.substr(0,headerEnd+4);
		istringstream iss(single_req);
		string method,path;
		iss>>method>>path;
		if(method=="POST")
		{
			process_request(clientsocket, directory, request);
			request.clear();
		}
		else
		{
			while (!single_req.empty() && (single_req[0] == '\r' || single_req[0] == '\n')) {
   					 single_req.erase(0, 1);
			}
			cout<<"SingleREQ "<<single_req<<endl;
			process_request(clientsocket,directory,single_req);
			request.erase(0,single_req.size());
		}
	}	
//	close(clientsocket);
}
void process_request(int clientsocket,string directory,string request)
{
	
   /*Extracting URL Path*/
		/* For Response 200 */	
		istringstream iss (request);
		string method,path,version,header;
		iss>>method>>path>>version>>header; /* this has data from stream*/

		/* finding /echo/ */  
		string echo_find="/echo/";
		int apos=request.find(echo_find);
		int incoming_data=apos+echo_find.length();
		string http_find="HTTP";
		int epos=request.find(http_find);   
		string desired_string(request.begin()+incoming_data,request.begin()+epos-1); //isko substr se bhi try krna hai 

		/*For user-agent content*/
		string agent_find="User-Agent:";
		int pos=request.find(agent_find);
		int agent_start_pos=pos+agent_find.length()+1;
		int agent_end_pos=request.find('\r',agent_start_pos);
		string agent_data=request.substr(agent_start_pos,agent_end_pos-agent_start_pos);   

		string response404="HTTP/1.1 404 Not Found\r\n\r\n";
		cout<<path<<endl;

		/*function to find content type*/
		string content_type=find_content_type(request);
		int content_length=content_type.length();
		if(path.find("/echo/")==0)
		{

			string response200;
			if(request.find("Connection: close")!=string::npos)
			{
				cout<<"WE ARE IN CONNN CLOSE"<<endl;
				response200="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
				+to_string(desired_string.length())+"\r\n"
				+"Connection: close";
				+"\r\n\r\n"
				+desired_string;
				
			send(clientsocket,response200.data(),response200.size(),0);
			close(clientsocket);
			}
			else if(request.find("Connection: close")==string::npos)
			{
				response200="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
				+to_string(desired_string.length())
				+"\r\n\r\n"
				+desired_string;

			send(clientsocket,response200.data(),response200.size(),0);
			}

			if(request.find("Accept-Encoding")!=string::npos)
				header_compress(clientsocket,header,path,request);	
			while (!response200.empty() && (response200[0] == '\r' || response200[0] == '\n')) {
    					response200.erase(0, 1);
				}
			
		}
		else if(path=="/")
		{
			string response200;
			
			if(request.find("Connection: close")!=string::npos)
			{
				response200="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n"
					    "Connection: close\r\n\r\n";
				
				close(clientsocket);
			}
			else if(request.find("Connection: close")==string::npos)
			{
				response200="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
			}
			
			while (!response200.empty() && (response200[0] == '\r' || response200[0] == '\n')) {
    					response200.erase(0, 1);
				}
			send(clientsocket,response200.data(),response200.size(),0);
			
		}	
		else if(path=="/user-agent")
		{
			string response200;
			
			if(request.find("Connection: close")==string::npos)
			{
				response200="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
				+to_string(agent_data.length())
				+"\r\n\r\n"
				+agent_data;
			}
			else if(request.find("Connection: close")!=string::npos)
			{
				response200="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
				+to_string(agent_data.length())
				+"\r\n\r\n"
				+agent_data
				+"Connection: close\r\n\r\n";
				
				close(clientsocket);
			}

			
			while (!response200.empty() && (response200[0] == '\r' || response200[0] == '\n')) {
    					response200.erase(0, 1);
				}
			send(clientsocket,response200.data(),response200.size(),0);
			
		}

		else if(method=="GET"&& path.find("/files/")==0)
		{
			//extracting filename
			string to_find="/files/";
			string filename=extractfilename(request,to_find);
			string fullpath=directory+filename;
			cout<<"FILENAME"<<filename<<endl;
			cout<<"FULLPATH"<<fullpath<<endl;
			ifstream file(fullpath,ios::binary);
			if(file.good())
			{
				stringstream buff_s;
				buff_s << file.rdbuf();
				string file_contents=buff_s.str();

				string response200="HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: "
					+to_string(file_contents.length())
					+"\r\n\r\n"
					+file_contents;
				
				while (!response200.empty() && (response200[0] == '\r' || response200[0] == '\n')) {
    					response200.erase(0, 1);
				}
				send(clientsocket,response200.data(),response200.size(),0);
					
			}
			else
			{
				string response404="HTTP/1.1 404 Not Found\r\n\r\n";
				send(clientsocket,response404.data(),response404.size(),0);
			}

		}
		else if(method=="POST" && path.find("/files/")==0)
		{
			string to_find="/files/";
			string filename=extractfilename(request,to_find);
			string fullpath=directory+filename;
			cout<<"FILENAME"<<filename<<endl;
			cout<<"FULLPATH"<<fullpath<<endl;
			//extracting data to write 
			string to_find_file="\r\n\r\n";
			int spos=request.rfind(to_find_file);
			int start=spos+to_find_file.length();
			int end=request.length();
			string client_str=request.substr(start,end-start);
//			cout<<"last char"<<request[end]<<endl;
	
	//		cout<<"CLIENT STR"<<client_str<<endl;
			//Write to file
			if(writeToFile(client_str,fullpath))
			{
				cout<<"File write done"<<endl;
				string response201="HTTP/1.1 201 Created\r\n\r\n";
				
				while (!response201.empty() && (response201[0] == '\r' || response201[0] == '\n')) {
    					response201.erase(0, 1);
				}
				send(clientsocket,response201.data(),response201.size(),0);
				
			}
			else
			{
				cout<<"Falied to write to file"<<endl;
			}	
		}
		else if(method =="GET" && header.find("gzip"))
		{
			header_compress(clientsocket,header,path,request);		
		}			
		else
			send(clientsocket,response404.data(),response404.size(),0);
}
int main(int argc, char *argv[]) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  std::cout << "Logs from your program will appear here!\n";

  
   int server_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return 1;
   }
   int reuse = 1;
   if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
     std::cerr << "setsockopt failed\n";
     return 1;
   }
  
   struct sockaddr_in server_addr;
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = INADDR_ANY;
   server_addr.sin_port = htons(4221);
  
   if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
     std::cerr << "Failed to bind to port 4221\n";
     return 1;
   }
  
   int connection_backlog = 5;
   if (listen(server_fd, connection_backlog) != 0) {
     std::cerr << "listen failed\n";
     return 1;
   }
  
   struct sockaddr_in client_addr;
   int client_addr_len = sizeof(client_addr);
  
   std::cout << "Waiting for a client to connect...\n";
  
   //Directory parser 
   string directory=parseDirectory(argc,argv);
   
   if(directory.empty())
   {
	cout<<"No directory in cmd"<<endl;
   }
   else
   {
	cout<<"directory:-"<<directory<<endl;
   }
   while(1)
   {
	   	
	int clientsocket=accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);


	   cout<<"Client Connected\n"<<endl;	
	   thread t(handle_client,clientsocket,directory); //can be done like thread (handle_client,clientsocket).detach();
           t.detach();
   }
   close(server_fd);

  return 0;
}
