//Developing a web server.

//This server can be executed in linux environment

//Web server running and gives the same page for all client requests.
//Web server gives different pages for different client requests.
//Handle mutliple file types ->.txt, .html, jpg, .mp3
//Error handling -> display error message if file is not found (HTTP 404 Not found error)




#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<sys/sendfile.h> /*sendfile() function is in this header file*/
#include <fcntl.h> // o_rdonly

#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    //home page of the server
    char homepage[]= "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n charset=UTF-8\r\n\r\n"
    "<html><head><title> Server home page </title>\r\n"
    "<style>body {background-color:#63F5E6} </style></head>\r\n"
    "<body><center><h1> IS2111 </br> Web server assignment</h1><br><hr>\r\n"
    "<p>Click on the file name to open a file</p>\r\n"
    "<a href=\"file1.txt\">Text file (file1.txt)</a><br>\r\n"
    "<a href=\"index.html\">HTML file (index.html)</a><br>\r\n"
    "<a href=\"network.jpg\">Image file (network.jpg)</a><br>\r\n"
    "<a href=\"audio.mp3\">Audio file (audio.mp3)</a><br>\r\n"
    "</body></html>\r\n";
    
    //error message to be printed, if a file is not found
    char *error="HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 70\n\n404 Page not found!  Please check the file name and insert it again";
    
    int fdimg; //file descripter- to display an image
    int audio; //use to serve a .mp3 file (audio file)
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    
        
        
                
        
    while(1)
    {
        printf("\nWaiting for connection..........\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        
        //fork() is used to create new processes by duplicating the current calling process(create child proceses from parent process)
        if(!fork()){         
		//communication
		char buffer[30000] = {0};
		valread = read( new_socket , buffer, 30000); //needed to display different web pages according to clients requests.
		printf("%s\n",buffer );
        		
                
        	//check index.html file exists...
		if(strstr(buffer, "GET /index.html")){
			printf("File exist!\n");
			
			char http_header[2048]="HTTP/1.1 200 OK\nContent-Type: text/html\r\n\n";		
			FILE *file;
			file=fopen("index.html", "r");
			
			char file_data[2048];//stores file data
			int chars;		
			
			 	int i=0;
				while ((chars = fgetc(file)) != EOF)//fgetc=to obtain input from a file single character at a time
									//EOF=End of File 
				{			
				    file_data[i]=chars;
				    i++;
				}
				
				//display index.html
				strcat(http_header, file_data);
				write(new_socket , http_header , strlen(http_header));
				fclose(file);
				
			 	file=NULL;
		    		close(new_socket);
			}
        
       
        
        
        
		//check file1.txt exists...
		else if(strstr(buffer, "GET /file1.txt"))//used to check whether the index file is exist.
		{ 
			printf("File exist!\n");
			
			FILE *file;
			file=fopen("file1.txt", "r");
			
			
			char file_data[1024];//stores file data
			int chars; //to read each character of the file	
			 	int i=0;
				while ((chars = fgetc(file)) != EOF)//fgetc=to obtain input from a file single character at a time
									//EOF=End of File
				{			
				    file_data[i]=chars;
				    i++;
				}
				
				//create a header first to display the text inside the text file.(headers will be used by the browser to
				// understand the task to be done-here display a txt file)
				char http_header[2048] ="HTTP/1.1 200 Ok\nContent-Type:text/plain\r\n\n";
		
				//copy the content of the index.html to the http_header array, inorder to display it in the browser.
				strcat(http_header, file_data);
				
				write(new_socket, http_header, strlen(http_header)); //dispaly content of txt file on the browser
				
				fclose(file);
				file=NULL;
			 
		    		close(new_socket);		    		
		}
	
	
	
	
	
		//network.jpg
		else if(!strncmp(buffer, "GET /network.jpg", 16)){
			char http_header[] ="HTTP/1.1 200 Ok\r\n"
			"Content-Type: image/jpeg\r\n\r\n";
		
				 write(new_socket, http_header, sizeof(http_header) - 1);
				 fdimg = open("network.jpg", O_RDONLY);				 
		        	
		                 sendfile(new_socket, fdimg, NULL, 14000);
		                
	    			  close(fdimg);
		                
		                 close(new_socket);
		}
	
		
		
	
	
		//audio.html file
		else if(strstr(buffer, "GET /audio.mp3")){
			printf("File exist!\n");
			
			char http_header[2048]="HTTP/1.1 200 OK\nContent-Type: audio/mpeg\r\n\n";	
			write(new_socket, http_header, sizeof(http_header) - 1);	
			
			audio = open("audio.mp3", O_RDONLY);
			sendfile(new_socket, audio, NULL, 90000);	//60000=buffer size to play the audio
		    	close(new_socket);				
		}
		
		
		
	
	
		//home page of the server
		else if(strstr(buffer, "GET / ")){
			printf("Home page of the server is displaying!");
			write(new_socket, homepage, sizeof(homepage)-1);
			close(new_socket);
		}
		
		
		
				
		
		//display "file not found" error message
		else 			
			write(new_socket, error, strlen(error));
			
		
		
		close(new_socket);
         	exit(0);
				
        }
        
        //close parent process
   	close(new_socket);
    }
    return 0;
}




