#include "gemini.hpp"

int	main(int ac, char* av[]) {
	if (ac != 3) {
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl ;
		return EXIT_FAILURE;
	}

	char* end;
	long port = strtol(av[1], &end, 10);

	if (*end != '\0' || port < 1024 || port > 65535) {
		std::cerr << "Invalid port number" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << port << std::endl ;

	try {
		Bot	gemini(static_cast<int>(port), to_string(av[2]));

		gemini.serveClient();
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl ;
	}
	return EXIT_SUCCESS;
}
