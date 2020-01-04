// build and write to disk all Ramsey R(G,k) graphs 
// by the one-vertex extension algorithm.
// Here we assume that G is K_3 or C_4 only.

#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

#include "Graph.h"

std::vector<size_t> clique;
std::list<std::vector<size_t>> cliques;

int n;
int d;

Graph G1;

void next(size_t l) {
    if (l == d) {
        cliques.push_back(clique);
        return;
    }

    for (size_t i = clique[l - 1] + 1; i + d <= G1.size() + l; i++) {
        bool B = true;	
        // trianglefree check		
        for (size_t j = 0; j < l; j++ ) {
            if (G1.edge(i, clique[j])) {
                B = false;
                break;
            }
        }

        // K4-free check
        if (!B) {
            continue;
        }

        clique[l] = i;
        next(l + 1);
    }
}

void getCliquesC4(const Graph& H) {
    clique.assign(d, 0);
    if (d == 0) {
        return;
    }

    G1.resize(H.size());

    for (size_t i = 0; i < H.size(); i++) {
        for (size_t j = i + 1; j < H.size(); j++) {
            for (size_t k = 0; k < H.size(); k++) {
                if (H.edge(i, k) && H.edge(k, j)) {
                    G1.addEdge(i,j);
                }
            }
        }
    }
		
    for (size_t i = 0; i + d <= G1.size(); i++) {
        clique[0] = i;
        next(1);
    }
}


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

    // count for all R(G, n)-graphs
    size_t all = 1;
    // counts for all R(G, n)-graphs by number of vertices
    std::vector<size_t> qv = {0, 1};
    std::vector<std::vector<size_t>> qve = { {}, {1} };
	
    // proceed to construct larger Ramsey graphs from smaller ones
    for (n = 2;; n++) {
        size_t q = 0;
        std::vector<size_t> qe;
        for (size_t e = 0; e <= n * (n - 1) / 2; e++) {
            for (d = 0; d <= n && d <= e; d++) {
		GraphSet graphs(n);
                for (int dd = d - 1; dd <= n - 1; dd++) {
                    if (dd == -1) {
                        continue;
                    }

                    std::string filename = address + "R(" + graph_name + "," + std::to_string(k) + ";" +
                                           std::to_string(n - 1) + "," + std::to_string(e - d) + "," + std::to_string(dd) + ").gr";
                    std::fstream stream;
                    stream.open(filename, std::ios::in | std::ios::binary);

                    if (stream.good()) {
                        Graph H(n - 1);
                        while (readGraph(stream, H)) {
                            Graph G = H + 1;
                            if (d > 0) {
                                cliques.clear();
                                getCliquesC4(H);

                                for (const std::vector<size_t>& clique : cliques) {
                                    for (size_t j = 0; j < d; j++) {
                                        G.addEdge(clique[j], n - 1);
                                    }
                                    if (G.deg() == d) {
                                        if (!G.subClique(k)) {
                                            graphs.insert(G.certify());
                                        }
                                    }
                                    for (size_t j = 0; j < d; j++) {
                                         G.killEdge(clique[j], n - 1);
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
                    stream.close();
                }
                if (graphs.size()) {
                    q += graphs.size();
                    all += graphs.size();

                    std::string path = address + "R(" + graph_name + "," + std::to_string(k) + ";" + std::to_string(n) + "," + std::to_string(e) + "," + std::to_string(d) + ").gr";
                    graphs.write(path);
                }
            }
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
