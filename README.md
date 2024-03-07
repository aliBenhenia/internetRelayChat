# ft_irc - Internet Relay Chat Server

## Version: 8

### Table of Contents
1. [Introduction](#introduction)
2. [General Rules](#general-rules)
3. [Mandatory Part](#mandatory-part)
    1. [Requirements](#requirements)
    2. [For MacOS Only](#for-macos-only)
    3. [Test Example](#test-example)
4. [Bonus Part](#bonus-part)
5. [Submission and Peer-Evaluation](#submission-and-peer-evaluation)

---

## Introduction
Welcome to ft_irc, an IRC server implementation in C++98. This project is focused on creating a robust Internet Relay Chat (IRC) server that adheres to solid standards and protocols for real-time communication over the internet.
 
### Internet Relay Chat (IRC)
IRC is a text-based communication protocol used on the internet. It enables real-time messaging, supporting both public and private communication. Users can exchange direct messages and join group channels. IRC clients connect to IRC servers, forming a network of interconnected servers.

<img width="1574" alt="general" src="https://github.com/aliBenhenia/internetRelayChat/assets/95689141/798bfb1d-749c-4832-a9d5-5adb24c29674">

## General Rules
- Ensure the program does not crash under any circumstances, even when running out of memory.
- Provide a Makefile that compiles source files without relinking.
- Compile code with `c++` using the flags `-Wall -Wextra -Werror`.
- Code must comply with the C++98 standard.
- Avoid external libraries, and Boost libraries are forbidden.

## Mandatory Part
### Requirements
- Develop an IRC server named `ircserv`.
- The server must handle multiple clients simultaneously without hanging.
- Use non-blocking I/O operations; forking is not allowed.
- Utilize only one `poll()` (or equivalent) for handling read, write, listen, and other operations.
- Implement features similar to an official IRC server, including user authentication, setting a nickname, joining channels, sending and receiving private messages, etc.
- Support communication between clients and the server via TCP/IP (v4 or v6).

### For MacOS Only
- Use `fcntl()` for MacOS to implement non-blocking mode.
- Use `fcntl(fd, F_SETFL, O_NONBLOCK)`; any other flag is forbidden.

### Test Example
To verify your server, use tools like `nc` and test various scenarios, including partial data reception and low bandwidth.

## Bonus Part
Enhance your IRC server with additional features if the mandatory part is perfect. Bonus features include handling file transfers and implementing a bot.

## Submission and Peer-Evaluation
Submit the required files, ensuring the mandatory part is perfect for the bonus part to be evaluated. Peer evaluation will be based on the correctness and functionality of your IRC server.

Feel free to explore additional features to make your IRC server even more robust and user-friendly. Good luck with your implementation!
