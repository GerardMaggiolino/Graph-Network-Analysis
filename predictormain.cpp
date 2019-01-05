/*  
 * File contains static methods to fully run the predictorandrecommender 
 * program, predicting future interactions and new collaborations of actors. 
 * Details of usage beneath. 
 */

#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

using namespace std;

const static string USAGE = 
    "./predictorandrecommender called with "
    "incorrect arguments.\nUsage: ./predictorandrecommender "
    "data.tsv predict_recommend_targets predicted_interact"
    " recommended_collab\n";

// Function declarations for main
static void BuildStructures(ifstream&, ifstream&);
static void FindInteractions(bool, ofstream&);
static bool compare(const pair<int, string>&, const pair<int, string>&);

// Bidirectional map for string <-> int conversions for matrix 
static unordered_map<string, int> name_to_int; 
static vector<string> int_to_name;

// Names of actors to find 
static vector<string> actors; 

// Adjacency matrix for graph that holds actor connections. [i][j] = connection
// from actor i to actor j. 
static vector<vector<int>> graph; 


/*
 * Parses command line arguments and calls file methods for the 
 * predictorandrecommender program.
 * 
 * Parameters: 
 *  argv[1] - data.tsv
 *      Tab delimited file of movie actor relationships. Header row expected.
 *      Rows should be formatted as actor name, movie title, and movie year.
 *  argv[2] - targets
 *      File of actor names to suggest for. Header row expected. 
 *  argv[3] - future_interactions.tsv 
 *      Output file of future interactions.
 *  argv[4] - new_collaborations.tsv 
 *      Output file of new collaborations.
 * Return: 
 *  int - 
 *      Exit status. -1 for unsuccessful reading or parsing, 0 otherwise. 
 */ 
int main(int argc, char *argv[]) {
    // Check number of arguments
    if (argc != 5) { 
        cout << USAGE; 
        return -1;
    }
    // Open files and check for successful opening
    ifstream tsv_file(argv[1]);
    ifstream actors_file(argv[2]); 
    ofstream interact_file(argv[3]); 
    ofstream collab_file(argv[4]); 
    if (!tsv_file || !actors_file || !interact_file || !collab_file) { 
        cout << "Failed to read or open files!\n"; 
        return -1; 
    }


    // Builds graph, actors, name_to_int, and int_to_name
    BuildStructures(tsv_file, actors_file);
    // Write top 4 future interactions to interact_file for each actor in actors
    cout << "Finding top predicted interactions ..." << endl;
    FindInteractions(true, interact_file); 
    // Write top 4 new collaborations to collab_file for each actor in actors
    cout << "Finding top recommended collaborations ..." << endl;
    FindInteractions(false, collab_file); 


    // Close all files
    tsv_file.close();
    actors_file.close();
    interact_file.close();
    collab_file.close();

    return 0;
}


/*
 * Builds adjacency matrix, bidirectional map for matrix access, and vector of
 * actors to find predictions for.
 * Modifies graph, actors, name_to_int, and int_to_name. 
 * 
 * Parameters:
 *  tsv - 
 *      Input file of actors and their movies. Assumes tsv file is 3 columns,
 *      with a header. First column contains actor name, second movie, third
 *      movie year. 
 *      Used to build graph, name_to_int, int_to_name
 *  actors_file - 
 *      Input file of actors to find connections for. First row contains
 *      header. Each new row is an actor name. 
 *      Used to build actors.
 */
static void BuildStructures(ifstream& tsv_file, ifstream& actors_file) { 

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
    cout << "Finished reading tsv ..." << endl; 

    // Create adjacency matrix of correct size
    graph = vector<vector<int>>(int_to_name.size(), 
        vector<int>(int_to_name.size(), 0));

    // Create adjacency matrix from movie_to_actor 
    for (auto pair : movie_to_actor) { 
        vector<string>& actors = pair.second; 
        int size = (int) actors.size();
        // Connect all actors for a movie to one another in graph
        for (int i = 0; i < size; i++) { 
            for (int j = i + 1; j < size; j++) { 
                graph[name_to_int[actors[i]]][name_to_int[actors[j]]] = 1;
                graph[name_to_int[actors[j]]][name_to_int[actors[i]]] = 1;
            }
        }
    }
    cout << "Finished creating graph ..." << endl; 

    // Skip header
    getline(actors_file, line);
    // Read in actors to find connections for 
    while (getline(actors_file, line)) { 
        actors.push_back(line); 
    }
}


/*
 * Writes top interactions of actors to output file, based on parameters. 
 * Requires built graph, actors, name_to_int, and int_to_name.
 *
 * Parameters:
 *  neighbor - 
 *      Whether to look for top interactions with neighbors xor not
 *      neighbors. true signifies searching for future interactions, while false
 *      signifies searching for potential new collaborations. 
 *      Future interactions -> neighbors, highest num common neighbors
 *      New collaborations -> not neighbor, highest num common neighbors
 *  out_file - 
 *      Output file of where to write predicted interactions to.
 */
static void FindInteractions(bool neighbor, ofstream& out_file) { 

    // Maximum number of interactions to report
    int predict_max = 4;

    // Stores actors & frequency counts for each possible interact / collab  
    vector<pair<int, string>> predicts; 

    // Write header to file 
    out_file << "Actor1,Actor2,Actor3,Actor4\n";

    // Find suggestions for each actor
    for (string actor : actors) { 
        cout << "Computing for (" << actor << ")" << endl; 
        vector<int>& row = graph[name_to_int[actor]]; 
        int row_size = (int)row.size(); 

        // Stores mutual neighbors for each other actor by col number
        vector<int> mutual_count(row_size, 0);

        // Calculates dot product of each row with other row for mutual count
        for (int dot_ind = 0; dot_ind < row_size; dot_ind++) { 
            for (int i = 0; i < row_size; i++) {
                mutual_count[dot_ind] += (graph[dot_ind][i] & row[i]);
            }
        }
        // Zero out it's own row entry 
        mutual_count[name_to_int[actor]] = 0;

        // Save possible predictions for actor
        for (int i = 0; i < row_size; i++) { 

            // Only check neighbors or not neighbors, depending on param
            if (row[i] == neighbor) { 
                // Retrieve frequency of mutual connections with vertex, name
                predicts.push_back(
                    pair<int, string>(
                    mutual_count[i], int_to_name[i]));
            }
        }
        // Sort predictions by freq, then string value using compare 
        sort(predicts.begin(), predicts.end(), compare);

        // Output predictions to file
        for (int i = 0; i < (int)predicts.size(); i++) { 
            // Zero for value of neighbor, greater than max num predictions
            if (!predicts[i].first || i >= predict_max) { 
                break;
            }
            out_file << predicts[i].second << (i != predict_max-1 ? "\t" : "");
        }
        out_file << "\n"; 

        predicts.clear();
    }
}


/* 
 * Helper function to compare pair<int, string>. When used as operator<, sorts
 * in decreasing order (largest first) for int, and increasing order (smallest
 * first) for string.
 */
static bool compare(const pair<int, string>& x, const pair<int, string>& y) { 
    // Compare strings in order of increasing order (smallest first)
    if (x.first == y.first) { 
        return x.second < y.second; 
    }
    // Compare int in decreasing order (largest first) 
    return x.first > y.first; 
}
