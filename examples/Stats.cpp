#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unordered_set>

#include "Graph.h"

// p must be == 3 (mod 4)
// we construct GF(p^2) by adding x satisfying x^2 + 1 = 0
template<int p>
class F2 {
public:

    friend struct std::hash<F2<p>>;

    static void normalize(int& x) {
        if (x >= 0) {
            x %= p;
        } else {
            x = -x;
            x %= p;
            x = p - x;
            x %= p;
        }
    }

    F2(int aa, int bb) : a(aa), b(bb) {
        normalize(a);
        normalize(b);
    }

    F2 operator+(const F2& r) const {
        return F2(a + r.a, b + r.b);
    }

    F2 operator-(const F2& r) const {
        return F2(a - r.a, b - r.b);
    }

    F2& operator=(const F2& r) {
        a = r.a;
        b = r.b;
        normalize(a);
        normalize(b);
        return *this;
    }

    F2 operator^(int n) const {
        F2 r(1, 0);
        F2 c(a, b);
       
        while (n) {
            if (n & 1) {
                r = r * c;
                --n;
            } else {
                c = c * c;
                n >>=1;
            }
        }
        return r;
    }

    F2 operator*(const F2& r) const {
        return F2(a * r.a - b * r.b, a * r.b + b * r.a);
    }

    F2 operator/(const F2& r) const {
        return *this * (r ^ (p * p - 2));
    }

    bool operator==(const F2& r) const {
        return (a - r.a) % p == 0 && (b - r.b) % p == 0;
    }

    bool operator!=(const F2& r) const {
        return (a - r.a) % p != 0 || (b - r.b) % p != 0;
    }

    bool operator==(int x) const {
        return (a - x) % p == 0 && b % p == 0;
    }

    bool operator!=(int x) const {
        return (a - x) % p != 0 || b % p != 0;
    }

    int order() const {
        if (a % p == 0 && b % p == 0) {
            return 0;
        }

        F2 r(a, b);
        int m = 1;
        while (r != 1) {
            r = r * F2(a, b);
            ++m;
        }
        return m;
    }
 
private:
    int a;
    int b;
};

// x^2 = x + 1
// (a + bx)(c + dx) = ac + (ad + bc)x + bd x^2 = ac + bd + (ad + bc + bd)x
template<>
F2<2> F2<2>::operator*(const F2<2>& r) const {
    return F2(a * r.a + b * r.b, a * r.b + b * r.a + b * r.b);
}

template<int p>
struct std::hash<F2<p>> {
    size_t operator()(const F2<p>& x) const {
        return x.a + x.b * p;
    }
};

// p must be == 3 (mod 4)
const int p = 139;
const int h_ord = 3 * (p + 1);
//const int fraq = 2;

void print_cycles(const Perm& P) {
    int n = P.size();
    std::vector<int> b(n);
    for (int i = 0; i < n; ++i) {
        if (b[i] == 0)  {
            if (P[i] == i) {
                b[i] = 1;
                continue;
            }

            std::cout << "(";
            int j = i;
            while (b[j] == 0) {
                b[j] = 1;
                std::cout << j + 1;
                j = P[j];
                if (b[j] == 0) {
                    std::cout << ",";
                }
            }
            std::cout << ")";
        }
    }
    std::cout << ", ";
    //std::cout << std::endl;

    // ok, I have an idea. Let's try it out


}


void check_aut_group() {
 
    std::string address = "/home/ivan/Projects/Extremal/some/EX(20, K33).gr";
    int n = 20;

    std::fstream stream;
    stream.open(address, std::ios::in | std::ios::binary);
    Graph G(n);
    readGraph(stream, G);
    readGraph(stream, G);

    G.certify();
    Group H = G.aut();
    std::cout << H.order() << std::endl;

  /*  PermList E = H.getElements();
    std::cout << E.size() << std::endl;

    // count group center
    PermList Z;

    for (const Perm& X : E) {
        bool central = true;
        for (const Perm& Q : E) {
            if (!(Q || X)) {
                central = false;
                break;
            }
        }
        if (central) {
            Z.push_back(X);
            for (int i = 0; i < n; ++i) {
                std::cout << X[i] << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "center is " << Z.size() << std::endl;*/
/*
    PermList E = H.getGenerators();
    std::cout << E.size() << std::endl;
    for (const Perm& P : E) {
        print_cycles(P);
    }*/


    std::vector<std::vector<int>> verts;
    for (int i = 0; i < 6; ++i)
    for (int j = i + 1; j < 6; ++j)
    for (int k = j + 1; k < 6; ++k) {
        verts.push_back({i, j, k});
    }

    std::cout << verts.size() << std::endl;

    Graph R(20);

    for (int i = 0; i < n; ++i) 
    for (int j = i + 1; j < n; ++j) {
        int q = 0;
        for (int k = 0; k < 6; ++k) {
            bool b1 = false;
            bool b2 = false;

            for (int x : verts[i]) {
                if (x == k)
                   b1 = true;
            }
            for (int x : verts[j]) {
                if (x == k)
                   b2 = true;
            }
            if (b1 && b2) 
                ++q;

        }
        if (q == 1) {
            R.addEdge(i, j);
        }
    }

    std::cout << R.edges() << std::endl;
    R.certify();
    std::cout << R.aut().order() << std::endl;

    std::cout << isomorphic(G, R) << std::endl;

}

