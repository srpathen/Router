#include <route.hpp>

struct intersectionPoint {
    int coordinate;
    bool above;
    bool below;
};

void RouteGraph::setMetGrid(map<int, metInformation> grid, met<int, viaInfo> vias, int inpMinMetalLayer, int inpMaxMetalLayer) {
    metGrid = grid;
    minMetalLayer = inpMinMetalLayer;
    maxMetalLayer = inpMaxMetalLayer;
    viaInfo = inpViaInfo;
}

void RouteGraph::addMetObstruction(pair <int, int> botLeftCorner, int width, int height, int metLayer) {
    obstructions.push_back((obstruction) {botLeftCorner, width, height, metLayer});
}

bool checkIfRectOverlap(pair<int, int> botLeftCorner1, int width1, int height1,
    pair<int, int> botLeftCorner2, int width2, int height2) {

    int rect1upperboundx = botLeftCorner1.first + width1;
    int rect1lowerboundx = botLeftCorner1.first;
    int rect1upperboundy = botLeftCorner1.second + height1;
    int rect1lowerboundy = botLeftCorner1.second;

    int rect2upperboundx = botLeftCorner2.first + width2;
    int rect2lowerboundx = botLeftCorner2.first;
    int rect2upperboundy = botLeftCorner2.second + height2;
    int rect2lowerboundy = botLeftCorner2.second;

    // y bounds overlap
    bool yBoundOverlap = ((rect2lowerboundy < rect1lowerboundy) && (rect1lowerboundy < rect2upperboundy)) || 
    ((rect2lowerboundy < rect1upperboundy) && (rect1upperboundy < rect2upperboundy));

    // x bounds overlap
    bool xBoundOverlap = ((rect2lowerboundx < rect1lowerboundx) && (rect1lowerboundx < rect2upperboundx)) || 
    ((rect2lowerboundx < rect1upperboundx) && (rect1upperboundx < rect2upperboundx));

    return yBoundOverlap && xBoundOverlap;
}

void RouteGraph::constructGraph() {
    //bool botMetLayer = false;
    bool topMetLayer = false;
    bool botMetLayer = false;

    vector<int> intersectionPoints;
    vector<int> intersectionPointsUp;
    vector<int> intersectionPointsDown;

    metInformation metInfo;
    metInformation metInfoAbove;
    metInformation metInfoBelow;

    pair<int, int> coordinate;
    pair<int, int> adjCoordinate;

    for (int layer = minMetalLayer; layer <= maxMetalLayer; layer++) {
        //if (layer == minMetalLayer) botMetLayer = true;
        if (layer == maxMetalLayer) topMetLayer = true;
        else                        botMetLayer = false;
        if (layer == minMetalLayer) botMetLayer = true;
        else                        botMetLayer = false;

        metInfo = metGrid[layer];
        
        intersectionPointsUp.clear();intersectionPointsDown.clear();intersectionPoints.clear();

        if (!topMetLayer) {
            metInfoAbove = metGrid[layer+1];
            for (int trackNum = 0; trackNum < metInfoAbove.numTracks; trackNum++) {
                intersectionPointsUp.push_back(metInfoAbove.offset + trackNum * (metInfoAbove.width + metInfoAbove.increment) + metInfoAbove.width/2);
            }
        }
        if (!botMetLayer) {
            metInfoBelow = metGrid[layer-1];
            for (int trackNum = 0; trackNum < metInfoBelow.numTracks; trackNum++) {
                intersectionPointsDown.push_back(metInfoBelow.offset + trackNum * (metInfoBelow.width + metInfoBelow.increment)+ metInfoBelow.width/2);
            }
        }

        intersectionPoints.insert(intersectionPoints.end(), intersectionPointsUp.begin(), intersectionPointsUp.end());
        intersectionPoints.insert(intersectionPoints.end(), intersectionPointsDown.begin(), intersectionPointsDown.end());
        sort(intersectionPoints.begin(), intersectionPoints.end());
        intersectionPoints.erase( unique(intersectionPoints.begin(),intersectionPoints.end()),intersectionPoints.end());

        
        for (int inter = 0; inter < (int) intersectionPoints.size(); inter++) {
            for (int trackNum = 0; trackNum < metInfo.numTracks; trackNum++) {
                if (metInfo.orientation == VERTICAL) {
                    coordinate.first = metInfo.offset + trackNum * (metInfo.width + metInfo.increment) + metInfo.width/2;
                    coordinate.second = intersectionPoints[inter];
                } else {
                    coordinate.first = intersectionPoints[inter];
                    coordinate.second = metInfo.offset + trackNum * (metInfo.width + metInfo.increment) + metInfo.width/2;
                }

                if (coordinateNodeMap[layer].find(coordinate) == coordinateNodeMap[layer].end())
                    coordinateNodeMap[layer][coordinate] = new routeNode();

                adjCoordinate = coordinate;
                coordinateNodeMap[layer][coordinate]->coordinate = coordinate;
                coordinateNodeMap[layer][coordinate]->metLayer = layer;

                if (inter != (int) intersectionPoints.size() - 1) {
                    if (metInfo.orientation == VERTICAL)    adjCoordinate.second = intersectionPoints[inter+1];
                    else                                    adjCoordinate.first = intersectionPoints[inter+1];

                    if (coordinateNodeMap[layer].find(adjCoordinate) == coordinateNodeMap[layer].end())
                        coordinateNodeMap[layer][adjCoordinate] = new routeNode();
                    
                    if (metInfo.orientation == VERTICAL) {
                        coordinateNodeMap[layer][coordinate]->nextNodes.north = coordinateNodeMap[layer][adjCoordinate];
                        coordinateNodeMap[layer][adjCoordinate]->nextNodes.south = coordinateNodeMap[layer][coordinate];
                    } else {
                        coordinateNodeMap[layer][coordinate]->nextNodes.east = coordinateNodeMap[layer][adjCoordinate];
                        coordinateNodeMap[layer][adjCoordinate]->nextNodes.west = coordinateNodeMap[layer][coordinate];
                    }
                }
            }
        }

        if (!topMetLayer) {
            for (int inter = 0; inter < (int) intersectionPointsUp.size(); inter++) {
                for (int trackNum = 0; trackNum < metInfo.numTracks; trackNum++) {
                    if (metInfo.orientation == VERTICAL) {
                        coordinate.first = metInfo.offset + trackNum * (metInfo.width + metInfo.increment) + metInfo.width/2;
                        coordinate.second = intersectionPointsUp[inter];
                    } else {
                        coordinate.first = intersectionPointsUp[inter];
                        coordinate.second = metInfo.offset + trackNum * (metInfo.width + metInfo.increment) + metInfo.width/2;
                    }
                    coordinateNodeMap[layer+1][coordinate] = new routeNode();
                    coordinateNodeMap[layer][coordinate]->nextNodes.up = coordinateNodeMap[layer+1][coordinate];
                    coordinateNodeMap[layer+1][coordinate]->nextNodes.down = coordinateNodeMap[layer][coordinate];
                }
            }
        }
    }
}

