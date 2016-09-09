# Distributed-File-System
Allows for the creation of a distributed file system in C

## How to Run
1. To compile the server and client:
  - Server: "gcc -g -Wall mainserver.c -o mainserver -lpthread"
  - Client: "gcc -Wall client.c -o client -lcrypto -lssl"

2. To run the servers:
  - DFS1: "./mainserver <port num> DFS1"
  - DFS2: "./mainserver <port num> DFS2"
  - DFS3: "./mainserver <port num> DFS3"
  - DFS4: "./mainserver <port num> DFS4"

3. To run the client:
  - GET: "./client GET <filename>
  - PUT: "./client PUT <filename>
  - LIST: "./client LIST
