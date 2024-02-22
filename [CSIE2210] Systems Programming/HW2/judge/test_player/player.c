#include "status.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


///////////////////////////////// Constant /////////////////////////////////
#define DEBUG 1
#define READ 0
#define WRITE 1
#define LEFT 0
#define RIGHT 1

char* log_file[8] = {
	"log_player0.txt","log_player1.txt","log_player2.txt",
	"log_player3.txt","log_player4.txt","log_player5.txt",
	"log_player6.txt","log_player7.txt",
};

char* fifo_file[7] = {
	"player8.fifo", "player9.fifo", "player10.fifo",
	"player11.fifo","player12.fifo","player13.fifo",
	"player14.fifo",
};

char* parent[15] = {
	"G",  // 0
	"G",  // 1
	"H",  // 2
	"H",  // 3
	"I",  // 4
	"I",  // 5
	"J",  // 6
	"J",  // 7
	"M",  // 8
	"M",  // 9
	"K",  // 10
	"N",  // 11
	"N",  // 12
	"L",  // 13
	"C",  // 14
};

char* FIFOTO[75];


///////////////////////////////// Struct /////////////////////////////////

// Player State
#define INIT 0
#define WAITING 1
#define PLAYING 2
#define EXITING 3
#define REAL 0
#define AGENT 1

typedef struct _ {
	// INFO
	char* id;    // Real player ID (char)
	char* pid;	// Process ID
	int ppid;	// Parent Process ID
	int state;  // State
	int type; 	// Real/Agent Player
	// PSSM
	Status stat; // Initial Status
	Status cur;  // Current Status
	// LOG
	int log;
	// FIFO
	int fifo;
} Player;

///////////////////////////////// Utility Function /////////////////////////////////
void err_sys(const char *x){
	perror(x);
	exit(1);
}

void log_int(Player* player, int x){
	char buf[512];
	sprintf(buf, "%d\n", x);
	write(player->log, buf, strlen(buf));
}

void log_str(Player* player, char* str){
	write(player->log, str, strlen(str));
}

void log_pipe_PSSM(Player* player, char* sid, char* spid, char* IPC, char* dir, char* tid, int tpid, int rpid, int hp, char bid, int fl){
	char buf[512];
	sprintf(buf, "%s,%s %s %s %s,%d %d,%d,%c,%d\n", sid, spid, IPC, dir, tid, tpid, rpid, hp, bid, fl);
	log_str(player, buf);
}

void log_fifo_PSSM(Player* player, char* sid, char* spid, char* IPC, char* dir, int tid, int rpid, int hp){
	char buf[512];
	sprintf(buf, "%s,%s %s %s %d %d,%d\n", sid, spid, IPC, dir, tid, rpid, hp);
	log_str(player, buf);
}

void handle_init_status(Player* p, char* str){
    const char* d = " ";
    char *s;
    
    s = strtok(str, d);
	p->stat.HP = atoi(s);
	s = strtok(NULL, d);
	p->stat.ATK = atoi(s);
	s = strtok(NULL, d);
	if(strcmp(s, "FIRE") == 0){
		p->stat.attr = FIRE;
	}else if(strcmp(s, "GRASS") == 0){
		p->stat.attr = GRASS;
	}else if(strcmp(s, "WATER") == 0){
		p->stat.attr = WATER;
	}
	s = strtok(NULL, d);
	p->stat.current_battle_id = s[0];
	s = strtok(NULL, d);
	p->stat.battle_ended_flag = atoi(s);

    return;
}

void player_init(Player* player){
	player->stat.real_player_id = atoi(player->id);
	FILE* f = fopen("player_status.txt", "r");
	char buf[128];
	for(int i=0; i<=player->stat.real_player_id; i++){
		fgets(buf, sizeof(buf), f);
	}
	fclose(f);
	// Parse status to PSSM
	handle_init_status(player, buf);
	player->cur = player->stat;
	fflush(stdin);
	fflush(stdout);
}

void agent_init(Player* player){
	player->stat.real_player_id = atoi(player->id);
	// Wait init PSSM from FIFO
	read(player->fifo, &(player->stat), sizeof(Status));
	log_fifo_PSSM(player, player->id, player->pid, "fifo", "from", player->cur.real_player_id, player->cur.real_player_id, player->cur.HP);
	player->cur = player->stat;
}

