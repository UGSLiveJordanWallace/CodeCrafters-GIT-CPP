#ifndef UTILS_CLASS
#define UTILS_CLASS

#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <zlib.h>
#include <openssl/sha.h>

class UTILS {
	public:
		static int init();
		static int hash_object(std::string filename);
		static int cat_file(std::string id);
};

#endif
