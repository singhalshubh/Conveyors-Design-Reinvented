//Stub for custom BFS implementations
#include "common.h"
#include "aml.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "csr_reference.h"
#ifdef __cplusplus
}
#endif
#include "bitmap_reference.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stdint.h>

#include "libgetput.h"
#include "selector.h"

//VISITED bitmap parameters
unsigned long *visited;
int64_t visited_size;

unsigned long *visited2;
int64_t visited_size2;

int nodes_in_level;

int64_t *pred_glob,*column;
int *rowstarts;
oned_csr_graph g;

typedef struct visitmsg {
	//both vertexes are VERTEX_LOCAL components as we know src and dest PEs to reconstruct VERTEX_GLOBAL
	int vloc;
	int vfrom;
} visitmsg;


class BottomUpBFSSelector: public hclib::Selector<2, visitmsg> {
    int *nodes_in_level;
    int64_t *pred_glob;
    void check_node_visited(visitmsg m, int sender_rank) {
            if(TEST_VISITEDLOC(m.vloc)){
                visitmsg m2 = {m.vfrom, m.vloc};
                send(1, m2, sender_rank);
            }
    }

    void add_to_next_level(visitmsg m, int sender_rank) {
        if (!TEST_VISITEDLOC2(m.vloc)) {

            SET_VISITEDLOC2(m.vloc);
            *nodes_in_level = *nodes_in_level + 1;

            pred_glob[m.vloc] = VERTEX_TO_GLOBAL(sender_rank, m.vfrom);
        }
    }

public:
BottomUpBFSSelector(int* _nodes_in_level, int64_t* _pred_glob): nodes_in_level(_nodes_in_level), pred_glob(_pred_glob) {
        mb[0].process = [this](visitmsg pkt, int sender_rank) { this->check_node_visited(pkt, sender_rank); };
        mb[1].process = [this](visitmsg pkt, int sender_rank) { this->add_to_next_level(pkt,sender_rank); };
    }
};

//user should provide this function which would be called once to do kernel 1: graph convert
void make_graph_data_structure(const tuple_graph* const tg) {
	//graph conversion, can be changed by user by replacing oned_csr.{c,h} with new graph format 
	convert_graph_to_oned_csr(tg, &g);
	column=g.column;
    rowstarts=g.rowstarts;

	visited_size = (g.nlocalverts + ulong_bits - 1) / ulong_bits;
	visited = (long unsigned int*)xmalloc(visited_size*sizeof(unsigned long));

    visited_size2 = (g.nlocalverts + ulong_bits - 1) / ulong_bits;
	visited2 = (long unsigned int*)xmalloc(visited_size2*sizeof(unsigned long));

    nodes_in_level = 0;

    CLEAN_VISITED2();
	//user code to allocate other buffers for bfs
}


void run_bfs_selector(int64_t root, int64_t* pred) {
    int64_t nvisited;
    long sum;
    unsigned int i,j,k,lvl=1;
    int rownext = 0;
    int rowlast = 1;
    int it_num=0;

    pred_glob=pred;

    CLEAN_VISITED();

    sum=1; nodes_in_level = 0;

    nvisited=1;
    if(VERTEX_OWNER(root) == rank) {
        pred[VERTEX_LOCAL(root)]=root;
        SET_VISITED(root);
    }

    // While there are vertices in current level
    while (sum) {
        BottomUpBFSSelector *bfss_ptr = new BottomUpBFSSelector(&nodes_in_level, pred_glob);
        hclib::finish([=]() {
                bfss_ptr->start();
                
                //for all vertices in current node send visit AMs to all neighbours
                for (int i = 0; i < g.nlocalverts; i++) {
                    
                    if (TEST_VISITEDLOC(i)) {
                        continue;
                    }
                    
                    for(int j=rowstarts[i];j<rowstarts[i+1];j++) {
                        //printf("j = %d, dest = %d, m = {%d, %d}\n", j, VERTEX_OWNER(COLUMN(j)), VERTEX_LOCAL(COLUMN(j)), q1[i]);
                        
                        if (TEST_VISITEDLOC(i)) {
                            break;
                        }

                        int dest = VERTEX_OWNER(COLUMN(j));
                        visitmsg m = {VERTEX_LOCAL(COLUMN(j)), i};
                        bfss_ptr->send(0, m, dest);
                    }
                }
                bfss_ptr->done(0);
            });
        lgp_barrier();
        delete bfss_ptr;
        it_num++;

        sum=nodes_in_level;
		aml_long_allsum(&sum);
        //printf("sum = %d\n", sum);
		nvisited+=sum;

        for(int i=0; i<g.nlocalverts; i++){
            if(TEST_VISITEDLOC2(i)){
                SET_VISITEDLOC(i);
            }
        }
        CLEAN_VISITED2();

		nodes_in_level=0;
    }
    lgp_barrier();
}

//user should provide this function which would be called several times to do kernel 2: breadth first search
//pred[] should be root for root, -1 for unrechable vertices
//prior to calling run_bfs pred is set to -1 by calling clean_pred
void run_bfs(int64_t root, int64_t* pred) {
    /*
    static bool first = true;
    if (first) {
        shmem_init();
        first = false;
    }
    */
    
	pred_glob=pred;
	//user code to do bfs
    const char *deps[] = { "system", "bale_actor" };
    //hclib::launch(deps, 2, [=] {
            hclib::finish([=] {
                    run_bfs_selector(root, pred);
                });
    //    });
    //shmem_finalize();
}

//we need edge count to calculate teps. Validation will check if this count is correct
//user should change this function if another format (not standart CRS) used
void get_edge_count_for_teps(int64_t* edge_visit_count) {
	long i,j;
	long edge_count=0;
	for(i=0;i<g.nlocalverts;i++)
		if(pred_glob[i]!=-1) {
			for(j=g.rowstarts[i];j<g.rowstarts[i+1];j++)
				if(COLUMN(j)<=VERTEX_TO_GLOBAL(my_pe(),i))
					edge_count++;
		}
	aml_long_allsum(&edge_count);
	*edge_visit_count=edge_count;
}

//user provided function to initialize predecessor array to whatevere value user needs
void clean_pred(int64_t* pred) {
	int i;
	for(i=0;i<g.nlocalverts;i++) pred[i]=-1;
}

//user provided function to be called once graph is no longer needed
void free_graph_data_structure(void) {
	free_oned_csr_graph(&g);
	free(visited);
}

//user should change is function if distribution(and counts) of vertices is changed
size_t get_nlocalverts_for_pred(void) {
	return g.nlocalverts;
}
