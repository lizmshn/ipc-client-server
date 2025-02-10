#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define PORT 12345
#define MAX_MEM_SIZE 1024
#define CMD_SIZE 256
#define DATA_SIZE 1024
typedef struct {
 int key;
 char data[MAX_MEM_SIZE];
} MemoryBlock;
#define MAX_BLOCKS 100
MemoryBlock memory[MAX_BLOCKS];
int memory_count = 0;
pthread_mutex_t memory_mutex = PTHREAD_MUTEX_INITIALIZER;
// Найти блок по ключу
MemoryBlock* find_block(int key) {
 for (int i = 0; i < memory_count; i++) {
 if (memory[i].key == key) {
 return &memory[i];
 }
 }
 return NULL;
}
// Обработчик клиента
void *client_handler(void [id209357492|*arg]) {
 int client_sock = *(int *)arg;
 free(arg);
 char buffer[CMD_SIZE];
 while (1) {
 int bytes = recv(client_sock, buffer, sizeof(buffer), 0);
 if (bytes <= 0) {
 break;
 }
 buffer[bytes] = '\0';
 char command[CMD_SIZE];
 int key;
 char data[DATA_SIZE];
 sscanf(buffer, "%s %d %[^\n]", command, &key, data);
 pthread_mutex_lock(&memory_mutex);
 if (strcmp(command, "shmget") == 0) {
 if (!find_block(key) && memory_count < MAX_BLOCKS) {
 memory[memory_count].key = key;
 memory[memory_count].data[0] = '\0';
 memory_count++;
 send(client_sock, "OK\n", 3, 0);
 } else {
 send(client_sock, "ERROR\n", 6, 0);
 }
 } else if (strcmp(command, "shmat") == 0) {
 MemoryBlock [id4294825|*block] = find_block(key);
 if (block) {
 send(client_sock, block->data, strlen(block->data), 0);
 } else {
 send(client_sock, "ERROR\n", 6, 0);
 }
 } else if (strcmp(command, "shmdt") == 0) {
 MemoryBlock [id4294825|*block] = find_block(key);
 if (block) {
 strncpy(block->data, data, MAX_MEM_SIZE);
 send(client_sock, "OK\n", 3, 0);
 } else {
 send(client_sock, "ERROR\n", 6, 0);
 }
 } else {
 send(client_sock, "UNKNOWN\n", 8, 0);
 }
 pthread_mutex_unlock(&memory_mutex);
 }
 close(client_sock);
 return NULL;
}
int main() {
 int server_sock = socket(AF_INET, SOCK_STREAM, 0);
 if (server_sock < 0) {
 perror("socket");
 exit(EXIT_FAILURE);
 }
 struct sockaddr_in server_addr = {0};
 server_addr.sin_family = AF_INET;
 server_addr.sin_addr.s_addr = INADDR_ANY;
 server_addr.sin_port = htons(PORT);
 if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
 perror("bind");
 exit(EXIT_FAILURE);
 }
 if (listen(server_sock, 5) < 0) {
 perror("listen");
 exit(EXIT_FAILURE);
 }
 printf("Server is running on port %d...\n", PORT);
 while (1) {
 struct sockaddr_in client_addr;
 socklen_t client_len = sizeof(client_addr);
 int *client_sock = malloc(sizeof(int));
 *client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
 if (*client_sock < 0) {
 perror("accept");
 continue;
 }
 pthread_t thread;
 pthread_create(&thread, NULL, client_handler, client_sock);
 pthread_detach(thread);
 }
 close(server_sock);
 return 0;
}
