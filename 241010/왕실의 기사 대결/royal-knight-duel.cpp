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

// L : 행, 열
// N : 기사 수
// Q : 명령 수
int L, N, Q; 

int ans = 0;

// 0 : 빈캄
// 1 : 함정
// 2 : 벽
vector<vector<int>> Board; 

vector<int> totDamaged;			// 각 기사가 받은 데미지
vector<bool> alive;				// 생존 여부
vector<pair<int, int>> stPos;	// 각 기사 위치(r,c)
vector<pair<int, int>> length;	// 각 기사 너비(h,w)
vector<int> health;				// 각 기사 체력
vector<pair<int, int>> commands;

// 북, 동, 남, 서
int dRow[]  = { -1, 0, 1, 0 };
int dCol[]  = { 0, 1, 0, -1 };

void Input()
{
    // L x L 체스판
	// 빈칸, 함정, 벽 / 체스 밖도 벽으로 간주
	// - 각 기사 위츼 r,c
	// - r,c 기준, (h * w) 만큼 직사각형
	// - 각 기사 체력 k
	cin >> L >> N >> Q;

	// 위아래, 좌우 각각 "벽"을 추가한다.
	Board.resize(L + 2, vector<int>(L + 2, 0));
	totDamaged.resize(N, 0);
	alive.resize(N, true);

	for (int r = 1; r < L + 1; ++r)
		for (int c = 1; c < L + 1; ++c)
			cin >> Board[r][c];

	// 벽 세우기
	// 왼
	for (int r = 0; r < L + 2; ++r)
		Board[r][0] = 2;
	// 오
	for (int r = 0; r < L + 2; ++r)
		Board[r][L+1] = 2;
	// 위
	for (int c = 0; c < L + 2; ++c)
		Board[0][c] = 2;
	// 아래
	for (int c = 0; c < L + 2; ++c)
		Board[L+1][c] = 2;

	// Debug
	// {
	// 	cout << "Board" << endl;
	// 	for (int r = 0; r < L + 2; ++r)
	// 	{
	// 		for (int c = 0; c < L + 2; ++c)
	// 			cout << Board[r][c] << " ";
	// 		cout << endl;
	// 	}
	// }

	for (int n = 0; n < N; ++n)
	{
		int r, c, h, w, k;
		cin >> r >> c >> h >> w >> k;

		// 이때 기사들끼리 겹치지 않음
		// 기사와 벽도 겹치지 않음.
		// 이때, 위,왼쪽으로 벽이 추가되었기 때문에 r,c에 1을 더해준다.
		stPos.push_back({ r, c });
		length.push_back({ h, w });
		health.push_back(k);
	}

	for (int q = 0; q < Q; ++q)
	{
		// 이미 사라진 기사 번호가 주어질 수 있음.
		int i, d;
		cin >> i >> d;
		commands.push_back({ i - 1, d });
	}
}

int getNearKnight(int curKnight, int row, int col)
{
	// 해당 row, col 에 있는 다른 knight 가 있는지 구한다.
	// 만약 없다면 -1 을 리턴한다.

	for (int k = 0; k < N; ++k)
	{
		if (alive[k] == false)
			continue;
		if (k == curKnight)	// 현재 기사와 같은 경우 비교할 필요 X
			continue;

		int stRow = stPos[k].first;
		int stCol = stPos[k].second;

		int height = length[k].first;
		int width = length[k].second;

		int edRow = stRow + height - 1;
		int edCol = stCol + width - 1;

		if (stRow <= row && row <= edRow &&
			stCol <= col && col <= edCol)
		{
			return k;
		}
	}

	return -1;
}

