#include "Commands.hpp"

command_identifiers Commands::get(std::string input) {
	std::map<std::string, command_identifiers> valid_commands_map;

	valid_commands_map["init"] = INIT;
	valid_commands_map["cat-file"] = CAT_FILE;
	valid_commands_map["hash-object"] = HASH_OBJECT;
	valid_commands_map["ls-tree"] = LS_TREE;
	valid_commands_map["write-tree"] = WRITE_TREE;
	valid_commands_map["commit-tree"] = COMMIT_TREE;

	if (valid_commands_map.contains(input)) {
		return valid_commands_map[input];
	}

	return INVALID;
}
