#ifndef DEFINES_HPP
#define DEFINES_HPP

#pragma once

// defines
#define localhost INADDR_LOOPBACK
#define anyhost INADDR_ANY
#define nohost INADDR_NONE
#define maxclients FD_SETSIZE

#define IRC_PORT 6667
#define RECV_MAX 1024
#define TCP_NODELAY 0x01

#define _PASS 1
#define _NICK 2
#define _USER 3
#define _QUITE 4

#define error -1
#define TBD -404 // To Be Determined
#define NICK_NA "*" // nickname not applicable

#define FD_MAX 10240
#define MAX_CLIENTS 3000

typedef std::string NAME;
typedef int IP, SOCKET;
typedef unsigned char CMD;
typedef unsigned short RS_CODE;

struct tuple {
	RS_CODE		code;
	CMD			cmd;
	std::string	message;

	tuple(RS_CODE code, CMD cmd, std::string message) : code(code), cmd(cmd), message(message) {};
};

// IRC status codes
#define RPL_WELCOME 001	// RFC2812	:Welcome to the Internet Relay Network <nick>!<user>@<host>	The first message sent after client registration. The text used varies widely
#define RPL_YOURHOST 002	// RFC2812	:Your host is <servername>, running version <version>	Part of the post-registration greeting. Text varies widely
#define RPL_CREATED 003 // RFC2812	:This server was created <date>	Part of the post-registration greeting. Text varies widely
#define RPL_MYINFO 004 // RFC2812	<server_name> <version> <user_modes> <chan_modes>	Part of the post-registration greeting
#define RPL_SAVENICK 043	// IRCnet	:<info>	Sent to the client when their nickname was forced to change due to a collision

#define ERR_UNKNOWNCOMMAND 421 // RFC1459	<command> :<reason>	Returned when the given command is unknown to the server (or hidden because of lack of access rights)
#define ERR_NONICKNAMEGIVEN 431 // RFC1459	:<reason>	Returned when a nickname parameter expected for a command isn't found
#define ERR_ERRONEUSNICKNAME 432 // RFC1459	<nick> :<reason>	Returned after receiving a NICK message which contains a nickname which is considered invalid, such as it's reserved ('anonymous') or contains characters considered invalid for nicknames. This numeric is misspelt, but remains with this name for historical reasons :)
#define ERR_NICKNAMEINUSE 433 // RFC1459	<nick> :<reason>	Returned by the NICK command when the given nickname is already in use

#define ERR_NEEDMOREPARAMS 461 // RFC1459	<command> :<reason>	Returned by the server by any command which requires more parameters than the number of parameters given
#define ERR_ALREADYREGISTERED 462 // RFC1459	:<reason>	Returned by the server to any link which attempts to register again
#define ERR_PASSWDMISMATCH	464 // RFC1459	:<reason>	Returned by the PASS command to indicate the given password was required and was either not given or was incorrect

#endif /* DEFINES_HPP */