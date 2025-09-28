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
using namespace std;

void handle_client(int clientsocket)
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
   else
	send(clientsocket,response404.data(),response404.size(),0);
 	 
}
int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment this block to pass the first stage
  
   int server_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return 1;
   }
  //
  // // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // // ensures that we don't run into 'Address already in use' errors
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
  
   while(1)
   {	
	   int clientsocket=accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);


	   cout<<"Client Connected\n"<<endl;	
	   thread t(handle_client,clientsocket);
           t.detach();
   }
   close(server_fd);

  return 0;
}
