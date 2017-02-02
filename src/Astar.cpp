/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Astar.cpp
--
-- PROGRAM:     Astar
--
-- FUNCTIONS:

--
-- DATE:        February 1, 2017
--
-- DESIGNER:    Fred Yang, Robert Arendac
--
-- PROGRAMMER:  Fred Yang, Robert Arendac
--
-- NOTES:
-- 
----------------------------------------------------------------------------------------------------------------------*/
#include <iostream>
#include <iomanip>
#include <queue>
#include <string>
#include <ctime>
#include "Node.h"
#include "Minion.h"
using namespace std;

// 8 possible directions
#define DIR_CAP     8
#define TILE_SIZE   60

// map row & column
const int row = TILE_SIZE;
const int col = TILE_SIZE;
static int map[row][col];

static int closedNodes[row][col]; // array of closed nodes (evaluated)
static int openNodes[row][col];   // array of open nodes (to be evaluated)
static int dirMap[row][col];      // array of directions

/* 8 possible movements
 * 0 - right, 1 - right down, 2 - down, 3 - left down
 * 4 - left, 5 - left up, 6 - up, 7 - right up
 */
static int dx[DIR_CAP]={1, 1, 0, -1, -1, -1, 0, 1};
static int dy[DIR_CAP]={0, 1, 1, 1, 0, -1, -1, -1};

// A-star algorithm. The route returned is a string of direction digits.
string pathFind( const int& xStart, const int& yStart, 
                 const int& xDest, const int& yDest)
{
    // priority queue index
    int index = 0;
    
    // temp index
    int i, j;
    
    // row / column index
    int x, y, xdx, ydy;
    
    // temp char
    char c;
    
    // path
    string path;
    
    // current node & child node
    static Node* curNode;
    static Node* childNode;
    
    // priority queue
    static priority_queue<Node> pq[2];
    
    // reset the node maps
    for(i = 0; i < row; i++)
    {
        for(j = 0; j < col; j++)
        {
            closedNodes[i][j] = 0;
            openNodes[i][j] = 0;
        }
    }

    // create the start node and push into open list
    curNode = new Node(xStart, yStart, 0, 0);
    curNode->updatePriority(xDest, yDest);
    pq[index].push(*curNode);

    // A* path finding
    while(!pq[index].empty())
    {
        // get the current node with the highest priority
        // from open list
        curNode = new Node(pq[index].top().getXPos(), pq[index].top().getYPos(), 
                           pq[index].top().getLevel(), pq[index].top().getPriority());

        x = curNode->getXPos(); 
        y = curNode->getYPos();

        // remove the node from the open list
        pq[index].pop(); 
        openNodes[x][y] = 0;
        
        // mark it on the closed nodes map
        closedNodes[x][y] = 1;

        // quit searching when the destination is reached
        if(x == xDest && y == yDest) 
        {
            // generate the path from destination to start
            // by following the directions
            path = "";
            while(!(x == xStart && y == yStart))
            {
                j = dirMap[x][y];
                c = '0' + (j + DIR_CAP/2)%DIR_CAP;
                path = c + path;
                x += dx[j];
                y += dy[j];
            }

            // garbage collection
            delete curNode;
            
            // empty the leftover nodes
            while(!pq[index].empty()) pq[index].pop();           
            return path;
        }

        // traverse neighbors
        for(i = 0; i < DIR_CAP;i++)
        {
            // neighbor coordinates
            xdx = x + dx[i]; 
            ydy = y + dy[i];

            // not evaluated & not outside (bound checking)
            if(!(xdx < 0 || xdx > col -1 || ydy < 0 || ydy > row - 1 
                || map[xdx][ydy] == 1 || closedNodes[xdx][ydy] == 1))
            {
                // generate a child node
                childNode = new Node(xdx, ydy, curNode->getLevel(), curNode->getPriority());
                childNode->nextLevel(i);
                childNode->updatePriority(xDest, yDest);

                // if it is not in the open list then add into that
                if(openNodes[xdx][ydy] == 0)
                {
                    openNodes[xdx][ydy] = childNode->getPriority();
                    pq[index].push(*childNode);
                    // update the parent direction info
                    dirMap[xdx][ydy] = (i + DIR_CAP/2)%DIR_CAP;
                }
                else if(openNodes[xdx][ydy] > childNode->getPriority())
                {
                    // update the priority info
                    openNodes[xdx][ydy]= childNode->getPriority();
                    // update the parent direction info
                    dirMap[xdx][ydy] = (i + DIR_CAP/2)%DIR_CAP;

                    // use a queue and a backup queue to put the best node (with highest priority)
                    // on the top of the queue, which can be chosen later on to build the path.
                    while(!(pq[index].top().getXPos() == xdx && 
                           pq[index].top().getYPos() == ydy))
                    {                
                        pq[1-index].push(pq[index].top());
                        pq[index].pop();       
                    }
                    pq[index].pop();
                    
                    // switch to pq with smaller size
                    if(pq[index].size() > pq[1-index].size()) 
                    {
                        index = 1 - index;
                    }
                    
                    while(!pq[index].empty())
                    {                
                        pq[1-index].push(pq[index].top());
                        pq[index].pop();       
                    }
                    index = 1 - index;
                    pq[index].push(*childNode);
                }
                else delete childNode;
            }
        }
        delete curNode;
    }
    
    return ""; // no route found
}

