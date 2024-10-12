#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <thread>
#include <cassert>
#include <vector>
#include <string>
#include <cstring>
#include <set>
#include <queue>
#include <math.h>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <sstream>

#define endl "\n"
#define INT_MAX int(1e9)
#define MAX 10001

// #define DEBUG 1

using namespace std;

int Q;
int N; // 도시 수
int M; // 간선 수

vector<vector<int>> minDistGraph;

// key : id
// value : <stCity, rev, dest>
struct Product
{
	int stCity;
	int rev;
	int dest;
};
unordered_map<int, Product> managedList;

int currentCity = 0;

vector<string> splitStr(string input, char deli)
{
	vector<string> answer;
	stringstream ss(input);
	string temp;

	while (getline(ss, temp, deli))
	{
		answer.push_back(temp);
	}

	return answer;
}

void Input()
{
	cin >> Q;

	// 1) 코드 트리 랜드 건설
	// lie 단위로 입력받는다
	string firstCmdStr;
	getline(cin, firstCmdStr, '\n');
	getline(cin, firstCmdStr, '\n');

	vector<string> firstCmds = splitStr(firstCmdStr, ' ');

	// N = *firstCmds[1].c_str() - '0';
	N = stoi(firstCmds[1].c_str());
	// M = *firstCmds[2].c_str() - '0';
	M = stoi(firstCmds[2].c_str());

	minDistGraph.resize(N, vector<int>(N, INT_MAX));

	// 자기 자신으로의 거리는 0 으로 초기화 한다.
	for (int i = 0; i < N; ++i)
		minDistGraph[i][i] = 0;

	firstCmds.erase(firstCmds.begin() + 0);
	firstCmds.erase(firstCmds.begin() + 0);
	firstCmds.erase(firstCmds.begin() + 0);

	for (int idx = 0; idx < firstCmds.size() / 3; ++idx)
	{
		int city1  = stoi(firstCmds[idx * 3]);
		// int city1  = *firstCmds[idx * 3].c_str() - '0';
		int city2  = stoi(firstCmds[idx * 3 + 1]);
		// int city2  = *firstCmds[idx * 3 + 1].c_str() - '0';
		int weight = stoi(firstCmds[idx * 3 + 2]);
		// int weight = *firstCmds[idx * 3 + 2].c_str() - '0';

		// 방향 없는 간선
		minDistGraph[city1][city2] = min(minDistGraph[city1][city2], weight);
		minDistGraph[city2][city1] = min(minDistGraph[city2][city1], weight);
	}

	// 각 도시 사이의 최단 거리 정보를 update 해줘야 한다.
	// 알고리즘 사용
	for (int k = 0; k < N; ++k)
	{
		for (int f = 0; f < N; ++f)
		{
			for (int s = 0; s < N; ++s)
			{
				if (f == k || s == k)
					continue;
				int FKDist = minDistGraph[f][k];
				int SKDist = minDistGraph[k][s];
				minDistGraph[f][s] = min(
					minDistGraph[f][s], FKDist + SKDist);
			}
		}
	}
}

