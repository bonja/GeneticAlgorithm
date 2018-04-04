
#include <cstdio>
#include <cstring>
#include <stdlib.h>

#include <iostream>

#include <string>
#include <list>
#include <algorithm>
#include <vector>

#include <time.h>
#include <assert.h>

#include "EdgeGraphReader.h"

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
	EdgeGraphReader *_eg;

public:
	Chromosome& operator=(const Chromosome& rval) {
		_score = rval._score;
		_score_set = rval._score_set;
		_gen_size = rval._gen_size;
		_eg = rval._eg;

		_gen = new char[_gen_size];
		memcpy(_gen, rval._gen, _gen_size*sizeof(char));
		return *this;
	}

	bool operator<(const Chromosome& rval) {
		assert(_score_set);
		assert(rval._score_set);
		return (_score < rval._score);
	}

	bool operator>(const Chromosome& rval) {
		assert(_score_set);
		assert(rval._score_set);
		return (_score > rval._score);
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
	Chromosome(const Chromosome& rval) {
		_score = rval._score;
		_score_set = rval._score_set;
		_gen_size = rval._gen_size;
		_eg = rval._eg;

		_gen = new char[_gen_size];
		memcpy(_gen, rval._gen, _gen_size*sizeof(char));
	}
private:
	void init(const char *gen, EdgeGraphReader *eg) {
		_eg = eg;

		_score = 0;
		_score_set = false;

		_gen_size = eg->get_vertex_size();
		_gen = new char[_gen_size];
		if (gen == NULL) {
			// Generate random one
			for(int i=0; i<_gen_size; ++i) {
				_gen[i] = rand()%2;
			}
		} else {
			memcpy(_gen, gen, _gen_size*sizeof(char));
		}
		// Fisrt one should be 1 by the definition.
		_gen[0] = 1;
	}

public:
	Chromosome(EdgeGraphReader *eg) {
		init(NULL, eg);
	}
	Chromosome(char *gen, EdgeGraphReader *eg) {
		init(gen, eg);
	}

	// GA implementations
public:
	// Xover
	Chromosome(const Chromosome& p1,
			const Chromosome& p2, float p1_w, float p2_w) {
		_eg = p1._eg;

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

	/*
	const char *get_pattern() const {
		return _gen;
	}

	int get_pattern_size() const {
		return _gen_size;
	}
	*/

public:
	int get_score() {
		if (!_score_set) {
			_score = _eg->score(_gen);
			_score_set = true;
		}

		return _score;
	}


private:
	Chromosome() {}
};

// Fully random search
Chromosome get_random_champ(EdgeGraphReader &eg) {
	time_t begin, now;
	begin = time(NULL);

	Chromosome champ(&eg);
	while(true) {
		if ((time(NULL) - begin) > (TIME_LIMIT - 1)) {
			break;
		}
		Chromosome chrom(&eg);
		
		int score = chrom.get_score();
		int champ_score = champ.get_score();

		if (score > champ_score) {
			champ = chrom;
		}
	}

	return champ;
}

// GA implementations
void selection(EdgeGraphReader &eg, vector<Chromosome> &population,
		Chromosome **p1, Chromosome **p2) {
	int idx_p1 = rand()%population.size();
	int idx_p2 = rand()%population.size();

	if (idx_p1 == idx_p2) {
		idx_p2 = (idx_p2 + 1)%population.size();
	}

	*p1 = &population[idx_p1];
	*p2 = &population[idx_p2];
}

void replace(EdgeGraphReader &eg, vector<Chromosome> &population,
		vector<Chromosome> &offsprings) {
}

Chromosome get_GA_champ(EdgeGraphReader &eg) {

	time_t begin, now;
	begin = time(NULL);

	// Champion
	Chromosome champ(&eg);

	// Population Initialize
	vector<Chromosome> population;
	for (int i=0; i<POPULATION_SIZE; ++i) {
		population.push_back(Chromosome(&eg));
	}

	for (vector<Chromosome>::iterator it = population.begin();
			it != population.end(); ++it) {
		int champ_score = champ.get_score();
		int score = (*it).get_score();

		if (score > champ_score) {
			champ = *it;
		}
	}
	sort(population.begin(), population.end());

	/* sort check
	for (vector<Chromosome>::iterator it = population.begin();
			it != population.end(); ++it) {
		int score = get_score(*it, eg);
		cout << score << endl;
	}
	*/

	// Life goes on...
	while(true) {
		//TODO - optimize every time check
		if ((time(NULL) - begin) > (TIME_LIMIT - 1)) {
			break;
		}

		vector<Chromosome> offsprings;
		for (int i=0; i<CROSS_PER_GENERATION; ++i) {
			Chromosome *p1, *p2;
			// weight for each parent
			float p1_w = 0.0, p2_w = 0.0;

			selection(eg, population, &p1, &p2);
			Chromosome child(*p1, *p2, p1_w, p2_w);

			// mutate ratio
			float m_w = 0.05;
			child.mutation(m_w);

			// New champ?
			int champ_score = champ.get_score();
			int score = child.get_score();
			if (score > champ_score) {
				champ = child;
			}

			offsprings.push_back(child);
		}

		replace(eg, population, offsprings);
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
	cout << "GA champ : " << GA_champ.get_score() << endl;

	return 0;
}
