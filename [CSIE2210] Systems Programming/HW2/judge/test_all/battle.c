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


char* log_file[14] = {
	"log_battleA.txt","log_battleB.txt","log_battleC.txt",
	"log_battleD.txt","log_battleE.txt","log_battleF.txt",
	"log_battleG.txt","log_battleH.txt","log_battleI.txt",
	"log_battleJ.txt","log_battleK.txt","log_battleL.txt",
	"log_battleM.txt","log_battleN.txt",
};

char* child[14][2] = {
	{"B","C"},   // A
	{"D","E"},   // B
	{"F","14"},  // C
	{"G","H"},   // D
	{"I","J"},   // E
	{"K","L"},   // F
	{"0","1"},   // G
	{"2","3"},   // H
	{"4","5"},   // I
	{"6","7"},   // J
	{"M","10"},  // K
	{"N","13"},  // L
	{"8","9"},   // M
	{"11","12"}, // N
};

char* parent[14] = {
	"-1", // A
	"A",  // B
	"A",  // C
	"B",  // D
	"B",  // E
	"C",  // F
	"D",  // G
	"D",  // H
	"E",  // I
	"E",  // J
	"F",  // K
	"F",  // L
	"K",  // M
	"L",  // N
};

int attr[14] = {
	0, // A
	1, // B
	2, // C
	2, // D
	0, // E
	0, // F
	0, // G
	1, // H
	2, // I
	1, // J
	1, // K
	1, // L
	0, // M
	2, // N
};


///////////////////////////////// Struct /////////////////////////////////

// Battle State
#define INIT 0
#define WAITING 1
#define PLAYING 2
#define PASSING 3
#define LEFT 0
#define RIGHT 1

typedef struct _ {
	// INFO
	char id;
	char* pid;
	int cpid[2]; // 0:left pid / 1:right pid
	int ppid;
	int state;
	Attribute attr;
	int winner;
	// PSSM
	Status player[2];
	// Pipe
	int read_pipe[2];  // 0:left 1:right
	int write_pipe[2]; // 0:left 1:right
	// LOG
	int log;

} Battle;


///////////////////////////////// Utility Function /////////////////////////////////
void err_sys(const char *x){
	perror(x);
	exit(1);
}

void log_int(Battle* battle, int x){
	char buf[512];
	sprintf(buf, "%d\n", x);
	write(battle->log, buf, strlen(buf));
}

void log_str(Battle* battle, char* str){
	write(battle->log, str, strlen(str));
}

void log_pipe_PSSM(Battle* battle, char sid, char* spid, char* IPC, char* dir, char* tid, int tpid, int rpid, int hp, char bid, int fl){
	char buf[512];
	sprintf(buf, "%c,%s %s %s %s,%d %d,%d,%c,%d\n", sid, spid, IPC, dir, tid, tpid, rpid, hp, bid, fl);
	log_str(battle, buf);
}


// int write_to_log(Battle* battle, char* str){
// 	fprintf(battle->log, str);
// }

void fork_child(Battle* battle, int i, int pipeRead[], int pipeWrite[]){
	
	if(pipe(pipeRead)<0){
		err_sys("pipeRead error\n");
	}
	if(pipe(pipeWrite)<0){
		err_sys("pipeWrite error\n");
	}

	if((battle->cpid[i] = fork()) < 0){
		err_sys("fork error\n");
	}
	else if(battle->cpid[i] == 0){
		// child 
		
		dup2(pipeRead[WRITE], STDOUT_FILENO);
		close(pipeRead[WRITE]);
		dup2(pipeWrite[READ], STDIN_FILENO);
		close(pipeWrite[READ]);

		close(pipeRead[READ]);
		close(pipeWrite[WRITE]);

		if(battle->id=='A' || battle->id=='B' || battle->id=='D' || battle->id=='E' || battle->id=='F'){
			int ret;
			if(i==0){ // left child
				ret = execl("./battle", "battle", child[battle->id-'A'][0], battle->pid, NULL);
			} else{   // right child
				ret = execl("./battle", "battle", child[battle->id-'A'][1], battle->pid, NULL);
			}
			if(ret < 0){
				err_sys("execl error\n");
			}
		}
		else if(battle->id=='G'|| battle->id=='H'|| battle->id=='I'|| battle->id=='J'|| battle->id=='M'||battle->id=='N'){
			if(i==0){
				execl("./player", "player", child[battle->id-'A'][0], battle->pid, NULL);
			} else{
				execl("./player", "player", child[battle->id-'A'][1], battle->pid, NULL);
			}
		}
		else{
			if(i==0){
				execl("./battle", "battle", child[battle->id-'A'][0], battle->pid, NULL);
			} else{
				execl("./player", "player", child[battle->id-'A'][1], battle->pid, NULL);
			}

		}
	}
	else{
		// parent
		close(pipeRead[WRITE]);
		close(pipeWrite[READ]);
	}
}

