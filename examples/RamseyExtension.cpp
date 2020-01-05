// build and write to disk all Ramsey R(G,k) graphs 
// by the one-vertex extension algorithm.
// Here we assume that G is K_3, K_4, C_4 or C_5 only.
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

#include "Graph.h"

const std::vector<std::string> names = {"K3", "K4", "C3", "C4", "C5", "3", "4"};

class ConeGenerator {
public:
    ConeGenerator(const Graph& H) : H(H), n(H.size()) {
    }

    std::vector<std::vector<size_t>> getConesK3(size_t deg) {
        d = deg;
        cones.clear();
        cone.assign(d, 0);
        if (d == 0) {
            return cones;
        }

        F = H;		
        for (size_t i = 0; i + d <= n; i++) {
            cone[0] = i;
            next3(1);
        }
        return std::move(cones);
    }

    std::vector<std::vector<size_t>> getConesK4(size_t deg) {
        d = deg;
        cones.clear();
        cone.assign(d, 0);
        if (d == 0) {
            return cones;
        }

        F = H;		
        for (size_t i = 0; i + d <= n; i++) {
            cone[0] = i;
            next4(1);
        }
        return std::move(cones);
    }

    std::vector<std::vector<size_t>> getConesC4(size_t deg) {
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
                        F.addEdge(i, j);
                    }
                }
            }
        }
		
        for (size_t i = 0; i + d <= n; i++) {
            cone[0] = i;
            next3(1);
        }
        return std::move(cones);
    }

    std::vector<std::vector<size_t>> getConesC5(size_t deg) {
        d = deg;
        cones.clear();
        cone.assign(d, 0);
        if (d == 0) {
            return cones;
        }

        F.resize(H.size());
        // looking for path i -- k -- l -- j
        for (size_t i = 0; i < n; i++) {
            for (size_t j = i + 1; j < n; j++) {
                for (size_t k = 0; k < n; k++) {
                    if (!H.edge(i, k) || k == j) {
                        continue;
                    }
                    for (size_t l = 0; l < n; l++) {
                        if (H.edge(k, l) && H.edge(l, j) && l != i) {
                            F.addEdge(i, j);
                        }
                    }
                }
            }
        }
		
        for (size_t i = 0; i + d <= n; i++) {
            cone[0] = i;
            next3(1);
        }
        return std::move(cones);
    }

private:
    void next3(size_t l) {
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

            if (!B) {
                continue;
            }

            cone[l] = i;
            next3(l + 1);
        }
    }

    void next4(size_t l) {
        if (l == d) {
            cones.push_back(cone);
            return;
        }

        for (size_t i = cone[l - 1] + 1; i + d <= n + l; i++) {
            bool B = true;	
            // K4-free check		
            for (size_t j = 0; j < l; j++ ) {
                for (size_t k = j + 1; k < l; ++k) {
                    if (F.edge(i, cone[j]) && F.edge(i, cone[k]) && F.edge(cone[j], cone[k])) {
                        B = false;
                        break;
                    }
                }
                if (!B) {
                    break;
                }
            }

            if (!B) {
                continue;
            }

            cone[l] = i;
            next4(l + 1);
        }
    }

    const Graph& H;
    size_t d;
    size_t n;
    std::vector<size_t> cone;
    std::vector<std::vector<size_t>> cones;
    Graph F;
};


int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Wrong number of arguments. We expact graph name G and positive integer n to compute set R(G, n)" << std::endl;
        return 1;
    }

    std::string graph_name = argv[1];
    size_t k = std::atoi(argv[2]);

    if (std::find(names.begin(), names.end(), graph_name) == names.end()) {
        std::cout << "Wrong graph name. We expect G to be K3, K4, C4 or C5 so far" << std::endl;
        return 1;       
    }
    if (graph_name == "K3" || graph_name == "C3") {
        graph_name = "3";
    }
    if (graph_name == "K4") {
        graph_name = "4";
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
    // counts of all R(G, n)-graphs by number of edges
    std::vector<size_t> qe = {1};
    // counts of all R(G, n)-graphs by number of vertices and edges
    std::vector<std::vector<size_t>> qve = { {}, {1} };
	
    // proceed to construct larger Ramsey graphs from smaller ones
    for (int n = 2;; n++) {
        size_t q = 0;
        std::vector<size_t> ve;
        for (int e = 0; e <= n * (n - 1) / 2; e++) {
            size_t qed = 0;
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
                            std::vector<std::vector<size_t>> cones;
                            if (graph_name == "C4") {
                                cones = cg.getConesC4(d);
                            } else if (graph_name == "C5") {
                                cones = cg.getConesC5(d);
                            } else if (graph_name == "4"){
                                cones = cg.getConesK4(d);
                            } else {
                                cones = cg.getConesK3(d);
                            }
                            for (const auto& cone : cones) {
                                for (size_t j = 0; j < d; j++) {
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
                    if (e == qe.size()) {
                        qe.push_back(0);
                    }

                    qe[e] += graphs.size();
                    qed += graphs.size();

                    std::string path = address + "R(" + graph_name + "," + std::to_string(k) + ";" + std::to_string(n) 
                                     + "," + std::to_string(e) + "," + std::to_string(d) + ").gr";
                    graphs.write(path);
                }
            }
            ve.push_back(qed);
            q += qed;
        }
        if (q == 0) {
            break;
        }
        qv.push_back(q);
        all += q;
        qve.push_back(std::move(ve));
    }

    // writing output
    auto lspace = [](size_t l, std::string s)->std::string {
        if (s.length() < l) {
            return std::string(l - s.length(), ' ') + s;
        } else {
            return s;
        }
    };
    auto rspace = [](size_t l, std::string s)->std::string {
        if (s.length() < l) {
            return s + std::string(l - s.length(), ' ');
        } else {
            return s;
        }
    };

    std::string title = "R(" + graph_name + "," + std::to_string(k) + ")";
    std::string first_line = title;
    std::string last_line = lspace(title.length(), "");
    first_line.push_back('|');
    last_line.push_back('|');
    for (size_t i = 1; i < qv.size(); ++i) {
        size_t l = std::to_string(qv[i]).length() + 1;
        first_line += lspace(l, std::to_string(i));
        last_line += lspace(l, std::to_string(qv[i]));
    }
    first_line.push_back('|');
    last_line.push_back('|');
    first_line += rspace(std::to_string(all).length(), "");
    last_line += std::to_string(all);
    std::string hor_line(last_line.length(), '-');
    hor_line[title.length()] = '+';
    hor_line[last_line.size() - std::to_string(all).length() - 1] = '+';

    std::cout << first_line << std::endl;
    std::cout << hor_line << std::endl;
    for (size_t j = 0; j < qe.size(); ++j) {
        std::string line = lspace(title.length(), "");
        line.push_back('|');
        for (size_t i = 1; i < qv.size(); ++i) {
            size_t l = std::to_string(qv[i]).length() + 1;
            size_t q = 0;
            if (j < qve[i].size()) {
                q = qve[i][j];
            }
            if (q) {
                line += lspace(l, std::to_string(q));
            } else {
                line += lspace(l, "");
            }
        }
        line.push_back('|');
        line += rspace(std::to_string(all).length(), std::to_string(qe[j]));
        std::cout << line << std::endl;
    }
    std::cout << hor_line << std::endl;
    std::cout << last_line << std::endl;
    return 0;
}
