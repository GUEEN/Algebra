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

std::string CertToString(const Certificate& cert)
{
	std::string s = "";
	for (int i = 0; i < cert.n; i++)
		s += cert[i];
	return s;
}

Certificate Cert(const std::string& s)
{
	int n = s.length();
	Certificate cert(n);
	for (int i = 0; i < n; i++)
		cert[i] = s[i];
	return cert;
}

void Structure::setReadStream(std::string path)
{
	stream.open(path, std::ios::in|std::ios::binary);
}

void Structure::writeStruct(const Certificate& cert)
{
	stream.write(cert.ptr(), cert.n);
}

void Structure::readStruct(Certificate& cert)
{
	stream.read(cert.ptr(), cert.n);
}

extern int compareCertificates(const Certificate& C, const Certificate& D)
{
	if (C.size() > D.size()) return -1;
	if (C.size() < D.size()) return 1;

	int l = C.size();
	int i = 0;

	while (i < l && C[i] == D[i]) i++;

	if (i >= l) return 0;

	if(C[i] > D[i]) return -1;
	if(C[i] < D[i]) return 1;
}

void Structure::certify()
{

	if (TopSearchNode && TopSearchNode->G)
		delete (TopSearchNode->G);
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

	int s = degsize();
	Top->Degg = new Deg[n];
	for (int i = 0; i < n; i++)
		Top->Degg[i].renew(s);
		
	Cell C(n);
	for (int i = 0; i < n; i++)		
		C.V[i] = i;

	Top->P.push_back(C);
	Top->NFixed = 0;
	Top->Depth = 0;
	Top->CellOrbits.id(n);

	Top->stabilise();
	
	cert = getCertificate(Top->B);
		
	delete[] Top->Degg;	
}

Group Structure::aut()
{

	if (TopSearchNode && TopSearchNode->G)
		delete (TopSearchNode->G);
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

	int s = degsize();
	Top->Degg = new Deg[n];
	for (int i = 0; i < n; i++)
		Top->Degg[i].renew(s);
		
	Cell C(n);

	for (int i = 0; i < n; i++)		
		C.V[i] = i;

	Top->P.push_back(C);
	Top->NFixed = 0;
	Top->Depth = 0;
	Top->CellOrbits.id(n);

	Top->stabilise();
	
	Group* AutoPtr = TopSearchNode->G;
	
	delete[] Top->Degg;

	return *AutoPtr;
}

// comparing for sort. Must return true if x goes before y
bool SearchNode::Compare(const int& x, const int& y)
{
	int s = SearchNode::Degg[x].n;
	if (Degg[y].n < s)
		return false;
	if (Degg[y].n > s)
		return true;
	
	int i = 0;
	while (i<s && Degg[x][i] == Degg[y][i]) i++;

	if (i >= s) return false;
	else
		if (Degg[x][i] < Degg[y][i])
			return true;
		else
			return false;
}

int SearchNode::orbitRep(int v)
{
	if (CellOrbits[v] < 0)
		return v;
	int w = orbitRep(CellOrbits[v]);
	CellOrbits[v] = w;
	return w;
}

inline void SearchNode::merge(int uRep, int vRep)
{

	int uSize = - CellOrbits[uRep];
	int vSize = - CellOrbits[vRep];

	int w;
	if (uSize < vSize)
	{
		CellOrbits[uRep] = vRep;
		w = vRep;
	}
	else
	{
		CellOrbits[vRep] = uRep;
		w = uRep;
	}
	CellOrbits[w] = -uSize - vSize;	
}

inline void SearchNode::updateOrbits(const Perm& Q)
{
	if (P.empty())
		return;

	Cell C = P[0];
	
	for (int ii = 0; ii < C.V.n; ii++)
	{
		int u = C[ii];
		int v = Q[u];
			
		int uRep = orbitRep(u);
        int vRep = orbitRep(v);		

        if (uRep != vRep)
			merge(uRep, vRep);
	}
}

