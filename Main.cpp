
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <algorithm>
#include <vector>
#include <queue>
#include <iostream>
#include <tuple>


/*
1. create a 2d array space and init it wth the appropriate x,y, and d values.
2. -1 = wall
3. place start and end nodes
4. create 2 lists, one that is nodes you are looking from and others are discovered nodes.
5. place the starting node in the lookingNodes list.
6. look (x +1, y) (x - 1 , y) (x , y + 1) (x , y - 1). you can check diagonal as well with (x-1, y-1) etc.
7. if the node you are looking at's D value isnt 0 dont add it to the discovered list. so if -1 for wall or other number for nodes that have been looked at dont add
8. when you do discover a node, set the node you are lookng at's (not the node that is looking) D value ++
9. when done looking with that node, move to next looking node and add d=0 nodes to discovered until you are out of looking nodes.
10. then run through the discovered node array and remove duplicates.
11. then start from step 6 again until eventually you run out of nodes to discover and your discovered nodes list is empty.
12. starting from the ending node path backwards, by looking at the ending nodes neighbors and path to the one with the lowest D value
13. you eventually reach the starting node as it is the lowest number always. the path you have taken is the best path.

*/

class Wave : public olc::PixelGameEngine
{
public:
	Wave()
	{
		sAppName = "Wave";
	}

public:
					

	int convert2dto1d(int x, int y)
	{
		return x * mapWidth + y;
	}

	struct Node
	{
		int x;
		int y;
		int BigD;

		Node(int x, int y, int BigD) : x(x), y(y), BigD(BigD) {
		};
	};


	std::vector <Node*> nodesVec;
	Node* startNode = nullptr;
	Node* endNode = nullptr;
	const int nodeSize = 32;
	const int mapWidth = 15, mapHeight = 15;

	void drawNodes()
	{
		//draw nodes
		for (int x = 0; x < mapWidth; x++)
		{
			for (int y = 0; y < mapHeight; y++)
			{
				FillRect(x * nodeSize, y * nodeSize, 28, 28, nodesVec[convert2dto1d(x, y)]->BigD == -1 ? olc::WHITE : olc::BLUE);

				if (nodesVec[convert2dto1d(x, y)] == startNode)
					FillRect(x * nodeSize, y * nodeSize, 28, 28, olc::GREEN);

				if (nodesVec[convert2dto1d(x, y)] == endNode)
					FillRect(x * nodeSize, y * nodeSize, 28, 28, olc::RED);



			}
		}
	}


	void wave_prop()
	{
						
		for(auto b: nodesVec)
		{
			if (b->BigD != -1)
				b->BigD = 0;
		}
						
		endNode->BigD = 1;
		std::vector<Node*>lookingNodes;
		lookingNodes.push_back(endNode);
						

		do
		{
			std::vector<Node*> discoveredNodes;
			for (auto& b : lookingNodes)
			{
				int x = b->x;
				int y = b->y;
				int bigD = b->BigD;

				nodesVec[convert2dto1d(x, y)]->BigD = bigD;

				//east
				if ((x + 1) < mapWidth && nodesVec[convert2dto1d(x + 1, y)]->BigD == 0)
				{
					nodesVec[convert2dto1d(x + 1, y)]->BigD = bigD+1;
					discoveredNodes.push_back(nodesVec[convert2dto1d(x+1,y)]);
				}
				//south
				if ((y + 1) < mapHeight && nodesVec[convert2dto1d(x, y + 1)]->BigD == 0)
				{
					nodesVec[convert2dto1d(x, y + 1)]->BigD = bigD + 1;
					discoveredNodes.push_back(nodesVec[convert2dto1d(x, y+1)]);
				}
				//west
				if ((x - 1) >= 0 && nodesVec[convert2dto1d(x - 1, y)]->BigD == 0)
				{
					nodesVec[convert2dto1d(x - 1, y)]->BigD = bigD + 1;
					discoveredNodes.push_back(nodesVec[convert2dto1d(x-1, y)]);
				}
				//north
				if ((y - 1) >= 0 && nodesVec[convert2dto1d(x, y - 1)]->BigD == 0)
				{
					nodesVec[convert2dto1d(x, y - 1)]->BigD = bigD + 1;
					discoveredNodes.push_back(nodesVec[convert2dto1d(x, y-1)]);
				}
							
			}

			sort(discoveredNodes.begin(), discoveredNodes.end());
			discoveredNodes.erase(std::unique(discoveredNodes.begin(), discoveredNodes.end()), discoveredNodes.end());
			lookingNodes.clear();
			lookingNodes.insert(lookingNodes.begin(), discoveredNodes.begin(), discoveredNodes.end());
							

						
		} while (!lookingNodes.empty()); //keep going until there are no more nodes to be discovered

	}

