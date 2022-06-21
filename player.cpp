#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}  // float to int
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};

int player;
const int SIZE = 15;
const int MAX = INT32_MAX;
const int MIN = INT32_MIN;
std::array<std::array<int, SIZE>, SIZE> board;

enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };

class GomokuBoard{
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 15;
    std::array<std::array<int, SIZE>, SIZE> board;
    int empty_count;
    int cur_player;
    bool done;
    int winner;
    const std::array<Point, 8> directions{ {
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    } };
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;  //depth is 3

    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        return true;
    }

    GomokuBoard() {};
    GomokuBoard(std::array<std::array<int, SIZE>, SIZE> new_board, int now_player){
        this->board = new_board;
        this->cur_player = now_player;
        this->done = false;
        this->winner = -1;
        for (int i = 0; i < 3; i++) {
            disc_count[i] = 0;
        }
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                disc_count [board[i][j]]++;
            }
        }
    }
    ~GomokuBoard() {};
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        // Check Win                //這裡有兩種做法，另外一種是檢查next_valid_spots是否為無
        if (checkwin(cur_player)) {
            done = true;
            winner = cur_player;
        }
        if (disc_count[EMPTY] == 0) {
            done = true;
            winner = EMPTY;
        }
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();

        return true;
    }
    bool checkwin(int disc){
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (is_disc_at(Point(i, j), disc)){
                    bool iswin = true;
                    if (i + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i+k, j), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                    iswin = true;
                    if (j + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i, j+k), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                    iswin = true;
                    if (i + 4 < SIZE && j + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i+k, j+k), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                    iswin = true;
                    if (i - 4 >= 0 && j + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i-k, j+k), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                }
            }
        }
        return false;
    }

};

std::array<std::array<int, SIZE>, SIZE> game_board;
std::vector<Point> next_valid_spots;

int get_val(int cnt, int eval, int player)
{
    if(player == BLACK){
        if(cnt == 1) return 2 * eval;
        else if(cnt == 2) return 20 * eval;
        else if(cnt == 3) return 200 * eval;
        else if(cnt == 4) return 2000 * eval;
        else if(cnt == 5) return 200000 * eval;
        else if(cnt > 5) return MAX * eval;
    }
    else if(player == WHITE){
        if(cnt == 1) return 2;
        else if(cnt == 2) return 10 * eval;
        else if(cnt == 3) return 100 * eval;
        else if(cnt == 4) return 1000 * eval;
        else if(cnt == 5) return 100000 * eval;
        else if(cnt > 5) return MAX * eval;
    }
    return 0;
}

int evaluate(GomokuBoard state, int k, int t, int p, int q) //計算有沒有活四、活三等等
{
    if(state.board[k][t] == EMPTY && state.board[p][q] == EMPTY)
        return 2;
    else return 1;
}

