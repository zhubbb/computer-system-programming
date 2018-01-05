#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utilities.h"  // DO NOT REMOVE this line
#include "implementation_reference.h"   // DO NOT REMOVE this line
#include <assert.h>
/***********************************************************************************************************************
 * @param buffer_frame - pointer pointing to a buffer storing the imported 24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param offset - number of pixels to shift the object in bitmap image up
 * @return - pointer pointing a buffer storing a modified 24-bit bitmap image
 * Note1: White pixels RGB(255,255,255) are treated as background. Object in the image refers to non-white pixels.
 * Note2: You can assume the object will never be moved off the screen
 **********************************************************************************************************************/

struct __attribute__((__packed__)) _three_byte_{
//    unsigned int a : 24;
    char a,b,c;
};
struct info{
    
    struct _three_byte_ color;
//    char pad;
    short x,y;
};
//struct _sixty3_byte_{
//    unsigned int h;
//    unsigned long int a,b,c,d,e,f,g;
//    struct _three_byte_ i;
//    
//};

/***********************************************************************************************************************
 * @param buffer_frame - pointer pointing to a buffer storing the imported 24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param rotate_iteration - rotate object inside frame buffer clockwise by 90 degrees, <iteration> times
 * @return - pointer pointing a buffer storing a modified 24-bit bitmap image
 * Note: You can assume the frame will always be square and you will be rotating the entire image
 **********************************************************************************************************************/

#define rotate_90(i,j,w,i2,j2) i2=j;j2=(w-i-1)
#define rotate_180(i,j,w,i2,j2) i2=(w-i-1);j2=(w-j-1)
#define rotate_270(i,j,w,i2,j2) i2=(w-1-j),j2=i
#define rotate_90_mirror_x(i,j,w,i2,j2) i2=(w-j-1);j2=(w-i-1) 
#define rotate_90_mirror_y(i,j,w,i2,j2) i2=j;j2=i
#define rotate_180_mirror_x(i,j,w,i2,j2) i2=i;j2=(w-j-1) // mirrory
#define rotate_180_mirror_y(i,j,w,i2,j2) i2=(w-i-1);j2=j //mirrorx
#define rotate_270_mirror_x(i,j,w,i2,j2) i2=j;j2=i //90+my
#define rotate_270_mirror_y(i,j,w,i2,j2) i2=(w-j-1);j2=(w-i-1) //90+mx
#define mirror_x(i,j,w,i2,j2) i2=(w-i-1);j2=j
#define mirror_y(i,j,w,i2,j2) i2=i;j2=(w-j-1)




#define assign_index(i,j,w) (i*w+j)

#define block_size 21
#define not_bg(x,white) (memcmp(&x, &white, 3))


