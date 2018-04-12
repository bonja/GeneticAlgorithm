#ifndef _EDGE_GRAPH_READER_H_
#define _EDGE_GRAPH_READER_H_

#include <string>
#include <list>

struct _GraphEdge;

class EdgeGraphReader {
private:
	int _vertex_count, _edge_count;
	std::list<struct _GraphEdge*>** _edges = NULL;

public:
	EdgeGraphReader(std::string filename);
	~EdgeGraphReader();

private:
	EdgeGraphReader() {}

public:
	int get_vertex_size();
	void print();
	int score(const char* pattern);
};

#endif
