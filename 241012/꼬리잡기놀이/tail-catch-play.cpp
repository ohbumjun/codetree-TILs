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
#include <deque>

#define endl "\n"
#define INT_MAX int(1e9)
#define MAX 10001

// #define DEBUG 1

using namespace std;

// n : 격자크기
// m : 팀 개수
// k : 라운드 수
int N, M, K;

// 초기 Board (변하는 정보)
vector<vector<int>> Board; 

int stGroup = 5;
int edGroup;

// 각 그룹 점수
unordered_map<int, int> Score;

unordered_map<int, vector<pair<int, int>>> GroupPoses;

// 4방향 (오,위,왼,아)
int dRow[] = { 0, -1, 0, 1 };
int dCol[] = { 1, 0, -1, 0 };

// group 만들기
void makeGroup(int headRow, int headCol, int group)
{
	// bfs
	// 방문 x
	// 상하좌우
	// 단, 현재가 head, 그 다음이 tail 이면 X
	vector<pair<int,int>> groupPos;
	vector<vector<bool>> check(N, vector<bool>(N, false));
	queue<pair<int, int>> q;
	q.push({ headRow, headCol });
	
	while (!q.empty())
	{
		int row, col;
		tie(row, col) = q.front();
		q.pop();

		groupPos.push_back({ row, col });
		check[row][col] = true;

		for (int d = 0; d < 4; ++d)
		{
			int nxtRow = row + dRow[d];
			int nxtCol = col + dCol[d];
			if (nxtRow < 0 || nxtRow >= N || nxtCol < 0 || nxtCol >= N)
				continue;
			// 0, 4 는 X
			if (Board[nxtRow][nxtCol] == 0 || Board[nxtRow][nxtCol] == 4)
				continue;
			if (row == headRow && col == headCol)
			{
				if (Board[nxtRow][nxtCol] == 3)
					continue;
			}
			if (check[nxtRow][nxtCol])
				continue;
			q.push({ nxtRow, nxtCol });
		}
	}

	GroupPoses[group] = groupPos;
}

void Input()
{
	cin >> N >> M >> K;

	Board.resize(N, vector<int>(N, 0));

	for (int r = 0; r < N; ++r)
		for (int c = 0; c < N; ++c)
			cin >> Board[r][c];

	int group = 5;
	/*
	* 	1) 팀을 짜야 한다.
	* - 순회하다가 1번을 만나면, 팀 구성
	* - BFS 로 구성
	* - 그리고 각  Board 를 팀 번호로 만들어준다.
	*   4보다 큰 값으로
	* 
	* - 팀 구성시 "머리" 바로 앞이 "꼬리" 인 케이스를 고려해야 한다.
	*   BFS 는 상하좌우 4방향 중 "미방문" 이면 가능
	*   단. 직전 좌표가 머리이면서, 다음이 3번이면 X
	*/
	for (int r = 0; r < N; ++r)
	{
		for (int c = 0; c < N; ++c)
		{
			// "머리" 부분을 시작으로 출발한다.
			if (Board[r][c] != 1)
				continue;
			makeGroup(r, c, group);
			const vector<pair<int, int>>& gPoses = GroupPoses[group];
			for (const pair<int, int>& pos : gPoses)
			{
				int gRow, gCol;
				tie(gRow, gCol) = pos;
				Board[gRow][gCol] = group;
			}
			Score[group] = 0;
			edGroup = group;
			group += 1;
		}
	}
}

void moveGroup(int group)
{
	// tail 을 먼저 pop 시킨다.
	vector<pair<int, int>>& groupPoses = GroupPoses[group];

	// 이동방식은 간단하다. tail pop 하고,
	// 새로운 head 를 추가하면 될 뿐이다.
	int prevTailRow, prevTailCol;
	tie(prevTailRow, prevTailCol) = groupPoses.back();
	groupPoses.pop_back();
	// 이동칸으로 복귀
	Board[prevTailRow][prevTailCol] = 4;

	// 새로운 머리칸 추가
	int headRow, headCol;
	tie(headRow, headCol) = groupPoses.front();
	for (int d = 0; d < 4; ++d)
	{
		int nxtRow = headRow + dRow[d];
		int nxtCol = headCol + dCol[d];
		// 범위 아웃
		if (nxtRow < 0 || nxtRow >= N || nxtCol < 0 || nxtCol >= N)
			continue;
		// 4 방향을 찾아가야 한다.
		if (Board[nxtRow][nxtCol] != 4)
			continue;
		// 그룹으로 표시
		Board[nxtRow][nxtCol] = group;
		groupPoses.insert(groupPoses.begin(), { nxtRow, nxtCol });
	}
}

void reverseGroup(int group)
{
	vector<pair<int, int>>& groupPoses = GroupPoses[group];
	vector<pair<int, int>> sorted;
	for (int i = groupPoses.size() - 1; i >= 0; --i)
		sorted.push_back(groupPoses[i]);
	groupPoses = sorted;
}

int getK(int group, int r, int c)
{
	int k = -1;

	for (int i = 0; i < GroupPoses[group].size(); ++i)
	{
		int row, col;
		tie(row, col) = GroupPoses[group][i];
		if (row == r && col == c)
		{
			k = i + 1;
			break;
		}
	}
	assert(k != -1);
	return k;
}

void Solve()
{
	/*
	* 2) 방향이 중요
	* - head, tail 은 구분해야 한다.
	* - 각 팀마다 좌표 정보를 모아둬도 된다. 
	* ([0.2] ~ 중간 ~ [0.1]) 
	* 맨 앞은 head, 맨 뒤는 tail
	* - 방향 전환은, 그냥 해당 배열을 reverse 해주면 된다.
	* 
	* 3) 이동
	* - 머리의 이동은 '4' 를 찾아가면 된다.
	*	- 이 과정에서 arr 수정
	*	- 새로운 좌표도 insert
	* 
	* - 꼬리 : team 배열 마지막 pop.
	*	- 단. 꼬리를 먼저 이동해야 한다.
	*	- 머리 다음이 "꼬리" 일 수 있기 때문이다.
	*/

	for (int k = 0; k < K; ++k)
	{
		// 각 팀 이동하기.
		for (int grp = stGroup; grp <= edGroup; ++grp)
			moveGroup(grp);

		// 공 던지기
		int lineDir, lineStRow, lineStCol;
		int divdend = k / N;
		int rest	= k % N;
		lineDir = divdend;
		if (divdend == 0)
		{
			// 위 -> 아
			lineStRow = rest;
			lineStCol = 0;
		}
		else if (divdend == 1)
		{
			// 왼 -> 오
			lineStRow = N - 1;
			lineStCol = rest;
		}
		else if (divdend == 2)
		{
			// 아 -> 위
			lineStRow = N - 1 - rest;
			lineStCol = N - 1;
		}
		else if (divdend == 3)
		{
			// 오 -> 왼
			lineStRow = 0;
			lineStCol = N - 1 - rest;
		}

		for (int n = 0; n < N; ++n)
		{
			int curLineRow = lineStRow + dRow[lineDir];
			int curLineCol = lineStCol + dCol[lineDir];
			// 범위 아웃이면 안된다.
			// 0 ~ 이동칸은 X
			if (Board[curLineRow][curLineCol] <= 4)
				break;
			int metGroup = Board[curLineRow][curLineCol];
			int metK = getK(metGroup, curLineRow, curLineCol);
			Score[metGroup] += (metK * metK);
			reverseGroup(metGroup);
			break;
		}
	}

	int ans = 0;

	for (int grp = stGroup; grp <= edGroup; ++grp)
		ans += Score[grp];

	cout << ans << endl;
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