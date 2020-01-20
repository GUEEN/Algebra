// build and write to the disk all Ramsey R(G,k) graphs 
// with a given number of >= n given by the gluing
// algorithm
// Here we assume that G is K_3, C_4 or C_5 only.
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>

#include "Graph.h"

class Interval {
public:
    Interval() {
    }
    Interval(const std::vector<int> b, const std::vector<int> t) : B(b), T(t) {
    }

    std::vector<int> B;
    std::vector<int> T;
};

class Glue {
public:
    Glue(size_t n, size_t k, size_t d) : n(n), k(k), d(d), graphs(n * (n - 1) / 2 + 1, n) {
    }

    void checkGraph(const Graph& G) {
        DG = G.getDegrees();
        min.clear();
        for (int i = 0; i < G.size(); i++) {
            int qq = 0;
            for (int j = 0; j < G.size(); j++) {
                if (G.edge(i, j)) {
                    qq++;
                }
            }
            if (qq == d - 1) {
                min.push_back(i);
            }
        }

        getIntervals(G);
        getIndependent(G);

        // Intervals = new Interval[d];
        std::vector<Interval> Intervals(d);
        nextInterval(G, Intervals, 0);
    }

    GraphSet& operator[](size_t e) {
        return graphs[e];
    }

private:

    void getIntervals(const Graph& G) {
        R3G = G;
        FsInts.clear();
        //List<int> A = new List<int>();
        //List<int> X = new List<int>();
        getI(std::vector<int>(), std::vector<int>());
    }

    void getI(std::vector<int> A, std::vector<int> X) {
        std::vector<int> u(R3G.size() + 1);

        for (size_t i = 0; i < A.size(); i++) {
            u[A[i]] = 1;
        }
        for (size_t i = 0; i < X.size(); i++) {
            u[X[i]] = 2;
        }

        std::vector<int> L = A;
        nextVex(R3G, L, u);
        FsInts.emplace_back(A, L);
        std::vector<int> XX = X;

        for (int ii = 0; ii < R3G.size(); ii++) {
            if (u[ii] == 0) {
                bool B = true;
                for (int jj = 0; jj < A.size(); jj++) {
                    if (R3G.edge(ii, A[jj])) {
                        B = false;
                    }
                }
                if (B) {
                    A.push_back(ii);
                    getI(A, XX);
                    A.pop_back();
                    XX.push_back(ii);
                }
            }
        }
    }

    void getIndependent(const Graph& G) {
        Independent.clear();
        getInd(G, std::vector<int>(), std::vector<int>());
    }

    void getInd(const Graph& G, std::vector<int> A, const std::vector<int>& X) {
        std::vector<int> u(R3G.size() + 1);
        for (int i = 0; i < A.size(); i++) {
            u[A[i]] = 1;
        }
        for (int i = 0; i < X.size(); i++) {
            u[X[i]] = 2;
        }
        std::vector<int> L = A;
        nextVex(G, L, u);
        Independent.emplace_back(A, L);

        std::vector<int> XX = X;

        for (int ii = 0; ii < G.size(); ii++) {
            if (u[ii] == 0) {
                bool B = true;
                for (int jj = 0; jj < A.size(); jj++) {
                    if (G.edge(ii, A[jj])) {
                        B = false;
                    }
                }
                if (B) {
                    A.push_back(ii);
                    getInd(G, A, XX);
                    A.pop_back();
                    XX.push_back(ii);
                }
            }
        }
    }

    void nextInterval(const Graph& G, std::vector<Interval> Ints, int level) {
        H = Graph(level);

        GSets.clear();
        // fill GSets with all indenendent subsets with >= 2 elements
        nextGV(std::vector<int>());

        if (level < d) {
            collapse(G, Ints, level);
            if (!FAIL) {
                for (const Interval& I : FsInts) {
                    std::vector<Interval> IInts(d);

                    for (int ii = 0; ii < level; ii++) {
                        IInts[ii] = Ints[ii];
                    }

                    IInts[level] = I;
                    nextInterval(G, IInts, level + 1);
               }
           }
        } else {
            // this happens when we have all intervals at least
            checkIntervals(G, Ints);
        }
    }

    void nextGV(std::vector<int> L) {
        if (L.empty()) {
            for (int i = 0; i < H.size(); i++) {
                L.push_back(i);
                nextGV(L);
                L.pop_back();
            }
        } else {
            if (L.size() >= 2) {
                GSets.push_back(L);
            }
            for (int i = L.back() + 1; i < H.size(); i++) {
                if (H.edge(i, L.back()) == false) {
                    L.push_back(i);
                    nextGV(L);
                    L.pop_back();
                }
            }
        }
    }

