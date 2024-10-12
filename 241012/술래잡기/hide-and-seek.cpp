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

// N : 격자 크기
// M : 도망자 수
// H : 나무 수
// K : 턴 횟수
int N, M, H, K;

// 술래 이동 방향 중심으로 진행하기
// 4방향 (위,오,아.왼)
int dRow[4] = { -1, 0, 1, 0 };
int dCol[4] = { 0, 1, 0, -1 };

// 도망자 정보들 (도망자들은 idx 로 표시)
vector<vector<vector<int>>> Board; 

vector<pair<int, int>> sulaePath;

// r, c, 바라보는 방향
vector<tuple<int, int, int>> Runners;

// 도망자 생존 여부
vector<bool> Alives;

// 만약 메모리 초과 걸리면, 그냥 1차원 벡터로 변경
vector<vector<bool>> Tree;

int sulaePathIdx = 0;
bool sulaeIncrease = true; // 정방향, 역방향
int sulaeDir = 0; // 맨 처음 위 방향

// 해당 위치로 이동후 방향을 변경해야 하는지 체크하는 함수
// (0,0) ~ (중앙) 에 도착한 케이스도 처리해준다.
void moveSulae()
{
	int nxtAccessIdx = sulaeIncrease ?
		sulaePathIdx + 1 : sulaePathIdx - 1;

	// (0.0)
	if (nxtAccessIdx == sulaePath.size() - 1)
	{
		// 다시 아래 방향
		sulaeIncrease = false;
		sulaeDir = 2;

	}
	else if (nxtAccessIdx == 0)
	{
		// 중앙 (다시 위 방향)
		sulaeIncrease = true;
		sulaeDir = 0;
	}
	else
	{
		int changeDir = sulaeIncrease ?
			1 : -1;

		// 나머지 경로상에 있는 경우.
		int curSulaePathIdx = sulaePathIdx;
		const pair<int, int>& curPos = sulaePath[curSulaePathIdx];
		int nxtSulaePathIdx = sulaePathIdx + changeDir;
		const pair<int, int>& nxtPos = sulaePath[nxtSulaePathIdx];
		int nxtnxtSulaePathIdx = sulaePathIdx + changeDir * 2;
		const pair<int, int>& nxtnxtPos = sulaePath[nxtnxtSulaePathIdx];
		
		int rowDiff = nxtPos.first - curPos.first;
		int colDiff = nxtPos.second - curPos.second;

		int nxtRowDiffs = nxtnxtPos.first - nxtPos.first;
		int nxtColDiffs = nxtnxtPos.second - nxtPos.second;

		// 4방향 (위,오,아.왼)
		// int dRow[4] = { -1, 0, 1, 0 };
		// int dCol[4] = { 0, 1, 0, -1 };

		if (rowDiff != nxtRowDiffs || colDiff != nxtColDiffs)
		{
			// 방향을 변경해야 한다.
			// nxtnxtPos 와 nxtPos 사이의 방향으로 변경해야 한다.
			if (nxtPos.first == nxtnxtPos.first)
			{
				// 행은 동일한데, 열 다름
				if (nxtPos.second < nxtnxtPos.second)
				{
					// 오른쪽
					sulaeDir = 1;
				}
				else
				{
					// 왼쪽
					sulaeDir = 3;
				}
			}
			else
			{
				// 열은 동일, 행 다름
				if (nxtPos.first < nxtnxtPos.first)
				{
					// 아래쪽
					sulaeDir = 2;
				}
				else
				{
					// 위쪽
					sulaeDir = 0;
				}
			}
		}
	}

	// 이동 처리
	sulaePathIdx = nxtAccessIdx;
}

void reverseSulaePath()
{
	// 이것을 뒤집는다.
	vector<pair<int, int>> temp;
	for (int i = sulaePath.size() - 1; i >= 0; --i)
		temp.push_back(sulaePath[i]);
	sulaePath = temp;
}

