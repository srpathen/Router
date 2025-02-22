#include <route.hpp>

int main() {
    map<int, metInformation> metGrid;
    metGrid[1] = (metInformation) {
        VERTICAL,   //orientation
        1,          // offset
        6,          // increment
        2,          // width
        5,          // numTracks
        1           // minSpacing
    };
    metGrid[2] = (metInformation) {
        HORIZONTAL,   //orientation
        1,          // offset
        6,          // increment
        2,          // width
        5,          // numTracks
        1           // minSpacing
    };
    metGrid[3] = (metInformation) {
        VERTICAL,   //orientation
        1,          // offset
        6,          // increment
        2,          // width
        5,          // numTracks
        1           // minSpacing
    };


    RouteGraph graph;
    graph.setMetGrid(metGrid,1,3);

    graph.constructGraph();
    graph.addMetObstruction({4,4}, 10, 1, 1);

    graph.outGridInfo(cout);

}