int main()
{
    srand(time(NULL));

    // create empty map
    for(int x = 0; x < row; x++)
    {
        for(int y = 0; y < col; y++) 
            map[x][y] = 0;
    }

    // fillout the map matrix with a '+' pattern of obstacles
    for(int x = row/8; x < row*7/8; x++)
    {
        map[x][col/2] = 1;
    }
    for(int y = col/8; y < col*7/8; y++)
    {
        map[row/2][y] = 1;
    }
    
    // randomly select start and end locations
    int xA, yA, xB, yB;
    switch(rand()%8)
    {
        case 0: xA=0;yA=0;xB=col-1;yB=row-1; break;
        case 1: xA=0;yA=row-1;xB=col-1;yB=0; break;
        case 2: xA=col/2-1;yA=row/2-1;xB=col/2+1;yB=row/2+1; break;
        case 3: xA=col/2-1;yA=row/2+1;xB=col/2+1;yB=row/2-1; break;
        case 4: xA=col/2-1;yA=0;xB=col/2+1;yB=row-1; break;
        case 5: xA=col/2+1;yA=row-1;xB=col/2-1;yB=0; break;
        case 6: xA=0;yA=row/2-1;xB=col-1;yB=row/2+1; break;
        case 7: xA=col-1;yA=row/2+1;xB=0;yB=row/2-1; break;
    }

    cout<<"Map Size (X,Y): "<<row<<","<<col<<endl;
    cout<<"Start: "<<xA<<","<<yA<<endl;
    cout<<"Goal: "<<xB<<","<<yB<<endl;
    
    // get the route
    clock_t start = clock();
    string route = pathFind(xA, yA, xB, yB);
    
    if(route == "") cout<<"No path found!"<<endl;
    
    clock_t end = clock();
    double time_elapsed = double(end - start);
    cout<<"Time to calculate the route (ms): "<<time_elapsed<<endl;
    cout<<"Route:"<<endl;
    cout<<route<<endl<<endl;

    // follow the route on the map and display it 
    if(route.length() > 0)
    {
        char c;
        int j;
        int x = xA;
        int y = yA;
        map[x][y] = 2; // start point 'S'
        
        for(size_t i = 0; i<route.length(); i++)
        {
            c = route.at(i);
            j = atoi(&c); 
            x = x + dx[j];
            y = y + dy[j];
            map[x][y] = 3; // route '*'
        }
        
        map[x][y] = 4; // target point 'T'
    
        // display the map with the route
        for(int y = 0; y < row; y++)
        {
            for(int x=0; x<col; x++)
                if(map[x][y] == 0)
                    cout<<"\033[0;32m.";
                else if(map[x][y] == 1)
                    cout<<"\033[0;49m\u0444"; //obstacle
                else if(map[x][y] == 2)
                    cout<<"\033[0;31mS"; //start
                else if(map[x][y] == 3)
                    cout<<"\033[0;35m*"; //route
                else if(map[x][y] == 4)
                    cout<<"\033[0;31mT"; //finish
            cout<<endl;
        }
    }
    //getchar(); // wait for a (Enter) keypress  
    return(0);
}
