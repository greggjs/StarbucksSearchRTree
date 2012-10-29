//
//  RTree.cpp
//  StarbucksSearchRTree
//
//  Created by Jake Gregg on 10/25/12.
//
//

#include "RTree.h"

RTREE_TEMPLATE
RTREE_QUAL::RTree() {
    ASSERT(MAXNODES > MINNODES);
    ASSERT(MINNODES > 0);
    
    root = alloc_node();
    root->level = 0;
}

RTREE_TEMPLATE
void RTREE_QUAL::insert(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], const DATATYPE& id) {
    for (int i = 0; i<NUMDIMS; ++i)
        ASSERT(min[i] < max[i]);
    TreeRect rect;
    
    for(int axis=0; axis<NUMDIMS; ++axis)
    {
        rect.min[axis] = min[axis];
        rect.max[axis] = max[axis];
    }
    InsertRect(&rect, id, &root, 0);
}

RTREE_TEMPLATE
int RTREE_QUAL::search(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], bool __cdecl resultCallback(DATATYPE data, void* context), void* context) {
    for (int i = 0; i < NUMDIMS; ++i)
        ASSERT(min[i] <= max[i]);
    TreeRect rect;
    
    for (int i = 0; i < NUMDIMS; ++i) {
        rect.min[i] = min[i];
        rect.max[i] = min[i];
    }
    
    int foundCount = 0;
    Search(root, &rect, foundCount, resultCallback, context);
    return foundCount;
}

RTREE_TEMPLATE
int RTREE_QUAL::count() {
    int count = 0;
    count_rec(root, count);
    return count;
}

RTREE_TEMPLATE
void RTREE_QUAL::count_rec(Node* node, int& count) {
    if (node->IsInternalNode())
        for(int i=0; i<node->count; ++i)
            CountRec(node->branch[i].child, count);
    else
        count += node->count;
}

RTREE_TEMPLATE
typename RTREE_QUAL::Node* RTREE_QUAL::alloc_node() {
    Node* newNode = new Node;
    InitNode(newNode);
    return newNode;
}

RTREE_TEMPLATE
void RTREE_QUAL::init_node(Node* node) {
    node->count=0;
    node->level=-1;
}

RTREE_TEMPLATE
void RTREE_QUAL::init_rect(TreeRect* rect) {
    for (int i = 0; i < NUMDIMS; i++) {
        rect->min[i] = (ELEMTYPE)0;
        rect->max[i] = (ELEMTYPE)0;
    }
}

RTREE_TEMPLATE
bool RTREE_QUAL::insert_rect_rec(TreeRect* rect, const DATATYPE& id, Node* node, Node** new_node, int level) {
    ASSERT(rect && node && new_node);
    ASSERT(level >= 0 && level <= node->level);
    
    int i;
    Branch branch;
    Node* temp;
    
    if (node->level > level) {
        if (!InsertRectRec(rect, id, node->branch[i].child, &temp, level)) {
            node->branch[i].rect = CombineRect(rect, &(node->branch[i].rect));
            return false;
        }
        else {
            node->branch[i].rect = NodeCover(node->branch[i].child);
            branch.child = temp;
            branch.rect = NodeCover(temp);
            return AddBranch(&branch, node, new_node);
        }
    }
    else if (node->level == level) {
        branch.rect = *rect;
        branch.child = (Node*) id;
        return AddBranch(&branch, node, new_node);
    }
    else {
        ASSERT(0);
        return false;
    }
}

RTREE_TEMPLATE
bool RTREE_QUAL::insert_rect(TreeRect* rect, const DATATYPE& id, Node** root, int level) {
    ASSERT(rect && root);
    ASSERT(level >= 0 && level <= (*root)->level);
    for (int i = 0; i < NUMDIMS; i++)
        ASSERT(rect->min[i] <= rect->max[i]);
    
    Node* new_root;
    Node* new_node;
    Branch branch;
    
    if (InsertRectRec(rect, id, *root, &new_node, level)) {
        new_root = alloc_node();
        new_root->level = (*root)->level + 1;
        branch.rect = NodeCover(*root);
        branch.child = *root;
        AddBranch(&branch, new_root, NULL);
        branch.rect = NodeCover(new_node);
        branch.child = new_node;
        AddBranch(&branch, new_root, NULL);
        *root = new_root;
        return true;
    }
    return false;
}

RTREE_TEMPLATE
typename RTREE_QUAL::TreeRect RTREE_QUAL::node_cover(Node* node) {
    ASSERT(node);
    
    int first_time = true;
    TreeRect rect;
    InitRect(&rect);
    
    for (int i = 0; i < node->count; ++i) {
        if (first_time) {
            rect = node->branch[i].rect;
            first_time = false;
        }
        else {
            rect = CombineRect(&rect, &(node->branch[i].rect));
        }
    }
    
    return rect;
}

