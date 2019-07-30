/*
*   Assignment1
*   Proxy C file
*   Name: Zicheng Huang 
*   UCID: 30009205
*   Tutorial: T02
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*Library for socket programming*/
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

/*Library for parsing strings*/
#include <string.h>

#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>

#include <netdb.h>

int main() {
	/* Initialize the sin_port */
	int portNum = 0;
	printf("Please enter the port number:\n");
	scanf("%d", &portNum);
	while((portNum < 8000) || (portNum > 9000)){
		printf("Error input port number and enter again.\n");
		scanf("%d", &portNum);
	}

    /* Initialize the multiThread*/
    int choice = 0;
    printf("Please choose whether you want multi-threaded or not (0 = off, 1 = on)\n");
    scanf("%d", &choice);
    while((choice < 0) || (choice > 1)){
        printf("Wrong choice!\n");
        scanf("%d", &choice);
    }

	/* Initialize the number of spelling errors you want*/
	int errorNum = 0;
	printf("Please enter the number of errors:\n");
	scanf("%d", &errorNum);
	while(errorNum <= 0){
		printf("Error input error numbers.\n");
		scanf("%d", &errorNum);
	}	
	////////////////////////////////

	/* Address initialization */
	struct sockaddr_in proxy_addr;
	memset(&proxy_addr, 0, sizeof(proxy_addr));
	proxy_addr.sin_family = AF_INET;
	proxy_addr.sin_port = htons(portNum);
	proxy_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create the listening socket */	
	int lstn_sock;	
	lstn_sock = socket(AF_INET, SOCK_STREAM, 0);	
	if (lstn_sock < 0) {	
		printf("Error in socket() while creating lstn_sock\n");
		exit(-1);	
	}	
	printf("Socket created successfully.\n");	
	
	/* Bind the socket to address and port */
	int status;	
	status = bind(lstn_sock, (struct sockaddr *) &proxy_addr, sizeof(struct sockaddr_in));
	if (status < 0) {
		printf("Error in bind()\n");
		exit(-1);
	}
	printf("Binding completed.\n");

	/* Connect to TCP server */
	status = listen(lstn_sock, 5);
	if (status < 0) {
		printf("Error in listen()\n");
		exit(-1);
	}
	printf("Listening for connection requests...\n");

	/* Main Loop for listening */
	while (1) {
		/* Accept a connection */
		int connected_sock;
		connected_sock = accept(lstn_sock, NULL, NULL);
		if (connected_sock < 0) {
			printf("Error in accept()\n");
			exit(-1);
		}
		printf("Connection established.\n");

		/* When user choose the multi-thread(Bonus)*/
        if(choice == 1){
            int pid = fork();
            if(pid != 0){
                continue;
            }
        }

		char client_input[32000];	
		bzero(client_input, sizeof(client_input));

		/* Receive data*/
		int count;
		while (1) {
			count = recv(connected_sock, client_input, sizeof(client_input), 0);
			if (count < 0) {
				printf("Error in recv()\n");
			} else {
				printf("Client message received!\n");
				break;
			}
		}

		/*Parse the HTTP message and get the host name of the goal web browser */
		char host[32000];
		char URL[32000];
		char client_in_cpy[32000];

        strcpy(client_in_cpy, client_input);

		char *path = strtok(client_in_cpy, "\r\n");

		printf("Found HTTP request: %s\n", path);

		if(path != NULL && sscanf(path, "GET http://%s", URL) == 1){
			printf("URL = %s\n", URL);
		}
		
		/*get host */
		for(int i = 0; i < strlen(URL); i++){
			if(URL[i] == '/'){
				strncpy(host, URL, i);
				host[i] = '\0';
				break;
			}
		}
		printf("Host has been found! %s\n", host);

		/* Address initialization */
		struct sockaddr_in server_addr;	
		struct hostent *address;
		address = gethostbyname(host);
		if(address == NULL){
			printf("Error call.\n");
			exit(-1);
		}
		memset(&server_addr, 0, sizeof(server_addr));
		bcopy((char *) address->h_addr, (char *) &server_addr.sin_addr.s_addr, address->h_length);
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(80);

		/* Create the listening socket */
		int sock;
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0) {
			printf("Error in socket() while creating lstn_sock\n");
			exit(-1);
		} else {
			printf("Socket Created!\n");
		}

		/* Connect to TCP server */
		int status;
		status = connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
		if (status < 0) {
			printf("Error in connect()\n");
			continue;
		} else {
			printf("Connected.\n");
		}

		/* Send the request to the proxy client*/
		send(sock, client_input, sizeof(client_input), 0);

		/* Use an array to store the receive message*/
		char rcv_message[32000];
		count = recv(sock, rcv_message, sizeof(rcv_message), MSG_WAITALL);
		if (count < 0) {
			printf("Error in recv()\n");
			exit(-1);
		}

		/* Check HTTP request and the content type*/
		char *ret;
		ret = strstr(rcv_message, "200 OK");
		char *checkType;
		if(ret != NULL){
			printf("200(OK)\n");
			if((checkType = strstr(rcv_message, "text/html")) == NULL && (checkType = strstr(rcv_message, "text/plain")) == NULL){
				int counter;
				counter = send(connected_sock, rcv_message, sizeof(rcv_message), 0);  
		        if (counter < 0) {
					printf("Error in send()\n");
				}
				close(sock);
				continue;
			}else{
				
				/* Check the content type whether it is html or txt*/
				int flag = 0;
				if((checkType = strstr(rcv_message, "text/html")) != NULL){
					flag = 1;
				}
				/* Use the random algorithm to create spelling errors */
                char s[32000];      
                char final_msg[32000];
                bzero(final_msg, sizeof(final_msg));
                
                int c[32000];        
                int flagF = 0;
                int i = 0;
                int j = 0;
                int m = 0;
                int n = 0;
                int t = 0;
                int r = 0;
                while(1){
                    if(rcv_message[r] == '-' && rcv_message[r+1] == '8'){
                        r = r + 6;
                        break;
                    }else{
                        r++;
                    }
                }
				int b = r; // store the first index in <data> 
                while(1){
                    s[n] = rcv_message[r];
                    n++;
                    r++;
                    if(rcv_message[r] == NULL){
                        break;
                    }
                }
                while(1){
                    printf("%c", s[j]);
                    j++;
                    if(s[j] == NULL){
                        break;
                    }
                }
                while(1){
                    if(s[i] == '<'){
                        flagF = 0;
                        while(s[i] != '>'){
                            if(s[i] == 't' && s[i+1] == 'i'){
                                t = 1;
                            }else{
                                t = 0;
                            }
                            i++;
                        }
                    }else{
                        if(s[i] != '>' && s[i] != '<'){
                            flagF = 1;
                        }
                    }
                    
                    if(flagF == 1 && s[i] >= 'A' && s[i] <= 'Z' && t == 0){
                        j = i;
                        c[m] = j;
                        m++;
                    }
                    else if(flagF == 1 && s[i] >= 'a' && s[i] <= 'z' && t == 0){
                        j = i;
                        c[m] = j;
                        m++;
                    }
                    i++;
                    if(s[i] == '<'){
                        flagF = 0;
                        while(s[i] != '>'){
                            i++;
                        }
                        if(s[i-1] == 'e' && s[i-2] == 'l' && s[i-3] == 't'){
                            t = 1;
                        }else{
                            t = 0;
                        }

                    }else{
                        if(s[i] != '>' && s[i] != '<'){
                            flagF = 1;
                        }
                    }
                    if(i == sizeof(s)){
                        break;
                    }
                }
                int a[errorNum];		
                srand(time(0));
                for(i = 0; i < errorNum; ){
                    a[i] = rand() % m;
                    for(j = 0; j < i; j++){
                        if(a[j] == a[i]){
                            break;
                        }
                    }
                    if(j == i) i++;
                }

                int p = 0;
                int q = 0;
                while(1){
                    if((s[(c[(a[q])])]) >= 'A' && (s[(c[(a[q])])]) <= 'Z'){
                        p = rand() % 26 + 65;
                        if(p != s[(c[(a[q])])]){
							s[(c[(a[q])])] = p;
                            q++;
                        }
                    }
                    else if((s[(c[(a[q])])]) >= 'a' && (s[(c[(a[q])])]) <= 'z'){
                        p = rand() % 26 + 97;
                        if(p != s[(c[(a[q])])]){
                            s[(c[(a[q])])] = p;
                            q++;
                        }
                    }
                    if(q == errorNum){
                        break;
                    }
                }

				/* Copy the information into the final_msg to send back*/
                int u = 0;
                while(1){
                    if(u < b){
                        final_msg[u] = rcv_message[u];
                        u++;
                    } else{
                        break;
                    }
                }
   
                strcat(final_msg, s);
                printf("%s\n", final_msg);

                /* Send data*/
                int counter;
                counter = send(connected_sock, final_msg, sizeof(final_msg), 0); 
                if (counter < 0) {
                    printf("Error in send()\n");
                }
                printf("-----------------------------------------------------------------------\n");

                /* Close the socket */
                close(sock);
			}
			/*Check HTTP information if it is not 200 OK*/
		}else{
			ret = strstr(rcv_message, "206 Partial Content");
			if(ret != NULL){
				printf("206(Partial Content)\n");
			}else{
				ret = strstr(rcv_message, "301 Moved Permanently");
				if(ret != NULL){
					printf("301(Moved Permanently)\n");
				}else{
					ret = strstr(rcv_message, "301 Found");
					if(ret != NULL){
						printf("301(Found)\n");
					}else{
						ret = strstr(rcv_message, "304 Not Modified");
						if(ret != NULL){
							printf("304(Not Modified)\n");
						}else{
							ret = strstr(rcv_message, "403 Forbidden");
							if(ret != NULL){
								printf("403(Forbidden)\n");
							}else{
								ret = strstr(rcv_message, "404 Not Found");
								if(ret != NULL){
									printf("404(Not Found)\n");
								}
							}
						}
					}
				}
			}
		}
		printf("--------------\n");

		close(connected_sock);
	}
	/* Close the socket */
	close(lstn_sock);
	return 0;
}