void battle_init(Battle* battle){
	// Pipe & fork
	int pipeRead[2][2], pipeWrite[2][2]; // Read from / Write to child [0:left/1:right]
	fork_child(battle, 0, pipeRead[0], pipeWrite[0]);
	fork_child(battle, 1, pipeRead[1], pipeWrite[1]);

	// Access Pipe
	for(int i=0; i<=1; i++){
		battle->read_pipe[i] = pipeRead[i][READ];
		battle->write_pipe[i] = pipeWrite[i][WRITE];
	}
}


///////////////////////////////// Main /////////////////////////////////
int main(int argc, char *argv[]){
	//TODO

	if(argc != 3){
		err_sys("Wrong Format: ./battle [battle_id] [parent_pid]\n");
	}

	setbuf(stdout ,NULL);

	char pid[1024];
	sprintf(pid, "%d", getpid());

	Battle battle; 
	battle.id = *argv[1];
	battle.pid = pid;
	battle.ppid =  atoi(argv[2]);
	battle.state = INIT;
	battle.attr = attr[battle.id-'A'];


	while(1){
		if(battle.state == INIT){
			battle.log = open(log_file[battle.id-'A'], O_RDWR|O_CREAT|O_APPEND, 0777);
			battle_init(&battle);

			// End INIT
			battle.state = WAITING;
		}
		else if(battle.state == WAITING){
			// Wait for left PSSM
			int ret1 = read(battle.read_pipe[0], &battle.player[0], sizeof(Status));
			log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "from", child[battle.id-'A'][0], battle.cpid[0], battle.player[0].real_player_id, battle.player[0].HP, battle.player[0].current_battle_id, battle.player[0].battle_ended_flag);
			// Wait for right PSSM
			int ret2 = read(battle.read_pipe[1], &battle.player[1], sizeof(Status));
			log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "from", child[battle.id-'A'][1], battle.cpid[1], battle.player[1].real_player_id, battle.player[1].HP, battle.player[1].current_battle_id, battle.player[1].battle_ended_flag);
			// Set current_battle_id
			battle.player[0].current_battle_id = battle.player[1].current_battle_id = battle.id;

			// Start Playing Mode
			battle.state = PLAYING;
		}
		else if(battle.state == PLAYING){
			// Check ATK double
			int doubled[2] = {0, 0};
			if(battle.player[0].attr == battle.attr){
				battle.player[0].ATK *= 2;
				doubled[0] = 1;
			}
			if(battle.player[1].attr == battle.attr){
				battle.player[1].ATK *= 2;
				doubled[1] = 1;
			}
			// Determine who attack first
			int start, second;
			if(battle.player[0].HP < battle.player[1].HP){
				start = LEFT, second = RIGHT;
			}
			else if(battle.player[0].HP > battle.player[1].HP){
				start = RIGHT, second = LEFT;
			}
			else{
				if(battle.player[0].real_player_id < battle.player[1].real_player_id){
					start = LEFT, second = RIGHT;
				}
				else{
					start = RIGHT, second = LEFT;
				}
			}
			// Player start attack
			battle.player[second].HP -= battle.player[start].ATK;
			if(battle.player[second].HP <= 0){
				// Battle ended
				battle.winner = start;
				battle.player[second].battle_ended_flag = battle.player[start].battle_ended_flag = 1;
				// Restore normal ATK
				if(doubled[0]==1) battle.player[0].ATK /= 2;
				if(doubled[1]==1) battle.player[1].ATK /= 2;
				// Log & Write for left pipe
				write(battle.write_pipe[0], &(battle.player[0]), sizeof(Status));
				log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "to", child[battle.id-'A'][0], battle.cpid[0], battle.player[0].real_player_id, battle.player[0].HP, battle.player[0].current_battle_id, battle.player[0].battle_ended_flag);
				// Write for right pipe
				write(battle.write_pipe[1], &(battle.player[1]), sizeof(Status));
				log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "to", child[battle.id-'A'][1], battle.cpid[1], battle.player[1].real_player_id, battle.player[1].HP, battle.player[1].current_battle_id, battle.player[1].battle_ended_flag);
				
				// Wait for second player's zombie
				waitpid(battle.cpid[second], NULL, 0);
				// Enter Passing Mode
				battle.state = PASSING;
				continue;
			}
			// Player second attack
			battle.player[start].HP -= battle.player[second].ATK;
			if(battle.player[start].HP <= 0){
				// Battle ended
				battle.winner = second;
				battle.player[second].battle_ended_flag = battle.player[start].battle_ended_flag = 1;
				// Restore normal ATK
				if(doubled[0]==1) battle.player[0].ATK /= 2;
				if(doubled[1]==1) battle.player[1].ATK /= 2;
				// Log & Write for left pipe
				write(battle.write_pipe[0], &(battle.player[0]), sizeof(Status));
				log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "to", child[battle.id-'A'][0], battle.cpid[0], battle.player[0].real_player_id, battle.player[0].HP, battle.player[0].current_battle_id, battle.player[0].battle_ended_flag);
				// Write for right pipe
				write(battle.write_pipe[1], &(battle.player[1]), sizeof(Status));
				log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "to", child[battle.id-'A'][1], battle.cpid[1], battle.player[1].real_player_id, battle.player[1].HP, battle.player[1].current_battle_id, battle.player[1].battle_ended_flag);
				
				// Wait for start player's zombie
				waitpid(battle.cpid[start], NULL, 0);
				// Enter Passing Mode
				battle.state = PASSING;
				continue;
			}

			// Round ended
			battle.player[second].battle_ended_flag = battle.player[start].battle_ended_flag = 0;
			// Restore normal ATK
			if(doubled[0]==1) battle.player[0].ATK /= 2;
			if(doubled[1]==1) battle.player[1].ATK /= 2;

			// Log & Write for left pipe
			write(battle.write_pipe[0], &(battle.player[0]), sizeof(Status));
			log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "to", child[battle.id-'A'][0], battle.cpid[0], battle.player[0].real_player_id, battle.player[0].HP, battle.player[0].current_battle_id, battle.player[0].battle_ended_flag);
			// Write for right pipe
			write(battle.write_pipe[1], &(battle.player[1]), sizeof(Status));
			log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "to", child[battle.id-'A'][1], battle.cpid[1], battle.player[1].real_player_id, battle.player[1].HP, battle.player[1].current_battle_id, battle.player[1].battle_ended_flag);

			// Enter Waiting Mode
			battle.state = WAITING;
			continue;
		}
		else if(battle.state == PASSING){

			if(battle.id == 'A'){
				char buf[512];
				sprintf(buf, "Champion is P%d.\n", battle.player[battle.winner].real_player_id);
				write(STDOUT_FILENO, buf, strlen(buf));

				// Wait for Winner
				waitpid(battle.cpid[battle.winner], NULL, 0);

				// Game Ended
				_exit(0);
				break;
			}
			else{
				// Wait for winner PSSM from child
				read(battle.read_pipe[battle.winner], &(battle.player[battle.winner]), sizeof(Status));
				log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "from", child[battle.id-'A'][battle.winner], battle.cpid[battle.winner], battle.player[battle.winner].real_player_id, battle.player[battle.winner].HP, battle.player[battle.winner].current_battle_id, battle.player[battle.winner].battle_ended_flag);
				
				// Write for winner PSSM to parent
				write(STDOUT_FILENO, &(battle.player[battle.winner]), sizeof(Status));
				log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "to", parent[battle.id-'A'], battle.ppid, battle.player[battle.winner].real_player_id, battle.player[battle.winner].HP, battle.player[battle.winner].current_battle_id, battle.player[battle.winner].battle_ended_flag);
				// Read for winner PSSM from parent
				read(STDIN_FILENO, &(battle.player[battle.winner]), sizeof(Status));
				log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "from", parent[battle.id-'A'], battle.ppid, battle.player[battle.winner].real_player_id, battle.player[battle.winner].HP, battle.player[battle.winner].current_battle_id, battle.player[battle.winner].battle_ended_flag);

				// Check Descendant
				if(battle.player[battle.winner].HP <= 0){
					// Write PSSM back to child
					write(battle.write_pipe[battle.winner], &(battle.player[battle.winner]), sizeof(Status));
					log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "to", child[battle.id-'A'][battle.winner], battle.cpid[battle.winner], battle.player[battle.winner].real_player_id, battle.player[battle.winner].HP, battle.player[battle.winner].current_battle_id, battle.player[battle.winner].battle_ended_flag);
					waitpid(battle.cpid[battle.winner], NULL, 0);

					// Battle Ended
					_exit(0);
					break;	
				}
				else{
					// Write PSSM back to child
					write(battle.write_pipe[battle.winner], &(battle.player[battle.winner]), sizeof(Status));
					log_pipe_PSSM(&battle, battle.id, battle.pid, "pipe", "to", child[battle.id-'A'][battle.winner], battle.cpid[battle.winner], battle.player[battle.winner].real_player_id, battle.player[battle.winner].HP, battle.player[battle.winner].current_battle_id, battle.player[battle.winner].battle_ended_flag);

					// Game Ended
					if(battle.player[battle.winner].current_battle_id == 'A' && battle.player[battle.winner].battle_ended_flag == 1){
						waitpid(battle.cpid[battle.winner], NULL, 0);

						// Game Ended
						_exit(0);
					}

					continue;
				}
			}
		}
	}
	
	return 0;
}