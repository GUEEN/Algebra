// build and write to disk all Ramsey R(G,k) graphs 
// by the one-vertex extension algorithm.
// Here we assume that G is K_3 or C_4 only.

#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

#include "Graph.h"

class ConeGenerator {
public:
    ConeGenerator(const Graph& H) : H(H), n(H.size()) {
    }

    std::vector<std::vector<size_t>> getConesC4(int deg) {
        d = deg;
        cones.clear();
        cone.assign(d, 0);
        if (d == 0) {
            return cones;
        }

        F.resize(H.size());

        for (size_t i = 0; i < n; i++) {
            for (size_t j = i + 1; j < n; j++) {
                for (size_t k = 0; k < n; k++) {
                    if (H.edge(i, k) && H.edge(k, j)) {
                        F.addEdge(i,j);
                    }
                }
            }
        }
		
        for (size_t i = 0; i + d <= n; i++) {
            cone[0] = i;
            next(1);
        }
        return std::move(cones);
    }

private:
    void next(size_t l) {
        if (l == d) {
            cones.push_back(cone);
            return;
        }

        for (size_t i = cone[l - 1] + 1; i + d <= n + l; i++) {
            bool B = true;	
            // trianglefree check		
            for (size_t j = 0; j < l; j++ ) {
                if (F.edge(i, cone[j])) {
                    B = false;
                    break;
                }
            }

            // K4-free check
            if (!B) {
                continue;
            }

            cone[l] = i;
            next(l + 1);
        }
    }

    const Graph& H;
    int d;
    int n;
    std::vector<size_t> cone;
    std::vector<std::vector<size_t>> cones;
    Graph F;
};


int main(int argc, char** argv) {
    // producing all R(3,k)-graphs 
    // producing all R(C4,k)-graphs
    if (argc != 3) {
        std::cout << "Wrong number of arguments. We expact graph name G and positive integer n to compute set R(G, n)" << std::endl;
        return 1;
    }

    std::string graph_name = argv[1];
    size_t k = std::atoi(argv[2]);

    if (graph_name != "K3" && graph_name != "C4") {
        std::cout << "Wrong graph name. We expect G to be K3 or C4 so far" << std::endl;
        return 1;       
    }
    if (graph_name == "K3") {
        graph_name = "3";
    }
   
    mkdir("../data/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("../data/RAMSEY/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    const std::string address = "../data/RAMSEY/R(" + graph_name + "," + std::to_string(k) + ")/";
    mkdir(address.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    // we start by writing the only Ramsey graph on 1 vertex to the corresponding file
    GraphSet one(1);
    one.insert(Graph(1).certify());
    std::string filename = address + "R(" + graph_name + "," + std::to_string(k) +";1,0,0).gr";
    one.write(filename);

    // count of all R(G, n)-graphs
    size_t all = 1;
    // counts of all R(G, n)-graphs by number of vertices
    std::vector<size_t> qv = {0, 1};
    // counts of all R(G, n)-graphs by number of vertices and edges
    std::vector<std::vector<size_t>> qve = { {}, {1} };
	
    // proceed to construct larger Ramsey graphs from smaller ones
    for (int n = 2;; n++) {
        size_t q = 0;
        std::vector<size_t> qe;
        for (int e = 0; e <= n * (n - 1) / 2; e++) {
            size_t qqe = 0;
            for (int d = 0; d <= n && d <= e; d++) {
		GraphSet graphs(n);
                for (int dd = std::max(d - 1, 0); dd <= n - 1; dd++) {
                    std::string filename = address + "R(" + graph_name + "," + std::to_string(k) + ";" +
                                           std::to_string(n - 1) + "," + std::to_string(e - d) + "," + std::to_string(dd) + ").gr";
                    std::fstream stream;
                    stream.open(filename, std::ios::in | std::ios::binary);
                    if (!stream.good()) {
                        continue;
                    }
                        
                    Graph H(n - 1);
                    while (readGraph(stream, H)) {
                        Graph G = H + 1;
                        if (d > 0) {
                            ConeGenerator cg(H);
                            auto cones = cg.getConesC4(d);
                            for (const auto& cone : cones) {
                                for (int j = 0; j < d; j++) {
                                    G.addEdge(cone[j], n - 1);
                                }
                                if (G.deg() == d) {
                                    if (!G.subClique(k)) {
                                        graphs.insert(G.certify());
                                    }
                                }
                                for (size_t j = 0; j < d; j++) {
                                    G.killEdge(cone[j], n - 1);
                                }
                            }
                        } else {
                            if (G.subClique(k)) {
                                continue;
                            }
                            graphs.insert(G.certify());
                        }
                    }
                }
                if (graphs.size()) {
                    q += graphs.size();
                    qqe += graphs.size();
                    all += graphs.size();

                    std::string path = address + "R(" + graph_name + "," + std::to_string(k) + ";" + std::to_string(n) 
                                     + "," + std::to_string(e) + "," + std::to_string(d) + ").gr";
                    graphs.write(path);
                }
            }
            qe.push_back(qqe);
        }
        if (q == 0) {
            break;
        }
        qv.push_back(q);
        qve.push_back(std::move(qe));

        std::cout << n << " --- " << qv[n] << std::endl;
    }

    // writing output

    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << all << std::endl;

    return 0;
}
