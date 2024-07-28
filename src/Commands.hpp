#ifndef COMMANDS_CLASS
#define COMMANDS_CLASS

#include <map>
#include <iostream>
#include <string>

enum command_identifiers {
	INIT,
	CAT_FILE,
	HASH_OBJECT,
	LS_TREE,
	WRITE_TREE,
	COMMIT_TREE,
	INVALID
};

class Commands {
	public:
		static command_identifiers get(std::string input);

	private: 
};

#endif