int turnRunnerDir(int d)
{
	// 좌우
	if (d == 0)
		return 2;
	if (d == 2)
		return 0;
	// 상하
	if (d == 1)
		return 3;
	if (d == 3)
		return 1;
}

void extractSulaePath()
{
	vector<vector<bool>> check(N, vector<bool>(N, false));

	int stR = 0, stC = 0, curD = 2;

	// 해당 방향으로 계속 가다가 이미 방문이면
	// d 을 - 1 하기
	queue<pair<int, int>> q;
	q.push({ stR, stC });

	while (!q.empty())
	{
		int r, c;
		tie(r, c) = q.front();
		q.pop();

		sulaePath.push_back({ r, c });
		check[r][c] = true;

		// 중앙에 도달
		if (r == N / 2 && c == N / 2)
			break;

		int nxtR = r + dRow[curD];
		int nxtC = c + dCol[curD];

		if (nxtR < 0 || nxtR >= N || nxtC < 0 || nxtC >= N
			|| check[nxtR][nxtC])
		{
			curD -= 1;
			if (curD < 0)
				curD = 3;
			nxtR = r + dRow[curD];
			nxtC = c + dCol[curD];
		}
		
		q.push({ nxtR, nxtC });
	};

	// 이것을 뒤집는다.
	vector<pair<int, int>> temp;
	for (int i = sulaePath.size() - 1; i >= 0; --i)
		temp.push_back(sulaePath[i]);
	sulaePath = temp;
}

void Input()
{
	cin >> N >> M >> H >> K;

	Board.resize(N, vector<vector<int>>(N));
	Tree.resize(N, vector<bool>(N, false));
	Runners.resize(M);
	Alives.resize(M, true);

	for (int m = 0; m < M; ++m)
	{
		int r, c, d;
		cin >> r >> c >> d;
		r -= 1;
		c -= 1;

		// 맨 처음에는 도망자 겹치지 X
		// 이동 중에는 겹칠 수 O
		if (d == 1)
		{
			// 좌우 유형
			// 오른쪽 보며 시작
			Runners[m] = { r, c, 1 };
		}
		else
		{
			// 상하 유형
			// 아래를 보며 시작
			Runners[m] = { r, c, 2 };
		}

		Board[r][c].push_back(m);
	}

	for (int h = 0; h < H; ++h)
	{
		int r, c;
		cin >> r >> c;
		r -= 1;
		c -= 1;
		Tree[r][c] = true;
	}
}

void eraseRunner(int row, int col, int runner)
{
	auto iter = find(
		Board[row][col].begin(),
		Board[row][col].end(),
		runner
	);
	int findIdx = distance(
		Board[row][col].begin(),
		iter
	);
	Board[row][col].erase(
		Board[row][col].begin() + findIdx
	);
}