void SearchNode::addGen(const Perm& P)
{
	if (G->Gu == nullptr)
		G->Gu = new Group(G->n);
	if (Next != nullptr)
		Next->G = G->Gu;	
	
	if (G->u == -1)
	{
		if (FixedPoint < G->n && FixedPoint >= 0)
			G->u = FixedPoint;
		else
		{
			G->u = 0;
			while (P[G->u] == G->u ) G->u++;
		}		
		
		G->Orbit[0] = G->u;
		G->Cosets[G->u].id(G->n);	
	}
		
	G->Generators.push_back(P);

	if (!CellOrbits.isConst(0))	
		updateOrbits(P);		

	int M = G->NPoints;
	int k = 0;
	while (k < M)
	{
		int v = G->Orbit[k];
		int w = P[v];

		if (G->Cosets[w].empty())
		{			
			G->Orbit[G->NPoints] = w;
			G->NPoints++;

			G->Cosets[w] = P * G->Cosets[v];
			G->Inverses[w] = !G->Cosets[w];
		}
		else
		{
			if(G->Inverses[w].empty())			
				G->Inverses[w] = !G->Cosets[w];			
			
			Perm Q = Mult(G->Inverses[w], P, G->Cosets[v]);
				
			if (!G->Gu->contains(Q))
			{
				if (Next)				
					Next->addGen(Q);				
				else
					G->Gu->addGen(Q);
			}
			
		}
		k++;
	}
	// apply new generators to all points
	while (k < G->NPoints)
	{
		int v = G->Orbit[k];		
	
		for (PermIt it = G->Generators.begin(); it!= G->Generators.end(); ++it)
		{	
			Perm Gen = *it;
			int w = Gen[v];
			if (G->Cosets[w].empty())
			{				
				G->Orbit[G->NPoints] = w;
				G->NPoints++;
				G->Cosets[w] = Gen * G->Cosets[v];
				G->Inverses[w] = !G->Cosets[w];
			}
			else
			{
				if (G->Inverses[w].empty())
					G->Inverses[w] = !G->Cosets[w];
				Perm Q = Mult(G->Inverses[w], Gen, G->Cosets[v]);

				if (!G->Gu->contains(Q))
				{
					if (Next)
						Next->addGen(Q);
					else
						G->Gu->addGen(Q);
				}
			}			
		}
		k++;
	}

}

inline void SearchNode::changeBase(int d)
{
	SearchNode* Node = LastBaseChange;
	Group* G = Node->G;

	if (G)
		if (G->Gu)
	{		
		while (Node)
		{
			if (Node->Depth <= d)
			{
				CellOrbits = 0;
				Cell C = Node->P[0];			
				for (int ii = 0; ii < C.V.n; ii++)
					Node->CellOrbits[C[ii]] = -1;
			}		
			Node = Node->Next;
		}
		Node = LastBaseChange;

		PermList Geners = G->Generators;

		int n = G->n;
		
		// clearing the stabilizer tower from LastBaseChange
		// it is important not to delete any groups for avoiding memory leak

		Group* GG = G;
		while (GG)
		{
			for (int i = 0; i < n; i++)
			{
				GG->Cosets[i].zero();
				GG->Inverses[i].zero();
			}

			GG->Generators.clear();

			GG->u = -1;			
			GG->NPoints = 1;
			GG = GG->Gu;			
		}		
		// backwards
		for (PermIt it = Geners.begin(); it!=Geners.end(); ++it)
			LastBaseChange->addGen(*it);
	}

}

void SearchNode::refine()
{

	// chosing a first non-discrete cell to refine
		
	for (PartIt it = P.begin(); it!= P.end(); ++it)
	{
		(*it).counted = false;
		(*it).discrete = false;
	}

	// if this partition is discrete, it is refined already	

	// if not discrete
	bool Stab = false;
	int s = S->degsize();
	int n = S->n;
		
	for (int i = 0; i < n; i++)		
		Degg[i].constant(0);	
		
	PartIt c = P.begin();	

	do // repeat until we get a stable partition
	{
		Cell C = *c;		
		for (int ii = 0; ii < C.V.n; ii++)
		{			
			for (int jj = 0; jj < n; jj++)
			{
				int col = S->color(C[ii],jj);
				if (col)
					Degg[jj][col-1]++;
			}
		}

		(*c).counted = true;
		Part PP;
		
		for (PartIt it = P.begin(); it!= P.end(); ++it)
		{
			// first we treat discrete cells by adding them to F array
			Cell CC = *it;
			if (CC.V.n == 1)// || C.discrete)
			{				
				for (int ii = 0; ii < CC.V.n; ii++)
				{
					NFixed++;
					F[NFixed - 1] = CC[ii];
				}
				continue;
			}

			// if this is a non-discrete cell					
			CC.V.sort(&SearchNode::Compare);
			CC.discrete = true;
			
			if (CC.V.n > 1)
			for (int ii = 0; ii < CC.V.n - 1; ii++)
			{				
				if (Degg[CC[ii]] == Degg[CC[ii+1]])
				{
					CC.discrete = false;
					break;
				}		
			}  
			// if this cell splits into one-cells, add them to F
			if (CC.discrete)
			{
				//for (IntIt it = C.V.begin(); it != C.V.end(); it++)
				for (int ii = 0; ii < CC.V.n; ii++)
				{
					NFixed++;
					F[NFixed - 1] = CC[ii];
				}
				continue;
			}

			// if there is still one cell
			if (Degg[CC.V[0]] == Degg[CC.V[CC.V.n - 1]])
			{
				PP.push_back(CC);
				continue;
			}				
				
			// splitting C into new cells
			int  l = 0;
			for (int i = 1; i < CC.V.n; i++)
			{
				if (!(Degg[CC.V[i]] == Degg[CC.V[i - 1]]))
				{
					// add a new cell					
					Cell CCC(CC.V, l, i - l);
					PP.push_back(CCC);
					CCC.V.null();
					l = i;
				}
			}
			
			Cell CCC(CC.V, l, CC.V.n - l);
			PP.push_back(CCC);
			CCC.V.null();
			//CC.V.null();
		}

		P = PP;

		// evaluating wether P is a stable partition
		Stab = true;
		IsDiscrete = true;

		c = P.begin();
		while (c!= P.end())
		{
			if (!(*c).counted)
			{
				Stab = false;				
				break;
			}
			else
				++c;
		}

		IsDiscrete = true;
		
		for (PartIt it = P.begin(); it!=P.end(); ++it)
			if ((*it).V.n > 1 && (*it).discrete == false)
			{
				IsDiscrete = false;
				break;
			}
		if (IsDiscrete)
			Stab = true;


	}
	while(!Stab);

	IsDiscrete = true;
	
	for (PartIt it = P.begin(); it!=P.end(); ++it)
		if (!(*it).discrete)
		{
			IsDiscrete = false;
			break;
		}	

}

