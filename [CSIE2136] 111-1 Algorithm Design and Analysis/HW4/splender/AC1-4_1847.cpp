#include "splender.h"

using namespace std;

#define DEBUG_MOVE 0
#define DEBUG_SCORE 0
#define DEBUG 0
#define DEPTH 1
#define INF INT32_MAX

#define PTisWin     100
#define PTpoints      2
#define PTbonus       1
#define PTgems        1


typedef struct _{
	int score[2];
	int gem[6];
	int cnt[3];
	int player_gem[2][7];
	int hand[2][5];
	set<int> imprisoned[2], buffer[3];

	_(){
		for(int i=0; i<2; i++) score[i] = 0;
		for(int i=0; i<6; i++) gem[i] = 4;
		for(int i=0; i<2; i++){
			for(int j=0; j<7; j++) player_gem[i][j] = 0;
		}
		for(int i=0; i<2; i++){
			for(int j=0; j<5; j++) hand[i][j] = 0;
		}
		
	}
	_(const _ &b){
		for(int i=0; i<2; i++) score[i] = b.score[i];
		for(int i=0; i<6; i++) gem[i] = b.gem[i];
		for(int i=0; i<3; i++) cnt[i] = b.cnt[i];
		for(int i=0; i<2; i++){
			for(int j=0; j<7; j++) player_gem[i][j] = b.player_gem[i][j];
		}
		for(int i=0; i<2; i++){
			for(int j=0; j<5; j++) hand[i][j] = b.hand[i][j];
		}
		for(int i=0; i<2; i++) imprisoned[i] = b.imprisoned[i];
		for(int i=0; i<3; i++) buffer[i] = b.buffer[i];
	}

} Board;

void my_check_buffer(Board &b);
bool Myplay (Board &b, struct move m, int player);
bool Myfail (Board b, struct move m, int player);
struct move make_move(int type, int id, int gem[5]);
int eval_pts(Board &b, int player);
vector<struct move> generate_move(Board &b, int player);
int minimaxAB(Board b, int depth, int alpha, int beta, int player);

Board board;

vector<card> deck_main;
vector<card> deck[3];

void init (vector<card> stack_1, vector<card> stack_2, vector<card> stack_3) {
	deck_main.insert(deck_main.end(), stack_1.begin(), stack_1.end());
	deck_main.insert(deck_main.end(), stack_2.begin(), stack_2.end());
	deck_main.insert(deck_main.end(), stack_3.begin(), stack_3.end());
	deck[0].insert(deck[0].end(), stack_1.begin(), stack_1.end());
	deck[1].insert(deck[1].end(), stack_2.begin(), stack_2.end());
	deck[2].insert(deck[2].end(), stack_3.begin(), stack_3.end());
	sort(deck_main.begin(), deck_main.end(), cmp);
	my_check_buffer(board);
	// for(int i=0; i<90; i++){
	// 	show_card(deck_main[i]);
	// 	cout << '\n';
	// }
}

struct move make_move(int type, int id, int gem[5]){
	struct move m;
	m.type = type;
	m.card_id = id;
	for(int i=0; i<5; i++) m.gem[i] = gem[i];
	return m; 
}

int eval_pts(Board &b){
	// isWin? : 100
	// points : 2
	// Bonus  : 1 
	// Gems   : 1

	int bns=0;
	for(int i=0; i<5; i++){
		bns += b.hand[0][i];
		bns -= b.hand[1][i];
	}
	if(DEBUG_SCORE){
				cout << "==============================================\n";

		cout << "Iswin: "<<(b.score[0]>=15)<< '\n';
		cout << "points: "<<(b.score[0] - b.score[1])<<'\n';
		cout << "bonus: " << bns << '\n';
		cout << "gems: " << (b.player_gem[0][6] - b.player_gem[1][6]) << '\n';
	}
	int pts = PTisWin * (b.score[0]>=15) + PTpoints * (b.score[0] - b.score[1]) + PTbonus * (bns) + PTgems * (b.player_gem[0][6] - b.player_gem[1][6]);

	return pts;
}