unsigned char* processAll(unsigned char* rendered_frame,struct info* info_array,int obj_size , unsigned width, unsigned height,
                               int x, int y,int rotate_iteration,int mirror_x, int mirror_y) {
    
//    unsigned int white=  0xffffff;
    unsigned char* buffer = allocateFrame(width, height);
    
   
    
    struct _three_byte_* r = (struct _three_byte_*)rendered_frame;
//    struct _three_byte_* b = (struct _three_byte_*)buffer_frame;
//        memset(r,white,(width * height * 3) * sizeof(char));  
//    printf("%d w,%d h",width,height);
    
    
    for(int i =0 ; i < obj_size; ++i){
//        assert(i<obj_size);
//        assert(i>-1);
        struct info inf = info_array[i];
         
        int v=inf.y;
        int h=inf.x;
// printf("%d x\n",x);
        
        int v1=v-y,h1=h+x;
        int v2=v1,h2=h1;
//        printf("%d,%d first\n",v2,h2);
        if(rotate_iteration){
            if(mirror_x){
                switch(rotate_iteration){
                    case 1: rotate_90_mirror_x(v1,h1,width,v2,h2);break;
                    case 2: rotate_180_mirror_x(v1,h1,width,v2,h2);break;
                    case 3: rotate_270_mirror_x(v1,h1,width,v2,h2);break;
                }
            }else if(mirror_y){
                switch(rotate_iteration){
                    case 1: rotate_90_mirror_y(v1,h1,width,v2,h2);break;
                    case 2: rotate_180_mirror_y(v1,h1,width,v2,h2);break;
                    case 3: rotate_270_mirror_y(v1,h1,width,v2,h2);break;
                }

            }else{
                switch(rotate_iteration){
                    case 1: rotate_90(v1,h1,width,v2,h2);break;
                    case 2: rotate_180(v1,h1,width,v2,h2);break;
                    case 3: rotate_270(v1,h1,width,v2,h2);break;
                }

            }

        }else if(mirror_x){
            mirror_x(v1,h1,width,v2,h2);
        }else if(mirror_y){
            mirror_y(v1,h1,width,v2,h2);
        }

//                        int d1= assign_index(v2,h2,width);
//                        int d2= assign_index(i,j,width);
//
//                        printf("%d,%d\n",v2,h2);
//                        printf("%d :c\n",inf.color);
//        assert(v2>-1);
//        assert(h2>-1);
//        assert(v2<width);
//        assert(h2<width);
       r[assign_index(v2,h2,width)]= inf.color;
               //b[assign_index(v,h,width)];
    }
    

//    for(int i = 0 ; i< height ; i= i + block_size){
//        for(int j = 0; j < width; j = j + block_size){
//            
//            int bound_i = i+block_size;
//            bound_i = bound_i > height ? height : bound_i;
//            int bound_j = j+block_size;
//            bound_j  =  bound_j > width ? width : bound_j;
////             printf(" lll %d bounid\n",bound_i);
//            for(int i2 = i ; i2< bound_i ; ++i2){
//                for(int j2 = j; j2 < bound_j; ++j2){
//                    //since it is guarantee that it is not off screen , we can directly identify the object and apply shift on it.
////                    printf("%d\n",b[assign_index(i2,j2,width)]);
////                    printf("%d;ttt\n",b[memcmp(&white,&b[assign_index(i2,j2,width)], sizeof(struct _three_byte_))]);
//                    
//                    if(not_bg(b[assign_index(i2,j2,width)],white)){
//                        //processing
//                        int v=i2-y;
//                        int h=j2+x;
//// printf("%d x\n",x);
//                        int v2=v,h2=h;
//                        if(rotate_iteration){
//                            if(mirror_x){
//                                switch(rotate_iteration){
//                                    case 1: rotate_90_mirror_x(v,h,width,v2,h2);break;
//                                    case 2: rotate_180_mirror_x(v,h,width,v2,h2);break;
//                                    case 3: rotate_270_mirror_x(v,h,width,v2,h2);break;
//                                }
//                            }else if(mirror_y){
//                                switch(rotate_iteration){
//                                    case 1: rotate_90_mirror_y(v,h,width,v2,h2);break;
//                                    case 2: rotate_180_mirror_y(v,h,width,v2,h2);break;
//                                    case 3: rotate_270_mirror_y(v,h,width,v2,h2);break;
//                                }
//
//                            }else{
//                                switch(rotate_iteration){
//                                    case 1: rotate_90(v,h,width,v2,h2);break;
//                                    case 2: rotate_180(v,h,width,v2,h2);break;
//                                    case 3: rotate_270(v,h,width,v2,h2);break;
//                                }
//                                
//                            }
//                            
//                        }else if(mirror_x){
//                            mirror_x(v,h,width,v2,h2);
//                        }else if(mirror_y){
//                            mirror_y(v,h,width,v2,h2);
//                        }
//                        
////                        int d1= assign_index(v2,h2,width);
////                        int d2= assign_index(i,j,width);
//                        
////                        printf("%d,%d\n",i2,j2);
//                       r[assign_index(v2,h2,width)]=b[assign_index(i2,j2,width)];
//                            
//                        
//                    }
//                    
//                }
//                
//            }
//
//            
//        }
//                
//    }
    
//    buffer_frame = copyFrame2(rendered_frame, buffer_frame, width, height);
    // free temporary image buffer
//    free(rendered_frame);
     memcpy(buffer,rendered_frame,(width * height * 3) * sizeof(char));
    return buffer;
}




    
    

        
    
    

    // there is no leftover//must be square. it did save cache hit. but the time is no improving. the overhead is copy_buffer, cannnot free buffer frame since it is used twice 
