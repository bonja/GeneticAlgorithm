
#include <cstdio>
#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <string>
#include <list>

using namespace std;

class Chromosome {
private:
	int _gen_size;
	char* _gen = NULL;

private:
	void init(int gen_size, char *gen) {
		_gen_size = gen_size;
		_gen = new char[gen_size];
		if (gen == NULL) {
			// Generate random one
			for(int i=0; i<_gen_size; ++i) {
				_gen[i] = rand()%2;
			}
		} else {
			for(int i=0; i<_gen_size; ++i) {
				_gen[i] = gen[i];
			}
		}
	}

public:
	Chromosome(int gen_size) {
		init(gen_size, NULL);
	}
	Chromosome(int gen_size, char *gen) {
		init(gen_size, gen);
	}

	void print() {
		if (_gen != NULL)
		{
			cout << "GEN : ";
			for(int i=0; i<_gen_size; ++i) {
				if (i%10 == 0)
					cout << endl;
				cout << int(_gen[i]);
			}
			cout << endl;
		}
	}

	~Chromosome() {
		if (_gen != NULL) {
			delete(_gen);
		}
	}

	const char *get_pattern() {
		return _gen;
	}
private:
	Chromosome() {}
};

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

class EdgeGraphReader {

private:
	int _vertex_count, _edge_count;
	list<GraphEdge*>** _edges = NULL;

public:
	int get_vertex_size() {
		return _vertex_count - 1;
	}

public:
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

	}

	void print() {
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

	int score(const char* pattern) {
		int score = 0;

		for(int i=0; i<get_vertex_size(); ++i) {

			if (pattern[i] != 0)
				continue;

			int v_0 = i+1;
			list<GraphEdge*>::iterator iter;
			for (iter = _edges[v_0]->begin(); iter != _edges[v_0]->end(); ++iter) {
				if (pattern[(*iter)->_v_to-1] == 1) {
#ifdef _PRINT_DEBUG
					cout << v_0 << " -> " << (*iter)->_v_to << endl;
#endif
					score += (*iter)->_weight;
				}
			}
		}

		return score;
	}

private:
	EdgeGraphReader() {}
};


int main() {
	// Fix rand seed
	srand(3850);

	EdgeGraphReader eg = EdgeGraphReader(string("maxcur.in"));
	eg.print();

	Chromosome chrom(eg.get_vertex_size());
	chrom.print();
	cout << eg.score(chrom.get_pattern()) << endl;

	return 0;
}