typedef enum {UP, DOWN, NORTH, SOUTH, EAST, WEST} direction;

void RouteGraph::removeEdge(pair<int, int> coordinate, int metLayer, direction dir) {
    routeNode* nodePtr = coordinateNodeMap[metLayer][coordinate];
    switch (dir) {
        case UP:
            if (nodePtr->nextNodes.up != nullptr) nodePtr->nextNodes.up->down = nullptr;
            break;
        case DOWN:
            if (nodePtr->nextNodes.down != nullptr) nodePtr->nextNodes.down->up = nullptr;
            break;
        case NORTH:
            if (nodePtr->nextNodes.north != nullptr) nodePtr->nextNodes.north->south = nullptr;
            break;
        case SOUTH:
            if (nodePtr->nextNodes.south != nullptr) nodePtr->nextNodes.south->north = nullptr;
            break;
        case EAST:
            if (nodePtr->nextNodes.east != nullptr) nodePtr->nextNodes.east->west = nullptr;
            break;
        case WEST:
            if (nodePtr->nextNodes.west != nullptr) nodePtr->nextNodes.west->east = nullptr;
            break;
        default:
            break;
    }
}

void RouteGraph::removeNode(pair<int, int> coordinate, int metLayer) {
    routeNode* nodePtr = coordinateNodeMap[metLayer][coordinate];

    if (nodePtr->nextNodes.up != nullptr) nodePtr->nextNodes.up->down = nullptr;
    if (nodePtr->nextNodes.down != nullptr) nodePtr->nextNodes.down->up = nullptr;
    if (nodePtr->nextNodes.north != nullptr) nodePtr->nextNodes.north->south = nullptr;
    if (nodePtr->nextNodes.south != nullptr) nodePtr->nextNodes.south->north = nullptr;
    if (nodePtr->nextNodes.east != nullptr) nodePtr->nextNodes.east->weat = nullptr;
    if (nodePtr->nextNodes.west != nullptr) nodePtr->nextNodes.west->east = nullptr;

    delete nodePtr;
    coordinateNodeMap[metLayer].erase(coordinate); 
}