//due to the way verify frame is called , you need at least 25 frame to pass the test. e.g CW,1 * 25 times

    





/***********************************************************************************************************************
 * WARNING: Do not modify the implementation_driver and team info prototype (name, parameter, return value) !!!
 *          Do not forget to modify the team_name and team member information !!!
 **********************************************************************************************************************/
void print_team_info(){
    // Please modify this field with something interesting
    char team_name[] = "lao zi bu shui le";

    // Please fill in your information
    char student1_first_name[] = "Xian";
    char student1_last_name[] = "Zhang";
    char student1_student_number[] = "1000564766";

    // Please fill in your partner's information
    // If yon't have partner, do not modify this
    char student2_first_name[] = "Qili";
    char student2_last_name[] = "Lu";
    char student2_student_number[] = "1000354537";

    // Printing out team information
    printf("*******************************************************************************************************\n");
    printf("Team Information:\n");
    printf("\tteam_name: %s\n", team_name);
    printf("\tstudent1_first_name: %s\n", student1_first_name);
    printf("\tstudent1_last_name: %s\n", student1_last_name);
    printf("\tstudent1_student_number: %s\n", student1_student_number);
    printf("\tstudent2_first_name: %s\n", student2_first_name);
    printf("\tstudent2_last_name: %s\n", student2_last_name);
    printf("\tstudent2_student_number: %s\n", student2_student_number);
}

