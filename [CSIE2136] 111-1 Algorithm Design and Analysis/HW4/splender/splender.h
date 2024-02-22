// Comment out the next line to disable game board output
//#define SPLENDER_DEBUG

// Uncomment the next line to interactively play with your own code
//#define SPLENDER_INTERACTIVE

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include <iomanip>
#include <set>
#include <sstream>
#include <climits>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

using namespace std;

#ifdef SPLENDER_INTERACTIVE
  #define SPLENDER_DEBUG
#endif

struct card{
  int id;
  int cost[5];
  int gem;
  int score;
};

struct move{
  int type; // 0 for initial, 1 for get gem, 2 for buy card, 3 for imprison card
  int card_id; // only for type 2 and 3
  int gem[5]; // only for type 1
};

namespace {

constexpr char serv_ip[] = "140.112.30.40";
constexpr int serv_port = 8877;

bool fail (struct move m, int player);
bool play (struct move m, int player);
void check_buffer ();

int MODE;

inline void wa() {
  cout << "Wrong Answer\n";
  exit(0);
}

inline void ac() {
  cout << "Accepted\n";
  exit(0);
}

vector<card> stk;

int n[3];

int score[2], gem[6] = {4, 4, 4, 4, 4, 4}, player_gem[2][7], hand[2][5], cnt[3];

set<int> imprisoned[2], buffer[3];

void show_move (struct move m, int p) {
#ifdef SPLENDER_DEBUG
  cerr << "player " << p << "'s move... " << endl;
  if(m.type == 1){
    cerr << "take ";
    for(int i = 0; i < 5; i++) cerr << m.gem[i] << " ";
    cerr << endl;
  }
  else{
    cerr << (m.type == 2 ? "buy " : "reserve ") << m.card_id << endl;
  }
#endif
}

void show_card(struct card c) {
  cout << setfill(' ') << setw(2) << c.id << " [";
  for (int i = 0; i < 5; i++) {
    cout << c.cost[i] << (i == 4 ? "" : " ");
  }
  cout << "][";
  cout << c.gem << " " << c.score << "]";
}

void show_player (int p) {
  cout << "Player " << p << "'s gem:   ";
  for (int i = 0; i < 6; i++) {
    cout << player_gem[p][i] << " ";
  }
  cout << "\n";
  cout << "Player " << p << "'s bonus: ";
  for (int i = 0; i < 5; i++) {
    cout << hand[p][i] << " ";
  }
  cout << "\n";
  cout << "Player " << p << "'s reserve: ";
  for (int i:imprisoned[p]) {
    show_card(stk[i]);
    cout << " ";
  }
  cout << "\n";
}

void show_board () {
  cout << "Remained Gem Tokens: ";
  for (int i = 0; i < 6; i++) {
    cout << gem[i] << " ";
  }
  cout << "\n";
  for (int i = 0; i < 3; i++) {
    // cout << "level " << i << " ";
    for (int a:buffer[i]) {
      show_card(stk[a]);
      cout << " ";
    }
    cout << "\n";
  }
  cout << "Score: " << score[0] << " VS " << score[1] << "\n";
}

void show () {
#ifdef SPLENDER_DEBUG
  cout << "==============================================\n";
  show_player(0);
  cout << "----------------------------------------------\n";
  show_board();
  cout << "----------------------------------------------\n";
  show_player(1);
  cout << "==============================================\n";
#endif
}

int find_stack_number (int id) {
  if (id < n[0]) {
    return 0;
  }
  if (id < n[0] + n[1]) {
    return 1;
  }
  if (id < n[0] + n[1] + n[2]) {
    return 2;
  }
  exit(-1);
}

int sockfd;

void Send(const string &msg, int size = -1) {
  if (size < 0) {
    size = msg.size() + 1;
  }
  // cout << "Send... " << msg.c_str() << endl;
  send(sockfd, msg.c_str(), size, 0);
  // cout << "Send... " << msg.c_str() << endl;
}

#define BUF_SIZE 1024

string Recv(int size = -1) {
  char buf[BUF_SIZE] = {};
  if (size < 0) {
    size = sizeof(buf);
  }

  if (size > BUF_SIZE) {
    stringstream tmp;
    string s, t;
    int bytesRead;
    while (size > 0) {
      bytesRead = recv(sockfd, buf, min(size, int(sizeof(buf))), 0);
      t = string(buf);
      // cout << t << endl;
      t.resize(bytesRead);
      // cout << t << endl;
      s += t;
      size -= bytesRead;
    }

    // cout << "Recv... " << s << endl;
    return s;
  }
  else {
    int ret = recv(sockfd, buf, size, 0);
    // cout << "Receive Successfully " << string(buf) << endl;

    // Check if the connection is down
    if (ret <= 0) {
      cerr << "connection is down" << endl;
      close(sockfd);
    }

    // cout << "Recv... " << string(buf) << endl;
    return string(buf);
  }
}

bool cmp (const card &x, const card &y) {
  return x.id < y.id;
}

void start (vector<card> &stack_1, vector<card> &stack_2, vector<card> &stack_3) {
  stringstream ss(Recv(1529));
  ss >> n[0] >> n[1] >> n[2];

  stk.resize(0);
  stack_1.resize(n[0]);
  stack_2.resize(n[1]);
  stack_3.resize(n[2]);

  for (int i = 0; i < n[0]; i++) {
    ss >> stack_1[i].id >> stack_1[i].cost[0] >> stack_1[i].cost[1] >> stack_1[i].cost[2] >> stack_1[i].cost[3] >> stack_1[i].cost[4] >> stack_1[i].gem >> stack_1[i].score;
    stk.push_back(stack_1[i]);
  }
  for (int i = 0; i < n[1]; i++) {
    ss >> stack_2[i].id >> stack_2[i].cost[0] >> stack_2[i].cost[1] >> stack_2[i].cost[2] >> stack_2[i].cost[3] >> stack_2[i].cost[4] >> stack_2[i].gem >> stack_2[i].score;
    stk.push_back(stack_2[i]);
  }
  for (int i = 0; i < n[2]; i++) {
    ss >> stack_3[i].id >> stack_3[i].cost[0] >> stack_3[i].cost[1] >> stack_3[i].cost[2] >> stack_3[i].cost[3] >> stack_3[i].cost[4] >> stack_3[i].gem >> stack_3[i].score;
    stk.push_back(stack_3[i]);
  }

  sort(stk.begin(), stk.end(), cmp);
}

int input_int(string s, int l, int r) {
  int ret = 0;
  string t;
  cout << s;
  cin >> t;
  for (char &c:t) {
    if (c < '0' || c > '9') {
      ret = l - 1;
      break;
    }
    else {
      ret = ret * 10 + c - '0';
    }
  }
  while (ret < l || ret > r) {
    cout << "Invalid Input\n";
    cout << s;
    cin >> t;
    for (char &c:t) {
      if (c < '0' || c > '9') {
        ret = l - 1;
        break;
      }
      else {
        ret = ret * 10 + c - '0';
      }
    }
  }
  return ret;
}

struct move server_move (struct move m) {
#ifdef SPLENDER_INTERACTIVE
  struct move ret;
  do {
    int tmp = input_int("You want to take tokens, purchase or reserve? (type 1 for take tokens, 2 for purchase a development card, 3 for reserve a development card) ", 1, 3);
    ret.type = tmp;
    switch (tmp) {
      case 1:
        ret.gem[0] = input_int("How many emerald tokens you want to purchase? (type number from 0 to 2) ", 0, 2);
        ret.gem[1] = input_int("How many diamond tokens you want to purchase? (type number from 0 to 2) ", 0, 2);
        ret.gem[2] = input_int("How many sapphire tokens you want to purchase? (type number from 0 to 2) ", 0, 2);
        ret.gem[3] = input_int("How many onyx tokens you want to purchase? (type number from 0 to 2) ", 0, 2);
        ret.gem[4] = input_int("How many ruby tokens you want to purchase? (type number from 0 to 2) ", 0, 2);
        break;
      case 2:
        ret.card_id = input_int("What card you want to purchase? (type card id) ", 0, n[0] + n[1] + n[2] - 1);
        break;
      case 3:
        ret.card_id = input_int("What card you want to reserve? (type card id) ", 0, n[0] + n[1] + n[2] - 1);
        break;
    }
  } while (fail(ret, 1));
  return ret;
#else
  stringstream ss;
  ss << m.type << " " << m.card_id << " " << m.gem[0] << " " << m.gem[1] << " " << m.gem[2] << " " << m.gem[3] << " " << m.gem[4] << " ";
  Send(ss.str().c_str());
  ss = stringstream(Recv());
  ss >> m.type >> m.card_id >> m.gem[0] >> m.gem[1] >> m.gem[2] >> m.gem[3] >> m.gem[4];
  return m;
#endif
}

bool fail (struct move m, int player) {
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
      // check the remain gem in pubilic is enough
      for (int i = 0; i < 5; i++) {
#ifdef SPLENDER_DEBUG
        cout << "m.gem[i] = " << m.gem[i] << " gem[i] = " << gem[i] << "\n";
#endif
        if (m.gem[i] > gem[i]) {
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
      return player_gem[player][6] + sum > 10;
    case 2:
      // check the card is on the board or imprisoned
      if ((!buffer[find_stack_number(m.card_id)].count(m.card_id)) && (!imprisoned[player].count(m.card_id))) {
        return true;
      }
      // check the player has enough gem
      for (int i = 0; i < 5; i++) {
        if (player_gem[player][i] + hand[player][i] < stk[m.card_id].cost[i]) {
          tmp += stk[m.card_id].cost[i] - player_gem[player][i] - hand[player][i];
        }
      }
      return tmp > player_gem[player][5];
    case 3:
      // check the card is on the board
      if (!buffer[find_stack_number(m.card_id)].count(m.card_id)) {
        return true;
      }
      // check whether take the gem lead to over 10
      if (player_gem[player][6] >= 10 || gem[5] <= 0) {
        return true;
      }
      // check over the limit of the imprison card
      return imprisoned[player].size() >= 3;
    default:
      return true;
  }
}

bool play (struct move m, int player) {
  switch (m.type) {
    case 1:
      // get the gem from the public area
      for (int i = 0; i < 5; i++) {
        gem[i] -= m.gem[i];
        player_gem[player][i] += m.gem[i];
        player_gem[player][6] += m.gem[i];
      }
      break;
    case 2:
      // put the gem into the public area
      for (int i = 0; i < 5; i++) {
        int tmp_gem = max(stk[m.card_id].cost[i] - hand[player][i], 0);
        if (tmp_gem > player_gem[player][i]) {
          player_gem[player][5] -= tmp_gem - player_gem[player][i];
          player_gem[player][6] -= tmp_gem - player_gem[player][i];
          gem[5] += tmp_gem - player_gem[player][i];
          tmp_gem = player_gem[player][i];
        }
        player_gem[player][i] -= tmp_gem;
        player_gem[player][6] -= tmp_gem;
        gem[i] += tmp_gem;
      }
      // get the card from the board or imprisoned
      if (buffer[find_stack_number(m.card_id)].count(m.card_id)) {
        buffer[find_stack_number(m.card_id)].erase(m.card_id);
      }
      else {
        imprisoned[player].erase(m.card_id);
      }
      hand[player][stk[m.card_id].gem]++;
      score[player] += stk[m.card_id].score;
      break;
    case 3:
      // imprison the card
      buffer[find_stack_number(m.card_id)].erase(m.card_id);
      imprisoned[player].insert(m.card_id);
      // get special gem
      gem[5]--;
      player_gem[player][5]++;
      player_gem[player][6]++;
      break;
  }
  return score[player] >= 15;
}

void check_buffer (vector<card> v[3]) {
  for (int i = 0; i < 3; i++) {
    while (buffer[i].size() < 4 && cnt[i] < n[i]) {
      buffer[i].insert(v[i][cnt[i]].id);
      cnt[i]++;
    }
  }
}

}

void init (vector<card> stack_1, vector<card> stack_2, vector<card> stack_3);

struct move player_move (struct move m);

int main (int argc, char* argv[]) {
#ifdef _WIN32
  WSADATA wsadata;
  if (WSAStartup(MAKEWORD(1, 1), &wsadata) == SOCKET_ERROR) {
      cerr << "WSAStartup() fail" << std::endl;
      exit(1);
  }
#endif

  if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
      cerr << "socket() fail" << std::endl;
      exit(1);
  }

  sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(serv_port);
  serveraddr.sin_addr.s_addr = inet_addr(serv_ip);

  if (connect(sockfd, (sockaddr*)&serveraddr ,sizeof(serveraddr)) == -1) {
      cerr << "connect() fail" << std::endl;
      exit(1);
  }

#ifdef SPLENDER_INTERACTIVE
  cout << "Playing interactively!" << endl;
#else
  MODE = input_int("Which test group you want to test? ", 0, 7);
#endif

  Send(to_string(MODE));
  vector<card> v[3];
  start(v[0], v[1], v[2]);
  init(v[0], v[1], v[2]);
  struct move player, server;
  server.type = 0;
  while (1) {
    check_buffer(v);
    show();
    player = player_move(server);
    show_move(player, 0);
    if (fail(player, 0)) {
#ifdef SPLENDER_DEBUG
      cout << "Invalid move from player.\n";
#endif
      wa();
    }
    if (play(player, 0)) {
#ifdef SPLENDER_DEBUG
      cout << "Player get " << score[0] << " point.\n";
#endif
      ac();
    }
    check_buffer(v);
    show();
    server = server_move(player);
    show_move(server, 1);
    if (fail(server, 1)) {
#ifdef SPLENDER_DEBUG
      cout << "Invalid move from server.\n";
#endif
      ac();
    }
    if (play(server, 1)) {
#ifdef SPLENDER_DEBUG
      cout << "Server get " << score[1] << " point.\n";
#endif
      wa();
    }
  }
#ifdef _WIN32
  closesocket(sockfd);
  WSACleanup();
#endif
}
