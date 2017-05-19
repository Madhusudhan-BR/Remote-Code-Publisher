#pragma once
/////////////////////////////////////////////////////////////////////
// stronglyConnectedComp - This header file is used to detect      //
// ver 1                    connected components                   //
//-----------------------------------------------------------------//
// Madhusudhan B R SUID-914198627                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Mac 15inch   , Windows 10                          //
// Application: Project #2, CSE687 - Object Oriented Design, S2017 //
// Author:      Madhusudhan.B.R, Syracuse University.		       //
//									                               //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package contains a graph class tht is and functions used to detect scc
*
*
* - manual information
* - maintenance information
*
*
*
*
* Required Files:
* ---------------
* display.h
*
* Maintenance History:
*---------------------
* ver 1.0 : 03-10-2017
*
*
*
*
*
*
*
*
* - first release
*/
// A C++ package to find strongly connected components in a given
// directed graph using Tarjan's algorithm (single DFS)
#include<iostream>
#include <list>
#include <stack>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include "../FileMgr/FileMgr.h"
#include "../FileSystem/FileSystem.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../Display/Display.h"
#define NIL -1
using namespace std;

// A class that represents an directed graph
class Graph
{
	int V;    // No. of vertices
	list<int> *adj;    // A dynamic array of adjacency lists

	NoSQLDB::NoSqlDb<std::string> strongCompDb;
			   // A Recursive DFS based function used by SCC()
	void SCCUtil(int u, int disc[], int low[],
		stack<int> *st, bool stackMember[], std::unordered_map<int, std::string> mapFilesFromInt);
public:
	Graph(int V);   
	void addEdge(int v, int w);   
	NoSQLDB::NoSqlDb<std::string> SCC(std::unordered_map<int,std::string> mapFilesFromInt);    
};
// Constructor
Graph::Graph(int V)
{
	this->V = V;
	adj = new list<int>[V];
}
// function to add an edge to graph
void Graph::addEdge(int v, int w)
{
	adj[v].push_back(w);
}

// A recursive function that finds and prints strongly connected
// components using DFS traversal

void Graph::SCCUtil(int u, int disc[], int low[], stack<int> *st,
	bool stackMember[], std::unordered_map<int, std::string> mapFilesFromInt)
{
	static int time = 0;
	disc[u] = low[u] = ++time;
	st->push(u);
	stackMember[u] = true;

	list<int>::iterator i;
	for (i = adj[u].begin(); i != adj[u].end(); ++i)
	{
		int v = *i;  
		if (disc[v] == -1)
		{
			SCCUtil(v, disc, low, st, stackMember, mapFilesFromInt);
			low[u] = min(low[u], low[v]);
		}

		else if (stackMember[v] == true)
			low[u] = min(low[u], disc[v]);
	}

	int w = 0;  
	if (low[u] == disc[u])
	{	
		std::vector<string>		outVect;
		while (st->top() != u)
		{	
			
			w = (int)st->top();
			outVect.push_back(mapFilesFromInt[w]);
			displaySCC(mapFilesFromInt[w]);
			stackMember[w] = false;
			st->pop();
		}
		NoSQLDB::Element<std::string> ele;
		
		w = (int)st->top();
		outVect.push_back(mapFilesFromInt[w]);
		//cout << mapFilesFromInt[w] << "\n";
		cout <<"\n"<<std::endl;
		displaySCC(mapFilesFromInt[w]);
		ele.children = outVect;
		strongCompDb.saveRecord(mapFilesFromInt[u], ele);
		stackMember[w] = false;
		st->pop();
		
	}
}

// The function to do DFS traversal. It uses SCCUtil()
NoSQLDB::NoSqlDb<std::string> Graph::SCC(std::unordered_map<int, std::string> mapFilesFromInt)
{
	int *disc = new int[V];
	int *low = new int[V];
	bool *stackMember = new bool[V];
	stack<int> *st = new stack<int>();

	for (int i = 0; i < V; i++)
	{
		disc[i] = NIL;
		low[i] = NIL;
		stackMember[i] = false;
	}

	for (int i = 0; i < V; i++)
		if (disc[i] == NIL)
			SCCUtil(i, disc, low, st, stackMember, mapFilesFromInt);
	
	return strongCompDb;
}