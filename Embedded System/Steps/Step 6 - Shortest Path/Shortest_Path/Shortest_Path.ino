/*
 * Connect empty arduino
 * 
 * Compile and upload the program
 * Open Serial Monitor and select "No line ending"
 * Continue the program
 */
 
#define INFINITY 9999
#define SIZE 7                    //No of vertices

int final[SIZE];
int final_dir[SIZE];
boolean mapCreated = false;
int count = 0;

struct prev_node {
  char direction[SIZE];
};

struct cur_node {
  struct prev_node pre[SIZE];
};

struct cur_node nodes[SIZE];
int G[SIZE][SIZE] = {
                      {0, 1, 0, 0, 0, 0, 0},
                      {1, 0, 1, 0, 1, 0, 0},
                      {0, 1, 0, 1, 0, 0, 0},
                      {0, 0, 1, 0, 1, 0, 0},
                      {0, 1, 0, 1, 0, 1, 1},
                      {0, 0, 0, 0, 1, 0, 1},
                      {0, 0, 0, 0, 1, 1, 0}
                    };
                    
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  
  int row, col;
  
  createMap();  
  
  for(row = 0; row < SIZE; row++) {
    for(col = 0; col < SIZE; col++) {
      Serial.print(G[row][col]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();
  
  mapCreated = true;
}



void loop()
{
  if(mapCreated){
    int starting_node, previous_node, ending_node;
  
    Serial.print("Enter the starting node: ");
    while(1) {
      if(Serial.available() > 0) {
        char sn = Serial.read();
        Serial.println(char(sn));
        starting_node = sn - '0';
        count++;
        break;
      }
    }
    
    if(count == 1) {
      Serial.print("Enter the previous node: ");
      while(1) {
        if(Serial.available() > 0) {
          char pn = Serial.read();
          Serial.println(char(pn));
          previous_node = pn - '0';
          count++;
          break;
        }
      }
    }
  
    if(count == 2) {
      Serial.print("Enter the ending node: ");
      while(1) {
        if(Serial.available() > 0) {
          char en = Serial.read();
          Serial.println(char(en));
          ending_node = en - '0';
          count++;
          break;
        }
      }
    }
  
    if(count == 3) {
      findPath(starting_node, previous_node, ending_node);
      Serial.println();
      count = 0;
    }
  }
}



void findPath(int starting_node, int previous_node, int ending_node) {
  int cost[SIZE][SIZE], distance[SIZE], pred[SIZE], visited[SIZE];
  int count, mindistance, nextnode, row, col;
  
  //pred[] stores the predecessor of each node
  //count gives the number of nodes seen so far

  //create the cost matrix
  for(row = 0; row < SIZE; row++) {
    for(col = 0; col < SIZE; col++) {
      if(G[row][col] == 0) {
        cost[row][col] = INFINITY;
      } else {
        cost[row][col] = G[row][col];
      }
    }
  }

  //initialize pred[],distance[] and visited[]
  for(row = 0; row < SIZE; row++) {
    distance[row] = cost[starting_node][row];
    pred[row] = starting_node;
    visited[row] = 0;
  }
  
  distance[starting_node] = 0;
  visited[starting_node] = 1;
  count = 1;
  
  while(count < (SIZE - 1)) {
    mindistance = INFINITY;
    
    //nextnode gives the node at minimum distance
    for(row = 0; row < SIZE; row++) {
      if((distance[row] < mindistance) && !visited[row]) {
        mindistance = distance[row];
        nextnode = row;
      }
    }
  
    //check if a better path exists through nextnode            
    visited[nextnode] = 1;
    for(row = 0; row < SIZE; row++) {
      if(!visited[row]) {
        if((mindistance + cost[nextnode][row]) < distance[row]) {
          distance[row] = mindistance + cost[nextnode][row];
          pred[row] = nextnode;
        }
      }
    }
    count++;
  }

  // //print the path, direction and distance of each node
  // for(row = 0; row < SIZE; row++) {
  //     if(row != starting_node) {
  //         Serial.println("\nDistance of node%d = %d", row, distance[row]);
          
  //         int i = 0;
  //         int final[SIZE];
  //         int final_dir[SIZE];
  //         col = row;
  //         final[i] = col;
  //         do {
  //             i++;
  //             col = pred[col];
  //             final[i] = col;
  //         }while(col != starting_node);

  //         //print path
  //         Serial.println("\nPath = ");
  //         Serial.println("%d", final[i]);
  //         for(int j = i-1; j>=0; j--) {
  //             Serial.println("->%d", final[j]);
  //         }

  //         //print direction
  //         final_dir[i] = nodes[final[i]].pre[previous_node].direction[final[i-1]];
  //          Serial.println("\n%c", final_dir[i]);
  //         for(int j = i-1; j > 0; j--) {
  //             final_dir[j] = nodes[final[j]].pre[final[j+1]].direction[final[j-1]];
  //              Serial.println("->%c", final_dir[j]);
  //         }

  //  }
  // }
  // Serial.println("\n");

  //print the path, direction and distance of a perticular end node
  row = ending_node;
  if(row != starting_node) {
    Serial.print("\nDistance of node");
    Serial.print(row);
    Serial.print(" = ");
    Serial.print(distance[row]);
    
    int i = 0;
    col = row;
    final[i] = col;
    do {
      i++;
      col = pred[col];
      final[i] = col;
    }while(col != starting_node);

    //print path
    Serial.println("\nPath = ");
    Serial.print(final[i]);
    for(int j = i-1; j >= 0; j--) {
      Serial.print("->");
      Serial.print(final[j]);
    }
    Serial.println();

    //print direction
    final_dir[i-1] = nodes[final[i]].pre[previous_node].direction[final[i-1]];
    Serial.print(char(final_dir[i-1]));
    for(int j = i-2; j >= 0; j--) {
      final_dir[j] = nodes[final[j+1]].pre[final[j+2]].direction[final[j]];
      Serial.print("->");
      Serial.print(char(final_dir[j]));
    }
    Serial.println();
  }
}



void createMap() {
  nodes[0].pre[0].direction[1] = 's';
  nodes[0].pre[1].direction[1] = 'u';

  nodes[1].pre[0].direction[0] = 'u';
  nodes[1].pre[0].direction[2] = 'l';
  nodes[1].pre[0].direction[4] = 's';
  nodes[1].pre[2].direction[0] = 'r';
  nodes[1].pre[2].direction[2] = 'u';
  nodes[1].pre[2].direction[4] = 'l';
  nodes[1].pre[4].direction[0] = 's';
  nodes[1].pre[4].direction[2] = 'r';
  nodes[1].pre[4].direction[4] = 'u';

  nodes[2].pre[1].direction[1] = 'u';
  nodes[2].pre[1].direction[3] = 'r';
  nodes[2].pre[3].direction[1] = 'l';
  nodes[2].pre[3].direction[3] = 'u';

  nodes[3].pre[2].direction[2] = 'u';
  nodes[3].pre[2].direction[4] = 'r';
  nodes[3].pre[4].direction[2] = 'l';
  nodes[3].pre[4].direction[4] = 'u';

  nodes[4].pre[1].direction[1] = 'u';
  nodes[4].pre[1].direction[3] = 'l';
  nodes[4].pre[1].direction[5] = 'r';
  nodes[4].pre[1].direction[6] = 's';
  nodes[4].pre[3].direction[1] = 'r';
  nodes[4].pre[3].direction[3] = 'u';
  nodes[4].pre[3].direction[5] = 's';
  nodes[4].pre[3].direction[6] = 'l';
  nodes[4].pre[5].direction[1] = 'l';
  nodes[4].pre[5].direction[3] = 's';
  nodes[4].pre[5].direction[5] = 'u';
  nodes[4].pre[5].direction[6] = 'r';
  nodes[4].pre[6].direction[1] = 's';
  nodes[4].pre[6].direction[3] = 'r';
  nodes[4].pre[6].direction[5] = 'l';
  nodes[4].pre[6].direction[6] = 'u';

  nodes[5].pre[4].direction[4] = 'u';
  nodes[5].pre[4].direction[6] = 'l';
  nodes[5].pre[6].direction[4] = 'r';
  nodes[5].pre[6].direction[6] = 'u';

  nodes[6].pre[4].direction[4] = 'u';
  nodes[6].pre[4].direction[5] = 'r';
  nodes[6].pre[5].direction[4] = 'l';
  nodes[6].pre[5].direction[5] = 'u';
}
