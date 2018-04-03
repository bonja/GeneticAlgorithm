
#include <cstdio>
#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <string>
#include <list>

#include <time.h>

using namespace std;

int TIME_LIMIT = 5;
int POPULATION_SIZE = 500;
int CROSS_PER_GENERATION = 200;

class Chromosome {
private:
	int _score;
	bool _score_set;
	int _gen_size;
	char* _gen = NULL;

public:
	Chromosome& operator=(const Chromosome& rval) {
		_score = rval._score;
		_score_set = rval._score_set;
		_gen_size = rval._gen_size;

		_gen = new char[_gen_size];
		for (int i=0; i<_gen_size; ++i) {
			_gen[i] = rval._gen[i];
		}
		return *this;
	}

	bool operator==(const Chromosome& rval) {
		if (_gen_size != rval._gen_size)
			return false;

		for (int i=0; i<_gen_size; ++i) {
			if (_gen[i] != rval._gen[i])
				return false;
		}

		return true;
	}
private:
	void init(int gen_size, const char *gen) {
		_gen_size = gen_size;
		_score = 0;
		_score_set = false;
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
		// Fisrt one should be 1 by the definition.
		_gen[0] = 1;
	}

public:
	Chromosome(int gen_size) {
		init(gen_size, NULL);
	}
	Chromosome(int gen_size, char *gen) {
		init(gen_size, gen);
	}

	// GA implementations
public:
	// Xover
	Chromosome(const Chromosome& p1,
			const Chromosome& p2, float p1_w, float p2_w) {
		_score = 0;
		_score_set = false;

		_gen_size = p1._gen_size;
		_gen = new char[_gen_size];
		for (int i=0; i<_gen_size; ++i) {
			_gen[i] = p1._gen[i];
		}
	}

	// Mutation
	void mutation(float weight) {
		// Not for first gen that should be 1 always
		for (int i=1; i<_gen_size; ++i) {
			if ((double)rand()/RAND_MAX < weight) {
				_gen[i] = _gen[i] == 0 ? 1 : 0;
			}
		}
	}

public:
	void print() {
#ifdef _PRINT_DEBUG
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
#endif
	}

	~Chromosome() {
		if (_gen != NULL) {
			delete(_gen);
		}
	}

	const char *get_pattern() const {
		return _gen;
	}

	int get_pattern_size() const {
		return _gen_size;
	}

public:
	void set_score(int score) {
		_score = score;
		_score_set = true;
	}

	bool get_score (int* score) const {
		*score = _score;
		return _score_set;
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

			// Need to add for reverse
			GraphEdge* edge_rev = new GraphEdge(v_to, v_from, weight);
			_edges[v_to]->push_back(edge_rev);
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
			//		cout << v_0 << " -> " << (*iter)->_v_to << " : " << (*iter)->_weight << endl;
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

int get_score(Chromosome &chrom, EdgeGraphReader &eg) {
	int score = 0;
	bool prev_calc = false;

	prev_calc = chrom.get_score(&score);
	if (prev_calc == false) {
		score = eg.score(chrom.get_pattern());
		chrom.set_score(score);
	}

	return score;
}

Chromosome get_random_champ(EdgeGraphReader &eg) {
	time_t begin, now;
	begin = time(NULL);

	Chromosome champ(eg.get_vertex_size());
	while(true) {
		if ((time(NULL) - begin) > (TIME_LIMIT - 1)) {
			break;
		}
		Chromosome chrom(eg.get_vertex_size());
		
		int score = get_score(chrom, eg);
		int champ_score = get_score(champ, eg);

		if (score > champ_score) {
			champ = chrom;
		}
	}

	return champ;
}
void selection(EdgeGraphReader &eg, Chromosome* population[],
		Chromosome **p1, Chromosome **p2) {
	int idx_p1 = rand()%POPULATION_SIZE;
	int idx_p2 = rand()%POPULATION_SIZE;

	if (idx_p1 == idx_p2) {
		idx_p2 = (idx_p2 + 1)%POPULATION_SIZE;
	}

	*p1 = population[idx_p1];
	*p2 = population[idx_p2];
}

void replace(EdgeGraphReader &eg, Chromosome* population[], Chromosome* offspring[]) {
}

Chromosome get_GA_champ(EdgeGraphReader &eg) {

	time_t begin, now;
	begin = time(NULL);

	// Champion
	Chromosome champ(eg.get_vertex_size());

	// Population Initialize
	Chromosome* population[POPULATION_SIZE];
	for (int i=0; i<POPULATION_SIZE; ++i) {
		population[i] = new Chromosome(eg.get_vertex_size());

		int champ_score = get_score(champ, eg);
		int score = get_score(*population[i], eg);

		if (score > champ_score) {
			champ = *population[i];
		}
	}

	// Life goes on...
	Chromosome* offspring[CROSS_PER_GENERATION] = { NULL, };
	while(true) {
		//TODO - optimize every time check
		if ((time(NULL) - begin) > (TIME_LIMIT - 1)) {
			break;
		}
		
		for (int i=0; i<CROSS_PER_GENERATION; ++i) {
			Chromosome *p1, *p2;
			// weight for each parent
			float p1_w = 0.0, p2_w = 0.0;

			selection(eg, population, &p1, &p2);
			offspring[i] = new Chromosome(*p1, *p2, p1_w, p2_w);

			// mutate ratio
			float m_w = 0.05;
			offspring[i]->mutation(m_w);

			// New champ?
			int champ_score = get_score(champ, eg);
			int score = get_score(*offspring[i], eg);
			if (score > champ_score) {
				champ = *offspring[i];
			}
		}

		replace(eg, population, offspring);

		for (int i=0; i<CROSS_PER_GENERATION; ++i) {
			if (offspring[i] != NULL) {
				delete(offspring[i]);
				offspring[i] = NULL;
			}
		}
	}

	// Terminate
	for (int i=0; i<POPULATION_SIZE; ++i) {
		delete(population[i]);
	}
	return champ;
}

int main() {

	// Fix rand seed
	srand(3850);

	EdgeGraphReader eg = EdgeGraphReader(string("maxcur.in"));

	/*
	Chromosome rand_champ = get_random_champ(eg);
	cout << "Random champ : " << get_score(rand_champ, eg) << endl;
	*/

	Chromosome GA_champ = get_GA_champ(eg);
	cout << "GA champ : " << get_score(GA_champ, eg) << endl;

	return 0;
}
