#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "../include/dacs.h"

#define DRONES 10
#define SIZE_X 18
#define SIZE_Y 18

/*****************GLOBAL*********************/
static char world[SIZE_Y][SIZE_X];

int airportX;
int airportY;
int done=0;

DRONE drone[DRONES];
pthread_mutex_t keys[SIZE_Y][SIZE_X];
pthread_mutex_t bufferKey;
pthread_t threadId[DRONES], printThreadId;
/********************END**********************/

int main()
{
  int i, j;
  airportX = (SIZE_X/2) -1;
  airportY = (SIZE_Y/2) -1;


  /*****************WORLD and DRONE INITIALIZATION********************/
  //Populate the world
  for (i=0; i<SIZE_Y; ++i){
    for(j=0; j<SIZE_X; ++j){
      world[i][j]='*';
    }
  }

  world[airportY][airportX]='A';

  //initialize drone id's, origin , and destination
  for(i =0; i<DRONES; ++i){
    //id's
    drone[i].dID =i;

    //origin
    drone[i].xOrig=airportX;
    drone[i].yOrig=airportY;

    //destenation
    drone[i].xDest=17;
    drone[i].yDest=17;

    //speed
    drone[i].speed=1;
  }
  /**************************END**********************************/

  /*********************THREAD and THREAD UTILITY INITIALIZATION*********************/
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  //initialize the mutex keys
  pthread_mutex_init (&bufferKey, NULL);
  for (i=0; i<SIZE_Y; ++i){
    for(j=0; j<SIZE_X; ++j){
      pthread_mutex_init(&keys[i][j],NULL);
    }
  }

  //pthread_create(&printThreadId, &attr, printThread, NULL);
  //create a thread to represent a drone
  for(i=0;i<DRONES; ++i){
    pthread_create(&threadId[i], &attr, droneThread, &drone[i]);
  }

  //wait on each thread to finish before joining main thread
  for(i=0; i<DRONES; ++i){
    pthread_join(threadId[i], NULL);
   /* if(i+1==DRONES){
      done=1;
    }*/
  }
  done=1;
  //pthread_join(printThreadId, NULL);
  /************************************END******************************************/

  /************************CLEAN UP****************************************/

  pthread_mutex_destroy(&bufferKey);
  for (i=0; i<SIZE_Y; ++i){
   for(j=0; j<SIZE_X; ++j){
      pthread_mutex_destroy(&keys[i][j]);
    }
  }
  /***********************END********************************************/

  return 0;
}


void *droneThread(void *arg)
{
  DRONE *mDrone = (DRONE*) arg;

  /******DEBUGGING*******/
  //drone->xOrig=airportX;
  //drone->yOrig=airportY;
  //drone->xDest=17;
  //drone->yDest=17;
  //drone->speed=1;
  //world[airportY][airportX]='!';
//world[4][3] = '!';
  //world[5][4]='!';
//world[3][4]='!';
//world[4][5]='!';
  //world[2][5]='!';
  //world[5][5]='!';
  //world[6][9]='!';
  //world[9][5]='!';
  /********END********/

  mDrone->status=1;
  mDrone->deliverStatus=0;
  moveControl(mDrone->xOrig, mDrone->yOrig, mDrone->xDest, mDrone->yDest, mDrone->speed, mDrone->dID, 'x');
  mDrone->deliverStatus=1;
  
  //goback home
  moveControl(mDrone->xDest, mDrone->yDest, mDrone->xOrig, mDrone->yOrig, mDrone->speed, mDrone->dID, 'y');
  
  mDrone->status=0;

  if(world[airportY][airportX] != 'A'){
    world[airportY][airportX]='A';
    //unlock mutex
    //pthread_mutex_unlock(&keys[airportY][airportX]);
    printWorld();
  }
  pthread_mutex_unlock(&keys[airportY][airportX]);

  

  pthread_exit((void*)0);
}
/*void *printThread(void* arg)
{
  for(;;){
    printWorld();
    sleep(.3);

    if(done==1){
      break;
    } 
  }
  pthread_exit((void*)0);
}*/