vector<struct move> generate_move(Board &b, int player){
	vector<struct move> moves;

	// type1
	int choice[5] = {0,0,1,1,1};
	do {
		struct move temp_move = make_move(1, -1, choice);
		if(Myfail(b, temp_move, player)){
			continue;
		}
		moves.push_back(temp_move);
    } while (next_permutation(choice, choice+5));

	choice[0]=choice[1]=choice[2]=choice[3]=0; choice[4]=2;
	do {
		struct move temp_move = make_move(1, -1, choice);
		if(Myfail(b, temp_move, player)){
			continue;
		}
		moves.push_back(temp_move);
    } while (next_permutation(choice, choice+5));

	//type2
	for(int i=0; i<3; i++){
		for(auto id : b.buffer[i]){
			struct move temp_move = make_move(2, id, choice);
			if(Myfail(b, temp_move, player)){
				continue;
			}
			moves.push_back(temp_move);
		}
	}
	for(auto id : b.imprisoned[player]){
		struct move temp_move = make_move(2, id, choice);
		if(Myfail(b, temp_move, player)){
			continue;
		}
		moves.push_back(temp_move);
	}

	//type3
	for(int i=0; i<3; i++){
		for(auto id : b.buffer[i]){
			struct move temp_move = make_move(3, id, choice);
			if(Myfail(b, temp_move, player)){
				continue;
			}

			moves.push_back(temp_move);
		}
	}
	

	return moves;
}

struct move player_move(struct move m) {
	// 0: me, 1: server
	Myplay(board, m, 1);
	struct move my_move;
	int maxPts = -INF;
	vector<struct move> moves = generate_move(board, 0);
	if(DEBUG_MOVE)cout << "Totalmoves: " << moves.size() << '\n';
	for(auto mv : moves){
		//show_move(m , 0);
		if(1==0) {
			cout << "==============================================\n";
			cout<<"type: " << mv.type << '\n';
			cout<<"card_id: " << mv.card_id << '\n';
			cout << "gems: ";
			for(int i=0; i<5; i++){
				cout << mv.gem[i] << ' ';
			}
			cout << '\n';
			cout << "==============================================\n";
		}
		Board temp_board(board);
		Myplay(temp_board, mv, 0);
		int minimaxValue = minimaxAB(temp_board, DEPTH-1, -INF, INF, 1);
		if(DEBUG_SCORE)cout << "minimaxValue: " << minimaxValue << '\n';
		if(minimaxValue > maxPts){
			maxPts = minimaxValue;
			my_move = mv;
		}
	}
		if(DEBUG_MOVE) {
			cout << "==============================================\n";
			cout<<"type: " << my_move.type << '\n';
			cout<<"card_id: " << my_move.card_id << '\n';
			cout << "gems: ";
			for(int i=0; i<5; i++){
				cout << my_move.gem[i] << ' ';
			}
			cout << '\n';
			cout << "==============================================\n";
		}
	Myplay(board, my_move, 0);
	return my_move;
}

int minimaxAB(Board b, int depth, int alpha, int beta, int player){
    if(depth == 0){
        //if(CHECK_MM) std::cout << "Look for " << state.myColor << '\n';
        int finalScore = eval_pts(b);
        return finalScore;
    }

    if(player == 0){
        //if(DEBUG) std::cout << "Maxmizing...\n";
		vector<struct move> moves = generate_move(b, player);
        int v = -INF;
		for(auto m : moves){
			Board temp_board(b);
			Myplay(temp_board, m, player);
            int minimaxValue = minimaxAB(temp_board, depth-1, alpha, beta, !player);
            // v := max(v, alphabeta)
            v = std::max(v, minimaxValue);
            // α := max(α, v)
            alpha = std::max(alpha, v);
            // prune
            if(beta <= alpha){
                break;
            }
		}

        if(DEBUG) std::cout<< "depth: "<<depth << " v: " << v<< "\n";
        return v;
    }
    else{
        //if(DEBUG) std::cout << "Minimizing...\n";
        
        vector<struct move> moves = generate_move(b, player);

        int v = INF;
        
        for(auto m: moves){
            Board temp_board(b);
			Myplay(temp_board, m, player);

            int minimaxValue = minimaxAB(temp_board, depth-1, alpha, beta, !player);
            // v := min(v, alphabeta)
            v = std::min(v, minimaxValue);
            // β := min(β, v)
            beta = std::min(beta, v);
            // prune
            if(beta <= alpha){
                break;
            }
        }
        if(DEBUG) std::cout<< "depth: "<<depth << " v: " << v<< "\n";
        return v;
    }
}