void Solve()
{
	/*
	* N 개 도시, M 개 간선
	* - 각 도시는 0 ~ N-1 번호
	* - 간선은 방향 X
	* - 2개 도시 연결 간선 "여러개" 가능
	* - "자기 자신" 연결 간선 존재 가능
	*/

	/*
	* 출발지 : '0' 번
	* 
	* 5가지 명령
	* 
	* 1) 코드 트리 랜드 건설
	* - 도시 수 n, 간선 수 m, 가중치 w
	* 
	* 2) 여행 상품 생성 (200)
	* - id, rev, dest 여행 상품 생성,
	* - "관리 목록" 에 추가
	* - 각 고유 id, 매출 rev, 도착지 dst
	* 
	* 3) 여행 상품 취소 (300)
	* - 고유 식별자 id "존재" 할 경우, 
	* "관리 목록" 에서 삭제
	* 
	* 4) 최적 여행 상품 판매 (400)
	* - "관리 목록" 중에서 "최적 여행 상품" 판매
	* - cost : "현재" 여행 상품 "출발지" ~ id 상품 도착지 까지 "최단 거리" 
	* 
	* if. 출발 ~ dest 도달 불가 시, "판매 불가" 상품
	* cost > rev 여서 "이득" 이 없어도 "판매 불가" 상품
	* 
	* 조건
	* a) - rev - cost 가 "최대" 
	* b) a)가 동일하면, id 가 가장 작은 상품
	* 
	* 가장 높은 우선순위 "1개" 판매.
	* 해당 상품 "id" 출력
	* "관리 목록" 에서 제거
	* 
	* 만약 판매 가능 상품 0 개 ? -> '-1' 출력
	*		"관리 목록" 제거 X
	* 
	* 5) 여행 상품 출발지 변경 (500)
	* - 여행 상품 출발지를 "전부" "s" 로 변경
	* - 이에 따라 각 상품의 cost id (비용) 은 달라짐
	*/

	// Q - 1 ? 맨 처음 input 받음
	for (int q = 0; q < Q - 1; ++q)
	{
		string cmdStrings;
		getline(cin, cmdStrings, '\n');
		vector<string> infos = splitStr(cmdStrings, ' ');

		string cmdKind = infos[0];

		if (cmdKind == "200")
		{
			// *2) 여행 상품 생성(200)
			// * -id, rev, dest 여행 상품 생성,
			// * -"관리 목록" 에 추가
			// * -각 고유 id, 매출 rev, 도착지 dst

			int id	= stoi(infos[1].c_str());
			// int id	= *infos[1].c_str() - '0';
			int rev = stoi(infos[2].c_str());
			// int rev = *infos[2].c_str() - '0';
			int dest = stoi(infos[3].c_str());
			// int dest = *infos[3].c_str() - '0';

			managedList[id] = { 0, rev, dest };
		}
		else if (cmdKind == "300")
		{
			// *3) 여행 상품 취소(300)
			// * -고유 식별자 id "존재" 할 경우,
			// * "관리 목록" 에서 삭제

			int id = stoi(infos[1].c_str());
			// int id = *infos[1].c_str() - '0';

			managedList.erase(id);
		}
		else if (cmdKind == "400")
		{
			// *4) 최적 여행 상품 판매(400)
			// * -"관리 목록" 중에서 "최적 여행 상품" 판매
			// * -cost : "현재" 여행 상품 "출발지" ~
			//			id 상품 도착지 까지 "최단 거리"
			// *if.출발 ~dest 도달 불가 시, "판매 불가" 상품
			// * cost > rev 여서 "이득" 이 없어도 "판매 불가" 상품
			// *
			// *조건
			// * a) - rev - cost 가 "최대"
			// * b) a)가 동일하면, id 가 가장 작은 상품
			// *
			// * 가장 높은 우선순위 "1개" 판매.
			// * 해당 상품 "id" 출력
			// * "관리 목록" 에서 제거
			// *
			// *만약 판매 가능 상품 0 개 ? -> '-1' 출력
			// * "관리 목록" 제거 X
			int maxProfit = -1;
			vector<int> sellPossibles;

			for (auto iter = managedList.begin();
				iter != managedList.end();
				++iter)
			{
				int pId = iter->first;
				int pStCity = iter->second.stCity;
				int pDest = iter->second.dest;
				int distBet = minDistGraph[pStCity][pDest];

				if (distBet == INT_MAX)
					continue;

				int cost = distBet;
				int profit = iter->second.rev - cost;

				// < 0 으로 해야하는지는 고민해야 한다.
				if (profit < 0)
					continue;

				if (profit > maxProfit)
				{
					sellPossibles.clear();
					sellPossibles.push_back(pId);
					maxProfit = profit;
				}
				else if (profit == maxProfit)
				{
					sellPossibles.push_back(pId);
				}
			}

			if (sellPossibles.size() == 0)
			{
				cout << -1 << endl;
			}
			else
			{
				sort(sellPossibles.begin(), sellPossibles.end());
				int sellPId = sellPossibles[0];
				cout << sellPId << endl;;
				managedList.erase(sellPId);
			}
		}
		else if (cmdKind == "500")
		{
			// *5) 여행 상품 출발지 변경(500)
			// * -여행 상품 출발지를 "전부" "s" 로 변경
			// * -이에 따라 각 상품의 cost id(비용) 은 달라짐

			// int s = *infos[1].c_str() - '0';
			int s = stoi(infos[1].c_str());

			for (auto iter = managedList.begin();
				iter != managedList.end();
				++iter)
			{
				int pId = iter->first;
				managedList[pId].stCity = s;
			}
		}
	}
}

int main() {
	ios::sync_with_stdio(false);

	cin.tie(NULL);
	cout.tie(NULL);

	// freopen("input_c.txt", "r", stdin);

	Input();

	Solve();

	return 0;
}