// #include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <set>
#include <map>
#include <bitset>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;
#define MAXD 32
#define MAXN 512
struct Inst;
bitset<MAXN> IN[MAXD], OUT[MAXD];
bitset<MAXN> DEF[MAXD], USE[MAXD];
map<string, int> BBname, BBvar, BBdef;
map<int, string> invBBname, invBBvar, invBBdef;
map<int, vector<int>> BBgraph, invBBgraph;
map<int, vector<Inst>> BBinst;
struct Inst {
	int defId, varId;
	vector<int> opargs;
	void print() {
		printf("\t\t%s : %s ,", invBBdef[defId].c_str(), invBBvar[varId].c_str());
		for (auto e : opargs)
			printf(" %s", invBBvar[e].c_str());
		puts("");
	}
};
static void listBB() {
	puts("=== Basic Block Information ===");
	for (auto &e : BBinst) {
		int bid = e.first;
		printf("\t--- %s ---\n", invBBname[bid].c_str());
		vector<Inst> &instSet = e.second;
		for (auto &I : instSet)
			I.print();
	}
	puts("===\n");
}
static void printBBinfo(int bid) {
	printf("\t--- %s ---\n", invBBname[bid].c_str());
	printf("\t\tDEF :");
	for (int i = 0; i < BBvar.size(); i++) {
		if (DEF[bid][i])
			printf(" %s", invBBvar[i].c_str());
	}
	puts("");
	printf("\t\tUSE :");
	for (int i = 0; i < BBvar.size(); i++) {
		if (USE[bid][i])
			printf(" %s", invBBvar[i].c_str());
	}
	puts("");
	printf("\t\tIN  :");
	for (int i = 0; i < BBvar.size(); i++) {
		if (IN[bid][i])
			printf(" %s", invBBvar[i].c_str());
	}
	puts("");
	printf("\t\tOUT :");
	for (int i = 0; i < BBvar.size(); i++) {
		if (OUT[bid][i])
			printf(" %s", invBBvar[i].c_str());
	}
	puts("");
}
static string trim(string s) {  
    if (s.empty())
        return s;  
    s.erase(0, s.find_first_not_of(" "));  
    s.erase(s.find_last_not_of(" ") + 1);
    return s;  
}
static int BBlookup(string s) {
	int sz = BBname.size();
	if (BBname.count(s))
		return BBname[s];
	invBBname[sz] = s;
	return BBname[s] = sz;
}
static int Deflookup(string s) {
	int sz = BBdef.size();
	if (BBdef.count(s))
		return BBdef[s];
	invBBdef[sz] = s;
	return BBdef[s] = sz;
}
static int Varlookup(string s) {
	int sz = BBvar.size();
	if (BBvar.count(s))
		return BBvar[s];
	invBBvar[sz] = s;
	return BBvar[s] = sz;
}
static void clearGraph() {
	BBname.clear();
	BBgraph.clear();
}
static void clearInst() {
	BBdef.clear();
	BBvar.clear();
	BBinst.clear();
}
static void initTable() {
	for (int i = 0; i < BBdef.size(); i++) {
		IN[i] = bitset<MAXN>();
		OUT[i] = bitset<MAXN>();
		DEF[i] = bitset<MAXN>();
		USE[i] = bitset<MAXN>();
	}
}
static void readGraph() {
	clearGraph();
	string line, trash;
	int stId = BBlookup("Entry");
	int edId = BBlookup("Exit");
	BBgraph[stId] = invBBgraph[stId] = vector<int>();
	BBgraph[edId] = invBBgraph[edId] = vector<int>();
	while (getline(cin, line)) {
		line = trim(line);
		if (line == "```")
			return ;
		stringstream sin(line);
		string u, v;
		sin >> u >> trash >> v;
		int uid, vid;
		uid = BBlookup(u), vid = BBlookup(v);
		BBgraph[uid].push_back(vid);
		invBBgraph[vid].push_back(uid);
	}
}
static void readInst() {
	clearInst();
	string line, trash;
	getline(cin, line);
	line = trim(line);
	if (line == "```")
		return ;
	do {
		string bname = line;
		int bid = BBlookup(bname);
		while (getline(cin, line)) {
			line = trim(line);
			if (line[0] == 'B' || line[0] == '`')	break;
			int did, vid, oid;
			string def, var;
			stringstream tin(line);
			tin >> def >> var;
			Inst inst;
			inst.defId = Deflookup(def);
			inst.varId = Varlookup(var);
			while (tin >> var)
				inst.opargs.push_back(Varlookup(var));
			BBinst[bid].push_back(inst);
		}
		if (line == "```")
			break;
	} while (true);
	int stId = BBlookup("Entry");
	int edId = BBlookup("Exit");
	BBinst[stId], BBinst[edId];
}
static void readIn() {
	string line;
	while (getline(cin, line)) {
		line = trim(line);
		if (line == "```") {
			readGraph();
			break;
		}
	}
	while (getline(cin, line)) {
		line = trim(line);
		if (line == "```") {
			readInst();
			break;
		}
	}
}
static bool assignAndDiff(bitset<MAXN> &A, bitset<MAXN> B) {
	bool test = (A != B);
	A = B;
	return test;
}
static void execute() {
	initTable();
	for (auto &e : BBinst) {
		int bid = e.first;
		vector<Inst> &instSet = e.second;
		for (auto &I : instSet) {
			DEF[bid].set(I.varId);
			for (auto &it : I.opargs)
				USE[bid].set(it);
		}
	}
	
	bool changed = false;
	do {
		changed = false;
		for (auto &e : BBinst) {
			int bid = e.first;
			changed |= assignAndDiff(IN[bid], USE[bid] | (OUT[bid]^(OUT[bid]&DEF[bid])));
			for (auto &succ : BBgraph[bid])
				changed |= assignAndDiff(OUT[bid], OUT[bid] | IN[succ]);
		}
	} while (changed == true);
}
static void writeOut() {
	listBB();
	puts("=== Basic Block Liveness ===");
	for (auto &e : BBinst) {
		int bid = e.first;
		printBBinfo(bid);
	}
	puts("===\n");
}
int main() {
	readIn();
	execute();
	writeOut();
	return 0;
}
