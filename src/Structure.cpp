#include <algorithm>

#include "Structure.h"

Deg* SearchNode::Degg;
const Structure* SearchNode::S;
bool SearchNode::IsDiscrete;
bool SearchNode::AutoFound;
bool SearchNode::Bexists;
SearchNode* SearchNode::LastBaseChange;
SearchNode* Structure::TopSearchNode;
int SearchNode::BasisOK;

std::fstream Structure::stream;

Perm SearchNode::B;
Perm SearchNode::F;

Cell::Cell() : Perm(), counted(false), discrete(false) {
};

Cell::Cell(size_t m): Perm(m), counted(false), discrete(false) {
};

Cell::Cell(const Cell& W, int s, int m): counted(false), discrete(false), Perm(m) {
    for (size_t i = 0; i < m; ++i) {
        data_[i] = W[s + i];
    }
};

void Cell::sort(const std::function<int(int,int)>& comp) {
    std::sort(data_, data_ + size_, comp);
}

std::string CertToString(const Certificate& cert) {
    std::string s;
    for (int i = 0; i < cert.size(); i++) {
        s.push_back(cert[i]);
    }
    return s;
}

Certificate Cert(const std::string& s) {
    size_t n = s.length();
    Certificate cert(n);
    for (size_t i = 0; i < n; i++) {
        cert[i] = s[i];
    }
    return cert;
}
/*
void Structure::writeStructList(std::string path, StructList& List, bool Append)
{
	if (Append)
		stream.open(path, std::ios::app | std::ios::out | std::ios::binary);
	else
		stream.open(path, std::ios::out|std::ios::binary);
	List.traverse(&Structure::writeStruct);		
	stream.close();
}*/

Structure::Structure(size_t n): n(n) {
};

size_t Structure::size() const {
    return n;
}

void Structure::setReadStream(const std::string& path) {
    stream.open(path, std::ios::in|std::ios::binary);
}

void Structure::writeStruct(const Certificate& cert) {
    stream.write((char*)cert.data(), cert.size());
}

void Structure::readStruct(Certificate& cert) {
    stream.read((char*)cert.data(), cert.size());
}

int compareCertificates(const Certificate& C, const Certificate& D) {
    if (C.size() > D.size()) {
        return -1;
    }
    if (C.size() < D.size()) {
        return 1;
    }
    size_t l = C.size();
    size_t i = 0;

    while (i < l && C[i] == D[i]) {
        i++;
    }

    if (i >= l) {
        return 0;
    }

    if (C[i] > D[i]) {
        return -1;
    }
    if (C[i] < D[i]) {
        return 1;
    }
}

bool isomorphic(const Structure& s, const Structure& t) {
    int r = compareCertificates(*s.cert, *t.cert);
    return r == 0;
}

void Structure::certify() {
    if (TopSearchNode) {
        delete TopSearchNode->G;
    }
    delete Structure::TopSearchNode;

    Structure::TopSearchNode = new SearchNode(n);
    SearchNode* Top = Structure::TopSearchNode;	

    Top->B.id(n);	
    Top->F.id(n);
    Top->S = this;
    Top->Bexists = false;
    Top->BasisOK = 0;
    Top->AutoFound = false;
    Top->LastBaseChange = Top;
    Top->G = new Group(n);

    size_t s = degsize();
    Top->Degg = new Deg[n];
    for (size_t i = 0; i < n; i++) {
        Top->Degg[i].assign(s, 0);
    }
	
    Cell C(n);

    Top->P.push_back(C);
    Top->NFixed = 0;
    Top->Depth = 0;
    Top->CellOrbits.id(n);

    Top->stabilise();

    cert = std::make_shared<Certificate>(getCertificate(Top->B));
    delete[] Top->Degg;
}

