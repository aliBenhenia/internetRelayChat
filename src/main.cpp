#include "server.hpp"
#include <thread>
#include <chrono>

Server*	serverInstance = NULL;

void	handleSignal(int signal) {
	std::cout << "Signal " << signal << " received" << std::endl;
	if (serverInstance != NULL) {
		delete serverInstance;
	}
	// for (int i = 0; i < 2; i++) {
	// 	std::cout << "Exiting in " << 2 - i << " seconds" << std::endl;
	// 	std::this_thread::sleep_for(std::chrono::seconds(1));
	// }
	exit(EXIT_SUCCESS);
}

int main(int ac, char** av) {
	// Check arguments number to start IRC server
	if (ac != 3) {
		std::cerr << "ERROR :Invalid number of arguments"
		<< "usage :./ircserv <PORT> <PASSWORD>" << std::endl ;
		return EXIT_FAILURE;
	}

	int port = std::atoi(av[1]);

	if (port == error) {
		std::cerr << "ERROR :Invalid port number" << std::endl;
		return EXIT_FAILURE;
	}

	// Handling signals
	signal(SIGINT, handleSignal);

	// Starting IRC server
	try {
		serverInstance = new Server(anyhost, port, to_string(av[2]));
		serverInstance->start();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl ;
		delete serverInstance;
		return (EXIT_FAILURE);
	}
	delete serverInstance;
	return (EXIT_SUCCESS);
}
