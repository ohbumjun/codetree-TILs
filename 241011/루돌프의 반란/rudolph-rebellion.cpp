#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <thread>
#include <cassert>
#include <vector>
#include <string>
#include <cstring>
// #include <climits>
#include <set>
#include <queue>
#include <math.h>
#include <stack>
#include <algorithm>
#include <unordered_map>

#define endl "\n"
#define INT_MAX int(1e9)
#define MAX 10001

// #define DEBUG 1

using namespace std;

int N, M, P, C, D;

// 점수
vector<int> scores;
// 탈락 여부
vector<bool> alive;
// 기절 타임 (turn 수로 지정)
vector<int> stun;
// 위치
vector<pair<int, int>> poses;
// 루돌프 위치
int roudRow, roudCol;
// 위치 2차원 벡터
vector<vector<int>> board;
// 상우하좌
int dRow[] = { -1, 0, 1, 0 };
int dCol[] = { 0, 1, 0, -1 };

int getOptDir(int dir)
{
	if (dir == 0)
		return 2;
	else if (dir == 1)
		return 3;
	else if (dir == 2)
		return 0;
	else if (dir == 3)
		return 1;
}

void Input()
{
	cin >> N >> M >> P >> C >> D;

	cin >> roudRow >> roudCol;
	roudRow -= 1;
	roudCol -= 1;

	// - 1은 아무것도 X
	// 0 이상부터 산타
	board.resize(N, vector<int>(N, -1));

	scores.resize(P, 0);
	alive.resize(P, true);
	stun.resize(P, 0);
	poses.resize(P);

	for (int p = 0; p < P; ++p)
	{
		int n, r, c;
		cin >> n >> r >> c;
		n -= 1;
		r -= 1;
		c -= 1;
		poses[n] = { r, c };
		board[r][c] = n;
	}
}

void moveSanta(int santa, int row, int col, int rowDir, int colDir, int moveCnt)
{
	//row,col 에 있는 santa 를 rowDir, colDir 방향으로 moveCnt 만큼 이동시킴
	queue<tuple<int, int, int, int>> q;
	q.push({ santa, row, col, moveCnt });

	while (!q.empty())
	{
		int santa, curRow, curCol, curMoveCnt;
		tie(santa, curRow, curCol, curMoveCnt) = q.front();
		q.pop();

		int nxtRow = curRow + rowDir * curMoveCnt;
		int nxtCol = curCol + colDir * curMoveCnt;

		// 범위를 벗어난다면
		if (nxtRow < 0 || nxtRow >= N || nxtCol < 0 || nxtCol >= N)
		{
			// 해당 산타 탈락 처리하고
			// 바로 return
			alive[santa] = false;
			return;
		}
		else
		{
			// 범위를 벗어나지 않는다면
			// 1) 해당 위치에 산타가 있다면
			if (board[nxtRow][nxtCol] != -1)
			{
				int oldSanta = board[nxtRow][nxtCol];
				q.push({oldSanta, nxtRow, nxtCol, 1});

				// 새로운 산타로 세팅
				board[nxtRow][nxtCol] = santa;
				poses[santa] = { nxtRow, nxtCol };
			}
			else
			{
				// 2) 산타가 없다면
				// 새로운 위치로 이동 처리하고 return
				board[nxtRow][nxtCol] = santa;
				poses[santa] = { nxtRow, nxtCol };
				return;
			}
		}

	}
}

