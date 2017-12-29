// CreateBlastDB.cpp - Does what the title says given either a FASTA file or a
// list of GI numbers. See --help for explained options
//
// Author: Matt Preston (website: matthewpreston.github.io)
// Created On: Oct 12, 2016
// Revised On: So many times for bug fixes
//			   Aug 11, 2017		Fixed flow of calling NCBI programs

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include "HelperFunctions.hpp"
#include "Taxonomy.hpp"

#define BLAST_DB_PATH "/media/Storage2/BlastDB"
#define DEFAULT_LINE_LENGTH 80
#define MIN_DESCRIPTION_LENGTH (DEFAULT_LINE_LENGTH/2)

namespace {
	const int SUCCESS                   = 0;
	const int ERROR_IN_COMMAND_LINE     = 1;
	const int ERROR_UNHANDLED_EXCEPTION = 2;
}

namespace po = boost::program_options;

int main(int argc, char *argv[]) {

	try {
		std::string appName = boost::filesystem::basename(argv[0]);
		int verbosity;
		std::vector<std::string> dbs;
		std::vector<std::string> refs;
		std::vector<std::string> gis;
		std::vector<std::string> taxa;
		std::string blastPath;
		std::string dbtype;
		std::string nodesFile;
		std::string output;
		bool getChildrenGIs;

		// Set up possible options
		po::options_description desc("Options", DEFAULT_LINE_LENGTH,
									 MIN_DESCRIPTION_LENGTH);
		desc.add_options()
			("help,h", "Display help")
			("blastPath,b", po::value<std::string>(&blastPath)
				->value_name("PATH")->default_value(BLAST_DB_PATH),
				"Path to BLAST databases")
			("children,c", po::value<bool>(&getChildrenGIs)
				->zero_tokens()->default_value(false)->implicit_value(true),
				"To be used when including taxonomy IDs, "
				"When last common ancestor is found, retrieve all GIs from all"
				"children when creating the database")
			("db,d", po::value< std::vector<std::string> >(&dbs)
				->value_name("FILE")->multitoken()->composing(),
				"Create database based off of pre-existing databases")
			("dbtype,D", po::value<std::string>(&dbtype)
				->value_name("STR")->default_value("nucl"), 
				"Type of database: \"nucl\" or \"prot\"")
			("gi,g", po::value< std::vector<std::string> >(&gis)
				->value_name("FILE")->multitoken()->composing(),
				"Create database using text file containing "
				"newline delimited GI numbers (allows multiple GI.txt)")
			("nodesFile,n", po::value<std::string>(&nodesFile)
				->value_name("FILE")->default_value("nodes.dmp"),
				"To be used when including taxonomy IDs, "
				"NCBI Taxonomy nodes file for finding last common ancestor, "
				"download: ftp://ftp.ncbi.nih.gov/pub/taxonomy/taxdump.tar.gz")
			("output,o", po::value<std::string>(&output)
				->value_name("STR")->default_value("out"), "Output prefix")
			("reference,r", po::value< std::vector<std::string> >(&refs)
				->value_name("FILE")->multitoken()->composing(),
				"Create database using FASTA records (allows multiple FASTA)")
			("taxa,t", po::value< std::vector<std::string> >(&taxa)
				->value_name("FILE")->multitoken()->composing(),
				"Create database using text file containing "
				"newline delimited taxonomy ids (allows multiple Taxa.txt)")
			("verbosity,v", po::value<int>(&verbosity)
				->value_name("INT")->default_value(0)->implicit_value(1),
				"Verbosity level")
			//("unreg", "Unrecognized options")
			;

		// Ensure arguments were inputted
		if (argc == 1) {
			std::cerr << "No arguments were specified\n";
			std::cout << "USAGE: " << appName << " [options]\n";
			std::cerr << "\n" << desc << std::endl;
			return ERROR_IN_COMMAND_LINE;
		}

		// Set up visual help info and parse out the command line
		po::command_line_parser parser(argc, argv);
		parser.options(desc).allow_unregistered().style(
			po::command_line_style::default_style |
			po::command_line_style::allow_slash_for_short);
		po::parsed_options parsed_options = parser.run();

		// Store the options
		po::variables_map vm;
		po::store(parsed_options, vm);
		po::notify(vm);

		// Visually see what was inputted
		if (vm.count("help")) {
			std::cout << "USAGE: " << appName << " [options]\n";
			std::cout << "\n" << desc << std::endl;
			return 0;
		}
		if (vm.count("blastPath")) {
			if (verbosity > 1)
				std::cout << "BLAST Path: " << blastPath << std::endl;
		}
		if (vm.count("dbtype")) {
			if (dbtype != "nucl" && dbtype != "prot") {
				std::cerr << "Database type must be either \"nucl\" or \"prot\""
						  << ": " << dbtype << std::endl;
				return ERROR_IN_COMMAND_LINE;
			}
			if (verbosity > 1)
				std::cout << "DBType: " << dbtype << std::endl;
		}
		if (vm.count("db")) {
			// The db given is an incomplete file name, just the prefix is to
			// be given
			/*
			try {
				FilesExist(dbs);
			} catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
				return ERROR_IN_COMMAND_LINE;
			}*/
			if (verbosity > 1)
				std::cout << "DBs: " << dbs << std::endl;
		}
		if (vm.count("gi")) {
			try {
				FilesExist(gis);
			} catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
				return ERROR_IN_COMMAND_LINE;
			}
			if (verbosity > 1)
				std::cout << "GIs: " << gis << std::endl;
		}
		if (vm.count("taxa")) {
			try {
				FilesExist(taxa);
			} catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
				return ERROR_IN_COMMAND_LINE;
			}
			if (verbosity > 1)
				std::cout << "Taxa: " << taxa << std::endl;
			// Nodes file only used when taxa option specified
			if (!FileExists(nodesFile)) {
				std::cerr << "Given nodes file does not exist: "
						  << nodesFile << std::endl;
				return ERROR_IN_COMMAND_LINE;
			}
		}
		if (vm.count("output")) {
			if (verbosity > 1)
				std::cout << "Output: " << output << std::endl;
		}
		if (vm.count("reference")) {
			try {
				FilesExist(refs);
			} catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
				return ERROR_IN_COMMAND_LINE;
			}
			if (verbosity > 1)
				std::cout << "References: " << refs << std::endl;
		}
		/*
		if (vm.count("unreg")) {
			if (verbosity > 1)
				std::cout << "Unrecognized options: "
						  << po::collect_unrecognized(parsed_options.options,
													  po::exclude_positional)
						  << std::endl;
		}
		*/

		// Find GI numbers given taxonomy IDs
		std::string tempGIsFile;
		if (!taxa.empty()) {
			std::vector<int> taxIDs, temp;

			// Consolidate taxIDs into one vector
			for (std::vector<std::string>::iterator it = taxa.begin();
				 it != taxa.end();
				 it++) {
				ReadFile(*it, temp);
				taxIDs.insert(taxIDs.end(), temp.begin(), temp.end());
			}

			// Find taxID of last common ancestor
			if (verbosity > 1)
				std::cout << "Finding LCA's taxonomy ID" << std::endl; 
			LCA_Finder lca_finder(nodesFile);
			int LCA_ID = lca_finder.GetLCA_ID<std::vector, int>(taxIDs);
			if (verbosity > 0)
				std::cout << "LCA ID: " << LCA_ID << std::endl;

			// Get the gis associated with LCA (and children)
			if (verbosity > 1)
				std::cout << "Finding the GI's associated with LCA"
						  << std::endl;
			tempGIsFile = GetTempFileName("LCA_GIs");
			system(("esearch"
					" -db nuccore"
					" -query \"txid" + 
							   boost::lexical_cast<std::string>(LCA_ID) + 
							   "[Organism:" +
							   ((getChildrenGIs) ? "exp" : "noexp") + "]\""
					" | "
					"efetch"
					" -format uid"
					" > " + tempGIsFile
					).c_str());

			// Check if anything was returned
			if (GetFileSize(tempGIsFile) > 0) {
				if (verbosity > 1)
					std::cout << "Found GI's; adding to GI list" << std::endl;
				gis.push_back(tempGIsFile);
			} else {
				std::cerr << "Warning: no direct links found for last common "
						  << "ancestor (ID: " << LCA_ID << "). Try using "
						  << "--children flag" << std::endl;
			}
		}

		// Create database from refs
		if (!refs.empty()) {
			std::string refList = ToCmdLineStr(refs.begin(), refs.end());
			std::string refDBName = ToCmdLineStr(refs.begin(), refs.end(), "_", 
												 &RemoveExtension);
												 
			// Build command for creating a BLAST database from reference FASTAs
			const std::string cmd = ("makeblastdb"
				" -dbtype " + dbtype +
				" -in " 	+ refList +
				" -out " 	+ refDBName +
				((verbosity > 0) ? "" : " >/dev/null 2>&1")
			);
			if (verbosity > 1)
				std::cout << "Executing: " << cmd << std::endl;
			system(cmd.c_str());
			dbs.push_back(refDBName);
		}

		// Create database from given GI numbers
		if (!gis.empty()) {

			// Prepare command line arguments
			std::string giList = ToCmdLineStr(gis.begin(), gis.end());
			std::string giDBName = ToCmdLineStr(gis.begin(), gis.end(), "_", 
												&RemoveExtension);

			// Set this silly parameter in order to create the database type?!?
			std::string blastDBName;
			if (dbtype == "nucl") 
				blastDBName = blastPath + "/nt";
			else 
				blastDBName = blastPath + "/nr";
			
			// Build command for aliasing multiple BLAST databases / GI files
			const std::string cmd = ("blastdb_aliastool"
				" -db " + blastDBName +
				" -dbtype " + dbtype +
				((gis.empty()) ? "" : (" -gilist " + giList)) +
				" -out " + giDBName +
				" -title " + giDBName +
				((verbosity > 0) ? "" : " >/dev/null 2>&1")
			);
			if (verbosity > 1)
				std::cout << "Executing: " << cmd << std::endl;
			system(cmd.c_str());
			dbs.push_back(giDBName);
		}
		
		// Create an aggregated database based off of previous databases, the
		// newly created reference database, and the newly created GI number db
		if (dbs.size()) {
			
			// Prepare command line arguments
			std::string dbList = ToCmdLineStr(dbs.begin(), dbs.end());
			
			// Set this silly parameter in order to create the database type?!?
			std::string blastDBName;
			if (dbtype == "nucl") 
				blastDBName = blastPath + "/nt";
			else 
				blastDBName = blastPath + "/nr";
			
			// Build command for aliasing multiple BLAST databases / GI files
			const std::string cmd = ("blastdb_aliastool"
				" -dbtype " + dbtype +
				((dbs.empty()) ? "" : (" -dblist " + dbList)) +
				" -out " + output +
				" -title " + output +
				((verbosity > 0) ? "" : " >/dev/null 2>&1")
			);
			if (verbosity > 1)
				std::cout << "Executing: " << cmd << std::endl;
			system(cmd.c_str());
		}

		// Cleanup
		if (!tempGIsFile.empty()) system(("rm " + tempGIsFile).c_str());

	} catch (const std::exception &e) {
		std::cerr << "An exception occurred:\n" << e.what() << std::endl;
		return ERROR_UNHANDLED_EXCEPTION;
	}
	return SUCCESS;
}