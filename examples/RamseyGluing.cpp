// build and write to the disk all Ramsey R(G,k) graphs 
// with a given number of >= n given by the gluing
// algorithm
// Here we assume that G is K_3, C_4 or C_5 only.
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
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
        // fill GSets with all independent subsets with >= 2 elements
        H = Graph(d);
        std::vector<int> L;
        nextGV(L);
        std::sort(GSets.begin(), GSets.end(), [](const std::vector<int>& first, const std::vector<int>& second){ 
            return first.back() < second.back();
        });

        gpos.assign(d + 1, 0);
        index.assign(d, 0);
        for (int i = GSets.size() - 1; i >= 0; --i) {
            int x = GSets[i].back();
            gpos[x] = i;
        }
        gpos[d] = GSets.size();
    }

    void checkGraph(const Graph& G0) {
        G = G0;
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

        getIntervals();
        Independent = FsInts;

        // Intervals = new Interval[d];
        std::vector<Interval> Intervals(d);
        nextInterval(Intervals, 0);
    }

    GraphSet& operator[](size_t e) {
        return graphs[e];
    }

private:

    void getIntervals() {
        FsInts.clear();
        std::vector<int> A;
        std::vector<int> X;
        getI(A, X);
    }

    void getI(std::vector<int>& A, std::vector<int>& X) {
        std::vector<int> u(G.size() + 1);

        for (size_t i = 0; i < A.size(); i++) {
            u[A[i]] = 1;
        }
        for (size_t i = 0; i < X.size(); i++) {
            u[X[i]] = 2;
        }

        std::vector<int> L = A;
        nextVex(L, u);
        FsInts.emplace_back(A, L);

        for (int ii = 0; ii < G.size(); ii++) {
            if (u[ii] == 0) {
                bool B = true;
                for (int x : A) {
                    if (G.edge(ii, x)) {
                        B = false;
                        break;
                    }
                }
                if (B) {
                    A.push_back(ii);
                    getI(A, X);
                    A.pop_back();
                    X.push_back(ii);
                }
            }
        }
        while (X.size() && u[X.back()] == 0) {
            X.pop_back();
        }
    }

    void nextInterval(std::vector<Interval> Ints, int level) {
       // H = Graph(level);
     /*   GSets.clear();
        // fill GSets with all indenendent subsets with >= 2 elements
        std::vector<int> L;
        nextGV(L);*/

        if (level < d) {
            collapse(Ints, level);
            if (!FAIL) {
                int ind = 0;
                if (level) {
                    ind = index[level - 1];
                }
                for (int i = ind; i < FsInts.size(); ++i) {
                    Ints[level] = FsInts[i];
                    index[level] = i;
                    nextInterval(Ints, level + 1);
                }
            }
        } else {
            // this happens when we have all intervals at least
            checkIntervals(Ints);
        }
    }

    void nextGV(std::vector<int>& L) {
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

    void nextVex(std::vector<int>& L, std::vector<int>& u) {
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
            nextVex(L, u);
        }
    }

    void checkIntervals(std::vector<Interval>& Ints) {
        collapse(Ints, d);
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

                for (int x : Ints[j].B) {
                    u[x] = 1;
                }
                for (int x : Ints[j].T) {
                    if (u[x] == 0) {
                        w = x;
                    }
                }
                std::vector<Interval> IInts = Ints;

                IInts[j].B.push_back(w);
                checkIntervals(IInts);
              
                //IInts[j].T.Remove(w);                         
                auto f = std::find(Ints[j].T.begin(), Ints[j].T.end(), w);
                Ints[j].T.erase(f);
                checkIntervals(Ints);
            } else {
                //we have a new good graph obtained from gluing
                Graph HH = G + (n - G.size());
                for (int i = 0; i < d; i++) {
                    for (int x : Ints[i].T) {
                        HH.addEdge(G.size() + i, x);
                    }
                }
                for (int i = 0; i < d; i++) {
                    HH.addEdge(n - 1, G.size() + i);
                }

                size_t edge = HH.edges(); 
                // if (Deg(HH) == d)
                graphs[edge].insert(HH.certify());
            }
        }
    }

    void collapse(std::vector<Interval>& Ints, int level) {
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
                    for (int x : Ints[i].T) {
                        u[x]++;
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
                    for (int x : Ints[i].T) {
                        u[x] = 1;
                    }
                }
                for (int i = 0; i < min.size(); i++) {
                    if (u[min[i]] == 0) {
                        FAIL = true;
                    }
                }
            }
