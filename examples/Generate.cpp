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
        for (size_t index = last; index < next; ++index) {
            const Graph& T = trees[index];
            std::vector<size_t> degrees = T.getDegrees();
            size_t d = n;
            for (size_t i = 0; i < T.size(); ++i) {
                if (degrees[i] > 1) {
                    continue;
                }
                size_t j = 0;
                while (T.edge(i, j) == false) {
                    ++j;
                }
                d = std::min(d, degrees[j]);
            }

            Graph S = T + 1;
            for (size_t i = 0; i + 1 < n; ++i) {
                if (degrees[i] > d + 1) {
                    continue;
                }

                S.addEdge(i, n - 1);
                S.certify();
                if (!list.contains(S)) {
                    list.insert(S);
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
