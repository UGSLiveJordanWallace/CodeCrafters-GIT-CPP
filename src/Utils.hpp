#ifndef UTILS_CLASS
#define UTILS_CLASS

#include <iostream>
#include <zlib.h>
#include <openssl/sha.h>
#include <string>

typedef struct {
	unsigned int mode;
	std::string name;
	std::string sha;
} Object;

class UTILS {
	public:
		static int init();
		static int hash_object(std::string filename, Object* obj);
		static int cat_file(std::string id);
		static int ls_tree(std::string object);
		static int write_tree();
		static int commit_tree(std::string tree, std::string* msg, std::string* link);
};

#endif