void printWorld()
{
  int x,y;
  pthread_mutex_lock(&bufferKey);
  printf("Active drones %d: ",DRONES);
  for(x=0;x<DRONES; ++x){
    printf("%d ", drone[x].status);
  }
  printf("\n");
  printf("Delivery status: ");
  for(x=0;x<DRONES; ++x){
    printf("%d ", drone[x].deliverStatus);
  }
  printf("\n");

  for(y=0; y< SIZE_Y; ++y){
    for(x=0; x<SIZE_X; ++x){
      printf("%c ", world[y][x]);
    }
    printf("\n");
  }
  printf("\n");
  pthread_mutex_unlock(&bufferKey);

}

void moveControl(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID, char startAxis)
{
  if(xDest > xOrig && yDest > yOrig){ //drone moving southeast, right then down or down then right
    moveSouthEast(xOrig,yOrig,xDest,yDest,speed,dID,startAxis);

  } else if (xDest < xOrig && yDest < yOrig){ //drone moving northwes, left then up or up then left
      moveNorthWest(xOrig,yOrig,xDest,yDest,speed,dID,startAxis);
  } else if(xDest > xOrig && yDest < yOrig){ //drone moving northeast, right then up or up then right
      moveNorthEast(xOrig,yOrig,xDest,yDest,speed,dID,startAxis);
  } else if(xDest< xOrig && yDest > yOrig){ // drone moving southwest, left then down or down then left

      moveSouthWest(xOrig,yOrig,xDest,yDest,speed,dID,startAxis);

  } else if (xDest==xOrig && yDest >yOrig){ //drone moving south
    moveNorthOrSouth(xOrig,yOrig,xDest,yDest,speed,dID,"south");

  } else if (xDest == xOrig && yDest < yOrig){ // drone moving north
    moveNorthOrSouth(xOrig,yOrig,xDest,yDest,speed,dID,"north");

  } else if (xDest < xOrig && yDest == yOrig){ //drone moving west
    moveWestOrEast(xOrig,yOrig,xDest,yDest,speed,dID,"west");

  } else if (xDest > xOrig && yDest == yOrig){ // drone moving east
    moveWestOrEast(xOrig,yOrig,xDest,yDest,speed,dID,"east");

  }


}

