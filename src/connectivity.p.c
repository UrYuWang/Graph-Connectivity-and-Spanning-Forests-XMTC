#include <xmtc.h>

/*******************************************************************
 * ATTENTION: declare any psBaseReg variables right below, before  *
 *            any function definitions                             *
 *******************************************************************/
psBaseReg psb0;
psBaseReg psb1;



#ifdef CHECK_CORRECTNESS
#include "checks.c"
#endif

//#define PRINT
//#define PRINT1
//#define PRINT2
//#define PRINT3
//#define PRINT4



int D_quit[N] = {1}; // 0: Not Quit, 1: Quit
int root_star[N] = {0}; // 0: not a rooted star 1: rooted star
int D_write[N];
int gl_var[N] = {0};

int main(void) {

    //  TODO compute connectivity
    int i,k = 0;
    psb0 = 0;

#ifdef PRINT
    printf("N : %d\n\n\n",N);
    printf("M : %d\n\n\n",M);
    printf("Edges are \n");
    for (i=0; i<M; i++) {
        printf("%d - %d \n", edges[i][0], edges[i][1]);
    }
    printf("\n\n");
    printf("Degrees are \n");
    for (i=0; i<N; i++) {
        printf("%d : %d\n", i, degrees[i]);
    }
    printf("\n\n");
#endif

    spawn(0,N-1){D[$] = $;}
    spawn(0,N-1){D_write[$] = $;}

    while (1) {

        psb1 = 0;

        ////////////////////////////// Step 1 //////////////////////////////
        spawn(0,N-1){
            D_quit[$] = 1;
            root_star[$] = 1;
            gl_var[$] = 0;
        }

        spawn(0,M-1){ //Step 1.1
            int local_var = 1;
            int u = edges[$][0];
            int v = edges[$][1];
            if (D[u] != D[v]) {
                D_quit[D[u]] = 0;
                ps(local_var,psb1);
            }
        }

        spawn(0,N-1){
            if(D[$] != D[D[$]]) {
                root_star[$] = 0;
                root_star[D[$]] = 0;
                root_star[D[D[$]]] = 0;
            }
        }

        spawn(0,N-1){
            int local_var = 1;
            if(D[$] == $){ //For each root 1.2
                if(root_star[$] == 0) {
                    D_quit[$] = 0; //Not quit
                    ps(local_var,psb1);
                }
          }
        }

        if(psb1 == 0) break;

        
#ifdef PRINT1
        
        printf("\n\n\n After Step 1 | Round %d\n",k+1);
        for (i=0; i<N; i++) {
            printf("%d\t : %d\t D_quit : %d\t root_star : %d\t gl_var : %d \n", i,D[i], D_quit[i],root_star[i],gl_var[i]);
        }
        printf("\n\n\nspanforest_size : %d\n",psb0);
#endif

        ////////////////////////////// Step 2 //////////////////////////////
        spawn(0,M-1){
            int u = edges[$][0];
            int val = 1;
            int local_var = 1;
            if(D[u] == D[D[u]] && D_quit[D[u]] == 0){
                int v = edges[$][1];
                if(D[v] < D[u]) {
                    psm(local_var, gl_var[D[u]]);
                    if (local_var==0) {
                        ps(val, psb0);
                        spanforest[val][0] = u;
                        spanforest[val][1] = v;
                        
                        D_write[D[u]] = D[v];
                        root_star[D[u]] = 0; //Revalidate being rooted star
                        root_star[D[v]] = 0; //Revalidate being rooted star
                    }
                }
            }
        }


        spawn(0,N-1){D[$] = D_write[D_write[$]];}
        
#ifdef PRINT2
        printf("\n\n\n After Step 2 | Round %d\n",k+1);
        for (i=0; i<N; i++) {
            printf("%d\t : %d\t D_quit : %d\t root_star : %d\t gl_var : %d \n", i,D[i], D_quit[i],root_star[i],gl_var[i]);
        }
        printf("\n\n\nspanforest_size : %d\n",psb0);
#endif

        ////////////////////////////// Step 3 //////////////////////////////
        spawn(0,M-1){
            int u = edges[$][0];
            int val = 1;
            int local_var = 1;
            if(root_star[D[u]] == 1 && D_quit[D[u]] == 0){
                int v = edges[$][1];
                if(D[v] > D[u]) {
                    psm(local_var, gl_var[D[u]]);
                    if(local_var==0)
                    {
                        ps(val, psb0);
                        spanforest[val][0] = u;
                        spanforest[val][1] = v;

                        D_write[D[u]] = D[v];
                    }
                }
            }
        }

        spawn(0,N-1){D[$] = D_write[D_write[$]];}
        
#ifdef PRINT3
        //printf("\n\n\nFlag1 : %d, Flag2 : %d, Flag3 : %d\n\n\n",flag1,flag2,flag3);
        printf("\n\n\n After Step 3 | Round %d\n",k+1);
        for (i=0; i<N; i++) {
            printf("%d\t : %d\t D_quit : %d\t root_star : %d\t gl_var : %d \n", i,D[i], D_quit[i],root_star[i],gl_var[i]);
        }
        printf("\n\n\nspanforest_size : %d\n",psb0);
#endif


        ////////////////////////////// Step 4 //////////////////////////////
        spawn(0,N-1){
            D_write[$] = D[D[$]];
        }
        
        spawn(0,N-1){D[$] = D_write[D_write[$]];}
        spawn(0,N-1){D_write[$] = D[$];}


#ifdef PRINT4
        printf("\n\n\n After Step 4 | Round %d\n",k+1);
        for (i=0; i<N; i++) {
            printf("%d\t : %d\t D_quit : %d\t root_star : %d\t gl_var : %d \n", i,D[i], D_quit[i],root_star[i],gl_var[i]);
        }
        printf("\n\n\nspanforest_size : %d\n",psb0);
#endif


    }//end of for

    spanforest_size = psb0;


#ifdef CHECK_CORRECTNESS
    check_correctness(CHECK_ALL);
#endif

    return 0;
} 