void check_44_construction() {
    int n = 8;
    std::vector<std::vector<int>> verts;
    for (int i = 0; i < n; ++i)
    for (int j = i + 1; j < n; ++j)
    for (int k = j + 1; k < n; ++k)
    for (int l = k + 1; l < n; ++l) {
        verts.push_back({i, j, k, l});
    }

    std::cout << verts.size() << std::endl;

    Graph R(verts.size());

    for (int i = 0; i < verts.size(); ++i) 
    for (int j = i + 1; j < verts.size(); ++j) {
        int q = 0;
        for (int k = 0; k < n; ++k) {
            bool b1 = false;
            bool b2 = false;

            for (int x : verts[i]) {
                if (x == k)
                   b1 = true;
            }
            for (int x : verts[j]) {
                if (x == k)
                   b2 = true;
            }
            if (b1 && b2) 
                ++q;

        }
        if (q == 2) {
            R.addEdge(i, j);
        }
    }

    std::cout << "vertices = " << R.size() << std::endl;
    std::cout << "edges = " << R.edges() << std::endl;
    int q_max = 0;

    for (int i = 0; i < R.size(); ++i) {
        std::vector<int> N;
        for (int j = 0; j < R.size(); ++j) {
            if (R.edge(i, j)) {
                N.push_back(j);
            }
        }

        for (int j = i + 1; j < R.size(); ++j)
        for (int k = j + 1; k < R.size(); ++k)
        for (int l = k + 1; l < R.size(); ++l) {
            int q = 0;
            for (int x : N) {
                if (R.edge(j, x) && R.edge(k, x) && R.edge(l, x)) {
                    ++q;
                }
            }
            q_max = std::max(q, q_max);
        }
        
    }// (2n n) * n^2
    
    std::cout << "K44 max = " << q_max << std::endl;

}

int binom2(int n) {
   return n * (n - 1) / 2;
}

int binom3(int n) {
   return n * (n - 1) * (n - 2) / 6;
}

void find_anything(const Graph& G) {
    //std::string address = "/home/ivan/Projects/Algebra/data/TURAN/K35/Extremal/EX(18, K35).gr";
    //std::string address = "/home/ivan/Projects/Algebra/data/TURAN/K33/Extremal/EX(20, K33).gr";
    //std::string address = "/home/ivan/Projects/Algebra/data/TURAN/K36/Extremal/EX(14, K36).gr";
    //std::string address = "/home/ivan/Projects/Algebra/data/TURAN/K23/Extremal/EX(19, K23).gr";

    //int n = 19;
    int n = G.size();

    //std::fstream stream;
    //stream.open(address, std::ios::in | std::ios::binary);
    //Graph G(n);

    //readGraph(stream, G);
    //readGraph(stream, G);
    //readGraph(stream, G);

    //G.certify();

    //std::cout << "Automorphisms " << G.aut().order() << std::endl;
    //std::cout << "Abelian " << G.aut().isAbelian() << std::endl;


  /*  PermList E = G.aut().getGenerators();
    std::cout << E.size() << std::endl;
    for (const Perm& P : E) {
        print_cycles(P);
    }*/

    std::vector degs = G.getDegrees();

    int sub_k12 = 0;
    int sub_k13 = 0;
    int sub_k23 = 0;

    for (int d : degs) {
        //std::cout << d << " " << binom2(d) << " ";
        std::cout << d << " ";
        sub_k12 += binom2(d);
        sub_k13 += binom3(d);
    }

    for (int i = 0; i < n; ++i)
    for (int j = i + 1; j < n; ++j) {
        int q = 0;
        for (int k = 0; k < n; ++k) {
            if (G.edge(i, k) && G.edge(j, k)) {
                ++q;
            }
        }
        sub_k23 += binom3(q);
    }


    std::cout << std::endl;
    //std::cout << G.aut().order() << std::endl;

    std::cout << "sub_k12 " << sub_k12 << std::endl;
    std::cout << "sub_k13 " << sub_k13 << std::endl;
    std::cout << "sub_k23 " << sub_k23 << std::endl;
    std::cout << 2.0 * sub_k23 / sub_k13 << std::endl;

    std::cout << "binom3 part " << binom3(n) << std::endl;
    std::cout << "binom3 part " << sub_k13 * 1.0 / binom3(n) << std::endl;

    std::map<int, int> f2;
    std::map<int, int> f3;

    std::cout << "Double intersections" << std::endl;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int q = 0;
            for (int x = 0; x < n; ++x) {
                if (G.edge(i, x) && G.edge(j, x)) {
                    ++q;
                }
            }
            f2[q]++;
        }
    }

    for (auto x : f2) {
        std::cout << x.first << " : " << x.second << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Triple intersections" << std::endl;

    double total3 = 1.0 * n * (n - 1) * (n - 2) / 6;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            for (int k = j + 1; k < n; ++k) {
                int q = 0;
                for (int x = 0; x < n; ++x) {
                    if (G.edge(i, x) && G.edge(j, x) && G.edge(k, x)) {
                        ++q;
                    }
                }            
                f3[q]++;
            }
        }
    }

    for (auto x : f3) {
        std::cout << x.first << " : " << x.second / total3 << std::endl;
    }
    int sum = 0;
    for (auto x : f3) {
        sum += x.first * x.second;
    }

