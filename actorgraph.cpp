/*
 * This file implements ActorGraph, a class used to find the shortest path
 * between two actors using movies as edges. Member function loadFromFile should
 * be called to initialize the ActorGraph prior to using findPath, which prints
 * the shortest path between the actors to a file. See function headers for
 * documentation. 
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <climits>
#include <queue>
#include <unordered_map>
#include "actorgraph.hpp"

using namespace std;


/*
 * ActorGraph loadFromFile creates all graph data structures from tab delimited 
 * actor, movie relationships. Initializes actor_map, movie_map, and vertices.
 *
 * Parameters: 
 *  in_filename - 
 *      Tab delimited filename of actor, movie relationships. Header expected.
 *      Each row is to be separated as actor name, movie name, and movie
 *      year. 
 *  use_weighted_edges - 
 *      If true, all edge weights will be equal to (2018 - Y) + 1,
 *      prioritizing newer movies in Dijkstra's algorithm. Otherwise, all 
 *      movies will be equally weighted.
 *
 * Returns: 
 *  bool -
 *      True indicates successful reading of file.  
 */
bool ActorGraph::loadFromFile(const char *in_filename,
                              const bool use_weighted_edges) {
    // Initialize the file stream
    ifstream infile(in_filename);
    if (!infile) 
        return false; 

    // Skip the header
    string s;
    getline(infile, s);

    // Parse file, building data structures
    while (getline(infile, s)) {

        // Parse the line by tabs 
        istringstream ss(s);
        string next;
        vector<string> record;
        while (getline(ss, next, '\t')) {
            record.push_back(next);
        }
        // Ensure exact formatting of three columns per line
        if (record.size() != 3) {
            return false;
        }
        
        string actor_name(record[0]);
        string movie_title(record[1].append("#@").append(record[2]));
        int movie_year = stoi(record[2]); 
        
        // Add movie to the actor 
        if (actor_map.find(actor_name) == actor_map.end()) { 
            actor_map[actor_name] = vector<string>();
        }
        actor_map[actor_name].push_back(movie_title);
        
        // Add actor to the movie
        if (movie_map.find(movie_title) == movie_map.end()) { 
            movie_map[movie_title] = pair<int, vector<string>>
                (use_weighted_edges ? (2018 - movie_year + 1) : 1,
                vector<string>());
        }
        movie_map[movie_title].second.push_back(actor_name);
    }

    // Initialize empty Vertex for all actors, for use in Dijkstra's algorithm
    for (auto p : actor_map) { 
        vertices[p.first] = new Vertex(p.first);
    }
    
    if (!infile.eof()) {
        return false;
    }
    infile.close();

    return true;
}


/* 
 * ActorGraph findPath uses Dijkstra's algorithm to find the shortest path
 * between two actors using mutual movies as edges, and writes this shortest
 * path to a specified file. 
 * 
 * Parameters: 
 *  ofstream & out_file - 
 *      File to write the shortest path to. File will remain open after call.
 *  string start_name - 
 *      The starting actor of the path.
 *  string end_name - 
 *      The ending actor of the path. 
 */ 
void ActorGraph::findPath(ofstream& out_file, 
    string start_name, string end_name) {

    // Min-heap for Dijkstra algorithm, weighted by lowest distance
    priority_queue<Vertex *, vector<Vertex *>, vertexComp> pq; 

    // Initialize all Vertices of graph
    for (auto v : vertices) { 
        v.second->dist = INT_MAX; 
        v.second->done = false;
        v.second->prev_actor = ""; 
        v.second->prev_movie = ""; 
    }

    // Add starting vertex to pq
    Vertex* working = vertices[start_name]; 
    working->dist = 0; 
    pq.push(working); 

    // Represents weight of current movie
    int weight; 

    // Explore working vertex, adding adjacent to pq following Dijkstra's
    while (pq.size()) { 
        working = pq.top(); 
        pq.pop();

        // Stop if ending vertex being explored 
        if (working->name == end_name)
            break;

        // Continue if vertex has already been explored
        if (working->done) 
            continue;

        working->done = true; 

        // Explore working's neighbors, by movie
        for (string movie : actor_map[working->name]) { 
            // Set weight of edge
            weight = movie_map[movie].first;
            for (string adj_actor : movie_map[movie].second) { 
                // Skip current actor 
                if (adj_actor == working->name) 
                    continue;
                // If distance through working less than previous best distance,
                // change the previous to working and push to pq
                if (working->dist + weight < vertices[adj_actor]->dist) { 
                    vertices[adj_actor]->prev_actor = working->name; 
                    vertices[adj_actor]->prev_movie = movie; 
                    vertices[adj_actor]->dist = working->dist + weight; 
                    pq.push(vertices[adj_actor]);
                }
            }
        }
    }

    // Reverse the order from end to start using a stack  
    stack<string> vs; 
    while (working->prev_actor != "") { 
        vs.push(working->name);
        vs.push(working->prev_movie);
        working = vertices[working->prev_actor];
    }
    vs.push(working->name);

    // Write to file from start to end with formatting 
    while (vs.size() > 1) { 
        out_file << "(" << vs.top() << ")--["; 
        vs.pop();
        out_file << vs.top() << "]-->"; 
        vs.pop();
    }
    out_file << "(" << vs.top() << ")";

}


/* 
 * ActorGraph dctor for dynamically allocated vertices of the graph
 */
ActorGraph::~ActorGraph() { 
    for (auto p : vertices) 
        delete p.second;
}


/* 
 * Vertex constructor for graph. Creates Vertex with initialized actor name.
 *
 * Parameters: 
 *  string n - 
 *      Initializes name of vertex 
 */
ActorGraph::Vertex::Vertex(string n) : name(n) {}
