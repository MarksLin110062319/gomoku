#include "bits/stdc++.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#define pii pair<int, int>
#define fi first
#define se second
#define no string::npos
using namespace std;

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

int player, people, computer, state_val, firststep = 0;
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;
int score[SIZE][SIZE];

struct node {
	int x, y, val;
	node(int _x, int _y, int who) : x(_x), y(_y), val(0) {
		if (who == people) val = -INT_MAX;
		else val = INT_MAX;
	}; 
};

void read_board(std::ifstream& fin) {
	int b = 0, w = 0;
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
			if (board[i][j] == BLACK) b++;
			if (board[i][j] == WHITE) w++;
        }
    }
	if (!b && !w) people = 1, computer = 2;
	else if (b > w) people = 2, computer = 1;
	else people = 1, computer = 2;
	if (b + w < 2) firststep = 1;
}

bool valid(int a, int b) {
	for (int i = max(0, a-1); i < min(SIZE, a+2); ++i) {
		for (int j = max(0, b-1); j < min(SIZE, b+2); ++j) {
			if (board[i][j] != EMPTY) return true;
		}
	}
	return false;
}

vector< pii > find_next_move() {
	vector< pii > re_vec;
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if (board[i][j] == EMPTY) {
				if (valid(i, j))
					re_vec.push_back( make_pair(i, j) );
			}
		}
	}
	return re_vec;
}

int dont_judge(int cnt, int live, bool isblock, int who) {
	if (who == people) {
		if (cnt >= 5 && !isblock) return 1000000;
		if (isblock) {
			if (cnt >= 4) return 100;
			if (cnt == 3 && live == 2) return 50;
			if (cnt == 3 && live == 1) return 10;
			if (cnt == 3 && !live) return 0;
			if (cnt == 2) return 5;
		} else {
			if (cnt == 4 && live == 2) return 1000;
			if (cnt == 4 && live == 1) return 100;
			if (cnt == 3 && live == 2) return 99;
			if (cnt == 3 && live == 1) return 75;
			if (cnt == 2 && live == 2) return 10;
			if (cnt == 2 && live == 1) return 2;
			if (cnt == 1 && live == 2) return 1;
		}
	} else {
		if (cnt >= 5 && !isblock) return 2000000;
		if (isblock) {
			if (cnt >= 4 && live == 2) return 999;
			if (cnt >= 4 && live == 1) return 99;
			if (cnt == 3 && live == 2) return 100;
			if (cnt == 3 && live == 1) return 50;
			if (cnt == 3 && !live) return 0;
			if (cnt == 2) return 10;
		} else {
			//if (cnt == 4 && live == 2) cout << "you are going to lose\n";
			if (cnt == 4 && live == 2) return 50000;
			if (cnt == 4 && live == 1) return 1000;
			if (cnt == 3 && live == 2) return 999;
			if (cnt == 3 && live == 1) return 99; 
			if (cnt == 2 && live == 2) return 50;
			if (cnt == 2 && live == 1) return 10;
			if (cnt == 1 && live == 2) return 3;
			if (cnt == 1 && live == 1) return 1;
		}
	}
	return 0;
}

bool inside(int i, int j) {
	if (i >= 0 && i < SIZE && j >= 0 && j < SIZE) return true;
	else return false;
}

void shorting(int i, int j, int ni, int nj, int& cnt, int& block, int& live, int who) {
	if (board[i][j] == who) {
		cnt++; 
		if (!inside(ni, nj)) state_val += dont_judge(cnt, live, block, who), cnt = 0, block = 0, live = 0;
	} else if (board[i][j] == EMPTY) {
		if (inside(ni, nj)) {
			if (cnt) {
				if (board[ni][nj] == who && !block) {
					state_val += dont_judge(cnt, live, false, who);
					block = 1; return;
				}
				state_val += dont_judge(cnt, live+1, block, who);
				block = 0, live = 1, cnt = 0;
			} else live = 1, block = 0; // ..o ..x ...
		} else {
			if (cnt) {
				if (!live && cnt + block >= 5) state_val += dont_judge(cnt, 0, block, who);
				if (live) state_val += dont_judge(cnt, live, block, who);
			} cnt = 0, block = 0, live = 0;
		}
	} else {
		if (cnt) {
			if (live) {
				state_val += dont_judge(cnt, live, block, who);		
			} else {
				state_val += dont_judge(cnt, live, false, who);
			}
		}
		cnt = 0, live = 0, block = 0;
	}
	return;
}

void count_horizontal(int who) {
	for (int i = 0; i < SIZE; ++i) {
		int cnt = 0, block = 0, live = 0;
		for (int j = 0; j < SIZE; ++j) {
			shorting(i, j, i, j+1, cnt, block, live, who);
		}
	}
}

