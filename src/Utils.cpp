#include "Utils.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

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

int UTILS::hash_object(std::string filename, Object* obj) {
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
		obj->sha = hashOutput;
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

		std::string identifier = decompressedContent.substr(0, decompressedContent.find(32));
		if (identifier == "tree") {
			std::cout << "Use 'ls-tree <tree_hash>' to print trees";
			return EXIT_FAILURE;
		} else if (identifier == "blob") {
			std::cout << decompressedContent.substr(n + 1);
		} else if (identifier == "commit") {
			std::string curr = decompressedContent.substr(n + 1);
			n = curr.find('\n');

			while (n != std::string::npos) {
				std::string line = curr.substr(0, n);
				std::cout << line << "\n";
				curr = curr.substr(n + 1);
				n = curr.find("\n");
			}
			std::cout << curr;
			
		}

		object.close();
	}
	
	return EXIT_SUCCESS;
}

int UTILS::ls_tree(std::string object) {
	std::ifstream tree(".git/objects/" + object.substr(0, 2) + "/" + object.substr(2), std::ios::binary);
	unsigned int size = 0;

	if (tree.is_open()) {
		std::string object_content{std::istreambuf_iterator<char>(tree), std::istreambuf_iterator<char>()};
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

		std::string identifier = decompressedContent.substr(0, decompressedContent.find(32));
		if (identifier != "tree") {
			std::cout << "Use 'cat-file <obj_hash>' to print commits and objects";
			return EXIT_FAILURE;
		}

		std::string curr = decompressedContent.substr(n + 1);
		while (n < curr.length()) {
			n = curr.find('\0');
			std::string entry = curr.substr(0, n + 41);

			std::string mode = entry.substr(0,6);
			std::string filetype = "";
			std::string hash = entry.substr(n, n + 41);

			std::string tmp = entry.substr(0, n); 
			std::string filename = tmp.substr(7);

			if (mode == "040000") {
				filetype = "tree";
			} else if (mode == "100644") {
				filetype = "blob";
			}

			std::cout << mode << " " << filetype << " " << hash << "\t" << filename << "\n";
			curr = curr.substr(n + 41);
		}

		tree.close();
	}
	return EXIT_SUCCESS;
}

Object init_object() {
	Object tree_contents;
	tree_contents.mode = 0;
	tree_contents.name = "";
	tree_contents.sha = "";
	return tree_contents;
}

std::string write(std::string filePath) {
	if (filePath == "./.git") {
		return "";
	}

	std::filesystem::path curr_path = filePath;
	if (std::filesystem::is_empty(curr_path)) {
		return "";
	}

	Object tree_obj = init_object();
	tree_obj.name = curr_path.filename().string();
	tree_obj.mode = 040000;
	std::string tree_content = "";

	for (auto& item : std::filesystem::directory_iterator(filePath)) {
		if (std::filesystem::is_directory(item)) {
			Object obj_tree = init_object();
			obj_tree.name = item.path().filename().string();
			obj_tree.sha = write(item.path().string());
			if (obj_tree.sha.empty()) {
				continue;
			}
			tree_content += "040000 " + obj_tree.name + '\0' + obj_tree.sha;
		} else {
			Object obj = init_object();
			UTILS::hash_object(item.path().string(), &obj);
			obj.mode = 100644;
			obj.name = item.path().filename().string();
			tree_content += std::to_string(obj.mode) + " " + obj.name + '\0' + obj.sha;
		}
	}

	std::string tree_header = "tree " + std::to_string(tree_content.length()) + '\0' + tree_content;

	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1((const unsigned char *)tree_header.c_str(), tree_header.size(), hash);
	std::stringstream ss;
	for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	tree_obj.sha = ss.str();

	uLong bound = compressBound(tree_header.size());
	std::vector<char> out(bound);
    compress((Bytef*)out.data(), &bound, (const Bytef*)tree_header.c_str(), (uLong)tree_header.size());
	std::string compressedOutput = std::string(out.data(), bound);

	std::string dir = ".git/objects/" + tree_obj.sha.substr(0, 2);
	std::filesystem::create_directory(dir);
	std::ofstream outFile(dir + "/" + tree_obj.sha.substr(2), std::ios::binary);
	if (outFile.is_open()) {
		outFile.write(compressedOutput.data(),compressedOutput.size());
		outFile.close();
	}

	return tree_obj.sha;
}

int UTILS::write_tree() {
	std::string root_sha = write("./");
	std::cout << "Root Sha: " << root_sha << "\n";
	return EXIT_SUCCESS;
}

int UTILS::commit_tree(std::string tree, std::string* msg, std::string* link) {
	std::string content = "tree " + tree + "\n";

	if (link != NULL) {
		content += "parent " + *link + "\n";
	}
	if (msg != NULL) {
		content += "\n" + *msg + "";
	}

	std::string commit = "commit " + std::to_string(content.length()) + '\0' + content;

	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1((const unsigned char *)commit.c_str(), commit.size(), hash);
	std::stringstream ss;
	for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	std::string commitSha = ss.str();

	uLong bound = compressBound(commit.size());
	std::vector<char> out(bound);
    compress((Bytef*)out.data(), &bound, (const Bytef*)commit.c_str(), (uLong)commit.size());
	std::string compressedOutput = std::string(out.data(), bound);

	std::string dir = ".git/objects/" + commitSha.substr(0, 2);
	std::filesystem::create_directory(dir);
	std::ofstream outFile(dir + "/" + commitSha.substr(2), std::ios::binary);
	if (outFile.is_open()) {
		outFile.write(compressedOutput.data(),compressedOutput.size());
		outFile.close();
		std::cout << commitSha << "\n";
	}

	return EXIT_SUCCESS;
}
