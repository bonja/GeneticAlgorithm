#include <fstream>
#include <iostream>

#include "EdgeGraphReader.h"

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
#ifdef _PRINT_DEBUG
		cout << _v_from << " -> " << _v_to << " (" << _weight << ")";
#endif
	}

private:
	_GraphEdge() {}

} GraphEdge;


EdgeGraphReader::EdgeGraphReader(string filename) {
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

		// Need to add for reverse
		GraphEdge* edge_rev = new GraphEdge(v_to, v_from, weight);
		_edges[v_to]->push_back(edge_rev);
	}
}

EdgeGraphReader::~EdgeGraphReader() {
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

int EdgeGraphReader::get_vertex_size()
{
	return _vertex_count - 1;
}

int EdgeGraphReader::score(const char* pattern) {
	int score = 0;

	for(int i=0; i<get_vertex_size(); ++i) {

		if (pattern[i] != 0)
			continue;

		int v_0 = i+1;
		list<GraphEdge*>::iterator iter;
		for (iter = _edges[v_0]->begin(); iter != _edges[v_0]->end(); ++iter) {
			if (pattern[(*iter)->_v_to-1] == 1) {
#ifdef _PRINT_DEBUG
		//		cout << v_0 << " -> " << (*iter)->_v_to << " : " << (*iter)->_weight << endl;
#endif
				score += (*iter)->_weight;
			}
		}
	}

	return score;
}


void EdgeGraphReader::print() {
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

