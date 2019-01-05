/*
 * This file declares ActorGraph, a class used to find the shortest path
 * between two actors using movies as edges. Member function loadFromFile should
 * be called to initialize the ActorGraph prior to using findPath, which prints
 * the shortest path between the actors to a file. See function headers for
 * documentation. 
 */

#ifndef ACTORGRAPH_HPP
#define ACTORGRAPH_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
using namespace std; 

class ActorGraph {
private:

    // Vertex class holds information necessary for Dijkstra's algorithm. 
    class Vertex { 
    public: 
        // Initializes vertex with actor name. 
        Vertex(string); 

        // Distance from starting vertex.
        int dist;
        // Flags vertex visited status. 
        bool done;
        // Previous actor and movie to specify which edge was traversed. 
        string prev_actor;
        string prev_movie;
        // Actor name represented by vertex. 
        const string name; 
    };

    // Comparator for Vertex class. Sorts STL containers by min distance being
    // higher priority.
    struct vertexComp { 
        bool operator()(Vertex* x, Vertex* y) { 
            if (x->dist != y->dist) 
                return x->dist > y->dist; 
            return x->prev_actor < y->prev_actor; 
        }
    };

    // Map from actor name to all movies with that actor.
    unordered_map<string, vector<string>> actor_map; 
    // Map from movie name to a pair of movie weight and all actors in movie.
    unordered_map<string, pair<int, vector<string>>> movie_map; 

    // Map from actor name to Vertex holding information for Dijkstra's.
    unordered_map<string, Vertex *> vertices; 

public:

    /*
     * Creates all graph data structures from tab delimited actor, movie
     * relationships. Initializes actor_map, movie_map, and vertices. 
     *
     * Parameters: 
     *  in_filename - 
     *      Tab delimited filename of actor, movie relationships. Header 
     *      expected. Each row is to be separated as actor name, movie name, 
     *      and movie year. 
     *  use_weighted_edges - 
     *      If true, all edge weights will be equal to (2018 - Y) + 1,
     *      prioritizing newer movies in Dijkstra's algorithm. Otherwise, all 
     *      movies will be equally weighted.
     *
     * Returns: 
     *  bool -
     *      True indicates successful reading of file.  
     */
    bool loadFromFile(const char *in_filename, const bool use_weighted_edges);

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
    void findPath(ofstream& out_file, string start_name, string end_name);

    /* 
     * Deallocates vertices
     */
    ~ActorGraph();
};

#endif  // ACTORGRAPH_HPP