void Solve()
{
	/*
	* >> N * N 격자
	* m 명의 도망자
	* - 중앙에서 시작 X
	* - 좌우 only / 상하 only 유형 2가지
	* - 좌우 only : 항상 right 보고 시작
	* - 상하 only : 항상 down 보고 시작
	*/

	/*
	* >> h 개의 나무
	* - 도망자와 겹쳐져서 주어질 수 있음
	*/
	int ans = 0;

	extractSulaePath();

	for (int turn = 1; turn <= K; ++turn)
	{
		/*
		* k 번 턴 => 2가지 과정 반복
		/

		/* 1) m 명 도망자 "동시에" 이동
		* - 술래와 거리 "3" "이하" 인 도망자만 이동
		* - 거리 abs() + abs()
		* - 이동 중간에 도망자들 위치는 겹칠 수 있다.
		*
		* a) 바라보는 칸 이동한 칸 -> 격자 안
		* - 술래가 있으면 이동 X
		* - 술래 없으면 해당 이동. 나무 있어도 상관 X
		*
		* b) 격자 밖
		* - 먼저 방향을 튼다.
		* - 바뀐 방향 1칸 이동 -> 술래 "없으면" 1칸 이동
		*
		*/
		for (int m = 0; m < M; ++m)
		{
			if (Alives[m] == false)
				continue;
			const tuple<int, int, int>& runnerInfo = Runners[m];
			int runRow, runCol, runDir;
			tie(runRow, runCol, runDir) = runnerInfo;
			int sulaeRow = sulaePath[sulaePathIdx].first;
			int sulaeCol = sulaePath[sulaePathIdx].second;
			int dist = abs(runRow - sulaeRow) + 
				abs(runCol - sulaeCol);
			if (dist > 3)
				continue;
			int nxtRow = runRow + dRow[runDir];
			int nxtCol = runCol + dCol[runDir];

			if (nxtRow < 0 || nxtRow >= N || nxtCol < 0 || nxtCol >= N)
			{
				// 격자 밖
				// 방향 전환
				runDir = turnRunnerDir(runDir);
				nxtRow = runRow + dRow[runDir];
				nxtCol = runCol + dCol[runDir];

				// 전환된 방향은 그대로 세팅해야 한다.
				if (nxtRow == sulaeRow && nxtCol == sulaeCol)
				{
					// 이동 X
					// 기존 위치로 정보 세팅
					Runners[m] = { runRow, runCol, runDir };
				}
				else
				{
					// 술래 없으면 이동
					Runners[m] = { nxtRow, nxtCol, runDir };

					// 기존 위치 도망자 제거
					eraseRunner(runRow, runCol, m);

					// 새로운 위치 도망자 추가
					Board[nxtRow][nxtCol].push_back(m);
				}
			}
			else
			{
				// 격자 안
				if (nxtRow == sulaeRow && nxtCol == sulaeCol)
				{
					// 이동 X
					// 기존 위치로 정보 세팅
					Runners[m] = { runRow, runCol, runDir };
				}
				else
				{
					// 술래 없으면 이동
					Runners[m] = { nxtRow, nxtCol, runDir };

					// 기존 위치 도망자 제거
					eraseRunner(runRow, runCol, m);

					// 새로운 위치 도망자 추가
					Board[nxtRow][nxtCol].push_back(m);
				}
			}
		}

		/*
		* 2) 그 다음 "술래" 이동
		* - 술래는 "정가운데" 에서 시작
		* - 달팽이 모양 이동 (시계방향)
		* - 끝에 도달하면, 다시 거꾸로 중심으로 (이 과정 반복)
		*
		* - 1턴 동안. 1칸 이동
		* - 이동 후, 위치가 "방향 틀어지는 지점" 이면 "바로 변경"
		* - (0.0) 혹은 (중앙)에 도달해도 "바로 변경"
		*
		* - "이동 이후"
		* - 시야에 있는 도망자 잡음.
		* - 술래위치 포함. 바라보는 방향 "3칸" 이내
		* - 해당 칸에 "나무" 있으면, "나무 칸" 의 "도망자" 는 안 잡힘.
		* - 잡힌 도망자는 사라진다.
		* - 술래 점수 획득 : turn * 도망자 수
		*/
		moveSulae();

		int catchRunnerCnt = 0;
		
		const pair<int, int>& sulaePos = sulaePath[sulaePathIdx];
		
		for (int i = 0; i < 3; ++i)
		{
			int visRow = sulaePos.first + dRow[sulaeDir] * i;
			int visCol = sulaePos.second + dCol[sulaeDir] * i;

			if (visRow < 0 || visRow >= N || visCol < 0 || visCol >= N)
				continue;

			if (Tree[visRow][visCol])
				continue;

			if (Board[visRow][visCol].size() == 0)
				continue;

			for (int jIdx = 0; jIdx < Board[visRow][visCol].size(); ++jIdx)
			{
				int runnerN = Board[visRow][visCol][jIdx];
				Alives[runnerN] = false;
			}

			catchRunnerCnt += turn * Board[visRow][visCol].size();

			Board[visRow][visCol].clear();
		}

		ans += catchRunnerCnt;
	}

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