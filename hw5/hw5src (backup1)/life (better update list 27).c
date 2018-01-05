/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "string.h"
/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/
#define THREAD_NUM 16
#define SCHDULER_THREAD 1



typedef struct info{
    char neighbour_count;
//    char new_count;
    char state;
}info;
typedef struct coord1{
        short i;
        short j;
        char used;
    } coord;
typedef struct status{
    char state;
    char update; 
}status; 

#define STATUS_NUM 32
status status_map[STATUS_NUM];

void init(int nrows, int ncols, char info_buffer[nrows][ncols], char* inboard );
void  update_hori_boundary(int nrows,int ncols, char buffer1[nrows][ncols], char buffer2[nrows][ncols],coord* update_list, coord* head  );
void  update_verti_boundary(int nrows,int ncols, char buffer1[nrows][ncols], char buffer2[nrows][ncols],coord* update_list, coord* head  );
void memcpy2(void* dest, void* src, size_t size);
/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/

    char*
sequential_game_of_life2 (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max);
    char *sequential_game_of_life_new (char* outboard, 
        const int nrows,
        const int ncols,
        char info_buffer[nrows][ncols],
        char info_buffer2[nrows][ncols],
        const int gens_max);


//
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max)
{
//     return sequential_game_of_life2 (outboard, inboard, nrows, ncols, gens_max);
    
    if(nrows<=32 || ncols<=32){
        return sequential_game_of_life (outboard, inboard, nrows, ncols, gens_max);
    }
    
   
    int new_rows = nrows;
    int new_cols= ncols;
    char info_buffer[new_rows][new_cols];
    char info_buffer2[new_rows][new_cols];
    
    
    init(new_rows,new_cols,info_buffer,inboard);
    memcpy(info_buffer2,info_buffer,new_rows*new_cols);
    
    int work_load = nrows/THREAD_NUM;
    
    //create threads
    pthread_t workers[THREAD_NUM];
    pthread_mutex_t locks[work_load];
    pthread_barrier_t worker_barrier;
    pthread_barrier_init(&worker_barrier, NULL , THREAD_NUM);
    
    //init lock
    for(int i=0;i< work_load; ++i){
        pthread_mutex_init(&locks[i],NULL);
    }
    //init thread
    for(int i=0; i< THREAD_NUM; ++i){
        ;
    }
    
    return sequential_game_of_life_new ( outboard, new_rows, new_cols,info_buffer,info_buffer2 ,gens_max );
  
}



void init(int nrows, int ncols, char info_buffer[nrows][ncols], char* inboard ){
    
    for(int i =0  ;i<STATUS_NUM; i++){
        char old_status = i>>4;
        char new_status = (((i&0b01111)|old_status) ==3);
        char update = new_status^old_status;
        status_map[i].state=new_status;
        status_map[i].update = update;
    }
    
    
//    const int LDA = ncols;
#define BOARD( __board, __i, __j )  (__board[(__i)*ncols + (__j)])   

    
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++)
        {
            const int inorth = mod (i-1, nrows); // for i = 1  , i==0 seperately loop to avoid bounding issue , separte thread? no sepearte thread just first check and another loop if found
            const int isouth = mod (i+1, nrows);
            const int jwest = mod (j-1, ncols);
            const int jeast = mod (j+1, ncols);

             char neighbor_count = 
                BOARD (inboard, inorth, jwest) + 
                BOARD (inboard, inorth, j) + 
                BOARD (inboard, inorth, jeast) + 
                BOARD (inboard, i, jwest) +
                BOARD (inboard, i, jeast) + 
                BOARD (inboard, isouth, jwest) +
                BOARD (inboard, isouth, j) + 
                BOARD (inboard, isouth, jeast);

            
            
            char state = BOARD (inboard, i, j)? 1:0 ;
            neighbor_count = neighbor_count + (state<<4);
            info_buffer[i][j] = neighbor_count;
            
//            info_buffer[i][j].state = BOARD (inboard, i, j) ;//alivep (neighbor_count, BOARD (inboard, i, j));
//            BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j)); //return ((x)|state)==3
//            printf("state: %d\n",state);
//            printf("state: %d\n",neighbor_count-(state<<4));
//            printf("state: %d\n",info_buffer[i][j].neighbour_count);
//            exit(-1);
        }
    }
    
    
    