    void nextVex(const Graph& G, std::vector<int>& L, std::vector<int>& u) {
        int ii = 0;
        bool B = true;
        while (ii < G.size() && B) {
            if (u[ii] == 0) {
                B = false;
                for (int jj = 0; jj < L.size(); jj++) {
                    if (G.edge(ii, L[jj]) == 1) {
                        B = true;
                    }
                }
            }
            ii++;
        }
        ii--;
        if (!B) {
            L.push_back(ii);
            u[ii] = 1;
            nextVex(G, L, u);
        }
    }

    void checkIntervals(const Graph& G, std::vector<Interval> Ints) {
        collapse(G, Ints, d);
        if (!FAIL) {
            int j = 0;
            bool B = false;
            for (int i = 0; i < d; i++) {
                if (Ints[i].T.size() > Ints[i].B.size()) {
                    B = true;
                    j = i;
                }
            }

            if (B) {
                //splitting an interval
                std::vector<int> u(n);
                int w = 0;

                for (int i = 0; i < Ints[j].B.size(); i++) {
                    u[Ints[j].B[i]] = 1;
                }
                for (int i = 0; i < Ints[j].T.size(); i++) {
                    if (u[Ints[j].T[i]] == 0) {
                        w = Ints[j].T[i];
                    }
                }
                std::vector<Interval> IInts(d);
                for (int ii = 0; ii < d; ii++) {
                    IInts[ii] = Ints[ii];
                }

                IInts[j].B.push_back(w);
                checkIntervals(G, IInts);

                IInts.clear();
                for (int ii = 0; ii < d; ii++) {
                    IInts.push_back(Ints[ii]);
                }
              
                //IInts[j].T.Remove(w);                         
                auto f = std::find(IInts[j].T.begin(), IInts[j].T.end(), w);
                IInts[j].T.erase(f);

                checkIntervals(G, IInts);

            } else {
                //mmmmm  we have a new (C4,k)-good graph, oh yes!
                Graph HH = G + (n - G.size());
                for (int ii = 0; ii < d; ii++) {
                    for (int jj = 0; jj < Ints[ii].T.size(); jj++) {
                        HH.addEdge(G.size() + ii, Ints[ii].T[jj]);
                    }
                }
                for (int ii = 0; ii < d; ii++) {
                    HH.addEdge(n - 1, G.size() + ii);
                }

                size_t edge = HH.edges(); 
                // if (Deg(HH) == d)
                graphs[edge].insert(HH.certify());
            }
        }
    }

