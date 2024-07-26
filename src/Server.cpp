#include "Commands.hpp"
#include "Utils.hpp"

int main(int argc, char *argv[]) {
	// Flush after every std::cout / std::cerr
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	std::cout << "\nLogs from your program will appear here!\n";

	if (argc < 2) {
		std::cerr << "No command provided.\n";
		return EXIT_FAILURE;
	}

	command_identifiers command = Commands::get(argv[1]);
	switch (command) {
		case INIT:
			if (UTILS::init() == EXIT_FAILURE) {
				return EXIT_FAILURE;
			};
			break;

		case CAT_FILE:
			if (argc < 4) {
				return EXIT_FAILURE;
			}

			if (strncmp(argv[2], "-p", 2) == 0) {
				if (UTILS::cat_file(argv[3]) == EXIT_FAILURE) {
					return EXIT_FAILURE;
				}
			}
			break;

		case HASH_OBJECT:
			if (argc < 4) {
				std::cout << "Missing file parameter" << "\n";
				return EXIT_FAILURE;
			}

			if (strncmp(argv[2], "-w", 2) == 0) {
				if (UTILS::hash_object(argv[3]) == EXIT_FAILURE) {
					return EXIT_FAILURE;
				}
			}
			std::cout << argc << "\n";
			break;

		case INVALID:
			std::cerr << "Invalid Arguments";
			break;

		default:
			std::cout << "Something Went Wrong";
			break;
	}

	return EXIT_SUCCESS;
}