//    memcpy(&info_buffer[0][1],&info_buffer[nrows-2][1],ncols-2);
//    memcpy(&info_buffer[nrows-1][1],&info_buffer[1][1],ncols-2);
    
//     memset(&info_buffer[0][1],0,ncols-2);
//    memset(&info_buffer[nrows-1][1],0,ncols-2);
    
//    for(int i = 1 ;i <ncols-1; i++){
//        info_buffer[0][i].neighbour_count =  0;
//        info_buffer[nrows-1][i].neighbour_count =  0;
//    }
//    
//    for(int i = 1 ;i <nrows-1; i++){
////        info_buffer[i][0] =  info_buffer[i][ncols-2];
////        info_buffer[i][ncols-1] =  info_buffer[i][1];
//        info_buffer[i][0].neighbour_count =  0;
//        info_buffer[i][ncols-1].neighbour_count =  0;
//    }
    
}




#define SWAP_BUFFER(buffer1,buffer2) char (*temp)[ncols] = buffer1; buffer1 = buffer2; buffer2 = temp;



#define UPDATE( i,j,inorth,isouth ,jwest, jeast,buffer2,new_state) \
    int change = new_state? 1:-1;\
    buffer2[i][jwest] += change;\
    buffer2[i][jeast] += change;\
    buffer2[inorth][j] += change;\
    buffer2[isouth][j] += change;\
    buffer2[inorth][jwest] += change;\
    buffer2[isouth][jwest] += change;\
    buffer2[inorth][jeast] += change;\
    buffer2[isouth][jeast] += change;\
    buffer2[i][j] = buffer2[i][j] + (change<<4);\
    

//indexi>0&&indexj>0&&indexi<(ncols-1)&&indexj<(ncols-1)&&
#define INSERT(indexi,indexj,list,head)\
    {int index = indexi*ncols+indexj;\
    if(indexi>0&&indexj>0&&indexi<(ncols-1)&&indexj<(ncols-1)&&list[index].used==0){\
    coord temp = head;\
    head.i=indexi; head.j=indexj;\
    list[index].used=1;list[index].i=temp.i;list[index].j=temp.j;\
    size2++;\
    }\
    }\

#define UPDATE_LIST( i,j,inorth,isouth ,jwest, jeast,list,head)\
    INSERT(i,jwest,list,head);\
    INSERT(i,jeast,list,head);\
    INSERT(inorth,j,list,head);\
    INSERT(isouth,j,list,head);\
    INSERT(inorth,jwest,list,head);\
    INSERT(isouth,jwest,list,head);\
    INSERT(inorth,jeast,list,head);\
    INSERT(isouth,jeast,list,head);\
    
    

//    buffer2[i][j].state = new_state
    int size2=0;
char *sequential_game_of_life_new (char* outboard, 
        const int nrows,
        const int ncols,
        char info_buffer[nrows][ncols],
        char info_buffer2[nrows][ncols],
        const int gens_max)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
//    const int LDA = nrows;
    int curgen, i, j;

    
    char (*buffer1)[ncols]= info_buffer;
    char (*buffer2)[ncols]= info_buffer2;
    
    
    
    int size =  nrows*ncols;
    
    int update_size = nrows*ncols*sizeof(coord);
//    coord update_list[size]; 
    
    size = nrows*ncols - (ncols<<1) - ((nrows-2)<<1);
