#include "gemini.hpp"

int	main() {
	try {
		Bot	gemini;

		gemini.serveClient();
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl ;
	}
	return EXIT_SUCCESS;
}