//    std::cout << sum / total3 << std::endl;*/

}


void find_something(const Graph& G) {
    //std::string address = "/home/ivan/Projects/Algebra/data/TURAN/K34/Extremal/EX(17, K34).gr";
    //std::string address = "/home/ivan/Projects/Algebra/data/TURAN/K33/Extremal/EX(20, K33).gr";
    //std::string address = "/home/ivan/Projects/Algebra/data/TURAN/K36/Extremal/EX(14, K36).gr";
    //std::string address = "/home/ivan/Projects/Algebra/data/TURAN/K35/Extremal/EX(18, K35).gr";
    //std::string address = "/home/ivan/Projects/Algebra/data/TURAN/K23/Extremal/EX(19, K23).gr";

    //int t = 3;
    //int n = 19;
    int n = G.size();

    //std::fstream stream;
    //stream.open(address, std::ios::in | std::ios::binary);
    //Graph G(n);

    //readGraph(stream, G);
    //readGraph(stream, G);
    //readGraph(stream, G);

    int sub_k13 = 0;
    int sub_k23 = 0;

    for (int i = 0; i < n; ++i) {
        int q = 0;
        for (int k = 0; k < n; ++k) {
            if (G.edge(i, k)) {
                ++q;
            }
        }       
        sub_k13 += q * (q - 1) * (q - 2) / 6;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int q = 0;
            for (int k = 0; k < n; ++k) {
                if (G.edge(i, k) && G.edge(j, k)) {
                    ++q;
                }
            }       
            sub_k23 += q * (q - 1) * (q - 2) / 6;
        }
    }

   // std::cout << sub_k13 * (t - 1) / 2 << std::endl;
    std::cout << sub_k23 << std::endl;

    std::cout << sub_k13 * 1.0 / sub_k23 << std::endl;

    // inequality we must have   sub_k13 * (t-1) / 2 >= sub_k23
    

}


int test(int q) {
    int n = q * q - 1;
    int d = q + 1;
    int e = (q * q - 1) * q / 2 + 1;

    std::cout << (n - 1) * (n - d) * (n - d - 1) - (2 * e - n - d + 1) * (2 * e - 2 * n - d + 2) << std::endl;
}

