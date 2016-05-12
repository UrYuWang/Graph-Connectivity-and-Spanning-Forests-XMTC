#ifdef CHECK_CORRECTNESS
#include "checks.c"
#endif

//#define PRINT
//#define BFS
#define DFS


#define NIL -1

#define WHITE 0
#define GRAY  1
#define BLACK 2



int parent[N];
int dist[N];
int col[N];
int state_node[N] = {WHITE};

/////////////////////// BFS ///////////////////////////

#ifdef BFS
struct Queue {
    int pointer;
    int Q[N];
} my_Q;


void BFS(struct Queue my_Q, int root){
    dist[root] = 0;
    my_Q.Q[my_Q.pointer] = root;
    my_Q.pointer++;
    int current;
    int i;
    while (my_Q.pointer != 0) {
        my_Q.pointer--;
        current = my_Q.Q[my_Q.pointer];
        //printf("current is : %d \n", current);
        for (i=0; i< degrees[current]; i++) {
            int n = edges[vertices[current]+i][1];
            if (dist[n] == NIL) {
                D[n] = root;
                spanforest[spanforest_size][0] = current;
                spanforest[spanforest_size][1] = n;
                spanforest_size++;
                
                dist[n] = dist[current] + 1;
                parent[n] = current;
                my_Q.Q[my_Q.pointer] = n;
                my_Q.pointer++;
                if(degrees[n]==1){col[n] = BLACK;}
            }
            else{col[n] = BLACK;}
        }
    }
    return;
}
#endif


/////////////////////// DFS ///////////////////////////
#ifdef DFS
void DFS_func (int u, int s){
    D[u] = s;
    state_node[u] = BLACK;
    for (int i=0; i<degrees[u]; i++) {
        int v = edges[vertices[u]+i][1];
        if(state_node[v] == WHITE){
            spanforest[spanforest_size][0] = u;
            spanforest[spanforest_size][1] = v;
            spanforest_size++;
            DFS_func(v,s);
        }
    }
    return;
}
#endif




int main(void) {
    //  TODO compute connectivity
    int i;
    
#ifdef PRINT
    printf("N : %d\n\n\n",N);
    printf("M : %d\n\n\n",M);
    printf("Edges are \n");
    for (i=0; i<M; i++) {
        printf("%d -> %d \n", edges[i][0], edges[i][1]);
    }
    printf("\n\n");
    printf("Degrees are \n");
    for (i=0; i<N; i++) {
        printf("%d : %d\n", i, degrees[i]);
    }
    printf("\n\n");
#endif
    
    spanforest_size = 0;
    
    
#ifdef BSF
     for (i=0; i<N; i++) {
        parent[i] = NIL;
        dist[i] = NIL;
        col[i] = WHITE;
     }
    
    
    my_Q.pointer = 0;
    for (i=0; i<N; i++) {
        my_Q.Q[i] = 0;
    }

    for (i=0; i<N; i++) {
        if(col[i] == WHITE) {
            D[i] = i;
            //printf("\n\nroot : %d \n", i);
            BFS(my_Q, i);
        }
    }
#endif

    
#ifdef DFS
    for (i=0; i<N; i++) {
        if(state_node[i] == WHITE) {DFS_func(i, i);}
        }
#endif
  
    
#ifdef PRINT
    printf("Node\tParent\tDist\troot\tCol\n");
    for (i=0; i<N; i++) {
        printf("%d\t%d\t%d\t%d\t%d \n",i,parent[i], dist[i], D[i], col[i]);
    }
    
    printf("spanforest_size : %d \n",spanforest_size);
    for (i=0; i<spanforest_size; i++) {
        printf("%d - %d \n",spanforest[i][0],spanforest[i][1]);
    }
#endif
    
    
#ifdef CHECK_CORRECTNESS
    check_correctness(CHECK_ALL);
#endif

    return 0;
} 
