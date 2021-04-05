#include <iostream>
#include <vector>
#include <ctime> 
using namespace std;

const float max_distance = 10.0f;

inline float prob() {
	return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
}

inline float genRandDistance() {
	return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (max_distance)));
}

//==========================================================
//	Element "node"
//==========================================================

struct node {
	node() :vertice(0), costToReach(0.0f) {}
	node(int v) :vertice(v) { costToReach = genRandDistance(); }
	node(int v, float c) :vertice(v), costToReach(c) {}
	int vertice; float costToReach;
};

inline node operator++(node& myNode)
{
	myNode.vertice++;
	return myNode;
}

ostream& operator<< (ostream& out, const node& myNode)
{
	out << "(" << myNode.vertice << " ," << myNode.costToReach << ") ";
	return out;
}

bool operator < (const node& lhs, const node& rhs)
{
	if (lhs.vertice < rhs.vertice)
		return true;
	else
		return false;
}

bool operator > (const node& lhs, const node& rhs)
{
	if (lhs.vertice > rhs.vertice)
		return true;
	else
		return false;
}

bool operator == (const node& lhs, const node& rhs)
{
	if (lhs.vertice == rhs.vertice)
		return true;
	else
		return false;
}

bool operator == (const node& lhs, int& rhs)
{
	if (lhs.vertice == rhs)
		return true;
	else
		return false;
}

//==========================================================
//	Linked List "slist"
//==========================================================
template <typename T>
struct slistelem {
	slistelem<T>() {}
	T data = 0;
	slistelem* next = nullptr;
};

template <typename T>
class slist { // singly linked list
public:
	slist() :h(0) {} //empty list
	~slist(); // { release(); } // destructor
	void prepend(T c); // adds to front of slist
	void prependOrdered(T c); // adds in order of vertex number
	void release();
	void del();
	void delete_chosen(int x);
	slistelem<T>* first() const { return h; }
	void print() const;
private:
	slistelem<T>* h; // list head
};

template <typename T>
void slist<T>::prepend(T c)
{
	slistelem<T>* temp = new slistelem<T>;
	//allocation fails bad_alloc exception thrown
	temp->next = h; //single link
	temp->data = c;
	h = temp; // update h
}

template <typename T>
void slist<T>::prependOrdered(T myElem)
{
	if ((h != nullptr) && (h->data == myElem))
		return; // same vertex, no need to insert again
	if ((h == nullptr) || (h->data > myElem))
		prepend(myElem);	// if the list is empty or the new element is already 
							// the smallest one, just prepent
	else
	{
		slistelem<T>* tempToIterate;
		tempToIterate = h;
		while (tempToIterate->next != nullptr && tempToIterate->next->data < myElem)
		{
			tempToIterate = tempToIterate->next;
		}
		if ((tempToIterate->next != nullptr) && (tempToIterate->next->data == myElem))
			return;
		slistelem<T>* temp = new slistelem<T>;
		//allocation fails bad_alloc exception thrown
		temp->next = tempToIterate->next; //single link
		temp->data = myElem;
		tempToIterate->next = temp; // update next
	}
}

template <typename T>
void slist<T>::print() const // object is unchanged
{
	slistelem<T>* temp = h;
	while (temp != 0) { // detect end of slist
		cout << temp->data << " -> ";
		temp = temp->next;
	}
	cout << "###" << endl;
}

template <typename T>
void slist<T>::del()
{
	slistelem<T>* temp = h;
	h = h->next; // presumes nonempty slist
	delete temp;
}

template <typename T>
void slist<T>::delete_chosen(int x)
{
	if (h == nullptr)
		return;
	slistelem<T>* temp = h;
	slistelem<T>* temp_delete = nullptr;
	if (h->data == x)
	{
		temp_delete = h;
		h = temp_delete->next;
		delete temp_delete;
	}
	else
	{
		while (temp != nullptr)
		{
			if (temp->next->data == x)
			{
				temp_delete = temp->next;
				temp->next = temp_delete->next;
				delete temp_delete;
				return;
			}
			else
			{
				temp = temp->next;
			}
		}
	}
}

template <typename T>
void slist<T>::release()
{
	while (h != 0)
		del();
}

template <typename T>
slist<T>::~slist() {
	release();	// march thru list with deletes
}

//==========================================================
//	Class "graph"
//==========================================================

