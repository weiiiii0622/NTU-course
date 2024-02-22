### Usage: ~b10902138/spjudge/judge

### Notice: 
1. Make sure you have your "scheduler.c" "threads.c" "threadtools.h" in the directory 
you run the judge.

2. Answers can be found at -> ~b10902138/spjudge/answer

3. [Testcase Issue] During the state when scheduler is handling the process,
data transmitions within FIFO may occur some error due to implentation details or system loading.
Testcases in the official judge gurantee to have no such issue, but not in this judge.

4. To specify which testcase to run, type ~b10902138/spjudge/judge
[testcaseNum]. Ex. ~b10902138/spjudge/judge 10
