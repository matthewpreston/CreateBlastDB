// HelperFunctions.tpp - Used to keep cluttering functions out of the way
// 
// Yeah I've never seen a .tpp before either but I wanted to keep the clutter
// out of the header file. Since I didn't want to do an instantiation .cpp
// file for using templates, I created this .tpp for their implementation.
// Essentially one can use the template classes as normal but doesn't need to
// go to the .cpp and add their own explicit instantiations. Check out where I
// got this idea: 
// http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
//
// Author: Matt Preston (website: matthewpreston.github.io)
// Created On: Oct 12, 2016
// Revised On: Never

#include <iostream>
#include <algorithm>
#include <iterator>

// ==== FUNCTIONS ==============================================================

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
						 const std::string sep,
						 std::string (*modify)(const std::string &)) {
	// Make sure modify is actually a function instead a pointer to NULL
	struct temp {
		static std::string modify(const std::string &a) { return a; }
	};
	if (modify == NULL)	modify = temp::modify;
	// See if there's tomfoolery about
	if (first == last) return "";
	// Create string
	std::string result = "\"" + modify(*first++);
	while (first != last)
		result += sep + modify(*first++);
	return result + "\"";
}

// A helper function to simplify printing vectors
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " ")); 
    return os;
}