template <typename T>
class graph {
public:
	graph() :m_nVertices(0), m_nEdges(0) {};
	graph(int N, float density) :m_nVertices(N), m_nEdges(0) {
		m_edgeList.reserve(N);
		for (int i = 0; i < m_nVertices; ++i) {
			m_edgeList.emplace_back(slist<T>());
		}

		genGraphWDensity(density);
	};
	void genGraphWDensity(float density);
	void print();
	int V() { return m_nVertices; }
	int E() { return m_nEdges; }
	bool adjacent(int x, int y);
	void neighbors(int x);
	void add(int x, int y);
	void delete_edge(int x, int y);

private:
	int m_nVertices;
	int m_nEdges;
	vector<slist<node>> m_edgeList;
};

template <typename T>
void graph<T>::genGraphWDensity(float density) {
	for (int i = 0; i < m_nVertices; ++i)
		for (int j = 0; j < m_nVertices; ++j)
			if (i != j) // no loops
			{
				if (prob() < density)
				{
					float distance = genRandDistance();
					m_edgeList[i].prependOrdered({ j, distance });
					m_edgeList[j].prependOrdered({ i, distance });
					m_nEdges++;
				}
			}
}

template <typename T>
void graph<T>::print() {
	for (int i = 0; i < m_nVertices; ++i)
	{
		cout << "Vertex " << i << ": ";
		m_edgeList[i].print();
	}
}

template <typename T>
bool graph<T>::adjacent(int x, int y) {
	if ((x >= m_nVertices) || (y >= m_nVertices))
		return false;
	else
	{
		slistelem<T>* temp = m_edgeList[x].first();
		while (temp != nullptr) {
			if (temp->data == y)
				return true;
			else
				if (temp->next != nullptr)
					temp = temp->next;
				else
					return false;
		}
		return false;
	}
}

template <typename T>
void graph<T>::neighbors(int x) {
	if (x < m_nVertices)
	{
		cout << "all nodes that can reach node " << x << " :" << endl;
		m_edgeList[x].print();
	}
}

template <typename T>
void graph<T>::add(int x, int y) {
	if (!(adjacent(x, y)))
	{
		float distance = genRandDistance();
		m_edgeList[x].prependOrdered({ y, distance });
		m_edgeList[y].prependOrdered({ x, distance });
		m_nEdges++;
	}
}

template <typename T>
void graph<T>::delete_edge(int x, int y) {
	if (adjacent(x, y))
	{
		m_edgeList[x].delete_chosen(y);
		m_edgeList[y].delete_chosen(x);
		m_nEdges--;
	}
}

int main()
{
	// seeding the random number generator
	srand((unsigned)time(NULL));

	std::cout << "Type the number of vertices of the graph\n";
	int N;
	cin >> N;

	std::cout << "Type the graph density\n";
	float density;
	cin >> density;

	graph<node> myGraph(N, density);
	cout << "printing graph: " << endl;
	myGraph.print();
	cout << "Number of vertices: " << myGraph.V() << endl;
	cout << "Number of edges: " << myGraph.E() << endl;
	cout << endl;

	// Testing member function "adjacent":
	cout << "Type node to check adjacents: " << endl;
	int my_node_nr;
	cin >> my_node_nr;

	for (int i = 0; i < 10; i++)
	{
		cout << "check if adjacent: " << my_node_nr << " and " << i << ": " << myGraph.adjacent(my_node_nr, i) << endl;
	}

	// Testing member function "neighbors":
	myGraph.neighbors(my_node_nr);

	// Testing member function "add":
	int my_node_nr2;
	cout << "Type nodes x and y to add an edge connecting them: " << endl;
	cout << "Type x: ";
	cin >> my_node_nr;
	cout << "Type y: ";
	cin >> my_node_nr2;
	myGraph.add(my_node_nr, my_node_nr2);
	cout << "printing graph: " << endl;
	myGraph.print();
	cout << "New number of edges: " << myGraph.E() << endl;
	cout << endl;

	// Testing member function "delete_edge"
	cout << "Type nodes x and y to remove the edge connecting them: " << endl;
	cout << "Type x: ";
	cin >> my_node_nr;
	cout << "Type y: ";
	cin >> my_node_nr2;
	myGraph.delete_edge(my_node_nr, my_node_nr2);
	cout << "printing graph: " << endl;
	myGraph.print();
	cout << "New number of edges: " << myGraph.E() << endl;
	cout << endl;

}