void moveSouthEast(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID ,char startAxis)
{
  int x,y;
  char id = dID +'0';
  
  if(startAxis =='x'){
  /********************START at X*************************************/


    for(x=xOrig; x<=xDest; x+=speed){

      if(x != xOrig && world[yOrig][x-speed]==id){
        //we moved to next pos and we need to put an * in last pos
        
        world[yOrig][x-speed]='*';
        pthread_mutex_unlock(&keys[yOrig][x-speed]);
      } 
      if (x-speed ==xOrig && world[airportY][airportX]=='*'){
        world[airportY][airportX]='A';
        pthread_mutex_unlock(&keys[airportY][airportX]);
      }

      if(world[yOrig][x] =='*' || world[yOrig][x] =='A'){
      pthread_mutex_lock(&keys[yOrig][x]);
      world[yOrig][x]=id;
      //sleep(.5);
      //pthread_mutex_lock(&keys[yOrig][x]);
      } else if((yOrig+speed<SIZE_Y) && (world[yOrig+speed][x-speed] =='*'|| world[yOrig+speed][x-speed] =='A')  ) {
          //something blocking the way try another way
          moveControl(x-speed, yOrig+speed, xDest, yDest, speed, dID,'y');
          break;
        } else if((yOrig-speed>0) && (world[yOrig-speed][x-speed] =='*'|| world[yOrig-speed][x-speed] =='A') ){
          moveControl(x-speed, yOrig-speed, xDest, yDest, speed, dID,'x');
          break;
        } 

      if(x+1>xDest){ //start traversing y axis
        for(y = yOrig; y<= yDest; y+=speed){
          if(y != yOrig && world[y-speed][x] ==id ){
            world[y-speed][x]='*';
            pthread_mutex_unlock(&keys[y-speed][x]);
          }

          if(y != yOrig){ // because was aready checked above when moving in x axis
            if(world[y][x] =='*' || world[y][x] =='A'){
            pthread_mutex_lock(&keys[y][x]);
            world[y][x]=id;
            //sleep(.5);
            //pthread_mutex_lock(&keys[y][x]);
            } else if((x+speed<SIZE_X) && (world[y-speed][x+speed] =='*' || world[y-speed][x+speed] =='A')){
              moveControl(x+speed, y-speed, xDest, yDest, speed, dID,'x');
              break;
            } else if (x-speed>0 &&(world[y-speed][x-speed] =='*' || world[y-speed][x-speed]=='A')){
              moveControl(x-speed,y-speed, xDest,yDest,speed,dID,'y');
              break;
            } 
          } else{
            world[y][x]=id;
            pthread_mutex_unlock(&keys[y][x]);
          }
          printWorld();
        }
      

      }
      printWorld();

    }
    /******************************************************************/


  } else if(startAxis=='y'){
  /********************START at Y*****************************/


    for(y=yOrig; y<=yDest; y+=speed){
      if(y != yOrig && world[y-speed][xOrig]==id){
        world[y-speed][xOrig]='*';
        pthread_mutex_unlock(&keys[y-speed][xOrig]);
      }
      if(y-speed == yOrig && world[airportY][airportX]=='*'){
        world[airportY][airportX] = 'A';
        pthread_mutex_unlock(&keys[airportY][airportX]);
      }


      if(world[y][xOrig] =='*' || world[y][xOrig] =='A'){
      pthread_mutex_lock(&keys[y][xOrig]);
      world[y][xOrig]=id;
      //sleep(.5);
      //pthread_mutex_lock(&keys[y][xOrig]);
      } else if((xOrig+speed<SIZE_X) && (world[y-speed][xOrig+speed] =='*'|| world[y-speed][xOrig+speed] =='A')){
        //something blocking the way try another way
        moveControl(xOrig+speed, y-speed, xDest, yDest, speed, dID,'x');
        break;
      } else if ((xOrig-speed>0) && (world[y-speed][xOrig-speed]=='*'|| world[y-speed][xOrig-speed]=='A')){
        moveControl(xOrig-speed, y-speed, xDest, yDest, speed,dID,'y');
        break;
      }

      if(y+1 > yDest){
        for(x = xOrig; x<= xDest; x+=speed){
          if(x != xOrig && world[y][x-speed]==id){

            world[y][x-speed]='*';
            pthread_mutex_unlock(&keys[y][x-speed]);
          }

          if(x != xOrig ){
            if(world[y][x] =='*' || world[y][x] =='A'){
              pthread_mutex_lock(&keys[y][x]);
              world[y][x]=id;
             // sleep(.5);
              //pthread_mutex_lock(&keys[y][x]);
            } else if((y+speed<SIZE_Y) && (world[y+speed][x-speed] =='*'|| world[y+speed][x-speed] =='A')){
              //something blocking the way try another way
              moveControl(x-speed, y+speed, xDest, yDest, speed, dID,'y');
              break;
            } else if((y-speed>0) && (world[y-speed][x-speed] =='*'|| world[y-speed][x-speed] =='A')){
              moveControl(x-speed, y-speed, xDest,yDest,speed,dID,'x');
              break;
            }
          } else {
            world[y][x] =id;
            pthread_mutex_unlock(&keys[y][x]);
          }
          
          printWorld();
        }

      }
      printWorld();
    }

    /*******************************************************/


  }
}