	void drawLine()
	{
		
		//check n,e,s,w's bigd and if its not -1 add it to the path list. then find the min of the path list until you eventually reach the starting node or there is no path
		int startX = startNode->x, startY = startNode->y; int currX = startX, currY = startY;
		std::list<std::pair<int, int>> pathList;
		pathList.push_back({ startX, startY });
		std::vector<std::pair<int, int>> truePath;
		bool pathExists = true;
		int minNum = 99999, tempx = 0,tempy = 0;
		if (startNode->BigD > 0) {
			do
			{
				//north
				if ((currY - 1) >= 0 && nodesVec[convert2dto1d(currX, currY - 1)]->BigD > 0)
				{
					pathList.push_back({ currX, currY - 1 });
				}
				//east
				if ((currX + 1) < mapWidth && nodesVec[convert2dto1d(currX + 1, currY)]->BigD > 0)
				{
					pathList.push_back({ currX + 1, currY });
				}
				//south
				if ((currY + 1) < mapHeight && nodesVec[convert2dto1d(currX, currY + 1)]->BigD > 0)
				{
					pathList.push_back({ currX, currY + 1 });
				}
				//west
				if ((currX - 1) >= 0 && nodesVec[convert2dto1d(currX - 1, currY)]->BigD > 0)
				{
					pathList.push_back({ currX - 1, currY });
				}

				if (pathList.empty()) //if no path can be found come out of the while loop
				{
					pathExists = false;
				}
				else
				{
					pathList.sort();
					for (auto n : pathList)
					{
						int comparatorNum = nodesVec[convert2dto1d(n.first, n.second)]->BigD;
						if (minNum > comparatorNum)
						{
							minNum = nodesVec[convert2dto1d(n.first, n.second)]->BigD;
							tempx = n.first;
							tempy = n.second;
						}
					}
					currX = tempx;
					currY = tempy;
					truePath.push_back({ currX,currY });
					pathList.clear();
				}
			} while (pathExists && (currX != endNode->x || currY != endNode->y));

			if (pathExists) {
				int tempX = startNode->x, tempY = startNode->y;
				
				for (auto b : truePath)
				{
					DrawLine(tempX * nodeSize + ((nodeSize) / 2), tempY * nodeSize + ((nodeSize) / 2), b.first * nodeSize + ((nodeSize) / 2), b.second * nodeSize + ((nodeSize) / 2), olc::YELLOW);
					tempX = b.first;
					tempY = b.second;
				}
			}
		}
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		for (int x = 0; x < mapWidth; x++)
		{
			for (int y = 0; y < mapHeight; y++)
			{
				if (x == 0 || y == 0 || x == (mapWidth - 1) || y == (mapHeight - 1))
					nodesVec.push_back(new Node(x,y,-1));
				else
					nodesVec.push_back(new Node(x,y,0));
			}
		}
		startNode = nodesVec[convert2dto1d(1, mapHeight / 2)];
		endNode = nodesVec[convert2dto1d(mapWidth - 2, mapHeight / 2)];
		endNode->BigD = 1;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		olc::vd2d vMouse = { (double)GetMouseX() / nodeSize, (double)GetMouseY() / nodeSize };

		if (GetMouse(0).bPressed) // Use mouse to draw maze, shift and ctrl to place start and end
		{
			if (vMouse.x >= 0 && vMouse.x < mapWidth) {
				if (vMouse.y >= 0 && vMouse.y < mapHeight)
				{
					if (GetKey(olc::SHIFT).bHeld)
						startNode = nodesVec[convert2dto1d(vMouse.x, vMouse.y)];
					else if (GetKey(olc::CTRL).bHeld)
						endNode = nodesVec[convert2dto1d(vMouse.x, vMouse.y)];
					else if (nodesVec[convert2dto1d(vMouse.x, vMouse.y)]->BigD == -1)
						nodesVec[convert2dto1d(vMouse.x, vMouse.y)]->BigD = 0;
					else
						nodesVec[convert2dto1d(vMouse.x, vMouse.y)]->BigD = -1;

					wave_prop();
				}
			}
		}

		drawNodes();
		drawLine();

		for (auto n : nodesVec)
			DrawString(n->x * nodeSize, n->y * nodeSize, std::to_string(n->BigD), olc::VERY_DARK_YELLOW);

		

		return true;
	}
};


int main()
{
	Wave path;
	if (path.Construct(512, 480, 2, 2))
		path.Start();

	return 0;
}
				