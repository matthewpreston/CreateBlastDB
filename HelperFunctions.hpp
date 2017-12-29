// HelperFunctions.hpp - Used to keep cluttering functions out of the way
//
// Author: Matt Preston (website: matthewpreston.github.io)
// Created On: Oct 12, 2016
// Revised On: Never

#ifndef HELPERFUNCTIONS_HPP
#define HELPERFUNCTIONS_HPP

#include <exception>
#include <string>
#include <vector>

// ==== CLASSES ================================================================

// An exception class to be thrown if a file doesn't exist
class FileDoesNotExist: public std::exception {
	std::string message;
public:
	FileDoesNotExist(const std::string &File);
	~FileDoesNotExist() throw();
	virtual const char *what() const throw();
};

// ==== FUNCTIONS ==============================================================

bool FileExists(const std::string &name);

// See if files exist (throws Exception if not)
void FilesExist(std::vector<std::string> &files);

// Remove file extensions (Ex. "foo.txt" -> "foo")
std::string RemoveExtension(const std::string &file);

// Creates a string containing all the different files for the command line to 
// read with a custom separator and modifying function.
// NOTE: Adds double quotes to beginning and end of string. Returns empty string
// if the two iterators are the same.
//
// Ex.
//    std::vector<std::string> files;
//    files.push_back("foo.fasta");
//	  files.push_back("bar.fasta");
//    std::cout << ToCmdLineStr(files.begin(), files.end()) << std::endl
//    //Output: "foo.fasta bar.fasta"
//	  std::cout << ToCmdLineStr(files.begin(), files.end(), "_", 
//								&RemoveExtension)
//			    << std::endl;
//	  //Output: "foo_bar"
template <typename iter>
std::string ToCmdLineStr(iter first, iter last,
						 const std::string sep=" ",
						 std::string (*modify)(const std::string &)=NULL);

// A helper function to simplify printing vectors
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v);

// Reads file into a string
std::string ReadFile(const std::string &fileName);

// Reads file into a vector of strings
void ReadFile(const std::string &fileName, std::vector<int> &output);

// Finds a file name to be used as a temporary dump of data
// Returns the file name so it can be deleted later
std::string GetTempFileName(const char *fileName, const char *ext = "temp");

// Returns the file size in bytes
long GetFileSize(std::string fileName);

// For templated functions and classes
#include "HelperFunctions.tpp"

#endif /* HELPERFUNCTIONS_HPP */