void count_vertical(int who) {
	for (int j = 0; j < SIZE; ++j) {
		int cnt = 0, block = 0, live = 0;
		for (int i = 0; i < SIZE; ++i) {
			shorting(i, j, i+1, j, cnt, block, live, who);
		}
	}
}

void count_dia1(int who) {
	for (int k = 0; k <= 2 * (SIZE - 1); k++) {
        int st = max(0, k - SIZE + 1), ed = min(SIZE - 1, k);
		int cnt = 0, block = 0, live = 0;
        for (int i = st; i <= ed; i++) {
            int j = k - i;	
			shorting(i, j, i+1, j-1, cnt, block, live, who);
		}
	}
}

void count_dia2(int who) {
	for (int k = 1 - SIZE; k < SIZE; k++) {
        int st = max(0, k), ed = min(SIZE + k - 1, SIZE - 1);
		int cnt = 0, block = 0, live = 0;
        for (int i = st; i <= ed; i++) {
            int j = i - k;	
			shorting(i, j, i+1, j+1, cnt, block, live, who);
		}
	}
}

int find_score(int who) {
	state_val = 0;
	count_horizontal(who);
	count_vertical(who);
	count_dia1(who);
	count_dia2(who);
	return state_val;
}

node alpha_beta(int depth, int alpha, int beta, int who) {
	node nownode(-2, -2, who);
	if (!depth) {
		/*for (int i = 0; i < SIZE; ++i) {
			for (int j = 0; j < SIZE; ++j) {
				cout << board[i][j] << " \n"[j==SIZE-1];
			}
		}*/
		int player_score = find_score(people), computer_score = find_score(computer);
		nownode.val = player_score - computer_score;
		//cout << "now : " << nownode.val << " " << player_score << " " << computer_score << "\n\n";
		return nownode;
	}
	vector< pii > next_move = find_next_move();
	int re_x = next_move[0].fi, re_y = next_move[0].se;
	if (who == player) {
		int max_val = -INT_MAX;
		for (auto it : next_move) {
			board[it.fi][it.se] = who;
			nownode = alpha_beta(depth - 1, alpha, beta, computer);
			board[it.fi][it.se] = EMPTY;
			if (max_val < nownode.val) re_x = it.fi, re_y = it.se, max_val = nownode.val;
			alpha = max(alpha, nownode.val);
			if (alpha >= beta) break;
		}
		nownode.val = max_val, nownode.x = re_x, nownode.y = re_y;
		return nownode;
	} else {
		int min_val = INT_MAX;
		for (auto it : next_move) {
			board[it.fi][it.se] = who;
			nownode = alpha_beta(depth - 1, alpha, beta, computer);
			board[it.fi][it.se] = EMPTY;
			if (min_val > nownode.val) re_x = it.fi, re_y = it.se, min_val = nownode.val;
			beta = min(beta, nownode.val);
			if (alpha >= beta) break;
		}
		nownode.val = min_val, nownode.x = re_x, nownode.y = re_y;
		return nownode;
	}
}

bool can_win(int& x, int& y) {
	vector< pii > next_move = find_next_move();
	for (auto it : next_move) {
		board[it.fi][it.se] = people;
		if (find_score(people) >= 1000000) {
			x = it.fi, y = it.se;
			board[it.fi][it.se] = EMPTY;
			return true;
		}
		board[it.fi][it.se] = EMPTY;
	}
	return false;
}

int flag, tmpv;
bool live_three(int& x, int& y) {
	vector< pii > next_move = find_next_move();
	for (auto it : next_move) {
		board[it.fi][it.se] = computer;
		int s = find_score(computer);
		board[it.fi][it.se] = EMPTY;
		if (s >= 50000) {
			cout << "score : " << s << " " << it.fi << " " << it.se << " hello\n";
			if (!flag) flag = 1, tmpv = s;
			else {
				if (s > tmpv) x = it.fi, y = it.se;
				return true;
			}
			x = it.fi, y = it.se;
		}
	}
	if (flag) return true;
	return false;
}

void write_valid_spot(std::ofstream& fout) {
	int x, y;
	if (firststep) {
		if (board[7][7] == EMPTY) x = 7, y = 7;
		else x = 8, y = 8;
		fout << x << " " << y << "\n";
		fout.flush();
		return;
	}
	if (can_win(x, y)) {
		//cout << people << " haha\n";
		fout << x << " " << y << "\n";
		fout.flush();
		return; 
	}
	if (live_three(x, y)) {
		fout << x << " " << y << "\n";
		fout.flush();
		return;
	}
	node ans_node = alpha_beta(3, -INT_MAX, INT_MAX, people);
	fout << ans_node.x << " " << ans_node.y << "\n";
	fout.flush();
	return;
}

int main(int, char** argv) {
	ios::sync_with_stdio(0); cin.tie(0);
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