//    printf("size %d\n",size);
//    printf("hi %d\n",ncols);
    coord* update_list = malloc(update_size);
    
    coord* update_list2 = malloc(update_size);//[nrows][ncols];
    
  
    coord head;
   
    head.i=1;
    head.j=2;
   
    short previ=0;
    short prevj=0;
    
   
    for (int i = 1; i < nrows-1; i++)
    {   
        for (int j = 1; j < ncols-1; j++)
        {
            int ilj = previ*ncols+prevj;
            update_list[ilj].i=i;
            update_list[ilj].j=j;
            update_list[ilj].used = 1;
            previ=i;
            prevj=j;
            
        }
    }
//    printf("hi\n");
    memset(update_list2,0,update_size);
//      for(int j = 0 ;j <ncols; j++){
//            int i = 0;
//            int il = i*ncols;
//            update_list2[il+j].used=1;
//            
//            i = nrows-1;
//            il = i*ncols;
//            update_list2[il+j].used=1;
//         }
//        for(int i = 1 ;i <nrows-1; i++){
//        int j = 0;
//        int il = i*ncols;
//         update_list2[il+j].used=1;
//        j=ncols-1;
//        update_list2[il+j].used=1;
//        }
//    printf("hi\n");
    
//    
//    void insert(int i , int j){
//        coord temp = head;
//        if(update_list[i][j].i>-1){
//            update_list[i][j] = temp;
//        }
//        head.i = i;
//        head.j=j;
//        
//    }
    
//    void get_next(int i, int j, int* i2, int*j2){
//        *i2=update_list[i][j].i;
//        *j2=update_list[i][j].j;
//            
//        
//    }
    
    
//    char update_row[nrows];
//    char update_row2[nrows];
//    
//    char update_list[nrows];
//    char update_list2[nrows];
//    
//    for(int i = 0;i<nrows;i++){
//        update_row[i]=1;
//    }
//    for(int i = 0;i<nrows;i++){
//        update_row2[i]=0;
//    }
//    
    
    
    
//    memset(update_row2,0,nrows);
//    int size = 0;
//    char record[nrows][ncols]; //use a loook up table for 100 times latter ? fail for moving obj
    for (curgen = 0; curgen < gens_max; curgen++)
    {
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
//        size = 0;
//        head.i=0;
//        head.j=0;
//         printf("size %d\n",size);
        
//        memcpy(update_row,update_row2,nrows);
//        memset(update_row2,0,nrows);
//      
         size2=0;
        coord current = head;
        for(int c= 0 ; c< size; c++){
            
            int i = current.i;
            int j = current.j;
            char info_key = buffer1[i][j];

            status new_status =  status_map[info_key]; 
            int update = new_status.update;
            char new_state = new_status.state;
            //put outside will be much faster
            const int inorth = i-1;//mod (i-1, nrows); // for i = 1  , i==0 seperately loop to avoid bounding issue , separte thread? no sepearte thread just first check and another loop if found
            const int isouth = i+1;//mod (i+1, nrows);
            const int jwest = j-1;//mod (j-1, ncols);
            const int jeast = j+1;//mod (j+1, ncols);

//             printf("%d, %d\n",i,j);
            if(update){
               UPDATE(i,j,inorth,isouth,jwest,jeast,buffer2,new_state);
               UPDATE_LIST(i,j,inorth,isouth,jwest,jeast,update_list2,head);
            }
            current = update_list[i*ncols+j];
//            int check  = current.i;
//            if(check>1024){
//                 printf("hi\n");;
//            }
            
        }
        
       
        memcpy(update_list,update_list2,update_size);
        memset(update_list2,0,update_size);
        
        update_hori_boundary(nrows,ncols,buffer1,buffer2,update_list,&head); //0.11%
        update_verti_boundary(nrows,ncols,buffer1,buffer2,update_list,&head); //0.79%
       size = size2;
//        SWAP_BUFFER(buffer1,buffer2);
        memcpy(buffer1,buffer2,ncols*nrows);
//         for(int j = 0 ;j <ncols; j++){
//            int i = 0;
//            int il = i*ncols;
//            update_list2[il+j].used=1;
//            
//            i = nrows-1;
//            il = i*ncols;
//            update_list2[il+j].used=1;
//         }
//        for(int i = 1 ;i <nrows-1; i++){
//        int j = 0;
//        int il = i*ncols;
//         update_list2[il+j].used=1;
//        j=ncols-1;
//        update_list2[il+j].used=1;
//        }
        
//        SWAP_BOARDS( outboard, inboard );

    }
