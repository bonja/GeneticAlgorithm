
#include <cstdio>
#include <iostream>
#include <fstream>

#include <string>

#include <list>

using namespace std;

typedef struct _GraphEdge {
public:
	int _v_from, _v_to;
	int _weight;

	// Constructor
public:
	_GraphEdge(int v_from, int v_to, int weight):
		_v_from(v_from),
		_v_to(v_to),
		_weight(weight)
	{
	}

	void print() {
		cout << _v_from << " -> " << _v_to << " (" << _weight << ")";
	}

private:
	_GraphEdge() {}

} GraphEdge;

class EdgeGraphReader {
public:
	int _vertex_count, _edge_count;
	list<GraphEdge*>** _edges = NULL;

	EdgeGraphReader(string filename) {
		ifstream fs(filename.c_str());

		// First line
		// vertex_count, edge_count
		string line;
		getline(fs, line);
		sscanf (line.c_str(), "%d %d", &_vertex_count, &_edge_count);
		
		// vertex idx starting from 1
		++_vertex_count;

		_edges = new list<GraphEdge*>*[_vertex_count]; // vertex idx starting from 1
		for (int i=0; i<_vertex_count; ++i) {
			_edges[i] = new list<GraphEdge*>();
		}

		// After all
		// v_from v_to weight
		for( string line; getline( fs, line ); ) {
			int v_from = -1, v_to = -1, weight = 0;

			if (line.length() <= 0) {
				continue;
			}

			sscanf(line.c_str(), "%d %d %d", &v_from, &v_to, &weight);

			GraphEdge* edge = new GraphEdge(v_from, v_to, weight);

			_edges[v_from]->push_back(edge);
		}

#ifdef _PRINT_DEBUG
		list<GraphEdge*>::iterator iter;
		for (int i=0; i<_vertex_count; ++i) {
			cout << "V(" << i << ") : ";
			for (iter = _edges[i]->begin(); iter != _edges[i]->end(); ++iter) {
				(*iter)->print();
			}
			cout << endl;
		}
#endif
	}
	~EdgeGraphReader() {
		if (_edges != NULL) {
			list<GraphEdge*>::iterator iter;
			for (int i=0; i<_vertex_count; ++i) {
				for (iter = _edges[i]->begin(); iter != _edges[i]->end(); ++iter) {
					delete(*iter);
				}
				delete(_edges[i]);
			}
			delete(_edges);
		}
	}

private:
	EdgeGraphReader() {}
};

int main() {
	EdgeGraphReader eg = EdgeGraphReader(string("maxcur.in"));

	return 0;
}
