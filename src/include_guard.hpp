#ifndef GUARD_HPP
#define GUARD_HPP

// C++98 Standard libraries and socket related libraries
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <vector>

#include <stdexcept>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>

// Utils libraries that are custom to this project
#include <to_string.hpp>
#include <colors.hpp>
#include <split.hpp>
#include <defines.hpp>
#include <receive.hpp>

// IRC server related libraries
#include <channel.hpp>
#include <client.hpp>
// #include <commands.hpp>

#endif