void findKnightAtDir(int dir, int knight, 
	vector<int>& knightsOnWay, vector<bool>& check)
{
	if (check[knight])
		return;

	knightsOnWay.push_back(knight);
	check[knight] = true;

	int stRow = stPos[knight].first;
	int stCol = stPos[knight].second;

	int height = length[knight].first;
	int width = length[knight].second;

	int rightCol = stCol + width - 1;
	int leftCol = stCol;

	int upRow = stRow;
	int downRow = stRow + height - 1;

	if (dir == 0) // 위
	{
		// 가장 위쪽 row 를 모아둔다.
		// 각 칸 마다, 위쪽에, 인접한 기사가 있는지 확인한다.
		// 만약 벽이라면 x
		for (int i = 0; i < width; ++i)
		{
			int curRow = upRow;
			int curCol = leftCol + i;
			int nxt = getNearKnight(knight, curRow - 1, curCol);
			if (nxt == -1)
				continue;
			findKnightAtDir(dir, nxt, knightsOnWay, check);
		}
	}
	else if (dir == 1)	// 오
	{
		for (int i = 0; i < height; ++i)
		{
			int curRow = upRow + i;
			int curCol = rightCol;
			int nxt = getNearKnight(knight, curRow, curCol + 1);
			if (nxt == -1)
				continue;
			findKnightAtDir(dir, nxt, knightsOnWay, check);
		}
	}
	else if (dir == 2) // 아래
	{
		for (int i = 0; i < width; ++i)
		{
			int curRow = downRow;
			int curCol = leftCol + i;
			int nxt = getNearKnight(knight, curRow + 1, curCol);
			if (nxt == -1)
				continue;
			findKnightAtDir(dir, nxt, knightsOnWay, check);
		}
	}
	else if (dir == 3) // 왼
	{
		for (int i = 0; i < height; ++i)
		{
			int curRow = upRow + i;
			int curCol = leftCol;
			int nxt = getNearKnight(knight, curRow, curCol - 1);
			if (nxt == -1)
				continue;
			findKnightAtDir(dir, nxt, knightsOnWay, check);
		}
	}
}

bool checkMove(int knight, int dir)
{
	int stRow = stPos[knight].first;
	int stCol = stPos[knight].second;

	int height = length[knight].first;
	int width = length[knight].second;

	int rightCol = stCol + width - 1;
	int leftCol = stCol;

	int upRow = stRow;
	int downRow = stRow + height - 1;

	if (dir == 0) // 위
	{
		// 위 모든 row 에 대해 검사
		for (int c = leftCol; c <= rightCol; ++c)
		{
			int curRow = upRow - 1;
			int curCol = c;
			if (Board[curRow][curCol] == 2)
				return false;
			if (getNearKnight(knight, curRow, curCol) != -1)
				return false;
		}
	}
	else if (dir == 1) // 오
	{
		// 오른쪽 모든 col 에 대해 검사
		for (int r = upRow; r <= downRow; ++r)
		{
			int curRow = r;
			int curCol = rightCol + 1;

			if (Board[curRow][curCol] == 2)
				return false;
			if (getNearKnight(knight, curRow, curCol) != -1)
				return false;
		}
	}
	else if (dir == 2) // 아
	{
		// 맨 아래 행에 대해 모두 검사
		for (int c = leftCol; c <= rightCol; ++c)
		{
			int curRow = downRow + 1;
			int curCol = c;
			if (Board[curRow][curCol] == 2)
				return false;
			if (getNearKnight(knight, curRow, curCol) != -1)
				return false;
		}
	}
	else if (dir == 3) // 왼
	{
		// 왼쪽 모든 col 에 대해 검사
		for (int r = upRow; r <= downRow; ++r)
		{
			int curRow = r;
			int curCol = leftCol - 1;

			if (Board[curRow][curCol] == 2)
				return false;
			if (getNearKnight(knight, curRow, curCol) != -1)
				return false;
		}
	}

	return true;
}

int calDamage(int knight)
{
	int damage = 0;

	int stRow = stPos[knight].first;
	int stCol = stPos[knight].second;

	int height = length[knight].first;
	int width = length[knight].second;

	for (int r = stRow; r < stRow + height; ++r)
		for (int c = stCol; c < stCol + width; ++c)
			if (Board[r][c] == 1)
				damage++;

	return damage;
}

