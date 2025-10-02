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
using namespace std;
bool writeToFile(string &client_str,string &filename) 
{
	ofstream file(filename);
	if(!file.is_open())
	{
		cout<<"File nhi khula kutch toh locha hai"<<endl;
		return false;	
	}

	cout<<"CLIENT STR:"<<client_str<<endl;	
	file<<client_str;

	file.close();
	
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
	
   /*Extracting URL Path*/
   char buffer[1024];
   int received=recv(clientsocket,buffer,sizeof(buffer)-1,0);
   string request;

   if(received>0)
   {
	buffer[received]='\0';
	request=string(buffer);
   }	
     
 
   /* For Response 200 */	
   istringstream iss (request);
   string method,path,version;
   iss>>method>>path>>version; /* this has data from stream*/
	
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
	
	string response200="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
                           +to_string(desired_string.length())
			   +"\r\n\r\n"
			   +desired_string
		           +"\r\n";
        send(clientsocket,response200.data(),response200.size(),0);
   }
   else if(path=="/")
   {
	string response200="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
	send(clientsocket,response200.data(),response200.size(),0);
   }	
   else if(path=="/user-agent")
   {
	string response200="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
			    +to_string(agent_data.length())
			    +"\r\n\r\n"
			    +agent_data
			    +"\r\n";
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
				  +file_contents
				  +"\r\n";
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
		//Write to file
		if(writeToFile(client_str,filename)==0)
		{
			cout<<"File write done"<<endl;
			string response201="HTTP/1.1 201 Created\r\n\r\n";
			send(clientsocket,response201.data(),response201.size(),0);
		}
		else
		{
			cout<<"Falied to write to file"<<endl;
		}	
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