Group Structure::aut() {
    if (TopSearchNode && TopSearchNode->G) {
        delete (TopSearchNode->G);
    }
    delete Structure::TopSearchNode;

    Structure::TopSearchNode = new SearchNode(n);
    SearchNode* Top = Structure::TopSearchNode;	

    Top->B.id(n);	
    Top->F.id(n);
    Top->S = this;
    Top->Bexists = false;
    Top->BasisOK = 0;
    Top->AutoFound = false;
    Top->LastBaseChange = Top;
    Top->G = new Group(n);

    size_t s = degsize();
    Top->Degg = new Deg[n];
    for (size_t i = 0; i < n; i++) {
        Top->Degg[i].assign(s, 0);
    }

    Cell C(n);

    Top->P.push_back(C);
    Top->NFixed = 0;
    Top->Depth = 0;
    Top->CellOrbits.id(n);

    Top->stabilise();
	
    Group* AutoPtr = TopSearchNode->G;

    delete[] Top->Degg;
    return *AutoPtr;
}

SearchNode::SearchNode(size_t n) : G(nullptr), Next(nullptr), OnBestPath(false), CellOrbits(n) {
};

SearchNode::~SearchNode() {
    delete Next;
};

// comparing for sort. Must return true if node x goes before y
bool SearchNode::Compare(int x, int y) {
    size_t s = SearchNode::Degg[x].size();
    if (Degg[y].size() < s) {
        return false;
    }
    if (Degg[y].size() > s) {
        return true;
    }

    size_t i = 0;
    while (i < s && Degg[x][i] == Degg[y][i]) {
        i++;
    }

    if (i >= s) {
        return false;
    }

    if (Degg[x][i] < Degg[y][i]) {
        return true;
    }
    return false;
}

int SearchNode::orbitRep(size_t v) {
    if (CellOrbits[v] < 0) {
        return v;
    }
    int w = orbitRep(CellOrbits[v]);
    CellOrbits[v] = w;
    return w;
}

void SearchNode::merge(size_t u, size_t v) {
    int u_size = -CellOrbits[u];
    int v_size = -CellOrbits[v];

    int w;
    if (u_size < v_size) {
        CellOrbits[u] = v;
        w = v;
    } else {
        CellOrbits[v] = u;
        w = u;
    }
    CellOrbits[w] = -u_size - v_size;
}

void SearchNode::updateOrbits(const Perm& Q) {
    if (P.empty()) {
        return;
    }

    const Cell& C = P.front();
    for (size_t i = 0; i < C.size(); i++) {
        int u = C[i];
        int v = Q[u];
    	
        int uRep = orbitRep(u);
        int vRep = orbitRep(v);		

        if (uRep != vRep) {
            merge(uRep, vRep);
        }
    }
}

void SearchNode::addGen(const Perm& P) {
    if (G->Gu == nullptr) {
        G->Gu = new Group(G->n);
    }
    if (Next != nullptr) {
        Next->G = G->Gu;
    }

    if (G->u == -1) {
        if (FixedPoint < G->n && FixedPoint >= 0) {
            G->u = FixedPoint;
        } else {
            G->u = 0;
            while (P[G->u] == G->u) {
                G->u++;
            }
        }
		
        G->Orbit[0] = G->u;
        G->Cosets[G->u].id(G->n);	
    }

    G->Generators.push_back(P);

    if (!CellOrbits.isConst(0))	{
        updateOrbits(P);
    }

    size_t M = G->NPoints;
    size_t k = 0;
    while (k < M) {
        int v = G->Orbit[k];
        int w = P[v];

        if (G->Cosets[w].empty()) {			
            G->Orbit[G->NPoints] = w;
            G->NPoints++;

            G->Cosets[w] = P * G->Cosets[v];
            G->Inverses[w] = !G->Cosets[w];
        } else {
            if (G->Inverses[w].empty())	{
                G->Inverses[w] = !G->Cosets[w];
            }

            Perm Q = Mult(G->Inverses[w], P, G->Cosets[v]);
            if (!G->Gu->contains(Q)) {
                if (Next) {
                    Next->addGen(Q);
                } else {
                    G->Gu->addGen(Q);
                }
            }
        }
	k++;
    }
    // apply new generators to all points
    while (k < G->NPoints) {
        int v = G->Orbit[k];		
        for (const Perm& Gen : G->Generators) {
            int w = Gen[v];
            if (G->Cosets[w].empty()) {
                G->Orbit[G->NPoints] = w;
                G->NPoints++;
                G->Cosets[w] = Gen * G->Cosets[v];
                G->Inverses[w] = !G->Cosets[w];
            } else {
                if (G->Inverses[w].empty()) {
                    G->Inverses[w] = !G->Cosets[w];
                }
                Perm Q = Mult(G->Inverses[w], Gen, G->Cosets[v]);
                if (!G->Gu->contains(Q)) {
                    if (Next) {
                        Next->addGen(Q);
                    } else {
                        G->Gu->addGen(Q);
                    }
                }
            }			
        }
        k++;
    }
}