int getFIFOID(char battle){
	switch (battle){
	case 'B':
		return 14;
	case 'D':
		return 10;
	case 'E':
		return 13;
	case 'G':
		return 8;
	case 'H':
		return 11;
	case 'I':
		return 9;
	case 'J':
		return 12;
	
	default:
		return -1;
	}
}

///////////////////////////////// Main /////////////////////////////////
int main(int argc, char *argv[]){
	//TODO
	if(argc != 3){
		err_sys("Wrong Format: ./player [player_id] [parent_pid]\n");
	}

	FIFOTO['B'] = "14"; FIFOTO['D'] = "10"; FIFOTO['E'] = "13"; FIFOTO['G'] = "8";
	FIFOTO['H'] = "11"; FIFOTO['I'] = "9"; FIFOTO['J'] = "12";

	char pid[1024];
	sprintf(pid, "%d", getpid());

	Player player; 
	player.id = argv[1];
	player.pid = pid;
	player.ppid =  atoi(argv[2]);
	player.state = INIT;
	if(atoi(player.id) >= 0 && atoi(player.id) <= 7) player.type = REAL;
	else player.type = AGENT;

	while(1){
		if(player.state == INIT){
			if(player.type == REAL){
				// Create Log
				player.log = open(log_file[atoi(player.id)-0], O_WRONLY|O_CREAT|O_APPEND, 0777);
				// Init player status
				player_init(&player);
			}
			else if(player.type==AGENT){
				// Create Fifo
				mkfifo(fifo_file[atoi(player.id)-8], 0777);
				player.fifo = open(fifo_file[atoi(player.id)-8], O_RDONLY);

				// Init Agent status
				//agent_init(&player);

				// Wait init PSSM from FIFO
				read(player.fifo, &(player.stat), sizeof(Status));
				player.log = open(log_file[player.stat.real_player_id], O_WRONLY|O_APPEND, 0777);
				
				player.cur = player.stat;
				log_fifo_PSSM(&player, player.id, player.pid, "fifo", "from", player.cur.real_player_id, player.cur.real_player_id, player.cur.HP);
				
			}
			// End INIT
			sleep(1);
			player.state = WAITING;
			continue;
		}
		else if(player.state == WAITING){
			// Recover
			if(player.cur.battle_ended_flag == 1){
				// Game Ended
				if(player.cur.current_battle_id == 'A'){
					_exit(0);
				}
				player.cur.HP += (player.stat.HP-player.cur.HP)/2;
				player.cur.battle_ended_flag = 0;
			}
			// Send PSSM
			write(STDOUT_FILENO, &(player.cur), sizeof(Status));
			log_pipe_PSSM(&player, player.id, player.pid, "pipe", "to", parent[atoi(player.id)-0], player.ppid, player.cur.real_player_id, player.cur.HP, player.cur.current_battle_id, player.cur.battle_ended_flag);

			player.state = PLAYING;
			continue;
		}
		else if(player.state == PLAYING){
			// Wait for update PSSM
			read(STDIN_FILENO, &(player.cur), sizeof(Status));
			log_pipe_PSSM(&player, player.id, player.pid, "pipe", "from", parent[atoi(player.id)-0], player.ppid, player.cur.real_player_id, player.cur.HP, player.cur.current_battle_id, player.cur.battle_ended_flag);
			
			if(player.cur.battle_ended_flag == 0){
				player.state = WAITING;
				continue;
			}
			else{
				if(player.cur.HP <= 0){ // Is Loser
					// Is a Real Player
					if(atoi(player.id) >= 0 && atoi(player.id) <= 7){
						// Check if battle is A (Last Battle)
						if(player.cur.current_battle_id == 'A'){
							_exit(0);
						}
						// Get FIFO
						int target_id = getFIFOID(player.cur.current_battle_id);
						// Restore HP
						player.cur.HP = player.stat.HP;
						player.cur.battle_ended_flag = 0;
						// Write Fifo to agent
						player.fifo = open(fifo_file[target_id-8], O_WRONLY);
						write(player.fifo, &(player.cur), sizeof(Status));
						log_fifo_PSSM(&player, player.id, player.pid, "fifo", "to", target_id, player.cur.real_player_id, player.cur.HP);

						// Exit
						_exit(0);
					}
					// Is a Agent Player
					else{
						// Exit
						_exit(0);
					}
				}
				else{ // Is winner
					player.state = WAITING;
				}
			}
			continue;
		}
	}

	return 0;
}