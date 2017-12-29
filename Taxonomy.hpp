// Taxonomy.hpp - Deals with a set of NCBI taxonomy IDs and returns the ID of
// the last common ancestor. With this ID in hand, one can find all nucleotide
// or protein records associated with this taxon. Can be configured to return
// taxons higher than the last common ancestor if a larger dataset is required
// (i.e. the subtaxa are not studied heavily, leaving little to no records to
// work with).
//
// Author: Matt Preston (website: matthewpreston.github.io)
// Created On: Mar 3, 2016
// Revised On: Never

#ifndef TAXONOMY_HPP
#define TAXONOMY_HPP

#include <list>
#include <map>
#include <string>
#include <vector>

// The data structure of nodes.dmp
struct TaxonNode {
	const int taxonID;
	const int parentID;
	const char *rank;
	const char *emblCode;
	const int divisionID;
	const bool inheritedDivFlag;
	const int geneticID;
	const bool inheritedGCFlag;
	const int mitochondrialGeneticCodeID;
	const bool inheritedMGCFlag;
	const bool genbankHiddenFlag;
	const bool hiddenSubtreeRootFlag;
	const char *comments;

	TaxonNode(std::vector<std::string> info);
};

// Last common ancestor finder
class LCA_Finder {
public:
	// Default constructor, needs later setup with nodes.dmp or tree hash table
	LCA_Finder();
	// Give nodes.dmp for setup to initialize hash table
	// Throws std::runtime_error if file does not exist
	LCA_Finder(const char *nodesDumpFile);
	LCA_Finder(std::string &nodesDumpFile);
	// Give a hash table of a tree
	LCA_Finder(std::map<int, TaxonNode> &TreeHashTable);
	// Loads nodes.dmp for tree hash table
	// Throws std::runtime_error if file does not exist
	void LoadData(const char *nodesDumpFile);
	void LoadData(std::string &nodesDumpFile);
	// Loads an existing tree hash table
	void LoadData(std::map<int, TaxonNode> &TreeHashTable);

	// Returns the parent taxID of a given taxID (-1 if doesn't exist)
	const int TraceParent(const int taxID);
	// Returns a list of taxID's starting from a given taxID to the root
	// If it doesn't exist in the tree, returns only the given taxID in the list
	std::list<int> TraceToRoot(const int taxID);
	// Returns the taxID of the LCA given a list of taxIDs
	// If an empty list, return -1
	// If somehow the tree hash table is actually disjoint (i.e. actually is
	// two trees and thus has multiple roots), then return -1
	template <template <typename, typename> class Container, typename Type>
	const int GetLCA_ID(Container<Type, std::allocator<Type> > &taxIDs);
private:
	std::map<int, TaxonNode> treeHashTable;
};

// Defines template functions and classes
#include "Taxonomy.tpp"

#endif // TAXONOMY_HPP