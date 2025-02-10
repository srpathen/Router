#include <route.hpp>

int main() {
    map<int, metInformation> metGrid;
    metGrid[1] = (metInformation) {VERTICAL, 1, 1, 1, 3, 2};
    metGrid[2] = (metInformation) {HORIZONTAL, 1, 1, 1 , 3, 0};

    RouteGraph graph;
    graph.setMetGrid(metGrid);

    graph.constructGraph();

    cout << "Nodes on Metal Layer 1" << endl;
    graph.outMetLayerNodes(cout, 1);
    cout << "Nodes on Metal Layer 2" << endl;
    graph.outMetLayerNodes(cout, 2);

}