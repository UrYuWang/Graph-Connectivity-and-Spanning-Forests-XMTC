/** HOW TO USE THIS FILE
 *  This file contains 1 function (check_correctness) which takes an argument
 *  'check_mask' that determines which checks to run. There are currently 5 
 *  checks:
 *    o the first checks that the resulting D is made of rooted stars
 *    o the second check converts D by assigning the node with the minimum ID
 *      as the root of each star. It relies on the 1st check being correct, 
 *      and doesn't actually check anything, but "normalizes" D so that it can
 *      be dumped and compared to a correct result
 *    o the third check makes sure the number of edges in the spanning forest 
 *      is correct
 *    o the fourth check makes sure that all edges in the spanning forest were
 *      indeed in the original graph
 *    o the fifth check makes sure there are no cycles in the spanning forest
 *
 *  The checks about D can be accessed using the CHECK_D mask and the checks 
 *  about the spanning forest can be accessed using the CHECK_FOREST mask.
 *  All the checks are enabled with the CHECK_ALL mask.
 *
 *  While the goal is to find the spanning forest, the D array with the 
 *  rooted stars helps us answer the question are v an u in the same connected 
 *  component in O(1) time. For that reason we want to compute it correctly as 
 *  well.
 */
int cluster[N];

#define VERBOSE

#define CHECK1 0x1
#define CHECK2 0x2
#define CHECK3 0x4
#define CHECK4 0x8
#define CHECK5 0x10

#define CHECK_ALL (CHECK1 | CHECK2 | CHECK3| CHECK4 | CHECK5)
#define CHECK_D (CHECK1 | CHECK2)
#define CHECK_FOREST (CHECK3 | CHECK4 | CHECK5)


int check_correctness(int check_mask) {
    int v;

    //CHECK 1. Check that D[D[v]] == D[v]
    if (check_mask & CHECK1) {
        int error1 = 0;
        for (v=0; v<N; v++) if (D[D[v]] != D[v]) {
            error1 += 1;
#ifdef VERBOSE
            printf("v=%d, D[v]=%d, D[D[v]]=%d\n", v, D[v], D[D[v]]);
#endif  
        }
        if (error1>0) {
            printf("FAILED Check 1: %d errors found\n", error1);
#ifndef DO_ALL_CHECKS
            return -1;
#endif
        }
    }
#ifdef VERBOSE
    printf("Done with Check 1\n");
#endif     


    // CHECK 2. Convert array D. choose the node with the min ID in each 
    //    rooted star and set it as the root. This eliminates non-determinism 
    if (check_mask & CHECK2) {
        for (v=0; v<N; v++) if (D[v]>v) {
            // find the min
            int min = v;
            int t;
            int dv = D[v];
            for (t=0; t<N; t++) if (D[t]==D[v] && t<v) min = t;
 
            for (t=0; t<N; t++) if (D[t]==dv) D[t]=min;
        }
#ifdef VERBOSE
        printf("Done with Check 2\n");
#endif     
    }


    // CHECK 3. check that the span-forest size value is correct
    // NOTE: correct_spawnforest_size is initialized using "magic" constants
    //       based on the size of the input (the number of nodes N). If the
    //       input datasets are changed, this code needs to be updated.
    if (check_mask & CHECK3) {
        int correct_spanforest_size=-1;
        if (N==19) correct_spanforest_size = 16; 
        else if (N==50) correct_spanforest_size = 47;
        else if (N==10000) correct_spanforest_size =  9995;
        else if (N==20000) correct_spanforest_size = 11361;
        else { // unexpected input size
            printf("FAILED Check 3: Encountered unexpected input size (N=%d)\n",N);
#ifndef DO_ALL_CHECKS
            return -3;
#endif
        }

        if (spanforest_size != correct_spanforest_size) {
            printf("FAILED Check 3: spanforest_size = %d instead of %d\n", spanforest_size, 
                correct_spanforest_size);
#ifndef DO_ALL_CHECKS
            return -3;
#endif
        }
#ifdef VERBOSE
        printf("Done with Check 3\n");
#endif     
    }
     
    // CHECK 4. check that all edges of the forest are in the original 
    int e;
    int srcNode, destNode;
    if (check_mask & CHECK4) {
        int error4 = 0;
        for (e=0; e<spanforest_size; e++) {
            srcNode = spanforest[e][0];
            if (srcNode<0 || srcNode >= M) { //out of bounds
                printf("FAILED Check 4: spanforest[%d][0]=%d is out of bounds (it should be in 0<=x<%d)\n", 
                    e, srcNode, M);
#ifndef DO_ALL_CHECKS
                return -4;
#endif
            }
            destNode = spanforest[e][1];
            if (destNode<0 || destNode >= M) { //out of bounds
                printf("FAILED Check 4: spanforest[%d][1]=%d is out of bounds (it should be in 0<=x<%d)\n", 
                    e, destNode, M);
#ifndef DO_ALL_CHECKS
                return -4;
#endif
            }
            int t;
            int found = 0;
            for (t=vertices[srcNode]; t < vertices[srcNode]+degrees[srcNode]; t++) {
                if (edges[t][1]==destNode) {
                    found = 1; break;
                }
            }
            if (!found) {
                printf("FAILED Check 4: edge (%d,%d) not found in original graph\n", srcNode, destNode);
#ifndef DO_ALL_CHECKS
                return -4;
#endif
            }
        } 
#ifdef VERBOSE
        printf("Done with Check 4\n");
#endif     
    }

    // CHECK 5. check that there are no cycles in the spanning forest
    //    like Kruskal's MST algorithm start with each verted being a tree
    //    and keep adding edges if they connect 2 trees
    if (check_mask & CHECK5) {
        int srcRoot, destRoot;
        for (v=0; v<N; v++) cluster[v] = v;
        for (e=0; e<spanforest_size; e++) {
            srcNode  = spanforest[e][0];
            destNode = spanforest[e][1];
 
            srcRoot = cluster[srcNode];
            while(srcRoot != cluster[srcRoot]) srcRoot = cluster[srcRoot];

            destRoot = cluster[destNode];        
            while(destRoot != cluster[destRoot]) destRoot = cluster[destRoot];
        
            if (srcRoot != destRoot) cluster[srcRoot] = destRoot;
            else {
                printf("FAILED Check 5: edge (%d,%d) introduces cycle in spaning forest\n",
                    srcNode,destNode);
#ifdef VERBOSE
                int t;
                printf("srcRoot=%d, destRoot=%d, e=%d\n", srcRoot, destRoot, e);
                for (t=0; t<=e; t++) printf("(%d,%d), ", spanforest[t][0], spanforest[t][1]);
                printf("\n");
#endif
#ifndef DO_ALL_CHECKS
                return -5;
#endif
            }
        }
#ifdef VERBOSE
        printf("Done with Check 5\n");
#endif     
    }
    if (check_mask & CHECK_D) {
        printf("PASS: Your program passed the 1st correctness test!\n");
        printf("NOTE: This doesn't mean your code is correct, you also need to dump\n");
        printf("       the D array and 'diff' it with the valid result in the corresponding\n");
        printf("       data/graphX directory\n");
    } else {
        printf ("PASS: Your program passed the correctness tests!\n");
    }
    return 0; // No errors
}
