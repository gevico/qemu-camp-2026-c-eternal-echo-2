#include <stdio.h>

#define MAX_ROW 5
#define MAX_COL 5

int maze[MAX_ROW][MAX_COL] = {
	0, 1, 0, 0, 0,
	0, 1, 0, 1, 0,
	0, 0, 0, 0, 0,
	0, 1, 1, 1, 0,
	0, 0, 0, 1, 0,
};

int main(void)
{
	/**
	 * 使用“显式队列”的 BFS 解迷宫：
	 * - 0 表示通路，1 表示墙
	 * - 起点 (0,0)，终点 (4,4)
	 * - BFS 按层扩展，首次到达终点时一定是最短路径
	 * - 通过 parent 索引记录前驱，最后从终点回溯到起点输出坐标
	 */
	typedef struct {
		int row;
		int col;
		int parent;  // 指向上一节点在 nodes 数组中的下标
	} Node;

	Node nodes[MAX_ROW * MAX_COL];
	int queue[MAX_ROW * MAX_COL];
	int visited[MAX_ROW][MAX_COL] = {0};
	int front = 0;
	int rear = 0;
	int node_count = 0;
	int found_index = -1;

	// 方向顺序：下、右、上、左
	const int dr[4] = {1, 0, -1, 0};
	const int dc[4] = {0, 1, 0, -1};

	nodes[node_count].row = 0;
	nodes[node_count].col = 0;
	nodes[node_count].parent = -1;
	queue[rear++] = node_count;
	visited[0][0] = 1;
	node_count++;

	while (front < rear) {
		int cur_index = queue[front++];
		int r = nodes[cur_index].row;
		int c = nodes[cur_index].col;

		if (r == MAX_ROW - 1 && c == MAX_COL - 1) {
			found_index = cur_index;
			break;
		}

		for (int d = 0; d < 4; d++) {
			int nr = r + dr[d];
			int nc = c + dc[d];

			if (nr < 0 || nr >= MAX_ROW || nc < 0 || nc >= MAX_COL) {
				continue;
			}
			if (maze[nr][nc] == 1 || visited[nr][nc]) {
				continue;
			}

			visited[nr][nc] = 1;
			nodes[node_count].row = nr;
			nodes[node_count].col = nc;
			nodes[node_count].parent = cur_index;
			queue[rear++] = node_count;
			node_count++;
		}
	}

	if (found_index == -1) {
		printf("No path!\n");
		return 0;
	}

	// 按测试要求：从终点到起点逐行输出
	for (int idx = found_index; idx != -1; idx = nodes[idx].parent) {
		printf("(%d, %d)\n", nodes[idx].row, nodes[idx].col);
	}

	return 0;
}