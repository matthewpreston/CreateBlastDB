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

// Returns the taxID of the LCA given a list of taxIDs
// If an empty list, return -1
// If somehow the tree hash table is actually disjoint (i.e. actually is
// two trees and thus has multiple roots), then return -1
template <template <typename, typename> class Container, typename Type>
const int LCA_Finder::GetLCA_ID(Container<Type, std::allocator<Type> > &taxIDs){
	if (taxIDs.size() == 0) return -1;
	if (taxIDs.size() == 1) return taxIDs.front();

	// Choose first taxID and find ancestors for it
	typename Container<Type, std::allocator<Type> >::iterator 
		taxIter = taxIDs.begin();
	std::list<int> ancestors = TraceToRoot(*taxIter);
	std::list<int>::iterator ancestorIter;

	// Iterate through taxa
	taxIter++;
	for (int taxID; taxIter != taxIDs.end(); taxIter++) {
		taxID = *taxIter;
		// See where the LCA for this taxID and the first taxID is
		do {
			for (ancestorIter = ancestors.begin();
				 ancestorIter != ancestors.end();
				 ancestorIter++) {
				if (taxID == *ancestorIter) break;
			}
			if (taxID == *ancestorIter) break;
			taxID = TraceParent(taxID);
		} while (taxID != -1);
		// Remove all ancestors from first taxon that are children of this LCA
		if (taxID == *ancestorIter) {
			ancestors.erase(ancestors.begin(), ancestorIter);
		} else { // Disjoint trees
			return -1;
		}
	}
	return ancestors.front();
}