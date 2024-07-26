#include "Utils.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <string>

int UTILS::init() {
	try {
		std::filesystem::create_directory(".git");
		std::filesystem::create_directory(".git/objects");
		std::filesystem::create_directory(".git/refs");

		std::ofstream headFile(".git/HEAD");
		if (headFile.is_open()) {
			headFile << "ref: refs/heads/main\n";
			headFile.close();
		} else {
			std::cerr << "Failed to create .git/HEAD file.\n";
			return EXIT_FAILURE;
		}

		std::cout << "Initialized git directory\n";
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int UTILS::hash_object(std::string filename) {
	std::ifstream filestream;
	filestream.open(filename);
	std::string content;
	getline(filestream, content);
	std::string raw_result = "blob " + std::to_string(content.length()) + '\0' + content;

	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1((const unsigned char *)raw_result.c_str(), raw_result.size(), hash);
	std::stringstream ss;
	for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	std::string hashOutput = ss.str();

	uLong bound = compressBound(raw_result.size());
	std::vector<char> out(bound);
    compress((Bytef*)out.data(), &bound, (const Bytef*)raw_result.c_str(), (uLong)raw_result.size());
	std::string compressedOutput = std::string(out.data(), bound);

	std::string dir = ".git/objects/" + hashOutput.substr(0, 2);
	std::filesystem::create_directory(dir);
	std::ofstream hashDumpy(dir + "/" + hashOutput.substr(2), std::ios::binary);
	if (hashDumpy.is_open()) {
		hashDumpy.write(compressedOutput.data(),compressedOutput.size());
		hashDumpy.close();
	} else {
		std::cerr << "Failed to create .git/objects " << raw_result << " file.\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int UTILS::cat_file(std::string id) {
	std::ifstream object(".git/objects/" + id.substr(0, 2) + "/" + id.substr(2), std::ios::binary);
	unsigned int size = 0;

	if (object.is_open()) {
		std::string object_content{std::istreambuf_iterator<char>(object), std::istreambuf_iterator<char>()};
		std::string decompressedContent;
		uLongf object_content_size = object_content.length();
		decompressedContent.resize(object_content_size);
		int result = uncompress((Bytef*) decompressedContent.data(), &object_content_size, (const Bytef*)object_content.data(), object_content.length());

		while (result == Z_BUF_ERROR) {
			object_content_size *= 2;
			decompressedContent.resize(object_content_size);
			result = uncompress((Bytef*) decompressedContent.data(), &object_content_size, (const Bytef*)object_content.data(), object_content.length());
		}

		decompressedContent.resize(object_content_size);

		int n = decompressedContent.find('\0');
		std::cout << decompressedContent.substr(n + 1);
		object.close();
	}
	
	return EXIT_SUCCESS;
}