void moveNorthWest(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID ,char startAxis)
{
  int x,y;
  char id = dID+'0';
  if(startAxis=='x'){


    for(x=xOrig; x>=xDest; x-=speed){

      if(x !=xOrig && world[yOrig][x+speed]==id){
        
        world[yOrig][x+speed]='*';
        pthread_mutex_unlock(&keys[yOrig][x+speed]);
      }
      if(x+speed == xOrig && world[airportY][airportX]=='*'){
        world[airportY][airportX]='A';
        pthread_mutex_unlock(&keys[airportY][airportX]);
      }

      if(world[yOrig][x] =='*' || world[yOrig][x] =='A'){
      pthread_mutex_lock(&keys[yOrig][x]);
      world[yOrig][x]=id;
      //sleep(.5);
      } else if(yOrig+speed<SIZE_Y && (world[yOrig+speed][x+speed]=='*'|| world[yOrig+speed][x+speed]=='A')){
        //something blocking the way try another way
        moveControl(x+speed, yOrig+speed, xDest, yDest, speed, dID,'y');
        break;

      } else if(yOrig-speed >0 &&(world[yOrig-speed][x+speed]=='*'||world[yOrig-speed][x+speed]=='A')){
        moveControl(x+speed, yOrig-speed,xDest, yDest,speed,dID,'x');
        break;
      }
      //world[yOrig][x]=id;

      if(x-1<xDest){

        for(y=yOrig; y>=yDest; y-=speed){
          if(y != yOrig && world[y+speed][x]==id){
            world[y+speed][x]='*';
            pthread_mutex_unlock(&keys[y+speed][x]);
          }
          if(y != yOrig){
            if(world[y][x] =='*' || world[y][x] =='A'){
              pthread_mutex_lock(&keys[y][x]);
              world[y][x]=id;
              //sleep(.5);
              //pthread_mutex_lock(&keys[y][x]);
            } else if (x+speed<SIZE_X &&(world[y+speed][x+speed]=='*' ||world[y+speed][x+speed]=='A')){
              //something blocking the way try another way
              moveControl(x+speed, y+speed, xDest, yDest, speed, dID,'x');
              break;
            } else if (x-speed>0 && (world[y+speed][x-speed]== '*' || world[y+speed][x-speed]=='A')){
              moveControl(x-speed, y+speed,xDest,yDest,speed,dID,'y');
              break;
            }
          } else {
            world[y][x]=id;
            pthread_mutex_unlock(&keys[y][x]);
          }
          printWorld();
        }

      }
      printWorld();
    }

  }else if (startAxis=='y'){


    for(y=yOrig; y>=yDest; y-=speed){
      if(y != yOrig && world[y+speed][xOrig]==id){
        world[y+speed][xOrig]='*';
        pthread_mutex_unlock(&keys[y+speed][xOrig]);
      }
      if(y+speed== yOrig && world[airportY][airportX]=='*'){
        world[airportY][airportX]='A';
        pthread_mutex_unlock(&keys[airportY][airportX]);
      }

      if(world[y][xOrig] =='*' || world[y][xOrig] =='A'){
      pthread_mutex_lock(&keys[y][xOrig]);
      world[y][xOrig]=id;
      //sleep(.5);
      //pthread_mutex_lock(&keys[y][xOrig]);
      } else if(xOrig+speed <SIZE_X &&(world[y+speed][xOrig+speed]=='*' || world[y+speed][xOrig+speed]=='A')){
        //something blocking the way try another way
        moveControl(xOrig+speed, y+speed, xDest, yDest, speed, dID,'y');
        break;
      } else if (xOrig-speed>0 &&(world[y+speed][xOrig-speed]=='*'||world[y+speed][xOrig-speed]=='A')){
        moveControl(xOrig-speed, y+speed,xDest,yDest,speed,dID,'x');
        break;
      }
      //world[y][xOrig]=id;

      if(y-1<yDest){
        for(x=xOrig; x>=xDest; x-=speed){
          if(x !=xOrig && world[y][x+speed]==id) {
            world[y][x+speed]='*';
            pthread_mutex_unlock(&keys[y][x+speed]);
          }
          if(x != xOrig){
            if(world[y][x] =='*' || world[y][x] =='A'){
              pthread_mutex_lock(&keys[y][x]);
              world[y][x]=id;
             // sleep(.5);
              //pthread_mutex_lock(&keys[y][x]);
            } else if (y+speed<SIZE_Y &&(world[y+speed][x+speed]=='*' ||world[y+speed][x+speed]=='A')){
              //something blocking the way try another way
              moveControl(x+speed, y+speed, xDest, yDest, speed, dID,'y');
              break;
            }else if(y-speed>0 &&(world[y-speed][x+speed]=='*'||world[y-speed][x+speed]=='A')){
              moveControl(x+speed, y-speed,xDest,yDest,speed,dID,'x');
              break;
            }
            world[y][x]=id;
            pthread_mutex_unlock(&keys[y][x]);
          }
          printWorld();
        }

      }
      printWorld();
    }

  }

}

