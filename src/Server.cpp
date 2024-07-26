#include "Commands.hpp"
#include "Utils.hpp"

int main(int argc, char *argv[]) {
	// Flush after every std::cout / std::cerr
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

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
				std::cout << "Missing file parameter" << "\n";
				return EXIT_FAILURE;
			} else {
				std::string option = argv[2];
				if (option == "-p") {
					if (UTILS::cat_file(argv[3]) == EXIT_FAILURE) {
						return EXIT_FAILURE;
					}
				}
			}

			break;

		case HASH_OBJECT:
			if (argc < 4) {
				std::cout << "Missing file parameter" << "\n";
				return EXIT_FAILURE;
			} else {
				std::string option = argv[2];
				if (option == "-w") {
					if (UTILS::hash_object(argv[3]) == EXIT_FAILURE) {
						return EXIT_FAILURE;
					}
				}
			}

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
