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
	 * 使用“显式栈”的 DFS 解迷宫：
	 * - 0 表示可走，1 表示墙
	 * - 起点 (0,0)，终点 (4,4)
	 * - 记录每个节点的父节点索引，找到终点后可反向回溯并输出路径
	 */
	typedef struct {
		int row;
		int col;
		int parent;  // 指向上一个路径节点在 nodes 数组中的下标
	} Node;

	Node nodes[MAX_ROW * MAX_COL];
	int stack[MAX_ROW * MAX_COL];
	int visited[MAX_ROW][MAX_COL] = {0};
	int top = -1;
	int node_count = 0;
	int found_index = -1;

	// 按“上、右、下、左”优先探索（与测试期望路径一致）
	const int dr[4] = {-1, 0, 1, 0};
	const int dc[4] = {0, 1, 0, -1};

	// 起点入栈
	nodes[node_count].row = 0;
	nodes[node_count].col = 0;
	nodes[node_count].parent = -1;
	stack[++top] = node_count;
	visited[0][0] = 1;
	node_count++;

	while (top >= 0) {
		int cur_index = stack[top--];
		int r = nodes[cur_index].row;
		int c = nodes[cur_index].col;

		// 到达终点，记录并结束搜索
		if (r == MAX_ROW - 1 && c == MAX_COL - 1) {
			found_index = cur_index;
			break;
		}

		/**
		 * 栈是 LIFO。为了让“上、右、下、左”先被处理，
		 * 这里按相反顺序“左、下、右、上”入栈。
		 */
		for (int d = 3; d >= 0; d--) {
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
			stack[++top] = node_count;
			node_count++;
		}
	}

	if (found_index == -1) {
		printf("No path!\n");
		return 0;
	}

	// 按测试要求：从终点到起点逐行输出坐标
	for (int idx = found_index; idx != -1; idx = nodes[idx].parent) {
		printf("(%d, %d)\n", nodes[idx].row, nodes[idx].col);
	}

	return 0;
}