/***********************************************************************************************************************
 * WARNING: Do not modify the implementation_driver and team info prototype (name, parameter, return value) !!!
 *          You can modify anything else in this file
 ***********************************************************************************************************************
 * @param sensor_values - structure stores parsed key value pairs of program instructions
 * @param sensor_values_count - number of valid sensor values parsed from sensor log file or commandline console
 * @param frame_buffer - pointer pointing to a buffer storing the imported  24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param grading_mode - turns off verification and turn on instrumentation
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/




void implementation_driver(struct kv *sensor_values, int sensor_values_count, unsigned char *frame_buffer,
                           unsigned int width, unsigned int height, bool grading_mode) {
    int processed_frames = 0;
//    printBMP(width, height, frame_buffer);
    
    //assert(sizeof(struct _three_byte_)==3);
            int size= width*height*sizeof(struct _three_byte_);
//        unsigned char* rendered_buffer = allocateFrame(width,height);
//        char rendered_buffer[width*height*3];
        struct info* info_array=  (struct info*)malloc((width * height) * sizeof(struct info));
//        bzero(info_array,zero);
        struct _three_byte_* f = (struct _three_byte_*)frame_buffer;
//         struct _three_byte_* r = (struct _three_byte_*)rendered_buffer;
        
        int white = 0xffffffff;
        
        int info_index=0;
//        int index=0;
        struct _three_byte_ c;
        
        int row_size = (width)*3 * sizeof(char);
        
       
        
      
        #define row_partition_ratio 5
        int small_width= width/row_partition_ratio;
        int small_row_size= small_width*3; //in byte
        
        
//        char* white_buffer = malloc(width*3);
//        memset(white_buffer,0xff,width*3);
        
     //   char small_white_buffer[small_row_size];
      //  memset(small_white_buffer,0xff,small_row_size);
        
//        	 char tiny_white_buffer[block_size*3];
//        memset(tiny_white_buffer,0xff,block_size*3);
        
        int block3= block_size*3;
        
        int leftover =width%small_width;    
        int leftover_row_size = leftover*3;
        
//        bool bg_check[height][row_partition_ratio+1];
//        bzero(bg_check,(row_partition_ratio+1)*sizeof(bool));
      
//         int index_temp=0;
//          int small_offset = small_width*row_partition_ratio;
//        for(int i=0;i<height;++i){
//           
//            for(int p=0;p<row_partition_ratio;++p){
//                int offset= p*small_width;
//                bg_check[i][p]=memcmp(&f[index_temp+offset],small_white_buffer,small_row_size);
//            }
//            if(leftover){
//                bg_check[i][row_partition_ratio]=memcmp(&f[index_temp+small_offset],small_white_buffer,leftover_row_size);
//            }
//            index_temp+=width;
//        }
        
        
        for(int i=0;i< height; ++i){
            for(int j=0;j<width;++j){
                char* a = &frame_buffer[i*width*3+j*3];
                 if(*a=='R'&&*(a+1)=='G'&&*(a+2)=='B'){
                                assert(i>1);
                                assert(i<(width-1));
                                assert(j>1);
                                assert(j<(width-1));
                 }
            }
        }
        
        
//for(int i=0;i< height; i=i+ block_size ){
// 		int tiling_boundi= i+block_size;
//            tiling_boundi = tiling_boundi> height? height:tiling_boundi;
//            
//            
//            for(int j=0; j< width; j= j+block_size){
//                
//                int tiling_boundj = j+block_size;
//                
//                int window_size ;
//                if(tiling_boundj>width){
//                    tiling_boundj= width;
//                    window_size = (width-j)*3;
//                }else{
//                    window_size = block3;
//                }
//                
////                int bp1 = j/ small_width;
////                int bp2 = tiling_boundj/ small_width;
//                
//                
//                
//                for(int i2=i;i2< tiling_boundi; ++i2){
//                
//                    int index=i2*width;
//                    
////                    if(j==0){
////                       
////                        for(int p=0;p<row_partition_ratio;++p){
////                            int offset= p*small_width;
////                            bg_check[i2][p]=memcmp(&f[index+offset],small_white_buffer,small_row_size);
////                        }
////                        if(leftover){
////                            bg_check[i2][row_partition_ratio]=memcmp(&f[index+small_offset],small_white_buffer,leftover_row_size);
////                        }
////                        
////                    }
//                     
//                    
////                    int j2i= j;
////                    int j2e= tiling_boundj;
////                    if(!bg_check[i2][bp1]){
////                        if(!bg_check[i2][bp2]){
////                            j2i=tiling_boundj;
////                        }else{
////                            j2i=bp2*small_width;
////                        }
////                    }else{
////                        if(!bg_check[i2][bp2]){
////                            j2e=bp2*small_width;
////                        }
////                    } 
//                    //(char)(c.a^0xff) || (char)(c.b^0xff) || (char)(c.c^0xff)
//                    //if(  memcmp(&f[index+j],tiny_white_buffer,window_size)   ){   //bg_check2[bp1]||bg_check2[bp2]){
//                        for(int j2=j; j2 < tiling_boundj;++j2){
//
//                            c = f[index+j2];
////                            printf("%d\n",c);
//                            if( not_bg(c,white)  ){
////                                printf("here\n");
//                               info_array[info_index].x=j2;
//                               info_array[info_index].y=i2;
//                               info_array[info_index].color=c;
//                               char* a = (char* )&c;
//                               if(*a=='R'&&*(a+1)=='G'&&*(a+2)=='B'){
//                                assert(i2>1);
//                                assert(i2<(width-1));
//                                assert(j2>1);
//                                assert(j2<(width-1));
//                               }
//                                ++info_index;
//                            }
//
//                        }
//                    //}
//                    
//                }
//                
//                
//                
//            }
//}
     
//        for(int i=0;i< height; ++i  ){
//            
//       
////          
//
////            if(memcmp(&f[index],white_buffer,row_size)){ // check if the whole role is white
//                
//            
//        
//                
//             int index=i*width;
//            
//         //       printf("%d\n",rs);
//            
//            
//            for(int si=0; si<row_partition_ratio; ++si){
//                
//                int offset= si*small_width;
//                if(memcmp(&f[index+offset],small_white_buffer,small_row_size)){
//                    int j2_end= offset+small_width;
//                    for(int j2=offset;j2< j2_end; j2=j2+1){
//                        
//                            c = f[index+j2];
//                            if( (char)(c.a^0xff) || (char)(c.b^0xff) || (char)(c.c^0xff) ){
//                               info_array[info_index].x=j2;
//                               info_array[info_index].y=i;
//                               info_array[info_index].color=c;
//                                ++info_index;
//                           }
//                            
//                     
////                    ++index;
//                    }
//                }    
//            }
//            int leftover =width%small_width;    
//            int current_row_index=small_width*row_partition_ratio;
//            index+=current_row_index;
//            
//            if( leftover&&memcmp(&f[index],small_white_buffer,leftover*3)){
//                
//            for(int j=current_row_index;j <  width; ++j){
//                
//       
//               
//                    c = f[index]; //not_bg(c,white)  //c.a!=-1 || c.b !=-1 || c.c!=-1 // (char)(c.a^0xff) || (char)(c.b^0xff) || (char)(c.c^0xff)
//               
//                    if( (char)(c.a^0xff) || (char)(c.b^0xff) || (char)(c.c^0xff) ){
//    //                    printf("a:%d,b:%d,c:%d\n",c.a,c.b,c.c);
//                        info_array[info_index].x=j;
//                        info_array[info_index].y=i;
//                        info_array[info_index].color=c;
//
//                         ++info_index;
//
//                    }
//                    ++index;
//                  
//
//
//             
//            }
//                
//            }//the second if brack for leftover rowise
//                
//                
//            
////           }//the first if bracket
//            
//            
//        }
        
    //    info_array= realloc(info_array, info_index * sizeof(struct info));
        
        
        //we can clear frame buffer this time
//        memset(f,white,size);
       
//         printf("info:%d\n",info_index);
//        for(int i = 0 ; i< info_index;i++){
//            printf("x:%d,y:%d\n",info_array[i].x,info_array[i].y);
//            printf("c:%d\n",info_array[i].color);
//        }
//        exit(-1);
//        free(&info_array[info_index]);

        

    
    int shiftvertical = 0 , shifthorizontal =0, rotate = 0, mirror_x=0, mirror_y=0;  
    for (int sensorValueIdx = 0; sensorValueIdx < sensor_values_count; sensorValueIdx++) {
//        printf("Processing sensor value #%d: %s, %d\n", sensorValueIdx, sensor_values[sensorValueIdx].key,
//               sensor_values[sensorValueIdx].value);
        
        
        //assume in the order of shift, rotate, mirror
        
        
        
        if (!strcmp(sensor_values[sensorValueIdx].key, "W")) {
            //printf("f\n");
           
            if(rotate&&mirror_x){
                
                switch(rotate){
                   
                    case 1:
                        shifthorizontal += sensor_values[sensorValueIdx].value;
                        break;
                    case 2:
                        shiftvertical += sensor_values[sensorValueIdx].value;
                        break;
                    case 3:
                        shifthorizontal -= sensor_values[sensorValueIdx].value;
                        break;
                     
                }
                
                
            }else if(rotate){
                switch(rotate){
                    
                    case 1:
                        shifthorizontal -= sensor_values[sensorValueIdx].value;
                        break;
                    case 2:
                        shiftvertical -=sensor_values[sensorValueIdx].value;
                        break;
                    case 3:
                        shifthorizontal += sensor_values[sensorValueIdx].value;
                        break;
                     
                }
                
            }else if(mirror_x){
                shiftvertical-=sensor_values[sensorValueIdx].value;
                
            }else {
                shiftvertical+= sensor_values[sensorValueIdx].value;
            }
            
            
            
            //frame_buffer = processMoveUp(frame_buffer, width, height, sensor_values[sensorValueIdx].value);
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "A")) {
            if(rotate&&mirror_y){
                
                switch(rotate){
                    
                    case 1:
                        shiftvertical += sensor_values[sensorValueIdx].value;
                        break;
                    case 2:
                        shifthorizontal -= sensor_values[sensorValueIdx].value;
                        break;
                    case 3:
                        shiftvertical -= sensor_values[sensorValueIdx].value;
                        break;
                     
                }
                
            }else if(rotate){
                switch(rotate){
                   
                    case 1:
                        shiftvertical -= sensor_values[sensorValueIdx].value;
                        break;
                    case 2:
                        shifthorizontal +=sensor_values[sensorValueIdx].value;
                        break;
                    case 3:
                        shiftvertical += sensor_values[sensorValueIdx].value;
                        break;
                     
                }
                
            }else if(mirror_y){
                shifthorizontal+=sensor_values[sensorValueIdx].value;
                
            }else{
                shifthorizontal-= sensor_values[sensorValueIdx].value;
            }
            
            
            
            //frame_buffer = processMoveLeft(frame_buffer, width, height, sensor_values[sensorValueIdx].value);
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "S")) {
            
             if(rotate&&mirror_x){
                
                switch(rotate){
                   
                    case 1:
                        shifthorizontal -= sensor_values[sensorValueIdx].value;
                        break;
                    case 2:
                        shiftvertical -= sensor_values[sensorValueIdx].value;
                        break;
                    case 3:
                        shifthorizontal += sensor_values[sensorValueIdx].value;
                        break;
                     
                }
                
                
            }else if(rotate){
                switch(rotate){
                 
                    case 1:
                        shifthorizontal += sensor_values[sensorValueIdx].value;
                        break;
                    case 2:
                        shiftvertical +=sensor_values[sensorValueIdx].value;
                        break;
                    case 3:
                        shifthorizontal -= sensor_values[sensorValueIdx].value;
                        break;
                     
                }
                
            }else if(mirror_x){
                shiftvertical+=sensor_values[sensorValueIdx].value;
                
            }else {
                shiftvertical-= sensor_values[sensorValueIdx].value;
            }
            
            
            
            //frame_buffer = processMoveDown(frame_buffer, width, height, sensor_values[sensorValueIdx].value);
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "D")) {
            
            if(rotate&&mirror_y){
                switch(rotate){
                 
                    case 1:
                        shiftvertical -= sensor_values[sensorValueIdx].value;
                        break;
                    case 2:
                        shifthorizontal +=sensor_values[sensorValueIdx].value;
                        break;
                    case 3:
                        shiftvertical += sensor_values[sensorValueIdx].value;
                        break;
                     
                }
                
                
                
            }else if(rotate){
                switch(rotate){
                
                    case 1:
                        shiftvertical += sensor_values[sensorValueIdx].value;
                        break;
                    case 2:
                        shifthorizontal -= sensor_values[sensorValueIdx].value;
                        break;
                    case 3:
                        shiftvertical -= sensor_values[sensorValueIdx].value;
                        break;
                     
                }
                
            }else if(mirror_y){
                shifthorizontal-=sensor_values[sensorValueIdx].value;
                
            }else{
                shifthorizontal+= sensor_values[sensorValueIdx].value;
            }
            
            
            
            //frame_buffer = processMoveRight(frame_buffer, width, height, sensor_values[sensorValueIdx].value);
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key,"CW")) {
            
            int temp_rotate = (4+(sensor_values[sensorValueIdx].value % 4))%4;
//            printf("temp:%d\n",temp_rotate);
            if(mirror_x==1){
                switch(temp_rotate){
                    case 0:
                        break ;
                    case 1:
                        rotate += 3;
                        break;
                    case 2:
                        mirror_x = 0;
                        mirror_y = 1;
                        break;
                    case 3:
                        rotate += 1;
                        break;
                     
                }
                
            }else if(mirror_y==1){
                switch(temp_rotate){
                    case 0:
                        break ;
                    case 1:
                        rotate += 3;
                        break;
                    case 2:
                        mirror_x = 1;
                        mirror_y = 0;
                        break;
                    case 3:
                        rotate += 1;
                        break;
                     
                }
                
            }else{
                rotate+=temp_rotate;
            }
            rotate = (4+rotate)%4;
            mirror_x = mirror_x % 2;
            mirror_y = mirror_y % 2;
            
            // processRotateCW(frame_buffer, width, height, sensor_values[sensorValueIdx].value);
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "CCW")) {
            
            int temp_rotate = (4-(sensor_values[sensorValueIdx].value % 4))%4;
//            if(temp_rotate>0){
//                temp_rotate= (4-temp_rotate)%4;
//            }else{
//                temp_rotate=-temp_rotate;
//            }
            
            
            
            if(mirror_x){
                switch(temp_rotate){
                    case 0:
                        break ;
                    case 1:
                        rotate += 3;
                        break;
                    case 2:
                        mirror_x -= 1;
                        mirror_y += 1;
                        break;
                    case 3:
                        rotate += 1;
                        break;
                     
                }
                
            }else if(mirror_y){
                switch(temp_rotate){
                    case 0:
                        break ;
                    case 1:
                        rotate += 3;
                        break;
                    case 2:
                        mirror_x += 1;
                        mirror_y -= 1;
                        break;
                    case 3:
                        rotate += 1;
                        break;
                     
                }
                
            }else{
                rotate+=temp_rotate;
            }
            rotate = (4+rotate)%4;
            mirror_x = mirror_x % 2;
            mirror_y = mirror_y % 2;
          
             //processRotateCCW(frame_buffer, width, height, sensor_values[sensorValueIdx].value);
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "MX")) {
            
            mirror_x+=1;
            mirror_x = mirror_x % 2;
            if(mirror_x&&mirror_y){
                mirror_y=0;
                mirror_x=0;
                rotate+=2;
                rotate = (4+rotate)%4;
            }
            
            //frame_buffer = processMirrorX(frame_buffer, width, height, sensor_values[sensorValueIdx].value);
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "MY")) {
            mirror_y+=1;
            mirror_y = mirror_y % 2;
            if(mirror_x&&mirror_y){
                mirror_y=0;
                mirror_x=0;
                rotate+=2;
                rotate = (4+rotate)%4;
            }
            
            //frame_buffer = processMirrorY(frame_buffer, width, height, sensor_values[sensorValueIdx].value);
//            printBMP(width, height, frame_buffer);
        }
        processed_frames += 1;
        if (processed_frames % 25 == 0) {
            rotate = (4+rotate)%4; //handle negative
           // old part ///////////////////////////// 
//            if(shiftvertical>0){
//                processMoveUp(frame_buffer,width,height,shiftvertical);
//            }else if(shiftvertical<0){
//                processMoveDown(frame_buffer,width,height,-shiftvertical);
//            }else{
//                ;
//            }
//            
//            if(shifthorizontal>0){
//                processMoveRight(frame_buffer,width,height,shifthorizontal);
//            }else if(shifthorizontal<0){
//                processMoveLeft(frame_buffer,width,height,-shifthorizontal);
//            }else{
//                ;
//            }
//            
//                        
//            processRotateCW(frame_buffer, width, height, rotate);
//           
//            if(mirror_x){
//                processMirrorX(frame_buffer, width, height,  0);
//            }
//            else if(mirror_y){
//                processMirrorY(frame_buffer, width, height, 0);
//            }
            
             // old part ///////////////////////////// 
            
//            printf("%d:v\n",shiftvertical);
//            printf("%d:h\n",shifthorizontal);
//            printf("%d:r\n",rotate);
//             printf("%d:x\n",mirror_x);
//            printf("%d:y\n",mirror_y);
//             printf("%d:test\n",(-13%4));
                 memset(frame_buffer,0xff,size);  
               char* buffer=  processAll(frame_buffer,info_array,info_index,width,height,shifthorizontal,shiftvertical,rotate,mirror_x,mirror_y);
//                 memset(r,white,size);
               verifyFrame(buffer, width, height, grading_mode);
             
               free(buffer);

    //            mirror_x =0 ;
    //            mirror_y=0;
    //            rotate=0;
    //            shifthorizontal=0;
    //            shiftvertical=0;

    //            printBMP(width, height, frame_buffer);
               
                
                 
                
         
               
             
                
         
            
            
            
        }
//        printf("%d:pro_dirver\n",processed_frames);
    }
  
   // printf("%d\n",rotate);
   
//    free(rendered_buffer);
  //  free(info_array);
//    free(white_buffer);
//    printBMP(width, height, frame_buffer);
    return;
}