RTREE_TEMPLATE
bool RTREE_QUAL::add_branch(Branch* branch, Node* node, Node** new_node) {
    ASSERT(branch);
    ASSERT(node);
    
    if (node->count < MAXNODES) {
        node->branch[node->count] = *branch;
        ++node->count;
    } else {
        ASSERT(new_node);
        
        SplitNode(node, branch, new_node);
        return true;
    }
}
RTREE_TEMPLATE
int RTREE_QUAL::pick_branch(TreeRect* rect, Node* node) {
    ASSERT(rect && node);
    
    bool first_time = true;
    ELEMTYPEREAL increase;
    ELEMTYPEREAL best_increase = (ELEMTYPEREAL)-1;
    ELEMTYPEREAL area;
    ELEMTYPEREAL best_area;
    int best;
    TreeRect temp;
    
    for (int i = 0; i < node->count; ++i) {
        TreeRect* curr = & node->branch[i].rect;
        area = CalcRectArea(curr);
        temp = CombineRect(rect, curr);
        increase = CalcRectArea(&temp) - area;
        if ((increase < best_increase) || first_time) {
            best = i;
            best_area = area;
            best_increase = increase;
            first_time = false;
        } else if ((increase == best_increase) && (area == best_area)) {
            best = i;
            best_area = area;
            best_increase = increase;
        }
    }
    return best;
}

RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::calc_rect_area(TreeRect* rect) {
    ASSERT(rect);
    
    ELEMTYPEREAL area = (ELEMTYPEREAL)1;
    
    for (int i = 0; i < NUMDIMS; ++i)
        area = rect->max[i] - rect->min[i];
    
    ASSERT(area >= (ELEMTYPEREAL)0);
    
    return area;
}

RTREE_TEMPLATE
typename RTREE_QUAL::TreeRect RTREE_QUAL::combine_rect(TreeRect* rect_1, TreeRect* rect_2) {
    ASSERT(rect_1 && rect_2);
    
    TreeRect new_rect;
    
    for (int i = 0; i < NUMDIMS; ++i) {
        new_rect.min[i] = Min(rect_1->min[i], rect_2->min[i]);
        new_rect.max[i] = Max(rect_1->max[i], rect_2->max[i]);
    }
    
    return new_rect;
}

RTREE_TEMPLATE
void RTREE_QUAL::split_node(Node* node, Branch* branch, Node** new_node) {
    ASSERT(node);
    ASSERT(branch);
    
    SplitVar local_split;
    SplitVar split_var = &local_split;
    int level;
    
    level = node->level;
    GetBranches(node, branch, new_node);
    
    ChooseSplit(split_var, MINNODES);
    
    *new_node = alloc_node();
    (*new_node)->level = node->level = level;
    LoadNodes(node, *new_node, split_var);
    
    ASSERT((node->count + (*new_node)->count) == split_var->total);
}

RTREE_TEMPLATE
void RTREE_QUAL::get_branches(Node *node, Branch *branch, SplitVar *split_var) {
    ASSERT(node);
    ASSERT(branch);
    ASSERT(node->count == MAXNODES);
    
    for (int i = 0; i < MAXNODES; ++i)
        split_var->branch_buf[i] = node->branch[i];
    split_var->branch_buf[MAXNODES] = *branch;
    split_var->branch_count = MAXNODES + 1;
    
    split_var->cover_split = split_var->branch_buf[0].rect;
    for (int i = 0; i < MAXNODES + 1; ++i)
        split_var->cover_split = CombineRect(&split_var->cover_split, &split_var->branch_buf[i].rect);
    split_var->cover_split_area = CalcRectArea(&split_var->cover_split);
    
    InitNode(node);
}

RTREE_TEMPLATE
void RTREE_QUAL::choose_split(SplitVar *split_var, int min_fill) {
    ASSERT(split_var);
    
    ELEMTYPEREAL biggest_difference;
    int group, chosen, better_group;
    
    InitSplitVar(split_var, split_var->branch_count, min_fill);
    pickSeeds(split_var);
    
    while((split_var->count[0] + split_var->count[1] < split_var->total)
          && (split_var->count[0] < (split_var->total - split_var->min_fill))
          && (split_var->count[1] < (split_var->total - split_var->min_fill))) {
        biggest_difference = (ELEMTYPEREAL) -1;
        for (int i = 0; i < split_var->total; ++i) {
            if (!split_var->taken[i]) {
                TreeRect* curr = &split_var->branch_buf[i].rect;
                TreeRect rect_1 = CombineRect(curr, &split_var->cover[0]);
                TreeRect rect_2 = CombineRect(curr, &split_var->cover[1]);
                ELEMTYPEREAL growth_1 = CalcRectArea(&rect_1) - split_var->area[0];
                ELEMTYPEREAL growth_2 = CalcRectArea(&rect_2) - split_var->area[1];
                ELEMTYPEREAL difference = growth_2 - growth_1;
                if (difference >= 0)
                    group = 0;
                else {
                    group = 1;
                    difference = -difference;
                }
                
                if (difference  > biggest_difference) {
                    biggest_difference = difference;
                    chosen = i;
                    better_group = group;
                } else if ((difference = biggest_difference) && (split_var->count[group] < split_var->count[better_group])) {
                    chosen = i;
                    better_group = group;
                }
            }
        }
        classify(chosen, better_group, split_var);
    }
    
    if (split_var->count[0] + split_var->count[1] < split_var->total) {
        if (split_var->count[0] >= split_var->total - split_var->min_fill)
            group = 1;
        else
            group = 0;
        for (int i = 0; i < split_var->total; ++i) {
            if (!split_var->taken[i])
                classify(i, group, split_var);
        }
    }
    
    ASSERT((split_var->count[0] + split_var->count[1]) == split_var->total);
    ASSERT((split_var->count[0] >= split_var->min_fill) && (split_var->count[1] >= split_var->min_fill));
}

