#ifndef ROUTE_H
#define ROUTE_H

#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

typedef enum {VERTICAL, HORIZONTAL} trackOrientation;


// Here are some unenforced rules about track information
// adjacent metal layers MUST have different orientations
// all vertical/horizontal layers DON'T need to have the same offset, increment or minWidth
typedef struct {
    trackOrientation orientation;   // the orientation of the track

    int offset;                     // the offset from bottom or left
    int increment;                  // the increment between each track
    int minWidth;                   // the minumum width of the track TODO: maybe add VIA enclosure rules

    int numTracks;                  // the number for tracks for each metal layer

    int metLayerAbove;              // the metal layer above (if none, or don't want to use 0)
} metInformation;

struct routeNode;

// These are all the possible adjacent nodes
// If there is no such adjacent node, then we make it NULL
struct adjacentNodes {
    // via routing
    routeNode* up;
    routeNode* down;

    // planar routing
    routeNode* north;
    routeNode* east;
    routeNode* south;
    routeNode* west;
};

struct routeNode {
    bool valid;
    int coordinateX;
    int coordinateY;
    int metalLayer;
    adjacentNodes nextNodes;
} ; 

class RouteGraph {
public:
    map<int, metInformation> metGrid;
    //vector<obstruction> obstructions;
    //vector<metObstruction> metObstructions;
    map<int, map<pair<int, int>, routeNode*>> coordinateNodeMap;

    void setMetGrid(map<int, metInformation> grid);
    //void addObstruction(obstruction obstruct);
    void constructGraph();
    
    void outNodeCoordinate(ostream& output, routeNode* nodePtr);
    void outNodeDetails(ostream& output, routeNode* nodePtr);
    void outMetLayerNodes(ostream& output, int metLayer);
};

#endif