void SearchNode::stabilise()
{
	int m = NFixed;
	OnBestPath = false;

	refine();
		
	int res = 1;
	
	if (Bexists)
		res = S->compareOrders(F, B, m, NFixed);
	
	int n = S->n;

	if (IsDiscrete)
	{
		if (Bexists)
		{	
			
			if (res == 0) // this means that we have afound an automorphism
			{
				Perm Q(n);
				for (int i = 0; i < n; i++ )
					Q[F[i]] = B[i];
				if (!S->TopSearchNode->G->contains(Q))
				{					
					S->TopSearchNode->addGen(Q);
					AutoFound = true;

				}
			}
			else if (res == 1) // if this ordering is better
			{
				B = F;
				SearchNode* Node = S->TopSearchNode;
				while (Node != nullptr)
				{
					Node->OnBestPath = true;
					Node = Node->Next;
				}
			}
		}
		else
		{
			B = F;			

			SearchNode* Node = S->TopSearchNode;
			while (Node != nullptr)
			{
				Node->OnBestPath = true;
				Node = Node->Next;
			}
			Bexists = true;
		}
		goto Finish;
	}
	else
	{
		if (res == 1)
			Bexists = false;
		else if (res == -1) goto Finish;
		// we get here only if result is 0
		
		if (Next == nullptr)
		{
			Next = new SearchNode(n);
			Next->S = S;
			Next->Depth = Depth + 1;
			if (G)
				Next->G = G->Gu;
		}
		
		CellOrbits = 0;		
		SearchNode* Su = Next;
		
		Cell C = P[0];
				
		for (int ii = 0; ii < C.V.n; ii++)
			CellOrbits[C[ii]] = -1;
				
		int u;
				
		int jj = 0;
				
		while (jj < C.V.n)
		{			
			u = C[jj];
			FixedPoint = u;
			// splitting the first cell into {u}{****}
			Part Pu;
			Cell C1(1);
					
			C1.V[0] = u;

			Pu.push_back(C1);

			Cell C2(C.V.n - 1);
			for (int j = 0; j < jj; j++)
				C2.V[j] = C.V[j];
			for (int j = jj + 1; j < C.V.n; j++)
				C2.V[j - 1] = C.V[j];
						
			Pu.push_back(C2);						
			
			PartIt it = P.begin();
			++it;
						
			while (it!=P.end())
			{
				Pu.push_back(*it);
				++it;
			}
			
			if (Depth > BasisOK)
				changeBase(Depth);

			Su->P = Pu;
			Su->NFixed = NFixed;

			BasisOK = Depth;
			LastBaseChange = this;

			Next->stabilise();

			CellOrbits[orbitRep(u)] -= n;

			if (AutoFound)
			{
				if (!OnBestPath) goto Finish;
				AutoFound = false;
			}
						
			while (jj < C.V.n && CellOrbits[orbitRep(C[jj])] < -n) jj++;
		}		
	}

Finish:
	// zero the Degg array 
	int s = S->degsize();
	for (int i = m; i < NFixed; i++)
		Degg[i].constant(0);	
	FixedPoint = -1;
	P.clear();
}

void StructSet::add(const Certificate& cert)
{
	set.insert(CertToString(cert));
}

void StructSet::write(std::string path, bool Append) const
{
	if (Append)
		Structure::stream.open(path, std::ios::app | std::ios::out | std::ios::binary);
	else
		Structure::stream.open(path, std::ios::out | std::ios::binary);
	for (auto it = set.begin(); it != set.end(); it++)	
		Structure::writeStruct(Cert(*it));
	Structure::stream.close();
}

void StructSet::clear()
{
	set.clear();
}

bool StructSet::contains(const Certificate& cert) const
{
	return set.count(CertToString(cert));
}