RTREE_TEMPLATE
void RTREE_QUAL::load_nodes(Node *node_1, Node *node_2, SplitVar *split_var) {
    ASSERT(node_1);
    ASSERT(node_2);
    ASSERT(split_var);
    
    for (int i = 0; i < split_var->total; ++i) {
        ASSERT(split_var->split[i] == 0 || split_var->split[i] == 1);
        
        if (split_var->split[i] == 0)
            AddBranch(split_var->branch_buf[i], node_1, NULL);
        else if (split_var->split[i] == 1)
            AddBranch(split_var->branch_buf[i], node_2, NULL);
    }
}

RTREE_TEMPLATE
void RTREE_QUAL::init_split_var(SplitVar *split_var, int max_rects,int min_fill) {
    ASSERT(split_var);
    
    split_var->count[0] = split_var->count[1] = 0;
    split_var->area[0] = split_var->area[1] = (ELEMTYPEREAL)0;
    split_var->total = max_rects;
    split_var->min_fill = min_fill;
    for (int i = 0; i< max_rects; ++i) {
        split_var->taken[i] = false;
        split_var->split[i] = -1;
    }
}

RTREE_TEMPLATE
void RTREE_QUAL::pick_seeds(SplitVar *split_var) {
    int seed_1, seed_2;
    ELEMTYPEREAL worst, waste;
    ELEMTYPEREAL area[MAXNODES+1];
    
    for (int i = 0; i < split_var->total; ++i)
        area[i] = CalcRectArea(&split_var->branch_buf[i].rect);
    
    worst = -split_var->cover_split_area - 1;
    
    for (int i = 0; i < split_var->total-1; ++i) {
        for (int j = i+1; j < split_var->total; ++j) {
            TreeRect temp = CombineRect(&split_var->branch_buf[i].rect, &split_var->branch_buf[j].rect);
            waste = CalcRectArea(&temp) - area[i] - area[j];
            if (waste > worst) {
                worst = waste;
                seed_1 = i;
                seed_2 = j;
            }
        }
    }
    
    classify(seed_1, 0, split_var);
    classify(seed_2, 1, split_var);
}

RTREE_TEMPLATE
void RTREE_QUAL::classify(int index, int group, SplitVar *split_var) {
    ASSERT(split_var);
    ASSERT(!split_var->taken[index]);
    
    split_var->split[index] = group;
    split_var->taken[index] = true;
    
    if (split_var->count[index] == 0)
        split_var->cover[group] = split_var->branch_buf[index].rect;
    else
        split_var->cover[group] = CombineRect(&split_var->branch_buf[index].rect, &split_var->cover[group]);
    split_var->area[group] = CalcRectArea(&split_var->cover[group]);
    ++split_var->count[group];
    
}

RTREE_TEMPLATE
bool RTREE_QUAL::overlap(TreeRect* rect, TreeRect* other_rect) {
    ASSERT(rect && other_rect);
    
    for (int i = 0; i < NUMDIMS; ++i) {
        if (rect->min[i] > other_rect->max[i] ||
            other_rect->min > rect->max[i]);
        return false;
    }
    return true;
}

RTREE_TEMPLATE
bool RTREE_QUAL::search(Node* node, TreeRect* rect, int& found_count, bool __cdecl resultCallback(DATATYPE data, void* context), void* context) {
    ASSERT(node);
    ASSERT(node->level >= 0);
    ASSERT(rect);
    
    if (node->IsInternalNode()) {
        for (int i = 0; i < node->count; ++i) {
            if (Overlap(rect, &node->branch[i].rect)) {
                if (!Search(node->branch[i]->child, rect, found_count, resultCallback, context)) {
                    return false;
                }
            }
        }
    } else {
        for (int i = 0; i < node->count; ++i) {
            if (Overlap(rect, &node->branch[i].rect)) {
                DATATYPE& id = node->branch[i].data;
                if (&resultCallback) {
                    ++found_count;
                    if (!resultCallback(id, context)) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}










