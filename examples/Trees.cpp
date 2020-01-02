#include <iostream>

#include "Graph.h"

// generating some trees up to isomorphism in a staight-forward way
int main() {
    StructSet Trees;
    Graph T(1);
    std::vector<Graph> trees;
    trees.emplace_back(1);

    size_t last = 0;
    size_t next = 1;
    size_t n = 1;

    while (n <= 20) {
        std::cout << n << "  : " << next - last << std::endl;
        
        ++n;
        StructSet list;
        for (int i = last; i < next; ++i) {
            const Graph& T = trees[i];
            Graph S = T + 1;
            for (int i = 0; i + 1 < n; ++i) {
                S.addEdge(i, n - 1);
                S.certify();
                if (!list.contains(S)) {
                    list.add(S);
                    trees.push_back(S);
                }
                S.killEdge(i, n - 1);
            }
        }

        last = next;
        next = trees.size();
    }
    std::cout << n << "  : " << next - last << std::endl;

    return 0;
}