void SearchNode::changeBase(int d) {
    SearchNode* node = LastBaseChange;
    Group* G = node->G;

    if (G == nullptr) {
        return;
    }
    if (G->Gu == nullptr) {
        return;
    }

    while (node) {
        if (node->Depth <= d) {
            CellOrbits = 0;
            const Cell& C = node->P.front();			
            for (size_t i = 0; i < C.size(); i++) {
                node->CellOrbits[C[i]] = -1;
            }
        }		
        node = node->Next;
    }
    node = LastBaseChange;
    PermList Geners = G->Generators;

    size_t n = G->n;
		
    // clearing the stabilizer tower from LastBaseChange
    // it is important not to delete any groups for avoiding memory leak

    Group* GG = G;
    while (GG) {
        for (size_t i = 0; i < n; i++) {
            GG->Cosets[i].clear();
            GG->Inverses[i].clear();
        }

        GG->Generators.clear();

        GG->u = -1;			
        GG->NPoints = 1;
        GG = GG->Gu;			
    }	
		
    // backwards
    for (const Perm& P : Geners) {
        LastBaseChange->addGen(P);
    }
}

void SearchNode::refine() {
    // chosing a first non-discrete cell to refine
    for (auto it = P.begin(); it != P.end(); ++it) {
        it->counted = false;
        it->discrete = false;
    }

    // if this partition is discrete, it is refined already	

    // if not discrete
    bool Stab = false;
    size_t s = S->degsize();
    size_t n = S->n;
		
    for (size_t i = 0; i < n; i++) {
        Degg[i].assign(s, 0);
    }
		
    auto c = P.begin();	

    do { // repeat until we get a stable partition
        const Cell& C = *c;		
        for (size_t i = 0; i < C.size(); i++) {			
            for (size_t j = 0; j < n; j++) {
                int col = S->color(C[i], j);
                if (col) {
                    Degg[j][col - 1]++;
                }
            }
        }

        c->counted = true;
        Part PP;

        for (Cell CC : P) {
	    // first we treat discrete cells by adding them to F array
	    if (CC.size() == 1 || CC.discrete) { //
                for (size_t i = 0; i < CC.size(); i++) {
                    F[NFixed++] = CC[i];
                }
                continue;
            }

            // if this is a non-discrete cell					
            CC.sort(&SearchNode::Compare);
            CC.discrete = true;	
            if (CC.size() > 1) {
                for (size_t i = 0; i + 1 < CC.size(); i++) {
                    if (Degg[CC[i]] == Degg[CC[i + 1]]) {
                        CC.discrete = false;
                        break;
                    }		
                }
            }
            // if this cell splits into one-cells, add them to F
            if (CC.discrete) {
                for (size_t i = 0; i < CC.size(); i++) {
                    F[NFixed++] = CC[i];
                }
                continue;
            }

            // if there is still one cell
            if (Degg[CC[0]] == Degg[CC[CC.size() - 1]]) {
                PP.push_back(std::move(CC));
                continue;
            }
		
            // splitting C into new cells
            int l = 0;
            for (size_t i = 1; i < CC.size(); i++) {
                if (Degg[CC[i]] != Degg[CC[i - 1]]) {
                    // add a new cell					
                    PP.emplace_back(CC, l, i - l);
                    l = i;
                }
            }
            PP.emplace_back(CC, l, CC.size() - l);
        }

        P = PP;
        // evaluating wether P is a stable partition
        Stab = true;
        IsDiscrete = true;

        c = P.begin();
        while (c != P.end()) {
            if (!c->counted) {
                Stab = false;				
                break;
            } else {
                ++c;
            }
        }

        IsDiscrete = true;
        for (auto it = P.begin(); it != P.end(); ++it) {
            if (it->size() > 1 && it->discrete == false) {
                IsDiscrete = false;
                break;
            }
        }
        if (IsDiscrete) {
            Stab = true;
        }
    } while(!Stab);

    IsDiscrete = true;
    for (auto it = P.begin(); it != P.end(); ++it) {
        if (!it->discrete) {
            IsDiscrete = false;
            break;
        }
    }
}