void Solve()
{
	// 1) 기사 이동
	// - 상하좌우 중 이동
	//	- 이동하려는 위치에 다른 기사 있음
	//		- 그 기사도 연쇄적으로 한칸 밀려난다. (이 과정 연속적)
	//  - 근데, 이동 방향에 "벽" 이 있다면, 모든 기사 이동 불가
	//		- 당연히 범위 밖도 "벽" 
	//  - 체스판 사라진 기사에게 "명령" 내려도 이동 X

	// 2) 대결 데미지
	// - 다른 기사 밀치게 되면, 밀려난 기사들 피해 입음
	//		- 최종 이동한 곳 기준 w*h 직사각형 범위 함정 "개수" 만큼 피해
	//		- 즉, 밀렸더라도, 해당 위치에 함정 없다면, 피해 X
	// - 피해만큼 체력 깎임
	//		- 남은 체력보다 큰 피해 -> 체스판에서 사라짐
	// - 단, 명령 받은 기사는 피해 X
	//		- "모두 밀린 이후" 데미지 계산

	// 3) Q 번 명령 -> 이후, 생존 기사들이 받은 "총 데미지 합"
	for (int q = 0; q < Q; ++q)
	{
		int knight = commands[q].first;
		int dir = commands[q].second;

		// 해당 기사가 이미 죽었다면 skip
		if (alive[knight] == false)
			continue;

		// findKnightAtDir 함수를 통해 해당 방향 끝에 있는 기사를 확인
		// 해당 기사를 기준으로 한칸 이동이 가능한지 확인
		vector<bool> knightsCheck(N, false);
		vector<int> knightsOnWay;
		findKnightAtDir(dir, knight, knightsOnWay, knightsCheck);
		
		// dir 에 따라 가장 끝 knight 를 확인해야 한다.
		// int endKnight = knightsOnWay.back();
		
		sort(knightsOnWay.begin(), knightsOnWay.end(), 
			[dir](int leftK, int rightK) {
				int leftStRow = stPos[leftK].first;
				int leftStCol = stPos[leftK].second;

				int leftHeight = length[leftK].first;
				int leftWidth = length[leftK].second;

				int leftEndRow = leftStRow + leftHeight - 1;
				int leftEndCol = leftStCol + leftWidth - 1;
				
				int rightStRow = stPos[rightK].first;
				int rightStCol = stPos[rightK].second;

				int rightHeight = length[rightK].first;
				int rightWidth = length[rightK].second;

				int rightEndRow = rightStRow + rightHeight - 1;
				int rightEndCol = rightStCol + rightWidth - 1;

				// 위
				if (dir == 0)
					return leftStRow > rightStRow; // 위가 뒤로
				// 오
				if (dir == 1)
					return leftEndCol < rightEndCol; // 오른쪽이 뒤로
				// 아
				if (dir == 2)
					return leftEndRow < rightEndRow; // 아래쪽이 뒤로
				// 왼
				if (dir == 3)
					return leftStCol > rightStCol; // 왼쪽이 뒤로
			});

		bool allMoved = true;

		// 각 knight 마다 이동 가능한지를 확인해야 한다.
		// idx, row, col
		vector<tuple<int, int, int>> oldPoses;
		for (int i = knightsOnWay.size() - 1; i >= 0; --i)
		{
			// 만약 가능하다면, 모두 이동
			int movedKnight = knightsOnWay[i];
			bool moveEnable = checkMove(movedKnight, dir);

			// 여기서 일단 잠깐 이동은 시켜줘야 한다.
			// 이후, allMoved 가 false 라면 다시 원상 복구 시켜주면 된다.
			if (moveEnable == false)
			{
				allMoved = false;
				break;
			}

			// 만약 가능하다면, 일단 이동 시키기
			int stRow = stPos[movedKnight].first;
			int stCol = stPos[movedKnight].second;

			int nxtRow = stRow + dRow[dir];
			int nxtCol = stCol + dCol[dir];

			oldPoses.push_back({ movedKnight, stRow, stCol });
			stPos[movedKnight] = { nxtRow, nxtCol };
		}
		
		// 다시 원상 복구 시킨다.
		for (int i = 0; i < oldPoses.size(); ++i)
		{
			int oldKnight, oldRow, oldCol;
			tie(oldKnight, oldRow, oldCol) = oldPoses[i];
			stPos[oldKnight] = { oldRow, oldCol };
		}

		// 하나라도 이동이 불가하다면 모든 녀석들 이동이 불가능하다.
		if (allMoved == false)
			continue;

		for (int i = knightsOnWay.size() - 1; i >= 0; --i)
		{
			// 만약 가능하다면, 모두 이동
			int movedKnight = knightsOnWay[i];

			int stRow = stPos[movedKnight].first;
			int stCol = stPos[movedKnight].second;

			int nxtRow = stRow + dRow[dir];
			int nxtCol = stCol + dCol[dir];

			stPos[movedKnight] = { nxtRow, nxtCol };

			// 민 주체는 데미지 X
			if (movedKnight == knight)
				continue;

			// - 이후, 함정 계산
			// - 사라지는 녀석 파악
			int damage = calDamage(movedKnight);
			health[movedKnight] -= damage;
			totDamaged[movedKnight] += damage;
			if (health[movedKnight] <= 0)
			{
				alive[movedKnight] = false;
			}
		}
	}

	for (int n = 0; n < N; ++n)
	{
		if (alive[n] == false)
			continue;
		ans += totDamaged[n];
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