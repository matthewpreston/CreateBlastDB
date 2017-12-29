// Taxonomy.cpp - Deals with a set of NCBI taxonomy IDs and returns the ID of
// the last common ancestor. With this ID in hand, one can find all nucleotide
// or protein records associated with this taxon. Can be configured to return
// taxons higher than the last common ancestor if a larger dataset is required
// (i.e. the subtaxa are not studied heavily, leaving little to no records to
// work with).
//
// Author: Matt Preston (website: matthewpreston.github.io)
// Created On: Mar 3, 2016
// Revised On: Never

#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include "HelperFunctions.hpp"
#include "Taxonomy.hpp"

TaxonNode::TaxonNode(std::vector<std::string> fields) 
	: taxonID(						atoi(fields[0].c_str()))
	, parentID(						atoi(fields[1].c_str()))
	, rank(								 fields[2].c_str())
	, emblCode(							 fields[3].c_str())
	, divisionID(					atoi(fields[4].c_str()))
	, inheritedDivFlag(				atoi(fields[5].c_str()))
	, geneticID(					atoi(fields[6].c_str()))
	, inheritedGCFlag(				atoi(fields[7].c_str()))
	, mitochondrialGeneticCodeID(	atoi(fields[8].c_str()))
	, inheritedMGCFlag(				atoi(fields[9].c_str()))
	, genbankHiddenFlag(			atoi(fields[10].c_str()))
	, hiddenSubtreeRootFlag(		atoi(fields[11].c_str()))
	, comments(							 fields[12].c_str())
{}

// Default constructor, needs later setup with nodes.dmp or tree hash table
LCA_Finder::LCA_Finder() {}

// Give nodes.dmp for setup to initialize hash table
// Throws std::runtime_error if file does not exist
LCA_Finder::LCA_Finder(const char *nodesDumpFile) {
	std::string file = nodesDumpFile;
	LoadData(file);
}

LCA_Finder::LCA_Finder(std::string &nodesDumpFile) {
	LoadData(nodesDumpFile);
}

// Give a hash table of a tree
LCA_Finder::LCA_Finder(std::map<int, TaxonNode> &TreeHashTable)
	: treeHashTable(TreeHashTable)
{}

// Loads nodes.dmp for tree hash table
// Throws std::runtime_error if file does not exist
void LCA_Finder::LoadData(const char *nodesDumpFile) {
	std::string file = nodesDumpFile;
	LoadData(file);
}

// Loads nodes.dmp for tree hash table
// Throws std::runtime_error if file does not exist
void LCA_Finder::LoadData(std::string &nodesDumpFile) {
	std::string fileContents = ReadFile(nodesDumpFile);

	// Delimit by rows, then by fields, load fields into a list, then into the
	// hash table
	size_t rowLast, rowNext, fieldLast, fieldNext;
	std::string row, field;
	std::string fieldDelim = "\t|\t";
	std::string rowDelim = "\t|\n";
	std::vector<std::string> fields;
	
	rowLast = 0;
	while ((rowNext=fileContents.find(rowDelim,rowLast)) != std::string::npos){
		row = fileContents.substr(rowLast, rowNext - rowLast);
		fieldLast = 0;
		fields.clear();
		while ((fieldNext=row.find(fieldDelim,fieldLast)) != std::string::npos){
			fields.push_back(row.substr(fieldLast, fieldNext - fieldLast));
			fieldLast = fieldNext + fieldDelim.length();
		}
		fields.push_back(row.substr(fieldLast, fieldNext));
		rowLast = rowNext + rowDelim.length();

		// Load hash table
		int taxonID = atoi(fields[0].c_str());
		TaxonNode node(fields);
		treeHashTable.insert(std::pair<int, TaxonNode>(taxonID, node));
	}
/*
	int count = 1;
	for (std::map<int, TaxonNode>::iterator it=treeHashTable.begin();
		it != treeHashTable.end();
		it++) {
		std::cout << "Row: " << count << "\n"
				  << "\tTaxon ID: " << it->first << "\n"
				  << "\tParent ID: " << it->second.parentID << "\n"
				  << "\tRank: " << it->second.rank << "\n"
				  << "\tEmblCode: " << it->second.emblCode << "\n"
				  << "\tDivision ID: " << it->second.divisionID << "\n"
				  << "\tInherited Div Flag: " << it->second.inheritedDivFlag << "\n"
				  << "\tGenetic ID: " << it->second.geneticID << "\n"
				  << "\tInherited GC Flag: " << it->second.inheritedGCFlag << "\n"
				  << "\tMito Genetic Code ID: " << it->second.mitochondrialGeneticCodeID << "\n"
				  << "\tInherited MGC Flag: " << it->second.inheritedMGCFlag << "\n"
				  << "\tGenbank Hidden Flag: " << it->second.genbankHiddenFlag << "\n"
				  << "\tComments: " << it->second.comments << "\n";
		count++;
	}
*/
}

// Loads an existing tree hash table
void LCA_Finder::LoadData(std::map<int, TaxonNode> &TreeHashTable) {
	treeHashTable = TreeHashTable;
}

// Returns the parent taxID of a given taxID (-1 if doesn't exist)
const int LCA_Finder::TraceParent(const int taxID) {
	std::map<int, TaxonNode>::iterator it = treeHashTable.find(taxID);
	return (it == treeHashTable.end()) ? -1 : it->second.parentID;
}

// Returns a list of taxID's starting from a given taxID to the root
// If it doesn't exist in the tree, returns only the given taxID in the list
std::list<int> LCA_Finder::TraceToRoot(const int taxID) {
	std::list<int> pathToRoot;
	int node = taxID;

	do {
		pathToRoot.push_back(node);
		node = TraceParent(node);
	} while (node != -1 && node != 1); // -1 is invalid, 1 indicates root
	return pathToRoot;
}