#include <route.hpp>

void RouteGraph::setMetGrid(map<int, metInformation> grid)   {metGrid = grid;}
//void RouteGraph::addObstruction(obstruction obstruct)           {obstructions.add(obstruct);}

// Here we will construct the routing graph, conisting of nodes and edges
void RouteGraph::constructGraph() {
    // Each node is the intersection of the track
    // An edge is either a via, or a metal track going laterally

    // For each metal layer find layer above and below and construct edges
    map<int, metInformation>::iterator layerIter;
    int metalLayer;

    // Metal information
    metInformation metInfo;
    metInformation metInfoAbove;

    vector<int> intersectionPoints;

    pair<int, int> nodeCoordinate, adjNodeCoordinate;
    int nodeCoordinateX, nodeCoordinateY;

    for (layerIter = metGrid.begin(); layerIter != metGrid.end(); layerIter++) {
        metalLayer = layerIter->first;
        metInfo = layerIter->second;

        if (metInfo.metLayerAbove != 0) metInfoAbove = metGrid[metInfo.metLayerAbove];

        if (metInfo.metLayerAbove != 0) {
            for (int i = 0; i < metInfoAbove.numTracks; i++) {
                intersectionPoints.push_back(metInfoAbove.offset + i * metInfoAbove.increment);
            }
        }
        
        sort(intersectionPoints.begin(), intersectionPoints.end());

        for (int i = 0; i < metInfo.numTracks; i++) {
            for (int j = 0; j < (int) intersectionPoints.size(); j++) {
                // first find out which coordinate it is that we care about
                if (metInfo.orientation == VERTICAL) {
                    nodeCoordinateX = metInfo.offset + i*metInfo.increment;
                    nodeCoordinateY = intersectionPoints[j];
                } else {
                    nodeCoordinateX = intersectionPoints[j];
                    nodeCoordinateY = metInfo.offset + i*metInfo.increment;
                }
                nodeCoordinate.first = nodeCoordinateX;
                nodeCoordinate.second = nodeCoordinateY;

                // if we already added this node, then don't reallocate it
                if (coordinateNodeMap[metalLayer].find(nodeCoordinate) == coordinateNodeMap[metalLayer].end()) {
                    coordinateNodeMap[metalLayer][nodeCoordinate] = new routeNode();
                    coordinateNodeMap[metalLayer][nodeCoordinate]->valid = true;
                }

                // fill in the other fields of the node
                coordinateNodeMap[metalLayer][nodeCoordinate]->metalLayer = metalLayer;
                coordinateNodeMap[metalLayer][nodeCoordinate]->coordinateX = nodeCoordinateX;
                coordinateNodeMap[metalLayer][nodeCoordinate]->coordinateY = nodeCoordinateY;

                if (j != (int) intersectionPoints.size()-1) {
                    // Now create the adjacent nodes on the same plane
                    if (metInfo.orientation == VERTICAL) {
                        // then we only care about the nodes north
                        adjNodeCoordinate.first = metInfo.offset + i*metInfo.increment;
                        adjNodeCoordinate.second = intersectionPoints[j+1];
                    } else {
                        // then we only care about the nodes east
                        adjNodeCoordinate.first = intersectionPoints[j+1];
                        adjNodeCoordinate.second = metInfo.offset + i*metInfo.increment;
                    }

                    // create the adjacent nodes, if it doesn't exist
                    if (coordinateNodeMap[metalLayer].find(adjNodeCoordinate) == coordinateNodeMap[metalLayer].end()) {
                        coordinateNodeMap[metalLayer][adjNodeCoordinate] = new routeNode();
                        coordinateNodeMap[metalLayer][adjNodeCoordinate]->valid = true;
                    }

                    // connect the vertices in the metal plane
                    // connect it both ways
                    if (metInfo.orientation == VERTICAL) {
                        coordinateNodeMap[metalLayer][nodeCoordinate]->nextNodes.north = coordinateNodeMap[metalLayer][adjNodeCoordinate];
                        coordinateNodeMap[metalLayer][adjNodeCoordinate]->nextNodes.south = coordinateNodeMap[metalLayer][nodeCoordinate];
                    } else {
                        coordinateNodeMap[metalLayer][nodeCoordinate]->nextNodes.east = coordinateNodeMap[metalLayer][adjNodeCoordinate];
                        coordinateNodeMap[metalLayer][adjNodeCoordinate]->nextNodes.west = coordinateNodeMap[metalLayer][nodeCoordinate];
                    }
                }

                if (metInfo.metLayerAbove != 0) {
                    // now do the same for the via plane
                    if (coordinateNodeMap[metalLayer+1].find(nodeCoordinate) == coordinateNodeMap[metalLayer+1].end()) {
                        coordinateNodeMap[metalLayer+1][nodeCoordinate] = new routeNode();
                        coordinateNodeMap[metalLayer+1][nodeCoordinate]->valid = true;
                    }

                    // connect the vertices on the via plane
                    coordinateNodeMap[metalLayer][nodeCoordinate]->nextNodes.up = coordinateNodeMap[metalLayer+1][nodeCoordinate];
                    coordinateNodeMap[metalLayer+1][nodeCoordinate]->nextNodes.down = coordinateNodeMap[metalLayer][nodeCoordinate];
                }

            }
        }  
    }
}

// This will be my suite of testing functions
void RouteGraph::outNodeCoordinate(ostream& output, routeNode* nodePtr) {
    if (nodePtr != nullptr)
        output << "Node Coordinate: (" << nodePtr->coordinateX << ", " << nodePtr->coordinateY << "), Metal Layer: " << nodePtr->metalLayer << endl;
    else output << "NULL" << endl;
}

void RouteGraph::outNodeDetails(ostream& output, routeNode* nodePtr) {
    outNodeCoordinate(output, nodePtr);
    output << "Adjacent Nodes:\n\t";
    output << "Above:\n\t\t";
    outNodeCoordinate(output, nodePtr->nextNodes.up);
    output << "\tBelow:\n\t\t";
    outNodeCoordinate(output, nodePtr->nextNodes.down);
    output << "\tNorth:\n\t\t";
    outNodeCoordinate(output, nodePtr->nextNodes.north);
    output << "\tEast:\n\t\t";
    outNodeCoordinate(output, nodePtr->nextNodes.east);
    output << "\tSouth:\n\t\t";
    outNodeCoordinate(output, nodePtr->nextNodes.south);
    output << "\tWest:\n\t\t";
    outNodeCoordinate(output, nodePtr->nextNodes.west);
}

void RouteGraph::outMetLayerNodes(ostream& output, int metLayer) {
    for (map<pair<int,int>, routeNode*>::iterator it = coordinateNodeMap[metLayer].begin(); 
            it != coordinateNodeMap[metLayer].end();
            it++) 
    {
        outNodeDetails(output, it->second);
    }
}