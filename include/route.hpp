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
struct metInformation {
    trackOrientation orientation;   // the orientation of the track

    int offset;                     // the offset from bottom or left
    int increment;                  // the increment between each track
    int width;                      // the minumum width of the track TODO: maybe add VIA enclosure rules

    int numTracks;                  // the number for tracks for each metal layer
    
    int minSpacing;                 // min spacing between two metal rectangles

    //enclosureRules nodeEnclosure;   // enclosure rules for the nodes within the track
};

struct viaInformation {
    enclosureRules botMetEnclosure;
    enclosureRules topMetEnclosure;
    pair<int, int> dimension;
}

struct enclosureRules {
    int north;
    int south;
    int east;
    int west;
};

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
    pair<int, int> coordinate;
    int metLayer;
    adjacentNodes nextNodes;
}; 

struct obstruction {
    pair<int, int> coordinate;
    int width;
    int height;
    int metLayer;
};

class RouteGraph {
public:
    map<int, metInformation> metGrid;
    map<int, viaInformation> viaInfo;
    int minMetalLayer, maxMetalLayer;
    vector<obstruction> obstructions;
    map<int, map<pair<int, int>, routeNode*>> coordinateNodeMap;

    void setMetGrid(map<int, metInformation> grid, met<int, viaInfo> vias, int inpMinMetalLayer, int inpMaxMetalLayer);
    void addMetObstruction(pair <int, int> botLeftCorner, int width, int height, int metLayer);
    void constructGraph();
    
    #ifdef TEST
    void outNodeCoordinate(ostream& output, routeNode* nodePtr);
    void outNodeDetails(ostream& output, routeNode* nodePtr);
    void outMetLayerNodes(ostream& output, int metLayer);
    void outMetLayerInfo(ostream& output, int metLayer);
    void outGridInfo(ostream& output);
    void printObstructions(ostream& output);
    #endif
};

#endif