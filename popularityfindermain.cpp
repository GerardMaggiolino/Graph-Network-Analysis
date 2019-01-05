/*
 * This file fully contains the methods necessary to run the popularityfinder
 * program, which finds actors of a certain popularity through k-core graph
 * decomposition.
 */

#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

using namespace std;

// Usage string
const static string USAGE = 
    "./popularityfinder called with "
    "incorrect arguments.\nUsage: ./popularityfinder "
    "data.tsv k pop_actors\n";

// Function declarations for main
static void BuildStructures(ifstream&);

// Bidirectional map for string <-> int conversions for matrix 
static unordered_map<string, int> name_to_int; 
static vector<string> int_to_name;

// Names of actors to find 
static vector<string> actors; 

// Adjacency matrix for graph that holds actor connections. [i][j] = connection
// from actor i to actor j. 
static vector<vector<int>> graph; 

// Holds count of each actor
static vector<int> counts;

/*
 * Runs the popularityfinder program, performing k-core graph decomposition
 * based on passed k. Edges between actors based on mutual movies. 
 *
 * Parameters: 
 *  argv[1] - data.tsv
 *      Tab delimited file of movie actor relationships. Header row expected.
 *      Rows should be formatted as actor name, movie title, and movie year.
 *  argv[2] - k
 *      The minimum number of connections between actors to remain in the output
 *      file. The target used for k-core decomposition.
 *  argv[3] - pop_actors 
 *      The output file name of actors popular enough, with at least k 
 *      connections. 
 */
int main(int argc, char *argv[]) {
    // Check number of cmdline args
    if (argc != 4) { 
        cout << USAGE; 
        return -1;
    }

    // Open files and save min num
    ifstream tsv_file(argv[1]);
    int k = stoi(argv[2]);
    ofstream out_file(argv[3]);

    // Check proper file opening
    if (!tsv_file || !out_file) { 
        cout << "Error opening file!" << endl;
        return -1;
    }

    // Create structures
    BuildStructures(tsv_file);

    bool not_done = true;
    // Perform k-core pruning
    while (not_done) {
        cout << "Pruning...\n";
        not_done = false;
        // Remove vertices lower than count
        for (int i = 0; i < (int)counts.size(); i++) {
            // If vertex lower than count and not already deleted
            if (counts[i] < k && counts[i] > -(int)counts.size()) {
                not_done = true;
                // Deinc each neighbor of vertex i
                for (int j = 0; j < (int)counts.size(); j++) {
                    if (graph[i][j])
                        counts[j]--;
                }
                // Set vertex to 'deleted', with negative counts
                counts[i] = -(int)counts.size();
            }
        }
    }


    // Keep actors with greater than k connections
    vector<string> names;
    for (int i = 0; i < (int)counts.size(); i++) { 
        if (counts[i] >= k)
            names.push_back(int_to_name[i]);
    }

    // Sort the actors alphabetically by name 
    sort(names.begin(), names.end());

    out_file << "Actor\n";
    for (string s : names) { 
        out_file << s << '\n';
    }

    tsv_file.close();
    out_file.close();

    return 0;
}


/*
 * Builds adjacency matrix and bidirectional map for matrix access.
 * Modifies graph, name_to_int, and int_to_name. 
 * 
 * Params
 *  tsv - 
 *      Input file of actors and their movies. Assumes tsv file is 3 columns,
 *      with a header. First column contains actor name, second movie, third
 *      movie year. 
 *      Used to build graph, name_to_int, int_to_name
 */ 
static void BuildStructures(ifstream& tsv_file) { 

    // Holds the next line when reading in the file
    string line;

    // Discard the header
    getline(tsv_file, line); 

    // Temporary data structure to build adjacency matrix, maps movies to actors
    unordered_map<string, vector<string>> movie_to_actor;

    // Read in the tsv file, ending when EOF reached
    while (getline(tsv_file, line)) {
        // Tokenize the line into strings 
        istringstream ss(line);
        vector<string> record;
        // Parse the line by tabs, storing each word in record 
        string next; 
        while (getline(ss, next, '\t')) {
                record.push_back(next);
        }

        string actor_name(record[0]);
        string movie_title(record[1].append(record[2]));

        // Build name_to_int, int_to_name
        if (!name_to_int.count(actor_name)) { 
            name_to_int[actor_name] = int_to_name.size();
            int_to_name.push_back(actor_name);
        }

        // Build temporary data structure which maps movies to actors
        if (!movie_to_actor.count(movie_title)) { 
            movie_to_actor[movie_title] = vector<string>();
        }
        movie_to_actor[movie_title].push_back(actor_name);

    }
    cout << "Finished reading tsv..." << endl; 

    // Create adjacency matrix of correct size
    graph = vector<vector<int>>(int_to_name.size(), 
        vector<int>(int_to_name.size(), 0));
    

    // Create adjacency matrix from movie_to_actor 
    for (auto pair : movie_to_actor) { 
        vector<string>& actors = pair.second; 
        int size = (int) actors.size();
        int actor_i, actor_j;
        for (int i = 0; i < size; i++) { 
            actor_i = name_to_int[actors[i]];
            for (int j = i + 1; j < size; j++) { 
                actor_j = name_to_int[actors[j]];
                graph[actor_i][actor_j] = 1;
                graph[actor_j][actor_i] = 1;
            }
        }
    }
    cout << "Finished creating graph..." << endl; 

    // Init counts vector 
    counts = vector<int>(int_to_name.size(), 0);

    for (int i = 0; i < (int)graph.size(); i++) { 
        for (int j = 0; j < (int)graph.size(); j++) { 
            counts[i] += graph[i][j];
        }
    }
    cout << "Finished first pass counts..." << endl; 
}