    void collapse(const Graph& G, std::vector<Interval>& Ints, int level) {
        FAIL = false;
        bool CC = false;
        do {
            CC = false;
            //degree inside rejection
            /*
            if (level == d) {
                int[] u = new int[n -d -1];
                for (int i = 0; i < d; i++)
                    for (int j = 0; j < Ints[i].T.Count; j++) u[Ints[i].T[j]]++;
                for (int i = 0; i < n - d - 1; i++) if (u[i] + DG[i] < d) FAIL = true;
            }*/
            {
                std::vector<int> u(n - d - 1);
                for (int i = 0; i < level; i++) {
                    for (int j = 0; j < Ints[i].T.size(); j++) {
                        u[Ints[i].T[j]]++;
                    }
                }
                for (int i = 0; i < n - d - 1; i++) {
                    if (u[i] + DG[i] < level) {
                        FAIL = true;
                    }
                }
            }

            //min degree >= d for new vertices
            for (int ii = 0; ii < level; ii++) {
                if (Ints[ii].T.size() + 1 < d) {
                    FAIL = true;
                }
            }

            //for final check, degree should be d
            if (level == d) {
                std::vector<int> u(n);
                for (int i = 0; i < d; i++) {
                    for (int j = 0; j < Ints[i].T.size(); j++) {
                        u[Ints[i].T[j]] = 1;
                    }
                }
                for (int i = 0; i < min.size(); i++) {
                    if (u[min[i]] == 0) {
                        FAIL = true;
                    }
                }
            }

            //degree increases for vertice, avoiding stupid automorphisms
            if (!FAIL) {
                for (int ii = 1; ii < level; ii++) {
                    if (Ints[ii - 1].B.size() > Ints[ii].T.size()) {
                        FAIL = true;
                    }
                }
            }
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // check no free independent k-set. Most difficult and most important!!!!!
            if (!FAIL) {
                for (int kk = 0; kk < GSets.size() && !FAIL; kk++) {
                    std::vector<int> L = GSets[kk];
                    bool C2 = false;
                    std::vector<int> u(n);
                    for (int i = 0; i < L.size(); i++) {
                        for (int j = 0; j < Ints[L[i]].T.size(); j++) {
                            u[Ints[L[i]].T[j]] = 1;//colored union of all tops  vvwvwv
                        }
                    }
                    for (int i = 0; i < Independent.size() && !FAIL; i++) {
                        bool B = true;
                        for (int j = 0; j < Independent[i].B.size(); j++) {
                            if (u[Independent[i].B[j]] == 1) B = false;
                        }
                        if (B) {
                            int qq = 0;
                            for (int j = 0; j < Independent[i].T.size(); j++) {
                                if (u[Independent[i].T[j]] == 0) {
                                    qq++;
                                }
                            }
                            if (qq >= k - L.size()) {
                                B = false;
                            }
                            if (!B) {
                                FAIL = true;
                            }
                        }                    
                    }

                    if (!FAIL) {
                        //modification of intervals. Really important, but I decide to modify it for all independents simultaneously. I'll begin this tomorrow. 15.11.11. LV
                        std::vector<int> w(n);
                        for (size_t i = 0; i < Independent.size(); i++) {
                            int qq = 0;
                            std::vector<int> v(n);
                            for (size_t j = 0; j < Independent[i].T.size(); j++) {
                                if (u[Independent[i].T[j]] == 0) {
                                    qq++;
                                    v[Independent[i].T[j]] = 1;
                                }
                            }
                            if (qq == k - L.size() - 1) {
                                for (size_t j = 0; j < Independent[i].T.size(); j++) {
                                    if (v[Independent[i].T[j]] == 0) {
                                        w[Independent[i].T[j]] = 1;
                                    }
                                }
                            }
                        }

                        for (size_t i = 0; i < L.size() && !C2; i++) {
                            std::vector<int> v(n);
                            for (size_t j = 0; j < L.size(); j++) {
                                if (j != i) {
                                    for (int ii = 0; ii < Ints[L[j]].T.size(); ii++) {
                                        v[Ints[L[j]].T[ii]] = 1;
                                    }
                                }
                            }
                            for (int j = 0; j < Ints[L[i]].B.size(); j++) {
                                v[Ints[L[i]].B[j]] = 1;
                            }
                            for (int j = 0; j < n; j++) {
                                if (w[j] == 1 && v[j] == 0) {
                                    C2 = true;
                                    Ints[L[i]].B.push_back(j);
                                }
                            }
                        }
                    }
                    if (C2) {
                        CC = true;
                    }
                }
            }
        } while (CC);
    }

    Graph R3G;
    Graph H;

    bool FAIL;
    std::vector<Interval> FsInts;
    std::vector<Interval> Independent;
    std::vector<std::vector<int>> GSets;
    std::vector<size_t> DG; // degree sequence
    std::vector<int> min;

    const size_t n;
    const size_t k;
    const size_t d;

    std::vector<GraphSet> graphs;
};

int main() {
    size_t k = 7;
    size_t n = 22;

    std::string graph_name = "3";

   // mkdir("../data/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
   // mkdir("../data/RAMSEY/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    const std::string address = "../data/RAMSEY/R(" + graph_name + "," + std::to_string(k - 1) + ")/";
   // mkdir(address.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    std::vector<size_t> qv;
    std::vector<size_t> qe(n * (n - 1) / 2 + 1);
    size_t q = 0;

    // for (d = 0; d * d - d + 1 <= n; d++)
    for (int d = 0; d < 20; d++) {
        Glue glue(n, k, d);
        for (int e = 120; e >= 0; e--) {
            for (int dd = 20; dd >= 0; dd--) {
                 // if (dd + 1 >= d)
                std::string path = address + "R(" + graph_name + "," + std::to_string(k - 1) + ";" + std::to_string(n - d - 1) 
                                 + "," + std::to_string(e) + "," + std::to_string(dd) + ").gr";
                std::fstream file;
                file.open(path, std::ios::in | std::ios::binary);

                if (file.good()) {
                   // std::cout << path << " is good" << std::endl;
                    Graph G(n - d - 1);
                    while (readGraph(file, G)) {
                        glue.checkGraph(G);
                        //NextVertex(G);
                    }
                }
            }
        }
        for (size_t e = 0; e <= n * (n - 1) / 2; e++) {
            GraphSet& graphs = glue[e];
            if (graphs.empty()) {
                continue;
            }

            std::string path = address + "R(" + graph_name + "," + std::to_string(k) + ";" + std::to_string(n) 
                                 + "," + std::to_string(e) + "," + std::to_string(d) + ").gl";
           // graphs.write(path);

            qe[e] += graphs.size();
            q += graphs.size();
        }
    }

    for (int e = 0; e <= n * (n - 1) / 2; ++e) {
        if (qe[e]) {
            std::cout << e << "   " << qe[e] << std::endl;
        }
    }

    std::cout << "---------------------" << std::endl;
    std::cout << q << std::endl;
}
