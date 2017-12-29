// HelperFunctions.cpp - Used to keep cluttering functions out of the way
//
// Author: Matt Preston (website: matthewpreston.github.io)
// Created On: Oct 12, 2016
// Revised On: Never

#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <boost/lexical_cast.hpp>
#include "HelperFunctions.hpp"
#include <iostream>

// ==== CLASSES ================================================================

// FileDoesNotExist - An exception class to be thrown if a file doesn't exist
FileDoesNotExist::FileDoesNotExist(const std::string &File) {
	message = std::string("File " + File + " does not exist");
}

FileDoesNotExist::~FileDoesNotExist() throw() {}

const char *FileDoesNotExist::what() const throw() {
	return message.c_str();
}

// ==== FUNCTIONS ==============================================================

inline bool FileExists(const std::string &name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

// See if files exist (throws Exception if not)
void FilesExist(std::vector<std::string> &files) {
	for (std::vector<std::string>::iterator it = files.begin();
		 it < files.end();
		 it++) {
		if (!FileExists(*it)) throw FileDoesNotExist(*it);
	}
}

// Remove file extensions
std::string RemoveExtension(const std::string &file) {
	std::size_t found = file.find(".");
	if (found != std::string::npos)
		return file.substr(0, found);
	else
		return file;
}

// Reads file into a string
// Pretty dangerous implementation to be honest, no checking if the containers
// used can even hold the fileSize
std::string ReadFile(const std::string &fileName) {
    std::ifstream ifs(fileName.c_str(),
    				  std::ios::in | std::ios::binary | std::ios::ate);

	if (ifs.fail()) 
		throw std::runtime_error(std::string("Cannot read: ") + fileName);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(&bytes[0], fileSize);

    return std::string(&bytes[0], fileSize);
}

// Reads file into a vector of ints
void ReadFile(const std::string &fileName, std::vector<int> &output) {
	std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::binary);

	if (ifs.fail()) 
		throw std::runtime_error(std::string("Cannot read: ") + fileName);
	std::istream_iterator<int> start(ifs), end;
	output.assign(start, end);
}

// Finds a file name to be used as a temporary dump of data
// Returns the file name so it can be deleted later
std::string GetTempFileName(const char *fileName, const char *ext) {
	std::string prefix=fileName, extension=ext;
	std::string tempFileName = prefix + "." + extension;

	if (!FileExists(tempFileName)) return tempFileName;
	int count = 0;
	do {
		count++;
		tempFileName = prefix + "_" + boost::lexical_cast<std::string>(count) +
					   "." + extension;
	} while (FileExists(tempFileName));
	return tempFileName;
}

// Returns file size in bytes
long GetFileSize(std::string filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}