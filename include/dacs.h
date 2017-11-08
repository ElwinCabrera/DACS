#ifndef DACS_H
#define DACS_H

typedef struct
{
        //newX and newY are not pointers because we will be updating it
        // if these were pointers then we could malloc size like an array
        int dID, xOrig, yOrig, xDest, yDest, speed, deliverStatus, status;
} DRONE;



void *droneThread(void* arg);

void *printThread(void* arg);

void printWorld();

void moveControl(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID, char srartAxis);
void moveSouthEast(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID, char startAxis);
void moveNorthWest(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID, char startAxis);
void moveNorthEast(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID, char startAxis);
void moveSouthWest(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID, char startAxis);
void moveNorthOrSouth(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID, char direction[]);
void moveWestOrEast(int xOrig, int yOrig, int xDest, int yDest, int speed, int dID, char direction[]);



#endif // DACS_H