void Solve()
{
	/*
	1) 게임판 구성
	총 M 번의 턴
	- 매 턴마다 루돌프, 산타 움직임
	- 루돌프 1번. 이후 산타 1 ~ P 차례대로 이동
	  - 기절 혹은 탈락 산타는 이동 X
	- 거리 계산 : (r - r)^2 + (c - c)^2
	*/
	for (int turn = 0; turn < M; ++turn)
	{
		/*
		2) 루돌프 움직임
		- 가장 가까운 산타 1칸 돌진
		- 대신, 탈락하지 않은 산타여야 한다.
		- 가까운 산타 2명 이상 (행큰, 열큰)
		- 8방향 이동 가능
		4) 루돌프에 의한 충돌
		  - 산타 + C 점수
		  - 산타는 루돌프가 이동해온 방향 C 칸 이동
		  - 밀려난 위치 게임 밖 -> 탈락
		  - 밀려난 칸에 다른 산타 ? 상호작용
		  - 산타 기절, 2 턴 이후부터 이동 가능
		*/
		int minDist = INT_MAX;
		vector<pair<int,int>> minSantaInfos;

		for (int p = 0; p < P; ++p)
		{
			if (alive[p] == false)
				continue;
			
			//if (stun[p] > turn) 기절도 충돌 대상
			//	continue;

			int santaRow	= poses[p].first;
			int santaCol	= poses[p].second;
			int curDist		= (roudRow - santaRow) * (roudRow - santaRow) + 
				(roudCol - santaCol) * (roudCol - santaCol);

			if (minDist > curDist)
			{
				minDist = curDist;
				minSantaInfos.clear();
				minSantaInfos.push_back({ santaRow , santaCol});
			}
			else if (minDist == curDist)
			{
				minSantaInfos.push_back({ santaRow , santaCol});
			}
		}
		
		// 행큰, 얼큰
		sort(minSantaInfos.begin(), minSantaInfos.end());

		const pair<int,int>& minSantaPos = minSantaInfos[minSantaInfos.size() - 1];
		int minSantaRow = minSantaPos.first;
		int minSantaCol = minSantaPos.second;
		int minSanta = board[minSantaRow][minSantaCol];

		int rouMovR = 0, rouMovC = 0;

		if (roudRow < minSantaRow)
			rouMovR = 1;
		else if (roudRow > minSantaRow)
			rouMovR = -1;

		if (roudCol < minSantaCol)
			rouMovC = 1;
		else if (roudCol > minSantaCol)
			rouMovC = -1;

		roudRow += rouMovR;
		roudCol += rouMovC;

		// 루돌프에 의한 충돌
		if (board[roudRow][roudCol] != -1)
		{
			int collideSanta = board[roudRow][roudCol];

			// 점수
			scores[collideSanta] += C;

			// 기절 처리
			stun[collideSanta] = turn + 2;

			// 산타 기존 위치 update
			board[roudRow][roudCol] = -1;

			// 안에서 새로운 산타 위치 정보 세팅해줄 것이다.
			moveSanta(collideSanta, 
				roudRow, roudCol, rouMovR, rouMovC, C);
		}
		else
		{
			// 별다른 조치 X
		}

		/*
		3) 산타 움직임
		- 1 ~ P 번까지 "순서대로" 이동
		- 기절 or 탈락 X
		- 루돌프에게 가까워지는 방향으로 1칸
		- 다른 산타 있는 칸. 혹은 게임밖 X
		- 움직일 수 있는 칸이 있더라도, 루돌프와 더 가까워지지 않으면 X
		- 상우하좌 우선순위 이동
		 5) 산타에 의한 충돌
		  - D 점 획득
		  - 이동 방향 반대로 D 칸 이동
		  - 마찬가지로 밀려난 위치 게임 밖 ? -> 탈락
		  - 밀려난 칸에 다른 산타 ? 상호작용
		  - 산타 기절, 2 턴 이후부터 이동 가능
		*/
		for (int p = 0; p < P; ++p)
		{
			if (alive[p] == false)
				continue;
			if (stun[p] > turn)
				continue;
			int stRow = poses[p].first;
			int stCol = poses[p].second;
			int distToRou = (stRow - roudRow) * (stRow - roudRow)
				+ (stCol - roudCol) * (stCol - roudCol);
			int moveDir = -1;
			for (int d = 0; d < 4; ++d)
			{
				int nxtRow = stRow + dRow[d];
				int nxtCol = stCol + dCol[d];
				// 범위 밖
				if (nxtRow < 0 || nxtRow >= N || nxtCol < 0 || nxtCol >= N)
					continue;
				// 다른 산타 X
				int info = board[nxtRow][nxtCol];
				assert(info != p);
				if (info != -1)
					continue;
				int nxtDist = (nxtRow - roudRow) * (nxtRow - roudRow)
					+ (nxtCol - roudCol) * (nxtCol - roudCol);
				if (distToRou > nxtDist)
				{
					distToRou = nxtDist;
					moveDir = d;
				}
			}
			if (moveDir == -1)
				continue;
			// 이동 처리
			int moveRow = stRow + dRow[moveDir];
			int moveCol = stCol + dCol[moveDir];
			// 기존 위치 X
			board[stRow][stCol] = -1;
			// 루돌프 충돌 여부 확인하기
			if (moveRow == roudRow && moveCol == roudCol)
			{
				// -D 점 획득
				scores[p] += D;

				// - 산타 기절, 2 턴 이후부터 이동 가능
				stun[p] = turn + 2;

				moveDir = getOptDir(moveDir);

				// 산타 이동 처리 (moveSanta 내부에서 사용)
				// board[moveRow][moveCol] = p;

				// - 이동 방향 "반대로" D 칸 이동
				// - 마찬가지로 밀려난 위치 게임 밖 ? ->탈락
				// - 밀려난 칸에 다른 산타 ? 상호작용
				moveSanta(p, roudRow, roudCol, dRow[moveDir], dCol[moveDir], D);
			}
			else
			{
				// 위치 정보 update
				board[moveRow][moveCol] = p;
				poses[p] = { moveRow, moveCol };
			}
		}

		/*
		7) 게임 종료
		P 명 산타 모두 탈락시 게임 종료
		매턴 마자, 아직 탈락 X 산타는 점수 + 1
		*/
		bool allDead = true;
		for (int p = 0; p < P; ++p)
		{
			if (alive[p])
			{
				allDead = false;
				break;
			}
		}

		if (allDead)
			break;

		for (int p = 0; p < P; ++p)
		{
			if (alive[p] == false)
				continue;
			scores[p] += 1;
		}
	}

	// 각 산타가 얻은 최종 점수 출력
	for (int p = 0; p < P; ++p)
	{
		cout << scores[p] << " ";
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