//    int size_update=0;
//    int i = head.i ; int j =head.j;
//    while(size>0){
//        
//        char state = buffer1[i][j].state;
//        int neighbour_count= buffer1[i][j].neighbour_count;
//        char new_state = (char)((neighbour_count|state)==3); 
//        int update = state^new_state;
//        const int inorth = mod (i-1, nrows); 
//        const int isouth = mod (i+1, nrows);
//        const int jwest = mod (j-1, ncols);
//        const int jeast = mod (j+1, ncols);
//        if(update){
//           UPDATE(i,j,inorth,isouth,jwest,jeast,buffer2,new_state);
//           insert(i,j);
//           --size_update;
//        }
//        get_next(i,j,&i,&j);
//        
//        
//    }
//    size = size_update;
    
    
    
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    
    for (i = 0; i < nrows; i++)
    {
        int il= (i)*(ncols);
        for (j = 0; j < ncols; j++)
        {
            outboard[il+j]=(buffer2[i][j]>16);


        }
            
    }
    return outboard;
}

void memcpy2(void* dest, void* src, size_t size){
    memcpy(dest,src,size);
}


#define LOWBOUND(i,max) i<0?max-1:i
#define HIGHBOUND(i,max) i>=max?0:i
void  update_hori_boundary(int nrows,int ncols, char buffer1[nrows][ncols], char buffer2[nrows][ncols] , coord* update_list, coord* head ){
        for(int j = 0 ;j <ncols; j++){
            int i = 0;
            int info_key= buffer1[i][j];
            status new_status =  status_map[info_key]; //(char)((neighbour_count|state)==3); 
            int update = new_status.update;
           
            int inorth = nrows-1;//mod (i-1, nrows); 
            int isouth = 1;//mod (i+1, nrows);
            int jwest =  LOWBOUND (j-1, ncols);
            int jeast =  HIGHBOUND (j+1, ncols);
            if(update){
                char new_state =  new_status.state;
                UPDATE(i,j,inorth,isouth,jwest,jeast,buffer2,new_state);
                UPDATE_LIST(i,j,inorth,isouth,jwest,jeast,update_list,(*head));
            }
             
            i = nrows-1;
            info_key= buffer1[i][j];
            new_status =  status_map[info_key]; //(char)((neighbour_count|state)==3); 
            update = new_status.update;
            inorth = nrows-2;//mod (i-1, nrows); 
            isouth = 0;//mod (i+1, nrows);
            jwest =  LOWBOUND (j-1, ncols);
            jeast =  HIGHBOUND (j+1, ncols);
            if(update){
                char new_state =  new_status.state;
                UPDATE(i,j,inorth,isouth,jwest,jeast,buffer2,new_state);
                UPDATE_LIST(i,j,inorth,isouth,jwest,jeast,update_list,(*head));
            }
            
        }
}

void  update_verti_boundary(int nrows,int ncols, char buffer1[nrows][ncols], char buffer2[nrows][ncols], coord* update_list, coord* head ){
    for(int i = 1 ;i <nrows-1; i++){
        int j = 0;
        int info_key= buffer1[i][j];
        status new_status =  status_map[info_key]; //(char)((neighbour_count|state)==3); 
        int update = new_status.update;

        int inorth = i-1;//mod (i-1, nrows);
        int isouth = i+1;//mod (i+1, nrows);
        int jwest =  ncols-1;//mod (j-1, ncols);
        int jeast =  1;//mod (j+1, ncols);

        if(update){
            char new_state =  new_status.state;
            UPDATE(i,j,inorth,isouth,jwest,jeast,buffer2,new_state);
            UPDATE_LIST(i,j,inorth,isouth,jwest,jeast,update_list,(*head));
        }

        j = ncols-1;
        info_key= buffer1[i][j];
        new_status =  status_map[info_key]; //(char)((neighbour_count|state)==3); 
        update = new_status.update;

        inorth = i-1;//mod (i-1, nrows);
        isouth = i+1;//mod (i+1, nrows);
        jwest =  ncols-2; //mod (j-1, ncols);
        jeast =  0;//mod (j+1, ncols);

        if(update){
            char new_state =  new_status.state;
            UPDATE(i,j,inorth,isouth,jwest,jeast,buffer2,new_state);
            UPDATE_LIST(i,j,inorth,isouth,jwest,jeast,update_list,(*head));
        }

    }
}