void moveNorthEast(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID ,char startAxis)
{
  int x,y;
  char id = dID+'0';
  
  if(startAxis=='x'){
    
    for(x=xOrig; x<=xDest; x+=speed){
      if(x != xOrig && world[yOrig][x-speed]==id){
        world[yOrig][x-speed]='*';
      }
      if(x-speed ==xOrig && world[airportY][airportX] =='*'){
        world[airportY][airportX]='A';
      }
      if(world[yOrig][x] =='*' || world[yOrig][x] =='A'){
      world[yOrig][x]=id;
      } else {
        //something blocking the way try another way
        moveControl(x-speed, yOrig, xDest, yDest, speed, dID,'y');

      }
      //world[yOrig][x]=id;

      if(x+1>xDest){

        for(y=yOrig; y>=yDest; y-=speed){
          if(y != yOrig && world[y+speed][x]==id){
            world[y+speed][x]='*';
          }
          if(world[y][x] =='*' || world[y][x] =='A'){
            world[y][x]=id;
          } else {
            //something blocking the way try another way
            moveControl(x, y+speed, xDest, yDest, speed, dID,'y');
          }
          //world[y][x]=id;
          //printWorld();
        }
      }
      //printWorld();
    }

  }else if(startAxis=='y'){

    for(y=yOrig; y>=yDest; y-=speed){
      if(y != yOrig && world[y+speed][xOrig]==id){
        world[y+speed][xOrig]='*';
      }
      if(y+speed ==yOrig && world[airportY][airportX]=='*'){
        world[airportY][airportX]='A';
      }
      if(world[y][xOrig] =='*' || world[yOrig][x] =='A'){
      world[y][xOrig]=id;
      } else {
        //something blocking the way try another way
        moveControl(xOrig, y+speed, xDest, yDest, speed, dID,'x');

      }

      //world[y][xOrig]=id;
      if(y-1<yDest){
        for(x=xOrig; x<=xDest; x+=speed){
          if(x != xOrig && world[y][x-speed]==id){
            world[y][x-speed]='*';
          }
          if(world[y][x] =='*' || world[y][x] =='A'){
            world[y][x]=id;
          } else {
            //something blocking the way try another way
            moveControl(x+speed, yOrig, xDest, yDest, speed, dID,'x');

          }
          //world[y][x]=id;
          //printWorld();
        }
      }
      //printWorld();
    }

  }
} 

void moveSouthWest(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID ,char startAxis)
{
  int x,y;
  char id = dID+'0';
  if(startAxis=='x'){
    
    for(x = xOrig; x>=xDest; x-=speed){
      if(x != xOrig && world[yOrig][x+speed]==id){
        world[yOrig][x+speed]='*';
      }
      if(x+speed ==xOrig && world[airportY][airportX]=='*'){
        world[airportY][airportX]='A';
      }

      if(world[yOrig][x] =='*' || world[yOrig][x] =='A'){
      world[yOrig][x]=id;
      } else {
        //something blocking the way try another way
        moveControl(x+speed, yOrig, xDest, yDest, speed, dID,'y');

      }
      //world[yOrig][x]=id;

      if(x-1<xDest){
        for(y=yOrig; y<=yDest; y+=speed){
          if(y != yOrig && world[y-speed][x]==id){
            world[y-speed][x]='*';
          }
          if(world[y][x] =='*' || world[y][x] =='A'){
            world[y][x]=id;
          } else {
            //something blocking the way try another way
            moveControl(x, y-speed, xDest, yDest, speed, dID,'y');

          }
          //world[y][x]=id;
          //printWorld();
        }
      }
      //printWorld();
    }
    
  }else if (startAxis =='y'){
    
    for(y= yOrig; y<=yDest; y+=speed){
      if(y != yOrig && world[y-speed][xOrig]==id){
        world[y-speed][xOrig]='*';
      }
      if(y-speed == yOrig && world[airportY][airportX]=='*'){
        world[airportY][airportX] ='A';
      }

      if(world[y][xOrig] =='*' || world[y][xOrig] =='A'){
      world[y][xOrig]=id;
      } else {
        //something blocking the way try another way
        moveControl(xOrig, y-speed, xDest, yDest, speed, dID,'x');

      }
      //world[y][xOrig]=id;

      if(y+1>yDest){
        for(x=xOrig; x>=xDest; x-=speed){
          if(x != xOrig && world[y][x+speed]==id){
            world[y][x+speed]='*';
          }
          if(world[y][x] =='*' || world[y][x] =='A'){
            world[y][x]=id;
          } else {
            //something blocking the way try another way
            moveControl(x+speed, y, xDest, yDest, speed, dID,'x');
          }
          //world[y][x]=id;
          //printWorld();
        }
      }
      //printWorld();
    }
  }
}


