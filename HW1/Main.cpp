
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <limits.h>

#include <iostream>

#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <vector>

#include <time.h>
#include <assert.h>

#include "EdgeGraphReader.h"

using namespace std;

int TIME_LIMIT = 180;
int POPULATION_SIZE = 800;
int CROSS_PER_GENERATION = 400;
int CUT_COUNT = 10;

int SELECTION_HIGH_RATE = 30;
int SAMPLING_COUNT = 3;
float SAMPLING_START = 0.01;
float SAMPLING_GRAD = 100;

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

		memcpy(_gen, rval._gen, _gen_size*sizeof(char));
		return *this;
	}

	bool operator<(Chromosome& rval) {
		return (get_score() < rval.get_score());
	}

	bool operator>(Chromosome& rval) {
		return (get_score() > rval.get_score());
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

public:
	const char* get_gen() const {
		return _gen;
	}

	float get_similarity(const Chromosome& compare) {
		assert(_gen_size == compare._gen_size);

		int match = 0;
		const char* compare_gen = compare.get_gen();
		for(int i=0; i<_gen_size; ++i) {
			if (_gen[i] == compare_gen[i]) {
				match += 1;
			}
		}

		return float(match) / (float)_gen_size;
	}

	// GA implementations
public:
	// Xover
	Chromosome(const Chromosome& p1, const Chromosome& p2) {
		assert(p1._gen_size == p2._gen_size);

		const char *p1_gen = p1.get_gen();

		init(p1_gen, p1._eg);

		int cut_count = CUT_COUNT;

		cross_over(p2, cut_count);
	}

	void cross_over(const Chromosome& partner, int cut_count) {
		const char *partner_gen = partner.get_gen();

		set<int> idxs_cut;
		while(idxs_cut.size() < cut_count) {
			idxs_cut.insert(rand()%(_gen_size+1));
		}

		bool my_turn = true;
		set<int>::iterator it = idxs_cut.begin();
		for(int i=0; i<_gen_size; ++i) {
			if ((it != idxs_cut.end()) && ((*it) == i)) {
				my_turn = my_turn ? false : true;
				++it;
			}

			if (!my_turn) {
				//_gen[i] = partner_gen[i];
				_gen[i] = my_turn ? _gen[i] : partner_gen[i];
			}
		}
	}

	// Xover - check diversity
	Chromosome(const Chromosome& p1,
			const Chromosome& p2, float grad_diversity) {
		assert(p1._gen_size == p2._gen_size);

		const char *p1_gen = p1.get_gen();
		const char *p2_gen = p2.get_gen();

		init(p1.get_gen(), p1._eg);

		if (grad_diversity > 0.3)	grad_diversity = 0.3;
		if (grad_diversity < -0.3)	grad_diversity = -0.3;

		// p1 is better, so choose a even number of cut.
		// more cut will increase diversity.
		int cut_count = ((int)((float)_gen_size / 15.0 + (float)_gen_size / 4.0 * (grad_diversity+0.3) / 0.6)) * 2;

		/*
		cout << "grad : " << grad_diversity << endl;
		cout << "cut_count : " << cut_count << endl;
		*/

		set<int> idxs_cut;
		while(idxs_cut.size() < cut_count) {
			idxs_cut.insert(rand()%(_gen_size+1));
		}

		bool p1_turn = true;
		set<int>::iterator it = idxs_cut.begin();
		for(int i=0; i<_gen_size; ++i) {
			if ((it != idxs_cut.end()) && ((*it) == i)) {
				p1_turn = p1_turn ? false : true;
				++it;
			}
			_gen[i] = p1_turn ? p1_gen[i] : p2_gen[i];
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
	time_t begin;
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

class Diversity {

private:
	int _gen_size;
	double _population_size;
	int _population_size_half;

	int *_gen_data;
	int _gen_data_count;

	double _deviation_sum;

public:
	Diversity(int gen_size, vector<Chromosome> &population) :
		_gen_size(gen_size)
	{
		_gen_data = new int[_gen_size];
		_gen_data_count = population.size();

		_population_size = (double)population.size();
		for(vector<Chromosome>::iterator it = population.begin();
				it != population.end(); ++it) {
			const char* gen = (*it).get_gen();
			for(int i=0; i<_gen_size; ++i) {
				if (gen[i] == 0) {
					++_gen_data[i];
				}
			}
		}
		
		_deviation_sum = 0.0;
		_population_size_half = _population_size / 2.0;
		for(int i=0; i<_gen_size; ++i) {
			_deviation_sum += abs(_gen_data[i] - _population_size_half);
		}
	}

	~Diversity() {
		delete(_gen_data);
	}

public:
	void add_gen(const char *gen) {
		for (int i=0; i<_gen_size; ++i) {
			if (gen[i] == 0) {
				_deviation_sum += _gen_data[i] < _population_size_half ? -1 : 1;
				++_gen_data[i];
			}
		}
		++_gen_data_count;
	}

	void remove_gen(const char *gen) {
		for (int i=0; i<_gen_size; ++i) {
			if (gen[i] == 0) {
				_deviation_sum += _gen_data[i] > _population_size_half ? -1 : 1;
				--_gen_data[i];
			}
		}
		--_gen_data_count;
	}

public:
	float get_diversity() {
		return (1.0 - _deviation_sum / (double)_gen_data_count / _population_size * 2.0);
	}

private:
	Diversity() {}
};

// GA implementations
void selection(EdgeGraphReader &eg, vector<Chromosome> &population,
		Chromosome **p1, Chromosome **p2) {

	// Pick a parent in the high 30 percent
	int idx_p1 = (population.size() - 1) - (rand()%(population.size()/SELECTION_HIGH_RATE));

	// Pick a bride
	int idx_p2 = rand()%population.size();
	if (idx_p1 == idx_p2) {
		idx_p2 = (idx_p2 + 1)%population.size();
	}

	*p1 = &population[idx_p1];
	*p2 = &population[idx_p2];
}

void selection_with_similarity(EdgeGraphReader &eg, vector<Chromosome> &population,
		Chromosome **p1, Chromosome **p2, float target_similarity) {

	// Pick a parent in the high 30 percent
	int idx_p1 = (population.size() - 1) - (rand()%(population.size()/30));

	// Pick a bride
	int idx_p2 = rand()%population.size();
	if (idx_p1 == idx_p2) {
		idx_p2 = (idx_p2 + 1)%population.size();
	}
	float diff_brid = target_similarity - population[idx_p1].get_similarity(population[idx_p2]);
	if (diff_brid < 0.0)	diff_brid *= -1;

	// Sampling to find a most similar bride
	for (int i=0; i<SAMPLING_COUNT; ++i) {
		int idx_cand = rand()%population.size();

		if ((idx_p1 == idx_cand) || (idx_p2 == idx_cand))
			continue;

		float diff_cand = target_similarity - population[idx_p1].get_similarity(population[idx_cand]);
		if (diff_cand < 0.0)	diff_cand *= -1;

		// Swapping
		if (diff_cand < diff_brid) {
			idx_p2 = idx_cand;
			diff_brid = diff_cand;
		}
	}

	/*
	cout << target_similarity << " : " << diff_brid << endl;
	*/

	*p1 = &population[idx_p1];
	*p2 = &population[idx_p2];
}

void replace(EdgeGraphReader &eg, vector<Chromosome> &population,
		vector<Chromosome> &offsprings, float *score_avg, int *score_max, int *score_min) {

	// force to replace with children
	int offsprings_size = offsprings.size();

	/*
	int idx_target = -1;
	for(vector<Chromosome>::iterator it = offsprings.begin();
			it != offsprings.end(); ++it) {
		idx_target += 1;

		*score_avg -= (float)population[idx_target].get_score()/(float)population.size();
		*score_avg += (float)(*it).get_score()/(float)population.size();

		population[idx_target] = (*it);
	}
	*/

	/* pick best peoples => converge too fast
	int prev_population_size = population.size();

	for(vector<Chromosome>::iterator it = offsprings.begin();
			it != offsprings.end(); ++it) {
		population.push_back(*it);
		*score_avg += (float)(*it).get_score()/(float)prev_population_size;
	}

	sort(population.begin(), population.end());

	while(population.size() > prev_population_size) {
		*score_avg -= (float)population[0].get_score()/(float)prev_population_size;
		population.erase(population.begin());
	}
	*/

	// Sampling check
	for(vector<Chromosome>::iterator it = offsprings.begin();
			it != offsprings.end(); ++it) {

		int idx_target = rand()%population.size();

		for (int i=0; i<SAMPLING_COUNT; ++i) {
			int idx_cand = rand()%population.size();
			if (idx_target == idx_cand)
				continue;

			if (population[idx_target].get_score() > population[idx_cand].get_score()) {
				idx_target = idx_cand;
			}
		}

		// converge too fast
	//	if((*it).get_score() > population[idx_target].get_score()) {
		*score_avg -= (float)population[idx_target].get_score()/(float)population.size();
		*score_avg += (float)(*it).get_score()/(float)population.size();

		population[idx_target] = *it; 
	}

	//TODO - optimization
	sort(population.begin(), population.end());
	
	*score_max = population[population.size()-1].get_score();
	*score_min = population[0].get_score();
}

void replace_with_diversity(EdgeGraphReader &eg, vector<Chromosome> &population,
		vector<Chromosome> &offsprings, float grad_diversity,
		Diversity &diversity, float *score_avg, int *score_max, int *score_min) {

	set<int> exchanged;

	bool find_similar = grad_diversity > 0 ? false : true;
	for(vector<Chromosome>::iterator it = offsprings.begin();
			it != offsprings.end(); ++it) {

		int idx_target = rand()%population.size();
		float similar_target = (*it).get_similarity(population[idx_target]);

		for (int i=0; i<SAMPLING_COUNT; ++i) {
			int idx_cand = rand()%population.size();
			if (idx_target == idx_cand)
				continue;

			int similar_cand = (*it).get_similarity(population[idx_cand]);
			if (find_similar && ( similar_cand > similar_target )) {
				idx_target == idx_cand;
				similar_target = similar_cand;
			}
		}

		if (exchanged.find(idx_target) == exchanged.end()) {
			diversity.remove_gen(population[idx_target].get_gen());
			diversity.add_gen((*it).get_gen());

			*score_avg -= (float)population[idx_target].get_score()/(float)population.size();
			*score_avg += (float)(*it).get_score()/(float)population.size();

			population[idx_target] = (*it);
			exchanged.insert(idx_target);
		}
	}
	//TODO - optimization
	sort(population.begin(), population.end());

	*score_max = population[population.size()-1].get_score();
	*score_min = population[0].get_score();
}

Chromosome get_GA_champ(EdgeGraphReader &eg) {

	time_t begin = time(NULL);

	// Population Initialize
	vector<Chromosome> population;
	for (int i=0; i<POPULATION_SIZE; ++i) {
		population.push_back(Chromosome(&eg));
	}
	sort(population.begin(), population.end());
	// Champion
	Chromosome champ(population.back());
	
	// Diversity initialize
	Diversity diversity(eg.get_vertex_size(), population);
#ifdef _PRINT_DEBUG
	cout << "Diversity : " << diversity.get_diversity() << endl;
#endif

	// Score static initialize
	float score_avg = 0.0;
	int score_max = 0, score_min = INT_MAX;
	for (vector<Chromosome>::iterator it = population.begin();
			it != population.end(); ++it) {
		int score = (*it).get_score();
		score_avg += (float)score / (float)population.size();
	}
	score_max = population[population.size()-1].get_score();
	score_min = population[0].get_score();

#ifdef _PRINT_DEBUG
	cout << "Score : " << score_avg << ", " << score_max << ", " << score_min << endl;
#endif

#ifdef _PRINT_DEBUG
	int generation = 0;
	time_t prev_remain = 0;
#endif
	// Life goes on...
	while(true) {
		//TODO - optimize every time check
		time_t remain = (TIME_LIMIT - 1) - (time(NULL) - begin);
		if (remain < 0) {
			break;
		}

		/*
		float target_similarity = (float)(remain + 1) / (float)TIME_LIMIT;
		float target_diversity = 0.4 + target_similarity/2;

		float current_diversity = diversity.get_diversity();
		*/

		vector<Chromosome> offsprings;
		for (int i=0; i<CROSS_PER_GENERATION; ++i) {
			Chromosome *p1, *p2;

			selection(eg, population, &p1, &p2);
			Chromosome child(*p1, *p2);
//			selection(eg, population, &p1, &p2, target_similarity);
//			Chromosome child(*p1, *p2, target_diversity - current_diversity);

			// mutate ratio
			float m_w = 0.05 + 0.1*float(remain)/TIME_LIMIT;
			/*
			if (score_avg > (score_min + 0.9*(score_max - score_min))) {
				cout << "Heavy mutation" << endl;
				m_w = 0.5;
			}
			*/
			child.mutation(0.05);

			// New champion
			int champ_score = champ.get_score();
			int score = child.get_score();
			if (score > champ_score) {
				champ = child;
			}

			offsprings.push_back(child);
		}

		replace(eg, population, offsprings, 
				&score_avg, &score_max, &score_min);

		/*
		replace_with_diversity(eg, population, offsprings,
				target_diversity - current_diversity,
				diversity, &score_avg, &score_max, &score_min);
				*/

#ifdef _PRINT_DEBUG
		generation += 1;
		if (prev_remain != remain) {
			prev_remain = remain;
			SAMPLING_COUNT = max(3, int(float(eg.get_vertex_size())*SAMPLING_START));
			SAMPLING_COUNT += float(eg.get_vertex_size())*SAMPLING_GRAD*(1.0-float(remain)/TIME_LIMIT);
			/*
			if (prev_remain == 90) {
				SAMPLING_COUNT = 30;
			}
			*/
			cout << generation << "th generation : " << remain << "/" << TIME_LIMIT << " - " << SAMPLING_COUNT << endl;
//			cout << "Diversity : " << diversity.get_diversity() << endl;
			cout << "Score : " << score_avg << ", " << score_max << ", " << score_min << endl;
		}
#endif
	}

	return champ;
}

int main() {

	// Fix rand seed
//	srand(3850);

	EdgeGraphReader eg = EdgeGraphReader(string("maxcur.in"));
#ifdef _FACTOR_TEST
	/*
	float population_factor[] = {1.0, 1.5, 2.0, 2.5};
	float cross_count_factor[] = {1.0, 1.5, 2.0, 2.5, 3.0};
	float cut_count_factor[] = {5.0, 10.0, 15.0, 20.0};
	*/
	/*
	float population_factor[] = {1.0, 4.0, 8.0};
	float cross_count_factor[] = {1.0, 3.0, 5.0};
	float cut_count_factor[] = {5.0, 15.0, 25.0};
	int select_rate_factor[] = {25, 30, 35};
	int sampling_factor[] = {5, 15, 25};
	*/
	float population_factor[] = {1.0};
	float cross_count_factor[] = {1.5};
	float cut_count_factor[] = {30.0};

	int select_rate_factor[] = {30};
	float sampling_start[] = {0.01, 0.02};
	float sampling_grad[] = {0.03, 0.05, 0.07};

	int p_f_max = sizeof(population_factor) / sizeof(population_factor[0]);
	int c_f_max = sizeof(cross_count_factor) / sizeof(cross_count_factor[0]);
	int cut_f_max = sizeof(cut_count_factor) / sizeof(cut_count_factor[0]);
	int sel_f_max = sizeof(select_rate_factor) / sizeof(select_rate_factor[0]);
	int sam_s_f_max = sizeof(sampling_start) / sizeof(sampling_start[0]);
	int sam_g_f_max = sizeof(sampling_grad) / sizeof(sampling_grad[0]);

	for(int p_f_idx = 0; p_f_idx<p_f_max; ++p_f_idx) {
		for (int c_f_idx =0; c_f_idx<c_f_max; ++c_f_idx) {
			for (int cut_f_idx =0; cut_f_idx<cut_f_max; ++cut_f_idx) {
				for (int sel_f_idx =0; sel_f_idx<sel_f_max; ++sel_f_idx) {
					for (int sam_s_f_idx =0; sam_s_f_idx<sam_s_f_max; ++sam_s_f_idx) {
						for (int sam_g_f_idx =0; sam_g_f_idx<sam_g_f_max; ++sam_g_f_idx) {
						float p_factor = population_factor[p_f_idx];
						float c_factor = cross_count_factor[c_f_idx];
						float cut_factor = cut_count_factor[cut_f_idx];
						float sel_factor = select_rate_factor[sel_f_idx];
						float sam_s_factor = sampling_start[sam_s_f_idx];
						float sam_g_factor = sampling_grad[sam_g_f_idx];

						POPULATION_SIZE = eg.get_vertex_size()*p_factor;
						CROSS_PER_GENERATION = POPULATION_SIZE/c_factor;
						CUT_COUNT = eg.get_vertex_size()/cut_factor;
						SELECTION_HIGH_RATE = sel_factor;
						SAMPLING_START = sampling_start[sam_s_f_idx];
						SAMPLING_GRAD = sampling_grad[sam_g_f_idx];

						Chromosome GA_champ = get_GA_champ(eg);
						cout << GA_champ.get_score() << "," \
							<< p_factor << "," << c_factor << "," \
							<< cut_factor << "," << sel_factor << "," << sam_s_factor << "," << sam_g_factor << endl;
						}
					}
				}
			}
		}
	}

#else
	/*
	POPULATION_SIZE = eg.get_vertex_size()*1.5;
	CROSS_PER_GENERATION = POPULATION_SIZE/2;
	CUT_COUNT = eg.get_vertex_size()/15;
	*/
	/*
	POPULATION_SIZE = eg.get_vertex_size()*5;
	CROSS_PER_GENERATION = POPULATION_SIZE/2;
	CUT_COUNT = eg.get_vertex_size()/5;
	*/
	/*
	POPULATION_SIZE = eg.get_vertex_size()*2.5;
	CROSS_PER_GENERATION = POPULATION_SIZE/3;
	CUT_COUNT = eg.get_vertex_size()/15;
	*/
	POPULATION_SIZE = eg.get_vertex_size()*1.0;
	CROSS_PER_GENERATION = POPULATION_SIZE/1.5;
	CUT_COUNT = eg.get_vertex_size()/30;

	SELECTION_HIGH_RATE = 30;
	SAMPLING_START = 0.02;
	SAMPLING_GRAD = 0.05;

#ifdef _RAND_TEST
	Chromosome rand_champ = get_random_champ(eg);
	cout << "Random champ : " << rand_champ.get_score() << endl;
#else
	Chromosome GA_champ = get_GA_champ(eg);
	cout << "GA champ : " << GA_champ.get_score() << endl;
#endif
#endif
	// TODO - dynamic cut count
	// TODO - dynamic mutation ratio
	// TODO - dynamic selection - polynomial? exponetial?

	return 0;
}
