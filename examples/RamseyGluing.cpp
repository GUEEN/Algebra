// build and write to the disk all Ramsey R(G,k) graphs 
// with a given number of >= n given by the gluing
// algorithm
// Here we assume that G is K_3 or C_4 only.
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>

#include "Graph.h"

const std::vector<std::string> names = {"K3", "C3", "C4", "3"};

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
    Glue(const std::string& name, size_t n, size_t k, size_t d) : graph_name(name), n(n), k(k), d(d), graphs(n * (n - 1) / 2 + 1, n) {
    }

    void setG(const Graph& G0) {
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

        getFeasibleIntervals();
        getIndependentIntervals();
    }

    void glueGH(const Graph& H0) {
        // H must have d vertices and contain no copy of G/e
        H = H0;
        DH = H.getDegrees();
        // fill GSets with all independent subsets with >= 2 elements
        std::vector<int> L;
        GSets.clear();
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

        std::vector<Interval> Intervals(d);
        nextInterval(Intervals, 0);
    }

    GraphSet& operator[](size_t e) {
        return graphs[e];
    }

private:

    void getFeasibleIntervals() {
        Graph G0(G.size());
        if (graph_name == "C4") {
            for (size_t i = 0; i < G.size() - 1; i++) {
                for (size_t j = i + 1; j < G.size(); j++) {
                    for (size_t l = 0; l < G.size(); l++) {
                        if (G.edge(i, l) && G.edge(l, j)) {
                            G0.addEdge(i, j);
                        }
                    }
                }
            }
            std::swap(G, G0);
        }

        FsInts.clear();
        std::vector<int> A;
        std::vector<int> X;
        getI(A, X, FsInts);

        if (graph_name == "C4") {
            std::swap(G, G0);
        }
    }

    void getIndependentIntervals() {
        if (graph_name == "3") {
            Independent = FsInts;
            return;
        }

        Independent.clear();
        std::vector<int> A;
        std::vector<int> X;
        getI(A, X, Independent);
    }

    void getI(std::vector<int>& A, std::vector<int>& X, std::vector<Interval>& Ints) {
        std::vector<int> u(G.size() + 1);

        for (size_t i = 0; i < A.size(); i++) {
            u[A[i]] = 1;
        }
        for (size_t i = 0; i < X.size(); i++) {
            u[X[i]] = 2;
        }

        std::vector<int> L = A;
        nextVex(L, u);
        Ints.emplace_back(A, L);

        for (size_t i = 0; i < G.size(); i++) {
            if (u[i] == 0) {
                bool B = true;
                for (int x : A) {
                    if (G.edge(i, x)) {
                        B = false;
                        break;
                    }
                }
                if (B) {
                    A.push_back(i);
                    getI(A, X, Ints);
                    A.pop_back();
                    X.push_back(i);
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
        // fill GSets with all independent subsets with >= 2 elements
        std::vector<int> L;
        nextGV(L);*/

        if (level < d) {
            collapse(Ints, level);
            if (!FAIL) {
                int ind = 0;
                if (graph_name == "3" && level) {
                    ind = index[level - 1];
                }
                if (graph_name == "C4" && level) {
                    if (DH[level - 1] == 0) {
                        ind = index[level - 1];
                    }
                    if (DH[level] == 1) {
                        if (level & 1) {
                            ind = index[level - 1];
                        } else {
                            ind = index[level - 2];
                        }
                    }
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
                Graph W = G + H + 1;
                for (int i = 0; i < d; i++) {
                    for (int x : Ints[i].T) {
                        W.addEdge(G.size() + i, x);
                    }
                }
                for (int i = 0; i < d; i++) {
                    W.addEdge(n - 1, G.size() + i);
                }

                size_t edge = W.edges(); 
                // if (Deg(HH) == d)
                graphs[edge].insert(W.certify());
            }
        }
    }

    void collapse(std::vector<Interval>& Ints, int level) {
        FAIL = false;
        bool CC = false;
        const size_t t = H.edges();
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
            for (int i = 0; i < level; i++) {
                if (Ints[i].T.size() + DH[i] + 1 < d) {
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
            /*
            if (!FAIL) {
                for (int i = 1; i < level; i++) {
                    if (i < 2 * t) {
                        if (i % 2 == 1 && Ints[i - 1].B.size() > Ints[i].T.size()) {
                            FAIL = true;
                        }
                        if (i % 2 == 0 && Ints[i - 2].B.size() > Ints[i].T.size()) {
                            FAIL = true;
                        }
                    } else if (i > 2 * t) {
                        if (Ints[i - 1].B.size() > Ints[i].T.size()) {
                            FAIL = true;
                        }
                    }
                }
            }*/

            if (graph_name == "C4") {
                // C4 rejection first test
                if (!FAIL) {
                    for (int i = 0; i < level - 1 && !FAIL; i++) {
                        for (int j = i + 1; j < level && !FAIL; j++) {
                            std::vector<int> u(n);
                            for (int x : Ints[i].B) {
                                u[x] = 1;
                            }
                            for (int x : Ints[j].B) {
                                if (u[x] == 1) {
                                    FAIL = true;
                                }
                            }
                        }
                    }
                }

                if (!FAIL) {
                    for (int ii = 0; ii < level - 1 && ii < 2 * t && !FAIL; ii += 2) {
                        bool C2 = false;
                        for (int x : Ints[ii].B) {
                            for (int y : Ints[ii + 1].B) {
                                if (G.edge(x, y)) {
                                    FAIL = true;
                                    break;
                                }
                            }
                            if (FAIL) {
                                break;
                            }
                        }
                        if (!FAIL) {
                            std::vector<int> U;
                            for (int x : Ints[ii].T) {
                                bool B = false;
                                for (int y : Ints[ii + 1].B) {
                                    if (G.edge(x, y)) {
                                        B = true;
                                        break;
                                    }
                                }

                                if (B) {
                                    U.push_back(x);
                                    C2 = true;
                                }
                            }
                            if (C2) {
                                for (int x : U) {
                                    auto f = std::find(Ints[ii].T.begin(), Ints[ii].T.end(), x);
                                    Ints[ii].T.erase(f);
                                }
                            } else {
                                U.clear();
                                for (int x : Ints[ii + 1].T) {
                                    bool B = false;
                                    for (int y : Ints[ii].B) {
                                        if (G.edge(x, y)) {
                                            B = true;
                                        }
                                    }
                                    if (B) {
                                        U.push_back(x);
                                    }
                                    if (B) {
                                        C2 = true;
                                    }
                                }
                                if (C2) {
                                    for (int x : U) {
                                        auto f = std::find(Ints[ii + 1].T.begin(), Ints[ii + 1].T.end(), x);
                                        Ints[ii + 1].T.erase(f);
                                    }
                                }
                            }
                        }
                        if (C2) {
                            CC = true;
                        }
                    }
                }
            }

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
    std::vector<size_t> DG;
    std::vector<size_t> DH;
    std::vector<size_t> gpos;
    std::vector<size_t> min;
    std::vector<size_t> index;
    const std::string graph_name;
    const size_t n;
    const size_t k;
    const size_t d;

    std::vector<GraphSet> graphs;
};

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Wrong number of arguments. We expect graph name G, positive integer k, and positive integer n to compute the set R(G, k, >=n)" << std::endl;
        return 1;
    }

    std::string graph_name = argv[1];
    size_t k = std::atoi(argv[2]);
    size_t n0 = std::atoi(argv[3]);

    if (std::find(names.begin(), names.end(), graph_name) == names.end()) {
        std::cout << "Wrong graph name. We expect G to be K3 or C4" << std::endl;
        return 1;
    }
    if (graph_name == "K3" || graph_name == "C3") {
        graph_name = "3";
    }

    mkdir("../data/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("../data/RAMSEY/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    const std::string address = "../data/RAMSEY/R(" + graph_name + "," + std::to_string(k - 1) + ")/";
    const std::string new_address = "../data/RAMSEY/R(" + graph_name + "," + std::to_string(k) + ")/";
    mkdir(new_address.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    size_t all = 0;
    std::vector<size_t> qv(n0);
    std::vector<size_t> qe;
    std::vector<std::vector<size_t>> qve(n0);

    for (int n = n0;; n++) {
        size_t q = 0;
        std::vector<size_t> ve;
        for (int d = 0; d < n; d++) {
            if (graph_name == "C4" && d * d - d + 1 > n) {
                break;
            }

            Glue glue(graph_name, n, k, d);
            std::vector<Graph> hGraphs;
            if (graph_name == "3") {
                hGraphs.emplace_back(d);
            }
            if (graph_name == "C4") {
                // adding P2-free graphs to a list
                for (size_t t = 0; 2 * t <= d && d < k + t; t++) {
                    Graph H(d);
                    for (size_t s = 0; s < t; ++s) {
                        H.addEdge(2 * s, 2 * s + 1);
                    }
                    hGraphs.push_back(std::move(H));
                }
            }

            for (int e = (n - d - 2) * (n - d - 1) / 2; e >= 0; e--) {
                for (int dd = n - d - 1; dd >= 0; dd--) {
                    if (graph_name == "C4" && dd + 1 < d) {
                        break;
                    }

                    std::string path = address + "R(" + graph_name + "," + std::to_string(k - 1) + ";" + std::to_string(n - d - 1) 
                                 + "," + std::to_string(e) + "," + std::to_string(dd) + ").gr";
                    std::fstream file;
                    file.open(path, std::ios::in | std::ios::binary);
                    if (file.good()) {
                        Graph G(n - d - 1);
                        while (readGraph(file, G)) {
                            glue.setG(G);
                            for (const Graph& H : hGraphs) {
                                glue.glueGH(H);
                            }
                        }
                    }
                }
            }
            bool empty = true;
            for (size_t e = 0; e <= n * (n - 1) / 2; e++) {
                GraphSet& graphs = glue[e];
                while (ve.size() <= e) {
                    ve.push_back(0);
                }
                ve[e] += graphs.size();
                if (graphs.empty()) {
                    continue;
                } else {
                    empty = false;
                }
                std::string path = new_address + "R(" + graph_name + "," + std::to_string(k) + ";" + std::to_string(n) 
                                               + "," + std::to_string(e) + "," + std::to_string(d) + ").gr";
               /* std::ifstream file;
                file.open(path, std::ios::in | std::ios::binary);
                if (!file.good()) {
                    file.close();
                    graphs.write(path);
                }*/
                while (qe.size() <= e) {
                    qe.push_back(0);
                }

                qe[e] += graphs.size();
                q += graphs.size();
            }
            if (q && empty) {
                break;
            }
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
    for (size_t i = n0; i < qv.size(); ++i) {
        size_t l = std::max(std::to_string(qv[i]).length(), std::to_string(i).length()) + 1;
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
    int j0 = 0;
    while (j0 < qe.size() && qe[j0] == 0) {
        j0++;
    }
    for (size_t j = j0; j < qe.size(); ++j) {
        std::string line = lspace(title.length(), std::to_string(j));
        line.push_back('|');
        for (size_t i = n0; i < qv.size(); ++i) {
            size_t l = std::max(std::to_string(qv[i]).length(), std::to_string(i).length()) + 1;
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