int state_value(GomokuBoard state) {
    int val = 0;

    int cnt = 0, eval = 0;
    //int flag = 0;
    //int valboard[15][15] = {{0, 0}};
    int k, t;

    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(state.board[i][j] == EMPTY) continue;

            //記住現在的顏色
            int COLOR = state.board[i][j];
            //四種方向
            //左下
            cnt = 0;
            for(k = i+1, t = j-1; state.board[k][t] == COLOR && t >= 0 && k < SIZE; k++, t--){
                cnt++;
            }
            if(state.board[i-1][j+1] == COLOR) cnt = 0;
            eval = evaluate(state, k, t, i-1, j+1);
            val += get_val(cnt, eval, COLOR);
            //右
            cnt = 0;
            for(t = j+1; state.board[i][t] == COLOR && t < SIZE; t++){
                cnt++;
            }
            if(state.board[i][j-1] == COLOR) cnt = 0;
            eval = evaluate(state, k, t, i, j-1);
            val += get_val(cnt, eval, COLOR);
            //右下
            cnt = 0;
            for(k = i+1, t = j+1; state.board[k][t] == COLOR && k < SIZE && t < SIZE; k++, t++){
                cnt++;
            }
            if(state.board[i-1][j-1] == COLOR) cnt = 0;
            eval = evaluate(state, k, t, i-1, j-1);
            val += get_val(cnt, eval, COLOR);
            //下
            cnt = 0;
            for(k = i+1; state.board[k][j] == COLOR && k < SIZE; k++){
                cnt++;
            }
            if(state.board[i-1][j] == COLOR) cnt = 0;
            eval = evaluate(state, k, t, i-1, j);
            val += get_val(cnt, eval, COLOR);


//            if(valboard[i][j] == 0){
//                valboard[i][j]++;
//                //左下
//                cnt = 0;
//                for(k = i+1, t = j-1; state.board[k][t] == COLOR && t >= 0 && k < SIZE; k++, t--){
//                    cnt++;
//                    valboard[k][t]++;
//                }
//                eval = evaluate(state, k, t, i-1, j+1);
//                val += get_val(cnt, eval, COLOR);
//                //右
//                cnt = 0;
//                for(t = j+1; state.board[i][t] == COLOR && t < SIZE; t++){
//                    cnt++;
//                    valboard[i][t]++;
//                }
//                eval = evaluate(state, k, t, i, j-1);
//                val += get_val(cnt, eval, COLOR);
//                //右下
//                cnt = 0;
//                for(k = i+1, t = j+1; state.board[k][t] == COLOR && k < SIZE && t < SIZE; k++, t++){
//                    cnt++;
//                    valboard[k][t]++;
//                }
//                eval = evaluate(state, k, t, i-1, j-1);
//                val += get_val(cnt, eval, COLOR);
//                //下
//                cnt = 0;
//                for(k = i+1; state.board[k][j] == COLOR && k < SIZE; k++){
//                    cnt++;
//                    valboard[k][j]++;
//                }
//                eval = evaluate(state, k, t, i-1, j);
//                val += get_val(cnt, eval, COLOR);
//            }
//            ////////////
//            else if(valboard[i][j] != 0){
//                //左下
//                cnt = 0;
//                for(k = i+1, t = j-1; state.board[k][t] == COLOR && k < SIZE && t >= 0; k++, t--){
//                    cnt++;
//                    if(valboard[k][t] == 0){
//                        flag = 1;
//                        valboard[k][t]++;
//                    }
//                }
//                eval = evaluate(state, k, t, i-1, j+1);
//                if(flag) val += get_val(cnt, eval, COLOR);
//                //右
//                cnt = 0;
//                for(t = j+1; state.board[i][t] == COLOR && t < SIZE; t++){
//                    cnt++;
//                    if(valboard[i][t] == 0){
//                        flag = 1;
//                        valboard[i][t]++;
//                    }
//                }
//                eval = evaluate(state, k, t, i, j-1);
//                if(flag) val += get_val(cnt, eval, COLOR);
//                //右下
//                cnt = 0;
//                for(k = i+1, t = j+1; state.board[k][t] == COLOR && k < SIZE && t < SIZE; k++, t++){
//                    cnt++;
//                    if(valboard[k][t] == 0){
//                        flag = 1;
//                        valboard[k][t]++;
//                    }
//                }
//                eval = evaluate(state, k, t, i-1, j-1);
//                if(flag) val += get_val(cnt, eval, COLOR);
//                //下
//                cnt = 0;
//                for(k = i+1; state.board[k][j] == COLOR && k < SIZE; k++){
//                    cnt++;
//                    if(valboard[k][j] == 0){
//                        flag = 1;
//                        valboard[k][j]++;
//                    }
//                }
//                eval = evaluate(state, k, t, i-1, j);
//                if(flag) val += get_val(cnt, eval, COLOR);
//            }

        }
    }
    return val;
}

// Fixed from Wikipedia.
int tree_search(GomokuBoard state, Point expected_point, int depth, int alpha, int beta) {
    GomokuBoard new_state = state;
    new_state.put_disc(expected_point);
    std::vector<Point> new_valid_spots = new_state.next_valid_spots;
    int val = 0;



    if (new_state.done || depth == 0) {
        return state_value(new_state);
    } else if (new_state.cur_player == player) {  // maximizing player
        val = MIN;
        for (auto new_p : new_valid_spots) {
            val = std::max(val, tree_search(new_state, new_p, depth-1, alpha, beta));
            alpha = std::max(alpha, val);
            if (beta <= alpha) {
                break;  // beta pruning
            }
        }
    } else if (new_state.cur_player == 3 - player) {  // opponent
        val = MAX;
        for (auto new_p : new_valid_spots) {
            val = std::min(val, tree_search(new_state, new_p, depth-1, alpha, beta));
            beta = std::min(beta, val);
            if (beta <= alpha) {
                break;  // alpha pruning
            }
        }
    }
    return val;
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
    GomokuBoard state(board, player);
    next_valid_spots = state.get_valid_spots();
}

void write_valid_spot(std::ofstream& fout) {
    GomokuBoard cur_state(board, player);
    Point decision;
    int val = 0;
    //int depth = 0;
    int greatest = MIN;
    int middle = 1;
    if(middle){
        for(int i = 0; i < SIZE; i++){
            for(int j = 0; j < SIZE; j++){
                if(cur_state.board[i][j] != EMPTY)
                    middle = 0;
            }
        }
    }
    if(middle){
        decision.x = 6;
        decision.y = 7;
    }
    else{
        for(int i = 0; i < SIZE; i++){
            for(int j = 0; j < SIZE; j++){
                Point pt(i, j);
                if(cur_state.board[i][j] == EMPTY){
//                    int val1 = tree_search(cur_state, pt, 1, MIN, MAX);
//                    int val2 = tree_search(cur_state, pt, 1, MIN, MAX);
//                    val = std::max(val1, val2);
                    val = tree_search(cur_state, pt, 1, MIN, MAX);
                    if (val >= greatest) {
                        greatest = val;
                        decision.x = i;
                        decision.y = j;
                        //break;
                    }
                }
            }
        }
    }
    fout << decision.x << " " << decision.y << std::endl;
    fout.flush();

}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
