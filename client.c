#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define CMD_SIZE 256
#define DATA_SIZE 1024
int send_command(const char [club19215441|*command], char [id20840309|*response]) {
 int sock = socket(AF_INET, SOCK_STREAM, 0);
 if (sock < 0) {
 perror("socket");
 return -1;
 }
 struct sockaddr_in server_addr = {0};
 server_addr.sin_family = AF_INET;
 server_addr.sin_port = htons(PORT);
 if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
 perror("inet_pton");
 close(sock);
 return -1;
 }
 if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
 perror("connect");
 close(sock);
 return -1;
 }
 send(sock, command, strlen(command), 0);
 int bytes = recv(sock, response, DATA_SIZE, 0);
 if (bytes > 0) {
 response[bytes] = '\0';
 } else {
 response[0] = '\0';
 }
 close(sock);
 return 0;
}
int shmget(int key) {
 char command[CMD_SIZE];
 char response[DATA_SIZE];
 snprintf(command, sizeof(command), "shmget %d", key);
 if (send_command(command, response) == 0 && strcmp(response, "OK\n") == 0) {
 return 0;
 }
 return -1;
}
char* shmat(int key) {
 char command[CMD_SIZE];
 static char response[DATA_SIZE];
 snprintf(command, sizeof(command), "shmat %d", key);
 if (send_command(command, response) == 0) {
 return response;
 }
 return NULL;
}
int shmdt(int key, const char [club105247662|*data]) {
 char command[CMD_SIZE];
 char response[DATA_SIZE];
 snprintf(command, sizeof(command), "shmdt %d %s", key, data);
 if (send_command(command, response) == 0 && strcmp(response, "OK\n") == 0) {
 return 0;
 }
 return -1;
}
int main() {
 if (shmget(1) == 0) {
 printf("Memory segment created.\n");
 }
 if (shmdt(1, "Hello, Distributed Memory!") == 0) {
 printf("Data written to memory.\n");
 }
 char [club105247662|*data] = shmat(1);
 if (data) {
 printf("Read from memory: %s\n", data);
 }
 return 0;
}
