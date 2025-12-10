# Client–Server Application (C Socket Programming)

A lightweight TCP Client–Server system written in C, created as part of a university networking coursework module.
This project showcases fundamental concepts in network programming, socket communication, and distributed system design — presented in a polished format suitable for a professional portfolio.

## 📌 Project Overview

This project implements a basic TCP-based client–server communication model using the C programming language and POSIX socket APIs.

The system consists of:

* A Server that listens on a specified port, accepts client requests, and responds according to the coursework specification.

* A Client that connects to the server, sends messages, and displays responses received from the server.

This project demonstrates:

* Low-level network programming

* Use of POSIX sockets

* Designing simple communication protocols

* Working with buffers, I/O, and error handling in C

## 🚀 Features
Server:

* Opens a TCP socket and listens for incoming client connections

* Accepts and handles one client at a time (standard for foundational coursework)

* Processes messages using a simple request–response protocol

* Prints connection logs and messages to the terminal

* Cleanly closes sockets and handles errors safely

Client:

* Connects to the server via IP address + port number

* Sends user input to the server over a TCP stream

* Receives and prints the server’s messages

* Continues interactive communication until termination

## ▶️ How to Compile and Run

1. Compile the server
gcc server.c -o server

2. Compile the client
gcc client.c -o client

3. Run the server
./server

4. Open a new terminal and run the client
./client

Example interaction:
Client: Hello
Server: Received: Hello

## 🛠 Technologies Used

* C programming language

* POSIX sockets (Linux / UNIX)

* TCP/IP

## 👤 Author

Nikki
Client–Server Networking Coursework Project (C Socket Programming)