void moveNorthOrSouth(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID ,char direction[])
{
  int y;
  char id =dID+'0';

  if(strcmp(direction,"north")==0){

    for(y=yOrig; y>=yDest; y-=speed){
      if(y != yOrig && world[y+speed][xOrig]==id){
        world[y+speed][xOrig]='*';
      }
      if(y+speed==yOrig && world[airportY][airportX]=='*'){
        world[airportY][airportX]='A';
      }
      world[y][xOrig]=id;
      //printWorld();
    }

  } else if (strcmp(direction, "south")==0){
      for(y=yOrig; y<=yDest; y+=speed){
        if(y != yOrig && world[y-speed][xOrig]==id){
          world[y-speed][xOrig]='*';
        }
        if(y-speed==yOrig && world[airportY][airportX]=='*'){
          world[airportY][airportX]='A';
        }
        world[y][xOrig]=id;
        //printWorld();
      }
  }
  
}

void moveWestOrEast(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID ,char direction[])
{
  int x;
  char id =dID+'0';

  if(strcmp(direction,"west")==0){

    for(x=xOrig; x>=xDest; x-=speed){
      if(x != xOrig && world[yOrig][x+speed]==id){
        world[yOrig][x+speed]='*';
        pthread_mutex_unlock(&keys[yOrig][x+speed]);
      }   
      if(x+speed==xOrig && world[airportY][airportX]=='*'){
        world[airportY][airportX]='A';
      }   
      world[yOrig][x]=id;
      //printWorld();
    }

  } else if (strcmp(direction, "east")==0){
      for(x=xOrig; x<=xDest; x+=speed){

        if(x != xOrig && world[yOrig][x-speed]==id){
          world[yOrig][x-speed]='*';
          pthread_mutex_unlock(&keys[yOrig][x+speed]);
        }
        if(x-speed==xOrig && world[airportY][airportX]=='*'){
          world[airportY][airportX]='A';
        }
        world[yOrig][x]=id;
        //printWorld();
      }   
  }
  
}

/**
For x axis
----------
If the destination (xDest) is > the originial position (xOrig) then it'll move right ->
If the destination (xDest) is <  the original position (xOrig) it'll move left <-

For Y axis
----------
If the destination (yDest) is < than the original position (yOrig) it'll move up ^
If the destination (yDest) is > than the original position (yOrig) it'll move down 

For x and y movement
--------------------
If you are moving in both the x and y direction you need a conbination of the above
there are 4 possible movements:

1) If (xDest) > (xOrig) and (yDest) > (yOrig) then the 
destination is south east (bottom righ) from current position, then you can move:
  a) starting from x axis: right then down (->, down)
          or
  b) starting from y axis: down then right(down, ->)

2) If (xDest) < (xOrig) and (yDest) < (yOrig) then the 
destination is northwest (top left) from current position, then you can move:

  a)starting from x axis: left then up (<-, ^)
            or
  b)starting from y axis: up then left (^,<-)

3) If (xDest) > (xOrig) and (xDest) < (yOrig) then the 
destination is northeast (top right) from current position, then yo can move:

  a)starting from x axis: right then up (->, ^)
        or
  b)starting from y axis: up then right (^,->)


4) If (xDest) < (xOrig) and (yDest) >(yOrig) then the 
destination is southwest (bottom left) from current position, then you can move: 

  a) starting from x axis: left then down (<-, down)
  b) starting from y axis: down then left (down, <-)


Note
----
1) everytime you are going to move in the x or y direction:
  a) check if the next position (i+1)is open (=='*') 
    -if true you can continue
    if false you set the origin as the current psition (xOrig = i) 
  b)check if next positin is less than max size at that axis(i+1<SIZE) if true you are ok. 
    -if moving up in y axis you have to check if (i-1>0) instead
    -if moving left in x axis check if (i-1 > 0 ) instead
  c) check if the speed is not to fast
*/