int main(int argc, char** argv) {

    //check_aut_group();
    //check_44_construction();

    //find_34_something();
    //find_34_anything();
    //return 0;

    std::vector<F2<p>> F;
    std::unordered_set<F2<p>> H;
    std::vector<F2<p>> Q;

    std::vector<int> ord;

    for (int a = 0; a < p; ++a)
    for (int b = 0; b < p; ++b) {
        F.emplace_back(a, b);

        int order = F.back().order();
        if (order > 0 && h_ord % order == 0) {
            //H.emplace_back(a, b);
            H.emplace(a, b);
        }

        ord.push_back(order);
    }

    std::cout << "H size is " << H.size() << std::endl;


    int max_c = 0;
    for (int i = 1; i < F.size(); ++i)
    for (int j = i + 1; j < F.size(); ++j) {
        int c = 0;
        for (const F2<p>& h1 : H)
        for (const F2<p>& h2 : H) {
            if (F[i] * h1 + F[j] * h2 == 1) {
                ++c;
            }
        }
        if (c > max_c) {
            max_c = c;
            std::cout << max_c << std::endl;
        }
    } 

    std::cout << "Maximum number of solutions to a*h1+b*h2=1 is " << max_c << std::endl;
    std::cout << "Field size is " << F.size() << std::endl;

    return 0;


    // building the extrmal graph now
    // vertices are F2^2 / H

    std::vector<std::pair<F2<p>, F2<p>>> V;
    
    for (const F2<p>& x : F) 
    for (const F2<p>& y : F) {
        if (x == 0 && y == 0) {
            continue;
        }
        /*if (x == 0 || y == 0) {
            continue;
        }

        if ( ((x / y) ^ ((p * p - 1) / fraq)) != 1) {
            continue;
        }*/

        bool good = true;
        for (int i = 0; i < V.size(); ++i) {
            const F2<p>& r = V[i].first;
            const F2<p>& s = V[i].second;
        
            for (const F2<p>& h : H) {
                if (r * h == x && s * h == y) {
                    good = false;
                    break;
                }
            }
        }

        if (good) {
            V.emplace_back(x, y);
        }

    }
    std::cout << V.size() << std::endl;

    F2<p> z(1, 3);
    std::cout << "z order = " << z.order() << std::endl;

    Graph R(V.size());
    for (int i = 0; i < V.size(); ++i) {
        for (int j = i + 1; j < V.size(); ++j) {
            F2<p> res = V[i].first * (V[j].first ^ p) + V[i].second * (V[j].second ^ p);
            res = res / z;
            //F2<p> res = V[i].first * V[j].first + V[i].second * V[j].second;
            bool good = false;

            if (H.find(res) != H.end()) {
                good = true;
            }
         /*   if (res == 0 || res == 1) {
                good = true;
            }
            for (const F2<p>& h : H) {
                if (res == z * h) {
                    good = true;
                    break;
                }
            }*/
            if (good) {
                R.addEdge(i, j);
            }
        }
    }

   // R.addEdge(0,19);

    std::cout << "Graph is built" << std::endl;

    std::vector<size_t> degs = R.getDegrees();
    /*std::cout << "Vertex degrees: " << std::endl;
    for (int d : degs) {
        std::cout << d << " ";
    }
    std::cout << std::endl;*/

    std::cout << "Vertices: " << R.size() << std::endl;
    std::cout << "Edges: " << R.edges() << std::endl;

   // std::cout << "Vertices theory: " << p * p * p - p * p + p - 1 << std::endl;
   // std::cout << "Edges theory: " << (p * p * p * p * p - p * p * p * p + p * p * p  - 2 * p * p + 1) / 2 << std::endl;

    //R.certify();
    //std::cout << "Automorphisms: " << R.aut().order() << std::endl;
    //find_anything(R);

    return 0;

    int r_max = 0;
    int verts = R.size();
    for (int i = 0; i < verts; ++i) {
        std::cout << i << " " << r_max << std::endl;
        for (int j = i + 1; j < verts; ++j) 
        for (int k = j + 1; k < verts; ++k) {
            int r = 0;
            for (int x = 0; x < verts; ++x) {
                if (R.edge(i, x) && R.edge(j, x) && R.edge(k, x)) {
                    ++r;
                }
            }
            if (r > r_max) {
                r_max = r;
            }
        }
    }

    std::cout << "Maximum triple degree is " << r_max << std::endl;

    return 0;


    std::string address = "/home/ivan/Projects/Extremal/some/EX(20, K33).gr";
    int n = 20;

    std::fstream stream;
    stream.open(address, std::ios::in | std::ios::binary);
    Graph G(n);
  /*  readGraph(stream, G);
    readGraph(stream, G);

    std::vector<std::pair<int, int>> edges;
    for (int i = 0; i < 20; ++i)
    for (int j = i + 1; j < 20; ++j) {
        if (G.edge(i, j)) {
            edges.emplace_back(i,j);
        }
    }*/
/*
    Graph H(90);
    for (int i = 0; i < 90; ++i) {
        for (int j = i + 1; j < 90; ++j) {
            if (edges[i].first == edges[j].first || edges[i].first == edges[j].second ||
                edges[i].second == edges[j].first || edges[i].second == edges[j].second) {
                H.addEdge(i, j);
            }
        }
    }

    H.certify();
    std::cout << "H order is " << H.aut().order() << std::endl;
*/

    while (readGraph(stream, G)) {
        G.certify();
        /*std::vector degs = G.getDegrees();
        for (int d : degs) {
            std::cout << d << " ";
        } */
        std::cout << G.aut().order() << std::endl;
        std::cout << "isomorphic to R " << isomorphic(G, R) << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
