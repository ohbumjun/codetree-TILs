#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#define MAX INT_MAX

using namespace std;
int Q, start;
int dist[2001];
bool checkId[30001];
struct landInfo {
    int u, w;
};

struct packageInfo {
    int id, revenue, dest, cost;
};

struct Cmp1 {
    bool operator()(landInfo a, landInfo b) {
        return a.w > b.w;
    }
};

struct Cmp2 {
    bool operator()(packageInfo a, packageInfo b) {
        if (a.cost != b.cost)return a.cost < b.cost;
        return a.id > b.id;
    }
};

vector<landInfo>lands[2001];
priority_queue<packageInfo,vector<packageInfo>,Cmp2>packages;

// 거리 초기화
void init() {
    for (int i = 0; i < 2001; i++) {
        dist[i] = MAX;
    }
}

// 다익스트라
void dijkstra() {
    init();
    dist[start] = 0;
    priority_queue<landInfo,vector<landInfo>,Cmp1>pq;
    pq.push({ start,0 });
    while (!pq.empty()) {
        landInfo now = pq.top();
        pq.pop();
        for (int i = 0; i < lands[now.u].size(); i++) {
            landInfo next = lands[now.u][i];
            if (dist[next.u] <= dist[now.u] + next.w)continue;
            pq.push(next);
            dist[next.u] = dist[now.u] + next.w;
        }
    }
}


// 양방향 간선 연결
void func1() {
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < m; i++) {
        int v, u, w;
        cin >> v >> u >> w;
        lands[v].push_back({ u,w });
        lands[u].push_back({ v,w });
    }
    dijkstra();
}

void func2() {
    int id, revenue, dest;
    int cost = 0;
    cin >> id >> revenue >> dest;
    if (dist[dest] == MAX) {
        cost = -1;
    }
    else {
        cost = revenue - dist[dest];
    }
    packages.push({ id,revenue,dest,cost });
    checkId[id] = true;
}

void func3() {
    int id;
    cin >> id;
    checkId[id] = false;
}

void func4() {
    if (packages.empty()) {
        cout << -1 << "\n";
        return;
    }
    else {
        while (!packages.empty()) {
            packageInfo now = packages.top();
            if (!checkId[now.id]) {
                packages.pop();
            }
            else {
                break;
            }
        }
        if (packages.empty()) {
            cout << -1 << "\n";
            return;
        }
        packageInfo now = packages.top();
        if (now.cost < 0) {
            cout << -1 << "\n";
        }
        else {
            packages.pop();
            checkId[now.id] = false;
            cout << now.id<<"\n";
        }
    }
}

void func5() {
    cin >> start;
    dijkstra();
    priority_queue<packageInfo, vector<packageInfo>, Cmp2>temp;
    while (!packages.empty()) {
        packageInfo now = packages.top();
        packages.pop();
        if (!checkId[now.id])continue;
        temp.push(now);
    }
    while (!temp.empty()) {
        packageInfo now = temp.top();
        int cost = 0;
        int dest = now.dest;
        int revenue = now.revenue;
        if (dist[dest] == MAX) {
            cost = -1;
        }
        else {
            cost = revenue - dist[dest];
        }
        now.cost = cost;
        packages.push(now);
        temp.pop();
    }
}

void input() {
    int cnt = 0;
    for (int i = 0; i < Q; i++) {
        int num;
        cin >> num;
        if (num == 100) {
            func1();
        }
        else if (num == 200) {
            func2();
        }
        else if (num == 300) {
            func3();
        }
        else if (num == 400) {
            func4();
        }
        else if (num == 500) {
            func5();
        }
    }
}

int main() {

    cin >> Q;
    input();
    return 0;
}