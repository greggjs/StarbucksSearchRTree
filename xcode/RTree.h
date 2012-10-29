//
//  RTree.h
//  StarbucksSearchRTree
//
//  Created by Jake Gregg on 10/25/12.
//
//

#ifndef StarbucksSearchRTree_RTree_h
#define StarbucksSearchRTree_RTree_h
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#define ASSERT assert
#ifndef Min
#define Min __min
#endif
#ifndef Max
#define Max __max
#endif

#define RTREE_TEMPLATE template<class DATATYPE, class ELEMTYPE, int NUMDIMS, class ELEMTYPEREAL, int TMAXNODES, int TMINNODES>
#define RTREE_QUAL RTree<DATATYPE, ELEMTYPE, NUMDIMS, ELEMTYPEREAL, TMAXNODES, TMINNODES>

template<class DATATYPE, class ELEMTYPE, int NUMDIMS, class ELEMTYPEREAL = ELEMTYPE, int TMAXNODES = 8, int TMINNODES = TMAXNODES / 2>

class RTree {
public:
    struct Node;
    enum { MAXNODES = TMAXNODES, MINNODES = TMINNODES, };
    RTree();
    virtual ~RTree();
    void insert(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], const DATATYPE& id);
    int search(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], bool __cdecl resultCallback(DATATYPE data, void* context), void* context);
    class Itt {
    private:
        enum { MAX_STACK = 32 };
        struct StackElement { Node* curr_node; int branch_index; };
        void init() { tos = 0; };
        bool find_next_data() {
            for (;;) {
                if (tos <= 0)
                    return false;
                StackElement curr = pop();
                if (curr.curr_node->IsLeaf()) {
                    if (curr.branch_index+1 < curr.curr_node->count) {
                        push(curr.curr_node, curr.branch_index + 1);
                        return true;
                    }
                }
                else {
                    if (curr.branch_index+1 < curr.curr_node->count)
                        push(curr.curr_node, curr.branch_index+1);
                    Node* next_node = curr.curr_node->branch[curr.branch_index].child;
                    push(next_node, 0);
                    if (next_node->IsLeaf())
                        return true;
                }
            }
        }
        void push(Node* a_node, int a_branch_index) {
            stack[tos].curr_node = a_node;
            stack[tos].branch_index = a_branch_index;
            ++tos;
            ASSERT(tos <= MAX_STACK);
        }
        StackElement& pop() {
            ASSERT (tos > 0);
            --tos;
            return stack[tos];
        }
        StackElement stack[MAX_STACK];
        int tos;
        friend RTree;
    public:
        Itt()   { init(); }
        ~Itt()  { }
        bool is_null()   { return (tos <= 0); }
        bool is_not_null()    { return (tos > 0); }
        DATATYPE& operator*()
        {
            ASSERT(is_not_null());
            StackElement& curr = stack[tos - 1];
            return curr.curr_node->branch[curr.branch_index].data;
        }
        bool operator++() { return find_next_data(); }
        void get_bounds(ELEMTYPE min[NUMDIMS], ELEMTYPE max[NUMDIMS]) {
            ASSERT(is_not_null());
            StackElement& curr = stack[tos -1];
            Branch& curBranch = curr.curr_node->branch[curr.branch_index];
            
            for (int i = 0; i < NUMDIMS; ++i) {
                min[i] = curBranch.rect_.min[i];
                max[i] = curBranch.rect_.max[i];
            }
        }
        
    };
    void get_first(Itt& itt) {
        itt.init();
        Node* first = root;
        while(first) {
            if (first->IsInternalNode() && first->count_ > 1)
                itt.push(first, 1);
            else if (first->isLeaf()) {
                if (first->count_)
                    itt.push(first, 0);
                break;
            }
            first = first->branch[0].child;
        }
    }
    void get_next(Itt& itt) { ++itt ; }
    bool is_null(Itt& itt) { return itt.is_null(); }
    DATATYPE& get_at(Itt& itt)   { return *itt; }
public:
    struct TreeRect {
        ELEMTYPE min[NUMDIMS];
        ELEMTYPE max[NUMDIMS];
    };
    struct Branch {
        TreeRect rect;
        union {
            Node* child;
            DATATYPE data;
        };
    };
    struct Node {
        bool IsInternalNode()   { return (level > 0); }
        bool IsLeaf()           { return (level == 0); }
        int count;
        int level;
        Branch branch[MAXNODES];
    };
    Node* root;
    struct SplitVar
    {
        int split[MAXNODES+1];
        int total;
        int min_fill;
        int taken[MAXNODES+1];
        int count[2];
        TreeRect cover[2];
        ELEMTYPEREAL area[2];
        
        Branch branch_buf[MAXNODES+1];
        int branch_count;
        TreeRect cover_split;
        ELEMTYPEREAL cover_split_area;
    };
    int count();
    void count_rec(Node* node, int& count);
    Node* alloc_node();
    void init_node(Node* node);
    void init_rect(TreeRect* rect);
    bool insert_rect_rec(TreeRect* rect, const DATATYPE& id, Node* node, Node** new_node, int level);
    bool insert_rect(TreeRect* rect, const DATATYPE& id, Node** root, int level);
    TreeRect node_cover(Node* node);
    bool add_branch(Branch* branch, Node* node, Node** new_node);
    int pick_branch(TreeRect* rect, Node* node);
    ELEMTYPEREAL calc_rect_area(TreeRect* rect);
    TreeRect combine_rect(TreeRect* rect_1, TreeRect* rect_2);
    void split_node(Node* node, Branch* branch, Node** new_node);
    void get_branches(Node* node, Branch* branch, SplitVar* split_var);
    void choose_split(SplitVar* split_var, int min_fill);
    void load_nodes(Node* node_1, Node* node_2, SplitVar* split_var);
    void init_split_var(SplitVar* split_var, int max_rects, int min_fill);
    void pick_seeds(SplitVar* split_var);
    void classify(int index, int group, SplitVar* split_var);
    bool overlap(TreeRect* rect, TreeRect* other_rect);
    bool search(Node* node, TreeRect* rect, int& found_count, bool __cdecl resultCallback(DATATYPE data, void* context), void* context);
};

#endif
