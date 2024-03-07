#include "server.hpp"

Server*	serverInstance = NULL;

void	handleSignal(int signal) {
	std::cout << "Signal " << signal << " received" << std::endl;
	if (serverInstance != NULL) {
		delete serverInstance;
	}
	exit(EXIT_SUCCESS);
}

int main(int ac, char** av) {
	// Check arguments number to start IRC server
	if (ac != 3) {
		std::cerr << "ERROR :Invalid number of arguments"
		<< "usage :./ircserv <PORT> <PASSWORD>" << std::endl ;
		return EXIT_FAILURE;
	}

	char* end;
	long port = strtol(av[1], &end, 10);

	if (*end != '\0' || port < 1024 || port > 65535) {
		std::cerr << "Invalid port number" << std::endl;
		return EXIT_FAILURE;
	}

	// Handling signals
	signal(SIGINT, handleSignal);

	// Starting IRC server
	try {
		serverInstance = new Server(anyhost, static_cast<int>(port), to_string(av[2]));
		serverInstance->start();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl ;
		delete serverInstance;
		return (EXIT_FAILURE);
	}
	delete serverInstance;
	return (EXIT_SUCCESS);
}