bool Myfail (Board b, struct move m, int player) {
  int sum = 0, mx = INT_MIN, mn = INT_MAX, tmp = 0;
  bool flag = true;
  switch (m.type) {
    case 1:
      // check the combination is legal
      for (int i = 0; i < 5; i++) {
        sum +=  m.gem[i];
        mx = max(mx, m.gem[i]);
        mn = min(mn, m.gem[i]);
      }
#ifdef SPLENDER_DEBUG
      cout << "max = " << mx << " min = " << mn << " sum = " << sum << '\n';
#endif
      switch (sum) {
        case 2:
          if (mx != 2 || mn != 0) {
            return true;
          }
          break;
        case 3:
          if (mx != 1 || mn != 0) {
            return true;
          }
          break;
        default:
          return true;
      }
      // check the remain gem in public is enough
      for (int i = 0; i < 5; i++) {
#ifdef SPLENDER_DEBUG
        cout << "m.gem[i] = " << m.gem[i] << " gem[i] = " << gem[i] << "\n";
#endif
        if (m.gem[i] > b.gem[i]) {
#ifdef SPLENDER_DEBUG
          cout << "Gem " << i << " is not enough\n";
#endif
          return true;
        }
      }
      // check over limit of hand
#ifdef SPLENDER_DEBUG
      cout << "Total number of gem = " << player_gem[player][6] + sum << "\n";
#endif
      return b.player_gem[player][6] + sum > 10;
    case 2:
      // check the card is on the board or imprisoned
      if ((!b.buffer[find_stack_number(m.card_id)].count(m.card_id)) && (!b.imprisoned[player].count(m.card_id))) {
        return true;
      }
      // check the player has enough gem
      for (int i = 0; i < 5; i++) {
        if (b.player_gem[player][i] + b.hand[player][i] < deck_main[m.card_id].cost[i]) {
          tmp += deck_main[m.card_id].cost[i] - b.player_gem[player][i] - b.hand[player][i];
        }
      }
      return tmp > b.player_gem[player][5];
    case 3:
      // check the card is on the board
      if (!b.buffer[find_stack_number(m.card_id)].count(m.card_id)) {
        return true;
      }
      // check whether take the gem lead to over 10
      if (b.player_gem[player][6] >= 10 || b.gem[5] <= 0) {
        return true;
      }
      // check over the limit of the imprison card
      return b.imprisoned[player].size() >= 3;
    default:
      return true;
  }
}

bool Myplay (Board &b, struct move m, int player) {
  switch (m.type) {
    case 1:
      // get the gem from the public area
      for (int i = 0; i < 5; i++) {
        b.gem[i] -= m.gem[i];
        b.player_gem[player][i] += m.gem[i];
        b.player_gem[player][6] += m.gem[i];
      }
      break;
    case 2:
      // put the gem into the public area
      for (int i = 0; i < 5; i++) {
        int tmp_gem = max(deck_main[m.card_id].cost[i] - b.hand[player][i], 0);
        if (tmp_gem > b.player_gem[player][i]) {
          b.player_gem[player][5] -= tmp_gem - b.player_gem[player][i];
          b.player_gem[player][6] -= tmp_gem - b.player_gem[player][i];
          b.gem[5] += tmp_gem - b.player_gem[player][i];
          tmp_gem = b.player_gem[player][i];
        }
        b.player_gem[player][i] -= tmp_gem;
        b.player_gem[player][6] -= tmp_gem;
        b.gem[i] += tmp_gem;
      }
      // get the card from the board or imprisoned
      if (b.buffer[find_stack_number(m.card_id)].count(m.card_id)) {
        b.buffer[find_stack_number(m.card_id)].erase(m.card_id);
      }
      else {
        b.imprisoned[player].erase(m.card_id);
      }
      b.hand[player][deck_main[m.card_id].gem]++;
      b.score[player] += deck_main[m.card_id].score;
      break;
    case 3:
      // imprison the card
      b.buffer[find_stack_number(m.card_id)].erase(m.card_id);
      b.imprisoned[player].insert(m.card_id);
      // get special gem
      b.gem[5]--;
      b.player_gem[player][5]++;
      b.player_gem[player][6]++;
      break;
  }
  my_check_buffer(b);
  return b.score[player] >= 15;
}

void my_check_buffer (Board &b) {
  for (int i = 0; i < 3; i++) {
    while (b.buffer[i].size() < 4 && b.cnt[i] < n[i]) {
      b.buffer[i].insert(deck[i][b.cnt[i]].id);
      b.cnt[i]++;
    }
  }
}
