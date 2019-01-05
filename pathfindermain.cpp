/* 
 * This file contains the main function for the pathfinder program, which finds
 * paths between actors through mutual movies. Use 
 *
 * make pathfinder
 * 
 * to make the program. Refer to the README or main function header for 
 * documentation on program use.
 */

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "actorgraph.hpp"

using namespace std;

// Usage strings for any errors encountered with file reading or parsing. 
const string USAGE = "Usage: ./pathfinder "
        "movie_tsv u/w pairs_tsv output_paths\n"
        "\tmovie_tsv -\tTab delimited file of movie actor relationships. "
        "Header row expected. Rows should be formatted as actor name, movie "
        "title, and movie year.\n\tu/w -\t\tWeighted or unweighted graph "
        "usage. Weighted usage signifies that newer movies should be preferred "
        " with lower weights when finding the shortest path.\n\tpairs_tsv -\t"
        "Tab delimited file of actors to find paths between. Header row "
        "expected. Rows should be formatted as starting actor, ending actor.\n"
        "\toutput_paths -\tName of file to create for output of shortest paths."
        ;
const string ERROR_ARG = " called with incorrect arguments.";
const string ERROR_PARAM = "Wrong parameter, must be u or w";
const string ERROR_READ_1 = "Error reading actors tsv file.";
const string ERROR_READ_2 = "Error reading pairs file or opening output file.";

/* 
 * Parses command line arguments and pairs file to obtain pairs to find the
 * shortest path for. Usage detailed through parameters. 
 *
 * Parameters: 
 *  argv[1] - data.tsv
 *      Tab delimited file of movie actor relationships. Header row expected.
 *      Rows should be formatted as actor name, movie title, and movie year.
 *  argv[2] - w/u
 *      Weighted or unweighted graph usage. Weighted usage signifies that newer
 *      movies should be preferred with lower weights when finding the shortest
 *      path. Unweighted will find the shortest path, disregarding movie year. 
 *  argv[3] - test_pairs.tsv
 *      Tab delimited file of actors to find paths between. Header row expected.
 *      Rows should be formatted as starting actor, ending actor. 
 *  argv[4] - out_paths
 *      Name of file to create for output of shortest paths. 
 *
 * Return: 
 *  int - 
 *      Exit status. -1 for unsuccessful reading or parsing, 0 otherwise. 
 */
int main(int argc, char *argv[]) {
    // Check correct number commandline args
    if (argc != 5) { 
        cout << argv[0] << ERROR_ARG << endl;
        cout << USAGE << endl; 
        return -1; 
    }
    // Check weighted edge parameter
    if (*argv[2] != 'u' && *argv[2] != 'w') { 
        cout << ERROR_PARAM << endl;
        cout << USAGE << endl; 
        return -1; 
    }

    // Create ActorGraph object to find shortest paths between actors.
    ActorGraph graph; 

    // Call loadFromFile to initialize ActorGraph, exiting if unsuccessful read.
    if (!graph.loadFromFile(argv[1], (*argv[2] == 'w') ? true : false)) {
        cout << ERROR_READ_1 << endl;
        return -1; 
    } 
    // Set up file streams for output and pairs 
    ifstream pairs(argv[3]);
    ofstream output(argv[4]); 
    if (!pairs || !output) { 
        cout << ERROR_READ_2 << endl;
        return -1;
    }

    // Skip header of pairs file 
    string line; 
    getline(pairs, line);
    // Write header to output file 
    output << "(actor)--[movie#@year]-->(actor)--...\n";

    // Read from pairs file, call ActorGraph findPath to find shortest path
    while (getline(pairs, line)) { 
        // Get two names from string
        istringstream ss(line);
        string next;
        vector<string> name_pairs;
        while (getline(ss, next, '\t')) {
            name_pairs.push_back(next);
        }

        // Find path and writes to file
        graph.findPath(output, name_pairs[0], name_pairs[1]);
        output << "\n";    
    }

    // Close all files 
    pairs.close();
    output.close();

    return 0;
}