void RouteGraph::refactorGridObstr() {
    int minSpacing;
    metInformation metInfo;

    pair<int, int> botLeft, topLeft, botRight, topRight;
    int trackStart, trackEnd, startEdge, endEdge;
    bool startEdgeIntersect, endEdgeIntersect;

    int trackCoord;
    routeNode* trackNode, trackNodeForward, trackNodeBackward;

    bool bot, top;

    // the rect information about each node for the via goign up and the via going down
    pair<int, int> nodeCoordUp, nodeCoordDown;
    int nodeHeightUp, nodeHeightDown;
    int nodeWidthUp, nodeHeightUp;

    viaInformation viaInfoBot, viaInfoTop;

    for (auto & obst: obstructions) {
        metInfo = metGrid[obst.metLayer]
        minSpacing = metInfo.minSpacing;

        top = obst.metLayer == maxMetalLayer;
        bot = obst.metLayer == minMetalLayer;
        up = (top) ? viaInfo[obst.metLayer] : NULL;
        down = (bot) ? viaInfo[obst.metLayer-1] : NULL;

        viaInfoTop = (top) ? NULL : viaInfo[obst.metLayer];
        viaInfoBot = (bot) ? NULL : viaInfo[obst.metLayer-1];

        for (int track = 0; track < metInfo.numTracks; track++) {
            trackStart = metInfo.offset + track * (metInfo.width + metInfo.increment);
            trackEnd = metInfo.offset + metInfo.width + track * (metInfo.width + metInfo.increment);

            startEdgeIntersect = (trackStart > startEdge) && (trackStart < endEdge);
            endEdgeIntersect = (trackEnd > startEdge) && (trackEnd < endEdge);

            // Look for node on the track
            for (auto & pair : coordinateNodeMap[obst.metLayer]) {
                trackCoord = (metInfo.orientation == VERTICAL) ? pair.first.first : pair.first.second;
                if (trackCoord == trackStart) {
                    trackNode = pair.second;
                    break;
                }
            }
            // Use this node and look for nodes to see where the intersection is
            trackNodeForward = trackNode;
            while (trackNodeForward != nullptr) {
                // check if the via here intersect with this obstruction
                if (!top) {
                    nodeCoordUp.first = trackNodeForward->coordinate.first - viaInfoTop.dimension.first/2 - viaInfoTop.botMetEnclosure.west;
                    nodeCoordUp.second = trackNodeForward->coordinate.second - viaInfoTop.dimension.second/2 - viaInfoTop.botMetEnclosure.south;
                    nodeHeightUp = viaInfoTop.botMetEnclosure.south + viaInfoTop.dimension.second + viaInfoTop.botMetEnclosure.north;
                    nodeWidthDown = viaInfoTop.botMetEnclosure.west + viaInfoTop.dimension.first + viaInfoTop.botMetEnclosure.east;
                    if (checkIfRectOverlap(nodeCoordUp, nodeWidthUp, nodeHeightUp)) {
                        
                    }
                }
                trackNodeForward = (metInfo.orientation == VERTICAL) ? trackNodeForward->nextNodes.north : trackNodeForward->nextNodes.east;
            }
            // start iterating downwards
            trackNodeBackward = trackNode;
            while (trackNodeBackward != nullptr) {
                trackNodeBackward = (metInfo.orientation == VERTICAL) ? trackNodeBackward->nextNodes.south : trackNodeBackward->nextNodes.west;
            }

        }
    }
}


#ifdef TEST
// This will be my suite of testing functions
void RouteGraph::outNodeCoordinate(ostream& output, routeNode* nodePtr) {
    if (nodePtr != nullptr)
        output << "(" << nodePtr->coordinate.first << "," << nodePtr->coordinate.second << "," << nodePtr->metLayer << ")";
    else output << "NULL";
}

void RouteGraph::outNodeDetails(ostream& output, routeNode* nodePtr) {
    output << "node ";
    outNodeCoordinate(output, nodePtr);
    output << " ";
    outNodeCoordinate(output, nodePtr->nextNodes.up);
    output << " ";
    outNodeCoordinate(output, nodePtr->nextNodes.down);
    output << " ";
    outNodeCoordinate(output, nodePtr->nextNodes.north);
    output << " ";
    outNodeCoordinate(output, nodePtr->nextNodes.east);
    output << " ";
    outNodeCoordinate(output, nodePtr->nextNodes.south);
    output << " ";
    outNodeCoordinate(output, nodePtr->nextNodes.west);
    output << endl;
}

void RouteGraph::outMetLayerNodes(ostream& output, int metLayer) {
    for (map<pair<int,int>, routeNode*>::iterator it = coordinateNodeMap[metLayer].begin(); 
            it != coordinateNodeMap[metLayer].end();
            it++) 
    {
        outNodeDetails(output, it->second);
    }
}

void RouteGraph::outMetLayerInfo(ostream& output, int metLayer) {
    output << "metinfo " << metLayer << " "
        << (metGrid[metLayer].orientation ? "HORIZONTAL" : "VERTICAL") << " " 
        << metGrid[metLayer].offset << " "
        << metGrid[metLayer].increment << " "
        << metGrid[metLayer].width << " "
        << metGrid[metLayer].numTracks << " " 
        << metGrid[metLayer].minSpacing << endl;
    outMetLayerNodes(output, metLayer);
}

void RouteGraph::outGridInfo(ostream& output) {
    for (int layer = minMetalLayer; layer <= maxMetalLayer; layer++)
        outMetLayerInfo(output, layer);
    printObstructions(output);
}

void RouteGraph::printObstructions(ostream& output) {
    for (auto & obst : obstructions) {
        output << "obstruction (" << obst.coordinate.first << "," << obst.coordinate.second << ") ";
        output << obst.width << " ";
        output << obst.height << " ";
        output << obst.metLayer << endl;
    }
}
#endif