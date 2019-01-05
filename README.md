# Actor Network Analysis 
This repo contains programs which finds paths between, predicts future
interactions and recommends new collaborations for, and finds popular groups of
actors using the movies they've acted in. Files associated with each 
program are detailed beneath. 

### Path Finder
```bash
make pathfinder
./pathfinder data/data.tsv u/w data/pathfinder_pairs out
```
**The pathfinder program finds a path between actors through mutual movies utilizing Dijkstra's algorithm.**

The program parses a *data.tsv* file containing 
columns of actor, movie, and year relationships. It builds and traverses a graph
using Dijkstra's algorithm for each actor pair in *pathfinder_pairs*, finding a 
path between actors through mutual movies, and writing this path to *out*. The 
*u* or *w* argument signifies unweighted or weighted graph traversal,
assigning lower weights to newer movies (prioritizing them in Dijkstra's). 

### Interaction Predictor and Collaboration Recommender
```bash
make predictorandrecommender
./predictorandrecommender data/data.tsv data/pred_rec_targets out_pred out_rec
```
**The predictorandrecommender program predicts future interactions between and recommends new collaborations for actors given their history of interactions.**

Weight between two actors is assigned based on the number of mutual
actor connections they have, where a connection between actors is made by 
sharing a movie. The top predicted interactions of an actor are based upon
highest weights for their existing connections, while the top recommended 
collaborations are based upon highest weights for actors they're not connected to. 
Actor connections are built from *data.tsv*. Suggestions for actors in
*pred_rec_targets* are written to *out_pred* and *out_rec*. The program utilizes
adjacency matrices and bidirectional maps for graph representation and querying.

### Popularity Finder 
```bash
make popularityfinder
./popularityfinder data/data.tsv k pop_actors
```
**The popularityfinder program performs k-core graph decomposition to find actors who have at least k other connections, with actors that also have above k connections.**

The resulting list of actors constructed and written to *pop_actors* each know
at least *k* other actors within that list. This means that actors with under k
connections do not count as a connection for any actors on the list - the
resulting list represents dense subgraphs of influential nodes within the actor
network. Connections between actors are found through mutual movies, built
through the data in *data.tsv*. 