/*
            //degree increases for vertex, avoiding automorphisms
            if (!FAIL) {
                for (int ii = 1; ii < level; ii++) {
                    if (Ints[ii - 1].B.size() > Ints[ii].T.size()) {
                        FAIL = true;
                    }
                }
            }*/
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // check no free independent k-set. Most difficult and most important
            if (!FAIL) {
                for (int kk = 0; kk < gpos[level] && !FAIL; kk++) {
                    const std::vector<int>& L = GSets[kk];
                    bool C2 = false;
                    std::vector<int> u(n);
                    for (int i = 0; i < L.size(); i++) {
                        for (int x : Ints[L[i]].T) {
                            u[x] = 1;
                            //colored union of all tops  vvwvwv
                        }
                    }
                    for (int i = 0; i < Independent.size() && !FAIL; i++) {
                        bool B = true;
                        for (int x : Independent[i].B) {
                            if (u[x] == 1) {
                                B = false;
                                break;
                            }
                        }
                        if (B) {
                            int qq = 0;
                            for (int x : Independent[i].T) {
                                if (u[x] == 0) {
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
                        //modification of intervals
                        std::vector<int> w(n);
                        for (size_t i = 0; i < Independent.size(); i++) {
                            int qq = 0;
                            std::vector<int> v(n);
                            for (int x : Independent[i].T) {
                                if (u[x] == 0) {
                                    qq++;
                                    v[x] = 1;
                                }
                            }
                            if (qq == k - L.size() - 1) {
                                for (int x : Independent[i].T) {
                                    if (v[x] == 0) {
                                        w[x] = 1;
                                    }
                                }
                            }
                        }

                        for (size_t i = 0; i < L.size() && !C2; i++) {
                            std::vector<int> v(n);
                            for (size_t j = 0; j < L.size(); j++) {
                                if (j != i) {
                                    for (int x : Ints[L[j]].T) {
                                        v[x] = 1;
                                    }
                                }
                            }
                            for (int x : Ints[L[i]].B) {
                                v[x] = 1;
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

    Graph G;
    Graph H;

    bool FAIL;
    std::vector<Interval> FsInts;
    std::vector<Interval> Independent;
    std::vector<std::vector<int>> GSets;
    std::vector<size_t> DG; // degree sequence
    std::vector<size_t> gpos;
    std::vector<size_t> min;
    std::vector<size_t> index;

    const size_t n;
    const size_t k;
    const size_t d;

    std::vector<GraphSet> graphs;
};

int main() {
    auto start = std::chrono::steady_clock::now();

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
    for (int d = 0; d < n; d++) {
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
                    }
                }
            }
        }
        bool empty = true;
        for (size_t e = 0; e <= n * (n - 1) / 2; e++) {
            GraphSet& graphs = glue[e];
            if (graphs.empty()) {
                continue;
            } else {
                empty = false;
            }
            std::string path = address + "R(" + graph_name + "," + std::to_string(k) + ";" + std::to_string(n) 
                                 + "," + std::to_string(e) + "," + std::to_string(d) + ").gl";
           // graphs.write(path);

            qe[e] += graphs.size();
            q += graphs.size();
        }
        if (q && empty) {
            break;
        }
    }

    for (int e = 0; e <= n * (n - 1) / 2; ++e) {
        if (qe[e]) {
            std::cout << e << "   " << qe[e] << std::endl;
        }
    }

    std::cout << "---------------------" << std::endl;
    std::cout << q << std::endl;
    
    auto end = std::chrono::steady_clock::now();
    std::cout << "Elapsed time in milliseconds : " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;
}