void SearchNode::stabilise() {
    size_t m = NFixed;
    OnBestPath = false;

    refine();
    int res = 1;
    if (Bexists) {
        res = S->compareOrders(F, B, m, NFixed);
    }

    size_t n = S->n;

    if (IsDiscrete) {
        if (Bexists) {
            if (res == 0) { // this means that we have afound an automorphism
                Perm Q(n);
                for (size_t i = 0; i < n; i++) {
                    Q[F[i]] = B[i];
                }
                if (!S->TopSearchNode->G->contains(Q)) {
                    S->TopSearchNode->addGen(Q);
                    AutoFound = true;
                }
            } else if (res == 1) { // if this ordering is better
                B = F;
                SearchNode* Node = S->TopSearchNode;
                while (Node != nullptr) {
                    Node->OnBestPath = true;
                    Node = Node->Next;
                }
            }
        } else {
            B = F;			
            SearchNode* Node = S->TopSearchNode;
            while (Node != nullptr) {
                Node->OnBestPath = true;
                Node = Node->Next;
            }
            Bexists = true;
        }
        goto Finish;
    } else {
        if (res == 1) {
            Bexists = false;
        } else if (res == -1) {
            goto Finish;
        }
        // we get here only if result is 0
        if (Next == nullptr) {
            Next = new SearchNode(n);
            Next->S = S;
            Next->Depth = Depth + 1;
            if (G) {
                Next->G = G->Gu;
            }
        }

        CellOrbits = 0;		
        SearchNode* Su = Next;

        //CellOrbits = new Perm(n);
        const Cell& C = P.front();
        for (size_t i = 0; i < C.size(); i++) {
            CellOrbits[C[i]] = -1;
        }
        int u;
        size_t jj = 0;
				
        while (jj < C.size()) {			
            u = C[jj];
            FixedPoint = u;
            // splitting the first cell into {u}{****}
            Part Pu;
            Cell C1(1);
            C1[0] = u;

            Pu.push_back(std::move(C1));
            Cell C2(C.size() - 1);
            for (size_t j = 0; j < jj; j++) {
                C2[j] = C[j];
            }
            for (size_t j = jj + 1; j < C.size(); j++) {
                C2[j - 1] = C[j];
            }
            Pu.push_back(std::move(C2));
					
            //for (int j = 1; j < P.size(); j++)
            auto it = P.begin();
            ++it;
            while (it != P.end()) {
                Pu.push_back(*it);
                ++it;
            }

            if (Depth > BasisOK) {
                changeBase(Depth);
            }

            Su->P = Pu;
            Su->NFixed = NFixed;

            BasisOK = Depth;
            LastBaseChange = this;

            Next->stabilise();

            CellOrbits[orbitRep(u)] -= n;

            if (AutoFound) {
                if (!OnBestPath) {
                    goto Finish;
                }
                AutoFound = false;
            }				
            while (jj < C.size() && CellOrbits[orbitRep(C[jj])] < -n) {
                jj++;
            }
        }		
    }

Finish:
    // zero the Degg array 
    size_t s = S->degsize();
    for (size_t i = m; i < NFixed; i++) {
        Degg[i].assign(s, 0);
        FixedPoint = -1;
        P.clear();
    }
}

void StructSet::add(const Certificate& cert) {
    set.insert(CertToString(cert));
}

void StructSet::write(std::string path, bool append) const {
    if (append) {
        Structure::stream.open(path, std::ios::app | std::ios::out | std::ios::binary);
    } else {
        Structure::stream.open(path, std::ios::out | std::ios::binary);
    }
    for (auto it = set.begin(); it != set.end(); it++) {
        Structure::writeStruct(Cert(*it));
    }
    Structure::stream.close();
}

size_t StructSet::size() const {
    return set.size();
}

void StructSet::clear() {
    set.clear();
}

bool StructSet::contains(const Certificate& cert) const {
    return set.count(CertToString(cert));
}