//
//#define SWAP_BOARDS( b1, b2 )  do { \
//  char* temp = b1; \
//  b1 = b2; \
//  b2 = temp; \
//} while(0)
//
//#define BOARD( __board, __i, __j )  (__board[(__i)*LDA + (__j)])
//
//    char*
//sequential_game_of_life2 (char* outboard, 
//        char* inboard,
//        const int nrows,
//        const int ncols,
//        const int gens_max)
//{
//    /* HINT: in the parallel decomposition, LDA may not be equal to
//       nrows! */
//    const int LDA = nrows;
//    int curgen, i, j;
//
//    for (curgen = 0; curgen < gens_max; curgen++)
//    {
//        /* HINT: you'll be parallelizing these loop(s) by doing a
//           geometric decomposition of the output */
//        for (i = 0; i < nrows; i++)
//        {
//            for (j = 0; j < ncols; j++)
//            {
//                const int inorth = mod (i-1, nrows); // for i = 1  , i==0 seperately loop to avoid bounding issue , separte thread? no sepearte thread just first check and another loop if found
//                const int isouth = mod (i+1, nrows);
//                const int jwest = mod (j-1, ncols);
//                const int jeast = mod (j+1, ncols);
//
//                const char neighbor_count = 
//                    BOARD (inboard, inorth, jwest) + 
//                    BOARD (inboard, inorth, j) + 
//                    BOARD (inboard, inorth, jeast) + 
//                    BOARD (inboard, i, jwest) +
//                    BOARD (inboard, i, jeast) + 
//                    BOARD (inboard, isouth, jwest) +
//                    BOARD (inboard, isouth, j) + 
//                    BOARD (inboard, isouth, jeast);
//
//                BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j)); //return ((x)|state)==3
//
//            }
//        }
//        SWAP_BOARDS( outboard, inboard );
//
//    }
//    /* 
//     * We return the output board, so that we know which one contains
//     * the final result (because we've been swapping boards around).
//     * Just be careful when you free() the two boards, so that you don't
//     * free the same one twice!!! 
//     */
//    return inboard;
//}

/*
 for (i = 1; i < nrows-1; i++)
        {
            

                for (j = 1; j < ncols-1; j++)
                {

//                    char state = buffer1[i][j].state;
                    int info_key= buffer1[i][j];
                    status new_status =  status_map[info_key]; //(char)((neighbour_count|state)==3); 
                   
                    int update = new_status.update;
                    char new_state = new_status.state;
//                    printf("count %d\n",neighbour_count);
//                    printf("update %d\n",update);

                    //the index placed outsiede will be faster?/
                    
                    //put outside will be much faster
                    const int inorth = i-1; //mod (i-1, nrows); // for i = 1  , i==0 seperately loop to avoid bounding issue , separte thread? no sepearte thread just first check and another loop if found
                    const int isouth = i+1;//mod (i+1, nrows);
                    const int jwest = j-1;//mod (j-1, ncols);
                    const int jeast = j+1;//mod (j+1, ncols);

                    if(update){
                       
    //                     record[i][j]=update;
//                        update();
                       UPDATE(i,j,inorth,isouth,jwest,jeast,buffer2,new_state);
                       

                    }

                }

           
            //every 1000 iter check if mov obj exit, if not only need to consider flip obj
        }
 
 */