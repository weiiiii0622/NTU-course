
user/_mp1-1:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <f5>:
#include "user/threads.h"

#define NULL 0

void f5(void *arg)
{
   0:	7179                	addi	sp,sp,-48
   2:	f406                	sd	ra,40(sp)
   4:	f022                	sd	s0,32(sp)
   6:	ec26                	sd	s1,24(sp)
   8:	e84a                	sd	s2,16(sp)
   a:	e44e                	sd	s3,8(sp)
   c:	e052                	sd	s4,0(sp)
   e:	1800                	addi	s0,sp,48
    int i = 10;
    while(1) {
        printf("thread 5: %d\n", i++);
  10:	45a9                	li	a1,10
  12:	00001517          	auipc	a0,0x1
  16:	b2650513          	addi	a0,a0,-1242 # b38 <thread_start_threading+0x10>
  1a:	00001097          	auipc	ra,0x1
  1e:	856080e7          	jalr	-1962(ra) # 870 <printf>
  22:	44ad                	li	s1,11
  24:	00001a17          	auipc	s4,0x1
  28:	b14a0a13          	addi	s4,s4,-1260 # b38 <thread_start_threading+0x10>
        if (i == 17) {
  2c:	49c5                	li	s3,17
  2e:	a011                	j	32 <f5+0x32>
        printf("thread 5: %d\n", i++);
  30:	84ca                	mv	s1,s2
            thread_exit();
        }
        thread_yield();
  32:	00001097          	auipc	ra,0x1
  36:	ac6080e7          	jalr	-1338(ra) # af8 <thread_yield>
        printf("thread 5: %d\n", i++);
  3a:	0014891b          	addiw	s2,s1,1
  3e:	85a6                	mv	a1,s1
  40:	8552                	mv	a0,s4
  42:	00001097          	auipc	ra,0x1
  46:	82e080e7          	jalr	-2002(ra) # 870 <printf>
        if (i == 17) {
  4a:	ff3913e3          	bne	s2,s3,30 <f5+0x30>
            thread_exit();
  4e:	00001097          	auipc	ra,0x1
  52:	ace080e7          	jalr	-1330(ra) # b1c <thread_exit>
  56:	bfe9                	j	30 <f5+0x30>

0000000000000058 <f4>:
    }
}

void f4(void *arg)
{
  58:	7179                	addi	sp,sp,-48
  5a:	f406                	sd	ra,40(sp)
  5c:	f022                	sd	s0,32(sp)
  5e:	ec26                	sd	s1,24(sp)
  60:	e84a                	sd	s2,16(sp)
  62:	e44e                	sd	s3,8(sp)
  64:	e052                	sd	s4,0(sp)
  66:	1800                	addi	s0,sp,48
    int i = 1000;

    while(1) {
        printf("thread 4: %d\n", i++);
  68:	3e800593          	li	a1,1000
  6c:	00001517          	auipc	a0,0x1
  70:	adc50513          	addi	a0,a0,-1316 # b48 <thread_start_threading+0x20>
  74:	00000097          	auipc	ra,0x0
  78:	7fc080e7          	jalr	2044(ra) # 870 <printf>
  7c:	3e900493          	li	s1,1001
  80:	00001a17          	auipc	s4,0x1
  84:	ac8a0a13          	addi	s4,s4,-1336 # b48 <thread_start_threading+0x20>
        if (i == 1011) {
  88:	3f300993          	li	s3,1011
  8c:	a011                	j	90 <f4+0x38>
        printf("thread 4: %d\n", i++);
  8e:	84ca                	mv	s1,s2
            thread_exit();
        }
        thread_yield();
  90:	00001097          	auipc	ra,0x1
  94:	a68080e7          	jalr	-1432(ra) # af8 <thread_yield>
        printf("thread 4: %d\n", i++);
  98:	0014891b          	addiw	s2,s1,1
  9c:	85a6                	mv	a1,s1
  9e:	8552                	mv	a0,s4
  a0:	00000097          	auipc	ra,0x0
  a4:	7d0080e7          	jalr	2000(ra) # 870 <printf>
        if (i == 1011) {
  a8:	ff3913e3          	bne	s2,s3,8e <f4+0x36>
            thread_exit();
  ac:	00001097          	auipc	ra,0x1
  b0:	a70080e7          	jalr	-1424(ra) # b1c <thread_exit>
  b4:	bfe9                	j	8e <f4+0x36>

00000000000000b6 <f2>:
        thread_yield();
    }
}

void f2(void *arg)
{
  b6:	7179                	addi	sp,sp,-48
  b8:	f406                	sd	ra,40(sp)
  ba:	f022                	sd	s0,32(sp)
  bc:	ec26                	sd	s1,24(sp)
  be:	e84a                	sd	s2,16(sp)
  c0:	e44e                	sd	s3,8(sp)
  c2:	e052                	sd	s4,0(sp)
  c4:	1800                	addi	s0,sp,48
    int i = 0;

    while(1) {
        printf("thread 2: %d\n", i++);
  c6:	4581                	li	a1,0
  c8:	00001517          	auipc	a0,0x1
  cc:	a9050513          	addi	a0,a0,-1392 # b58 <thread_start_threading+0x30>
  d0:	00000097          	auipc	ra,0x0
  d4:	7a0080e7          	jalr	1952(ra) # 870 <printf>
  d8:	4485                	li	s1,1
  da:	00001a17          	auipc	s4,0x1
  de:	a7ea0a13          	addi	s4,s4,-1410 # b58 <thread_start_threading+0x30>
        if (i == 9) {
  e2:	49a5                	li	s3,9
  e4:	a011                	j	e8 <f2+0x32>
        printf("thread 2: %d\n", i++);
  e6:	84ca                	mv	s1,s2
            thread_exit();
        }
        thread_yield();
  e8:	00001097          	auipc	ra,0x1
  ec:	a10080e7          	jalr	-1520(ra) # af8 <thread_yield>
        printf("thread 2: %d\n", i++);
  f0:	0014891b          	addiw	s2,s1,1
  f4:	85a6                	mv	a1,s1
  f6:	8552                	mv	a0,s4
  f8:	00000097          	auipc	ra,0x0
  fc:	778080e7          	jalr	1912(ra) # 870 <printf>
        if (i == 9) {
 100:	ff3913e3          	bne	s2,s3,e6 <f2+0x30>
            thread_exit();
 104:	00001097          	auipc	ra,0x1
 108:	a18080e7          	jalr	-1512(ra) # b1c <thread_exit>
 10c:	bfe9                	j	e6 <f2+0x30>

000000000000010e <f3>:
{
 10e:	7179                	addi	sp,sp,-48
 110:	f406                	sd	ra,40(sp)
 112:	f022                	sd	s0,32(sp)
 114:	ec26                	sd	s1,24(sp)
 116:	e84a                	sd	s2,16(sp)
 118:	e44e                	sd	s3,8(sp)
 11a:	e052                	sd	s4,0(sp)
 11c:	1800                	addi	s0,sp,48
    struct thread *t4 = thread_create(f4, NULL);
 11e:	4581                	li	a1,0
 120:	00000517          	auipc	a0,0x0
 124:	f3850513          	addi	a0,a0,-200 # 58 <f4>
 128:	00001097          	auipc	ra,0x1
 12c:	960080e7          	jalr	-1696(ra) # a88 <thread_create>
    thread_add_runqueue(t4);
 130:	00001097          	auipc	ra,0x1
 134:	9bc080e7          	jalr	-1604(ra) # aec <thread_add_runqueue>
    struct thread *t5 = thread_create(f5, NULL);
 138:	4581                	li	a1,0
 13a:	00000517          	auipc	a0,0x0
 13e:	ec650513          	addi	a0,a0,-314 # 0 <f5>
 142:	00001097          	auipc	ra,0x1
 146:	946080e7          	jalr	-1722(ra) # a88 <thread_create>
    thread_add_runqueue(t5);
 14a:	00001097          	auipc	ra,0x1
 14e:	9a2080e7          	jalr	-1630(ra) # aec <thread_add_runqueue>
        printf("thread 3: %d\n", i++);
 152:	6489                	lui	s1,0x2
 154:	71048593          	addi	a1,s1,1808 # 2710 <__global_pointer$+0x1357>
 158:	00001517          	auipc	a0,0x1
 15c:	a1050513          	addi	a0,a0,-1520 # b68 <thread_start_threading+0x40>
 160:	00000097          	auipc	ra,0x0
 164:	710080e7          	jalr	1808(ra) # 870 <printf>
 168:	71148493          	addi	s1,s1,1809
 16c:	00001a17          	auipc	s4,0x1
 170:	9fca0a13          	addi	s4,s4,-1540 # b68 <thread_start_threading+0x40>
        if (i == 10003) {
 174:	6989                	lui	s3,0x2
 176:	71398993          	addi	s3,s3,1811 # 2713 <__global_pointer$+0x135a>
 17a:	a011                	j	17e <f3+0x70>
        printf("thread 3: %d\n", i++);
 17c:	84ca                	mv	s1,s2
        thread_yield();
 17e:	00001097          	auipc	ra,0x1
 182:	97a080e7          	jalr	-1670(ra) # af8 <thread_yield>
        printf("thread 3: %d\n", i++);
 186:	0014891b          	addiw	s2,s1,1
 18a:	85a6                	mv	a1,s1
 18c:	8552                	mv	a0,s4
 18e:	00000097          	auipc	ra,0x0
 192:	6e2080e7          	jalr	1762(ra) # 870 <printf>
        if (i == 10003) {
 196:	ff3913e3          	bne	s2,s3,17c <f3+0x6e>
            thread_exit();
 19a:	00001097          	auipc	ra,0x1
 19e:	982080e7          	jalr	-1662(ra) # b1c <thread_exit>
 1a2:	bfe9                	j	17c <f3+0x6e>

00000000000001a4 <f1>:
    }
}

void f1(void *arg)
{
 1a4:	7179                	addi	sp,sp,-48
 1a6:	f406                	sd	ra,40(sp)
 1a8:	f022                	sd	s0,32(sp)
 1aa:	ec26                	sd	s1,24(sp)
 1ac:	e84a                	sd	s2,16(sp)
 1ae:	e44e                	sd	s3,8(sp)
 1b0:	e052                	sd	s4,0(sp)
 1b2:	1800                	addi	s0,sp,48
    int i = 100;

    struct thread *t2 = thread_create(f2, NULL);
 1b4:	4581                	li	a1,0
 1b6:	00000517          	auipc	a0,0x0
 1ba:	f0050513          	addi	a0,a0,-256 # b6 <f2>
 1be:	00001097          	auipc	ra,0x1
 1c2:	8ca080e7          	jalr	-1846(ra) # a88 <thread_create>
    thread_add_runqueue(t2);
 1c6:	00001097          	auipc	ra,0x1
 1ca:	926080e7          	jalr	-1754(ra) # aec <thread_add_runqueue>
    struct thread *t3 = thread_create(f3, NULL);
 1ce:	4581                	li	a1,0
 1d0:	00000517          	auipc	a0,0x0
 1d4:	f3e50513          	addi	a0,a0,-194 # 10e <f3>
 1d8:	00001097          	auipc	ra,0x1
 1dc:	8b0080e7          	jalr	-1872(ra) # a88 <thread_create>
    thread_add_runqueue(t3);
 1e0:	00001097          	auipc	ra,0x1
 1e4:	90c080e7          	jalr	-1780(ra) # aec <thread_add_runqueue>
    
    while(1) {
        printf("thread 1: %d\n", i++);
 1e8:	06400593          	li	a1,100
 1ec:	00001517          	auipc	a0,0x1
 1f0:	98c50513          	addi	a0,a0,-1652 # b78 <thread_start_threading+0x50>
 1f4:	00000097          	auipc	ra,0x0
 1f8:	67c080e7          	jalr	1660(ra) # 870 <printf>
 1fc:	06500493          	li	s1,101
 200:	00001a17          	auipc	s4,0x1
 204:	978a0a13          	addi	s4,s4,-1672 # b78 <thread_start_threading+0x50>
        if (i == 105) {
 208:	06900993          	li	s3,105
 20c:	a011                	j	210 <f1+0x6c>
        printf("thread 1: %d\n", i++);
 20e:	84ca                	mv	s1,s2
            thread_exit();
        }
        thread_yield();
 210:	00001097          	auipc	ra,0x1
 214:	8e8080e7          	jalr	-1816(ra) # af8 <thread_yield>
        printf("thread 1: %d\n", i++);
 218:	0014891b          	addiw	s2,s1,1
 21c:	85a6                	mv	a1,s1
 21e:	8552                	mv	a0,s4
 220:	00000097          	auipc	ra,0x0
 224:	650080e7          	jalr	1616(ra) # 870 <printf>
        if (i == 105) {
 228:	ff3913e3          	bne	s2,s3,20e <f1+0x6a>
            thread_exit();
 22c:	00001097          	auipc	ra,0x1
 230:	8f0080e7          	jalr	-1808(ra) # b1c <thread_exit>
 234:	bfe9                	j	20e <f1+0x6a>

0000000000000236 <main>:
    }
}

int main(int argc, char **argv)
{
 236:	1141                	addi	sp,sp,-16
 238:	e406                	sd	ra,8(sp)
 23a:	e022                	sd	s0,0(sp)
 23c:	0800                	addi	s0,sp,16
    printf("mp1-1\n");
 23e:	00001517          	auipc	a0,0x1
 242:	94a50513          	addi	a0,a0,-1718 # b88 <thread_start_threading+0x60>
 246:	00000097          	auipc	ra,0x0
 24a:	62a080e7          	jalr	1578(ra) # 870 <printf>
    struct thread *t1 = thread_create(f1, NULL);
 24e:	4581                	li	a1,0
 250:	00000517          	auipc	a0,0x0
 254:	f5450513          	addi	a0,a0,-172 # 1a4 <f1>
 258:	00001097          	auipc	ra,0x1
 25c:	830080e7          	jalr	-2000(ra) # a88 <thread_create>
    thread_add_runqueue(t1);
 260:	00001097          	auipc	ra,0x1
 264:	88c080e7          	jalr	-1908(ra) # aec <thread_add_runqueue>
    thread_start_threading();
 268:	00001097          	auipc	ra,0x1
 26c:	8c0080e7          	jalr	-1856(ra) # b28 <thread_start_threading>
    printf("\nexited\n");
 270:	00001517          	auipc	a0,0x1
 274:	92050513          	addi	a0,a0,-1760 # b90 <thread_start_threading+0x68>
 278:	00000097          	auipc	ra,0x0
 27c:	5f8080e7          	jalr	1528(ra) # 870 <printf>
    exit(0);
 280:	4501                	li	a0,0
 282:	00000097          	auipc	ra,0x0
 286:	276080e7          	jalr	630(ra) # 4f8 <exit>

000000000000028a <strcpy>:
#include "kernel/fcntl.h"
#include "user/user.h"

char*
strcpy(char *s, const char *t)
{
 28a:	1141                	addi	sp,sp,-16
 28c:	e422                	sd	s0,8(sp)
 28e:	0800                	addi	s0,sp,16
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
 290:	87aa                	mv	a5,a0
 292:	0585                	addi	a1,a1,1
 294:	0785                	addi	a5,a5,1
 296:	fff5c703          	lbu	a4,-1(a1)
 29a:	fee78fa3          	sb	a4,-1(a5)
 29e:	fb75                	bnez	a4,292 <strcpy+0x8>
    ;
  return os;
}
 2a0:	6422                	ld	s0,8(sp)
 2a2:	0141                	addi	sp,sp,16
 2a4:	8082                	ret

00000000000002a6 <strcmp>:

int
strcmp(const char *p, const char *q)
{
 2a6:	1141                	addi	sp,sp,-16
 2a8:	e422                	sd	s0,8(sp)
 2aa:	0800                	addi	s0,sp,16
  while(*p && *p == *q)
 2ac:	00054783          	lbu	a5,0(a0)
 2b0:	cb91                	beqz	a5,2c4 <strcmp+0x1e>
 2b2:	0005c703          	lbu	a4,0(a1)
 2b6:	00f71763          	bne	a4,a5,2c4 <strcmp+0x1e>
    p++, q++;
 2ba:	0505                	addi	a0,a0,1
 2bc:	0585                	addi	a1,a1,1
  while(*p && *p == *q)
 2be:	00054783          	lbu	a5,0(a0)
 2c2:	fbe5                	bnez	a5,2b2 <strcmp+0xc>
  return (uchar)*p - (uchar)*q;
 2c4:	0005c503          	lbu	a0,0(a1)
}
 2c8:	40a7853b          	subw	a0,a5,a0
 2cc:	6422                	ld	s0,8(sp)
 2ce:	0141                	addi	sp,sp,16
 2d0:	8082                	ret

00000000000002d2 <strlen>:

uint
strlen(const char *s)
{
 2d2:	1141                	addi	sp,sp,-16
 2d4:	e422                	sd	s0,8(sp)
 2d6:	0800                	addi	s0,sp,16
  int n;

  for(n = 0; s[n]; n++)
 2d8:	00054783          	lbu	a5,0(a0)
 2dc:	cf91                	beqz	a5,2f8 <strlen+0x26>
 2de:	0505                	addi	a0,a0,1
 2e0:	87aa                	mv	a5,a0
 2e2:	4685                	li	a3,1
 2e4:	9e89                	subw	a3,a3,a0
 2e6:	00f6853b          	addw	a0,a3,a5
 2ea:	0785                	addi	a5,a5,1
 2ec:	fff7c703          	lbu	a4,-1(a5)
 2f0:	fb7d                	bnez	a4,2e6 <strlen+0x14>
    ;
  return n;
}
 2f2:	6422                	ld	s0,8(sp)
 2f4:	0141                	addi	sp,sp,16
 2f6:	8082                	ret
  for(n = 0; s[n]; n++)
 2f8:	4501                	li	a0,0
 2fa:	bfe5                	j	2f2 <strlen+0x20>

00000000000002fc <memset>:

void*
memset(void *dst, int c, uint n)
{
 2fc:	1141                	addi	sp,sp,-16
 2fe:	e422                	sd	s0,8(sp)
 300:	0800                	addi	s0,sp,16
  char *cdst = (char *) dst;
  int i;
  for(i = 0; i < n; i++){
 302:	ca19                	beqz	a2,318 <memset+0x1c>
 304:	87aa                	mv	a5,a0
 306:	1602                	slli	a2,a2,0x20
 308:	9201                	srli	a2,a2,0x20
 30a:	00a60733          	add	a4,a2,a0
    cdst[i] = c;
 30e:	00b78023          	sb	a1,0(a5)
  for(i = 0; i < n; i++){
 312:	0785                	addi	a5,a5,1
 314:	fee79de3          	bne	a5,a4,30e <memset+0x12>
  }
  return dst;
}
 318:	6422                	ld	s0,8(sp)
 31a:	0141                	addi	sp,sp,16
 31c:	8082                	ret

000000000000031e <strchr>:

char*
strchr(const char *s, char c)
{
 31e:	1141                	addi	sp,sp,-16
 320:	e422                	sd	s0,8(sp)
 322:	0800                	addi	s0,sp,16
  for(; *s; s++)
 324:	00054783          	lbu	a5,0(a0)
 328:	cb99                	beqz	a5,33e <strchr+0x20>
    if(*s == c)
 32a:	00f58763          	beq	a1,a5,338 <strchr+0x1a>
  for(; *s; s++)
 32e:	0505                	addi	a0,a0,1
 330:	00054783          	lbu	a5,0(a0)
 334:	fbfd                	bnez	a5,32a <strchr+0xc>
      return (char*)s;
  return 0;
 336:	4501                	li	a0,0
}
 338:	6422                	ld	s0,8(sp)
 33a:	0141                	addi	sp,sp,16
 33c:	8082                	ret
  return 0;
 33e:	4501                	li	a0,0
 340:	bfe5                	j	338 <strchr+0x1a>

0000000000000342 <gets>:

char*
gets(char *buf, int max)
{
 342:	711d                	addi	sp,sp,-96
 344:	ec86                	sd	ra,88(sp)
 346:	e8a2                	sd	s0,80(sp)
 348:	e4a6                	sd	s1,72(sp)
 34a:	e0ca                	sd	s2,64(sp)
 34c:	fc4e                	sd	s3,56(sp)
 34e:	f852                	sd	s4,48(sp)
 350:	f456                	sd	s5,40(sp)
 352:	f05a                	sd	s6,32(sp)
 354:	ec5e                	sd	s7,24(sp)
 356:	1080                	addi	s0,sp,96
 358:	8baa                	mv	s7,a0
 35a:	8a2e                	mv	s4,a1
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 35c:	892a                	mv	s2,a0
 35e:	4481                	li	s1,0
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
 360:	4aa9                	li	s5,10
 362:	4b35                	li	s6,13
  for(i=0; i+1 < max; ){
 364:	89a6                	mv	s3,s1
 366:	2485                	addiw	s1,s1,1
 368:	0344d863          	bge	s1,s4,398 <gets+0x56>
    cc = read(0, &c, 1);
 36c:	4605                	li	a2,1
 36e:	faf40593          	addi	a1,s0,-81
 372:	4501                	li	a0,0
 374:	00000097          	auipc	ra,0x0
 378:	19c080e7          	jalr	412(ra) # 510 <read>
    if(cc < 1)
 37c:	00a05e63          	blez	a0,398 <gets+0x56>
    buf[i++] = c;
 380:	faf44783          	lbu	a5,-81(s0)
 384:	00f90023          	sb	a5,0(s2)
    if(c == '\n' || c == '\r')
 388:	01578763          	beq	a5,s5,396 <gets+0x54>
 38c:	0905                	addi	s2,s2,1
 38e:	fd679be3          	bne	a5,s6,364 <gets+0x22>
  for(i=0; i+1 < max; ){
 392:	89a6                	mv	s3,s1
 394:	a011                	j	398 <gets+0x56>
 396:	89a6                	mv	s3,s1
      break;
  }
  buf[i] = '\0';
 398:	99de                	add	s3,s3,s7
 39a:	00098023          	sb	zero,0(s3)
  return buf;
}
 39e:	855e                	mv	a0,s7
 3a0:	60e6                	ld	ra,88(sp)
 3a2:	6446                	ld	s0,80(sp)
 3a4:	64a6                	ld	s1,72(sp)
 3a6:	6906                	ld	s2,64(sp)
 3a8:	79e2                	ld	s3,56(sp)
 3aa:	7a42                	ld	s4,48(sp)
 3ac:	7aa2                	ld	s5,40(sp)
 3ae:	7b02                	ld	s6,32(sp)
 3b0:	6be2                	ld	s7,24(sp)
 3b2:	6125                	addi	sp,sp,96
 3b4:	8082                	ret

00000000000003b6 <stat>:

int
stat(const char *n, struct stat *st)
{
 3b6:	1101                	addi	sp,sp,-32
 3b8:	ec06                	sd	ra,24(sp)
 3ba:	e822                	sd	s0,16(sp)
 3bc:	e426                	sd	s1,8(sp)
 3be:	e04a                	sd	s2,0(sp)
 3c0:	1000                	addi	s0,sp,32
 3c2:	892e                	mv	s2,a1
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 3c4:	4581                	li	a1,0
 3c6:	00000097          	auipc	ra,0x0
 3ca:	172080e7          	jalr	370(ra) # 538 <open>
  if(fd < 0)
 3ce:	02054563          	bltz	a0,3f8 <stat+0x42>
 3d2:	84aa                	mv	s1,a0
    return -1;
  r = fstat(fd, st);
 3d4:	85ca                	mv	a1,s2
 3d6:	00000097          	auipc	ra,0x0
 3da:	17a080e7          	jalr	378(ra) # 550 <fstat>
 3de:	892a                	mv	s2,a0
  close(fd);
 3e0:	8526                	mv	a0,s1
 3e2:	00000097          	auipc	ra,0x0
 3e6:	13e080e7          	jalr	318(ra) # 520 <close>
  return r;
}
 3ea:	854a                	mv	a0,s2
 3ec:	60e2                	ld	ra,24(sp)
 3ee:	6442                	ld	s0,16(sp)
 3f0:	64a2                	ld	s1,8(sp)
 3f2:	6902                	ld	s2,0(sp)
 3f4:	6105                	addi	sp,sp,32
 3f6:	8082                	ret
    return -1;
 3f8:	597d                	li	s2,-1
 3fa:	bfc5                	j	3ea <stat+0x34>

00000000000003fc <atoi>:

int
atoi(const char *s)
{
 3fc:	1141                	addi	sp,sp,-16
 3fe:	e422                	sd	s0,8(sp)
 400:	0800                	addi	s0,sp,16
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 402:	00054603          	lbu	a2,0(a0)
 406:	fd06079b          	addiw	a5,a2,-48
 40a:	0ff7f793          	andi	a5,a5,255
 40e:	4725                	li	a4,9
 410:	02f76963          	bltu	a4,a5,442 <atoi+0x46>
 414:	86aa                	mv	a3,a0
  n = 0;
 416:	4501                	li	a0,0
  while('0' <= *s && *s <= '9')
 418:	45a5                	li	a1,9
    n = n*10 + *s++ - '0';
 41a:	0685                	addi	a3,a3,1
 41c:	0025179b          	slliw	a5,a0,0x2
 420:	9fa9                	addw	a5,a5,a0
 422:	0017979b          	slliw	a5,a5,0x1
 426:	9fb1                	addw	a5,a5,a2
 428:	fd07851b          	addiw	a0,a5,-48
  while('0' <= *s && *s <= '9')
 42c:	0006c603          	lbu	a2,0(a3)
 430:	fd06071b          	addiw	a4,a2,-48
 434:	0ff77713          	andi	a4,a4,255
 438:	fee5f1e3          	bgeu	a1,a4,41a <atoi+0x1e>
  return n;
}
 43c:	6422                	ld	s0,8(sp)
 43e:	0141                	addi	sp,sp,16
 440:	8082                	ret
  n = 0;
 442:	4501                	li	a0,0
 444:	bfe5                	j	43c <atoi+0x40>

0000000000000446 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 446:	1141                	addi	sp,sp,-16
 448:	e422                	sd	s0,8(sp)
 44a:	0800                	addi	s0,sp,16
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  if (src > dst) {
 44c:	02b57463          	bgeu	a0,a1,474 <memmove+0x2e>
    while(n-- > 0)
 450:	00c05f63          	blez	a2,46e <memmove+0x28>
 454:	1602                	slli	a2,a2,0x20
 456:	9201                	srli	a2,a2,0x20
 458:	00c507b3          	add	a5,a0,a2
  dst = vdst;
 45c:	872a                	mv	a4,a0
      *dst++ = *src++;
 45e:	0585                	addi	a1,a1,1
 460:	0705                	addi	a4,a4,1
 462:	fff5c683          	lbu	a3,-1(a1)
 466:	fed70fa3          	sb	a3,-1(a4)
    while(n-- > 0)
 46a:	fee79ae3          	bne	a5,a4,45e <memmove+0x18>
    src += n;
    while(n-- > 0)
      *--dst = *--src;
  }
  return vdst;
}
 46e:	6422                	ld	s0,8(sp)
 470:	0141                	addi	sp,sp,16
 472:	8082                	ret
    dst += n;
 474:	00c50733          	add	a4,a0,a2
    src += n;
 478:	95b2                	add	a1,a1,a2
    while(n-- > 0)
 47a:	fec05ae3          	blez	a2,46e <memmove+0x28>
 47e:	fff6079b          	addiw	a5,a2,-1
 482:	1782                	slli	a5,a5,0x20
 484:	9381                	srli	a5,a5,0x20
 486:	fff7c793          	not	a5,a5
 48a:	97ba                	add	a5,a5,a4
      *--dst = *--src;
 48c:	15fd                	addi	a1,a1,-1
 48e:	177d                	addi	a4,a4,-1
 490:	0005c683          	lbu	a3,0(a1)
 494:	00d70023          	sb	a3,0(a4)
    while(n-- > 0)
 498:	fee79ae3          	bne	a5,a4,48c <memmove+0x46>
 49c:	bfc9                	j	46e <memmove+0x28>

000000000000049e <memcmp>:

int
memcmp(const void *s1, const void *s2, uint n)
{
 49e:	1141                	addi	sp,sp,-16
 4a0:	e422                	sd	s0,8(sp)
 4a2:	0800                	addi	s0,sp,16
  const char *p1 = s1, *p2 = s2;
  while (n-- > 0) {
 4a4:	ca05                	beqz	a2,4d4 <memcmp+0x36>
 4a6:	fff6069b          	addiw	a3,a2,-1
 4aa:	1682                	slli	a3,a3,0x20
 4ac:	9281                	srli	a3,a3,0x20
 4ae:	0685                	addi	a3,a3,1
 4b0:	96aa                	add	a3,a3,a0
    if (*p1 != *p2) {
 4b2:	00054783          	lbu	a5,0(a0)
 4b6:	0005c703          	lbu	a4,0(a1)
 4ba:	00e79863          	bne	a5,a4,4ca <memcmp+0x2c>
      return *p1 - *p2;
    }
    p1++;
 4be:	0505                	addi	a0,a0,1
    p2++;
 4c0:	0585                	addi	a1,a1,1
  while (n-- > 0) {
 4c2:	fed518e3          	bne	a0,a3,4b2 <memcmp+0x14>
  }
  return 0;
 4c6:	4501                	li	a0,0
 4c8:	a019                	j	4ce <memcmp+0x30>
      return *p1 - *p2;
 4ca:	40e7853b          	subw	a0,a5,a4
}
 4ce:	6422                	ld	s0,8(sp)
 4d0:	0141                	addi	sp,sp,16
 4d2:	8082                	ret
  return 0;
 4d4:	4501                	li	a0,0
 4d6:	bfe5                	j	4ce <memcmp+0x30>

00000000000004d8 <memcpy>:

void *
memcpy(void *dst, const void *src, uint n)
{
 4d8:	1141                	addi	sp,sp,-16
 4da:	e406                	sd	ra,8(sp)
 4dc:	e022                	sd	s0,0(sp)
 4de:	0800                	addi	s0,sp,16
  return memmove(dst, src, n);
 4e0:	00000097          	auipc	ra,0x0
 4e4:	f66080e7          	jalr	-154(ra) # 446 <memmove>
}
 4e8:	60a2                	ld	ra,8(sp)
 4ea:	6402                	ld	s0,0(sp)
 4ec:	0141                	addi	sp,sp,16
 4ee:	8082                	ret

00000000000004f0 <fork>:
# generated by usys.pl - do not edit
#include "kernel/syscall.h"
.global fork
fork:
 li a7, SYS_fork
 4f0:	4885                	li	a7,1
 ecall
 4f2:	00000073          	ecall
 ret
 4f6:	8082                	ret

00000000000004f8 <exit>:
.global exit
exit:
 li a7, SYS_exit
 4f8:	4889                	li	a7,2
 ecall
 4fa:	00000073          	ecall
 ret
 4fe:	8082                	ret

0000000000000500 <wait>:
.global wait
wait:
 li a7, SYS_wait
 500:	488d                	li	a7,3
 ecall
 502:	00000073          	ecall
 ret
 506:	8082                	ret

0000000000000508 <pipe>:
.global pipe
pipe:
 li a7, SYS_pipe
 508:	4891                	li	a7,4
 ecall
 50a:	00000073          	ecall
 ret
 50e:	8082                	ret

0000000000000510 <read>:
.global read
read:
 li a7, SYS_read
 510:	4895                	li	a7,5
 ecall
 512:	00000073          	ecall
 ret
 516:	8082                	ret

0000000000000518 <write>:
.global write
write:
 li a7, SYS_write
 518:	48c1                	li	a7,16
 ecall
 51a:	00000073          	ecall
 ret
 51e:	8082                	ret

0000000000000520 <close>:
.global close
close:
 li a7, SYS_close
 520:	48d5                	li	a7,21
 ecall
 522:	00000073          	ecall
 ret
 526:	8082                	ret

0000000000000528 <kill>:
.global kill
kill:
 li a7, SYS_kill
 528:	4899                	li	a7,6
 ecall
 52a:	00000073          	ecall
 ret
 52e:	8082                	ret

0000000000000530 <exec>:
.global exec
exec:
 li a7, SYS_exec
 530:	489d                	li	a7,7
 ecall
 532:	00000073          	ecall
 ret
 536:	8082                	ret

0000000000000538 <open>:
.global open
open:
 li a7, SYS_open
 538:	48bd                	li	a7,15
 ecall
 53a:	00000073          	ecall
 ret
 53e:	8082                	ret

0000000000000540 <mknod>:
.global mknod
mknod:
 li a7, SYS_mknod
 540:	48c5                	li	a7,17
 ecall
 542:	00000073          	ecall
 ret
 546:	8082                	ret

0000000000000548 <unlink>:
.global unlink
unlink:
 li a7, SYS_unlink
 548:	48c9                	li	a7,18
 ecall
 54a:	00000073          	ecall
 ret
 54e:	8082                	ret

0000000000000550 <fstat>:
.global fstat
fstat:
 li a7, SYS_fstat
 550:	48a1                	li	a7,8
 ecall
 552:	00000073          	ecall
 ret
 556:	8082                	ret

0000000000000558 <link>:
.global link
link:
 li a7, SYS_link
 558:	48cd                	li	a7,19
 ecall
 55a:	00000073          	ecall
 ret
 55e:	8082                	ret

0000000000000560 <mkdir>:
.global mkdir
mkdir:
 li a7, SYS_mkdir
 560:	48d1                	li	a7,20
 ecall
 562:	00000073          	ecall
 ret
 566:	8082                	ret

0000000000000568 <chdir>:
.global chdir
chdir:
 li a7, SYS_chdir
 568:	48a5                	li	a7,9
 ecall
 56a:	00000073          	ecall
 ret
 56e:	8082                	ret

0000000000000570 <dup>:
.global dup
dup:
 li a7, SYS_dup
 570:	48a9                	li	a7,10
 ecall
 572:	00000073          	ecall
 ret
 576:	8082                	ret

0000000000000578 <getpid>:
.global getpid
getpid:
 li a7, SYS_getpid
 578:	48ad                	li	a7,11
 ecall
 57a:	00000073          	ecall
 ret
 57e:	8082                	ret

0000000000000580 <sbrk>:
.global sbrk
sbrk:
 li a7, SYS_sbrk
 580:	48b1                	li	a7,12
 ecall
 582:	00000073          	ecall
 ret
 586:	8082                	ret

0000000000000588 <sleep>:
.global sleep
sleep:
 li a7, SYS_sleep
 588:	48b5                	li	a7,13
 ecall
 58a:	00000073          	ecall
 ret
 58e:	8082                	ret

0000000000000590 <uptime>:
.global uptime
uptime:
 li a7, SYS_uptime
 590:	48b9                	li	a7,14
 ecall
 592:	00000073          	ecall
 ret
 596:	8082                	ret

0000000000000598 <putc>:

static char digits[] = "0123456789ABCDEF";

static void
putc(int fd, char c)
{
 598:	1101                	addi	sp,sp,-32
 59a:	ec06                	sd	ra,24(sp)
 59c:	e822                	sd	s0,16(sp)
 59e:	1000                	addi	s0,sp,32
 5a0:	feb407a3          	sb	a1,-17(s0)
  write(fd, &c, 1);
 5a4:	4605                	li	a2,1
 5a6:	fef40593          	addi	a1,s0,-17
 5aa:	00000097          	auipc	ra,0x0
 5ae:	f6e080e7          	jalr	-146(ra) # 518 <write>
}
 5b2:	60e2                	ld	ra,24(sp)
 5b4:	6442                	ld	s0,16(sp)
 5b6:	6105                	addi	sp,sp,32
 5b8:	8082                	ret

00000000000005ba <printint>:

static void
printint(int fd, int xx, int base, int sgn)
{
 5ba:	7139                	addi	sp,sp,-64
 5bc:	fc06                	sd	ra,56(sp)
 5be:	f822                	sd	s0,48(sp)
 5c0:	f426                	sd	s1,40(sp)
 5c2:	f04a                	sd	s2,32(sp)
 5c4:	ec4e                	sd	s3,24(sp)
 5c6:	0080                	addi	s0,sp,64
 5c8:	84aa                	mv	s1,a0
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
 5ca:	c299                	beqz	a3,5d0 <printint+0x16>
 5cc:	0805c863          	bltz	a1,65c <printint+0xa2>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
 5d0:	2581                	sext.w	a1,a1
  neg = 0;
 5d2:	4881                	li	a7,0
 5d4:	fc040693          	addi	a3,s0,-64
  }

  i = 0;
 5d8:	4701                	li	a4,0
  do{
    buf[i++] = digits[x % base];
 5da:	2601                	sext.w	a2,a2
 5dc:	00000517          	auipc	a0,0x0
 5e0:	5cc50513          	addi	a0,a0,1484 # ba8 <digits>
 5e4:	883a                	mv	a6,a4
 5e6:	2705                	addiw	a4,a4,1
 5e8:	02c5f7bb          	remuw	a5,a1,a2
 5ec:	1782                	slli	a5,a5,0x20
 5ee:	9381                	srli	a5,a5,0x20
 5f0:	97aa                	add	a5,a5,a0
 5f2:	0007c783          	lbu	a5,0(a5)
 5f6:	00f68023          	sb	a5,0(a3)
  }while((x /= base) != 0);
 5fa:	0005879b          	sext.w	a5,a1
 5fe:	02c5d5bb          	divuw	a1,a1,a2
 602:	0685                	addi	a3,a3,1
 604:	fec7f0e3          	bgeu	a5,a2,5e4 <printint+0x2a>
  if(neg)
 608:	00088b63          	beqz	a7,61e <printint+0x64>
    buf[i++] = '-';
 60c:	fd040793          	addi	a5,s0,-48
 610:	973e                	add	a4,a4,a5
 612:	02d00793          	li	a5,45
 616:	fef70823          	sb	a5,-16(a4)
 61a:	0028071b          	addiw	a4,a6,2

  while(--i >= 0)
 61e:	02e05863          	blez	a4,64e <printint+0x94>
 622:	fc040793          	addi	a5,s0,-64
 626:	00e78933          	add	s2,a5,a4
 62a:	fff78993          	addi	s3,a5,-1
 62e:	99ba                	add	s3,s3,a4
 630:	377d                	addiw	a4,a4,-1
 632:	1702                	slli	a4,a4,0x20
 634:	9301                	srli	a4,a4,0x20
 636:	40e989b3          	sub	s3,s3,a4
    putc(fd, buf[i]);
 63a:	fff94583          	lbu	a1,-1(s2)
 63e:	8526                	mv	a0,s1
 640:	00000097          	auipc	ra,0x0
 644:	f58080e7          	jalr	-168(ra) # 598 <putc>
  while(--i >= 0)
 648:	197d                	addi	s2,s2,-1
 64a:	ff3918e3          	bne	s2,s3,63a <printint+0x80>
}
 64e:	70e2                	ld	ra,56(sp)
 650:	7442                	ld	s0,48(sp)
 652:	74a2                	ld	s1,40(sp)
 654:	7902                	ld	s2,32(sp)
 656:	69e2                	ld	s3,24(sp)
 658:	6121                	addi	sp,sp,64
 65a:	8082                	ret
    x = -xx;
 65c:	40b005bb          	negw	a1,a1
    neg = 1;
 660:	4885                	li	a7,1
    x = -xx;
 662:	bf8d                	j	5d4 <printint+0x1a>

0000000000000664 <vprintf>:
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
vprintf(int fd, const char *fmt, va_list ap)
{
 664:	7119                	addi	sp,sp,-128
 666:	fc86                	sd	ra,120(sp)
 668:	f8a2                	sd	s0,112(sp)
 66a:	f4a6                	sd	s1,104(sp)
 66c:	f0ca                	sd	s2,96(sp)
 66e:	ecce                	sd	s3,88(sp)
 670:	e8d2                	sd	s4,80(sp)
 672:	e4d6                	sd	s5,72(sp)
 674:	e0da                	sd	s6,64(sp)
 676:	fc5e                	sd	s7,56(sp)
 678:	f862                	sd	s8,48(sp)
 67a:	f466                	sd	s9,40(sp)
 67c:	f06a                	sd	s10,32(sp)
 67e:	ec6e                	sd	s11,24(sp)
 680:	0100                	addi	s0,sp,128
  char *s;
  int c, i, state;

  state = 0;
  for(i = 0; fmt[i]; i++){
 682:	0005c903          	lbu	s2,0(a1)
 686:	18090f63          	beqz	s2,824 <vprintf+0x1c0>
 68a:	8aaa                	mv	s5,a0
 68c:	8b32                	mv	s6,a2
 68e:	00158493          	addi	s1,a1,1
  state = 0;
 692:	4981                	li	s3,0
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
 694:	02500a13          	li	s4,37
      if(c == 'd'){
 698:	06400c13          	li	s8,100
        printint(fd, va_arg(ap, int), 10, 1);
      } else if(c == 'l') {
 69c:	06c00c93          	li	s9,108
        printint(fd, va_arg(ap, uint64), 10, 0);
      } else if(c == 'x') {
 6a0:	07800d13          	li	s10,120
        printint(fd, va_arg(ap, int), 16, 0);
      } else if(c == 'p') {
 6a4:	07000d93          	li	s11,112
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
 6a8:	00000b97          	auipc	s7,0x0
 6ac:	500b8b93          	addi	s7,s7,1280 # ba8 <digits>
 6b0:	a839                	j	6ce <vprintf+0x6a>
        putc(fd, c);
 6b2:	85ca                	mv	a1,s2
 6b4:	8556                	mv	a0,s5
 6b6:	00000097          	auipc	ra,0x0
 6ba:	ee2080e7          	jalr	-286(ra) # 598 <putc>
 6be:	a019                	j	6c4 <vprintf+0x60>
    } else if(state == '%'){
 6c0:	01498f63          	beq	s3,s4,6de <vprintf+0x7a>
  for(i = 0; fmt[i]; i++){
 6c4:	0485                	addi	s1,s1,1
 6c6:	fff4c903          	lbu	s2,-1(s1)
 6ca:	14090d63          	beqz	s2,824 <vprintf+0x1c0>
    c = fmt[i] & 0xff;
 6ce:	0009079b          	sext.w	a5,s2
    if(state == 0){
 6d2:	fe0997e3          	bnez	s3,6c0 <vprintf+0x5c>
      if(c == '%'){
 6d6:	fd479ee3          	bne	a5,s4,6b2 <vprintf+0x4e>
        state = '%';
 6da:	89be                	mv	s3,a5
 6dc:	b7e5                	j	6c4 <vprintf+0x60>
      if(c == 'd'){
 6de:	05878063          	beq	a5,s8,71e <vprintf+0xba>
      } else if(c == 'l') {
 6e2:	05978c63          	beq	a5,s9,73a <vprintf+0xd6>
      } else if(c == 'x') {
 6e6:	07a78863          	beq	a5,s10,756 <vprintf+0xf2>
      } else if(c == 'p') {
 6ea:	09b78463          	beq	a5,s11,772 <vprintf+0x10e>
        printptr(fd, va_arg(ap, uint64));
      } else if(c == 's'){
 6ee:	07300713          	li	a4,115
 6f2:	0ce78663          	beq	a5,a4,7be <vprintf+0x15a>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
 6f6:	06300713          	li	a4,99
 6fa:	0ee78e63          	beq	a5,a4,7f6 <vprintf+0x192>
        putc(fd, va_arg(ap, uint));
      } else if(c == '%'){
 6fe:	11478863          	beq	a5,s4,80e <vprintf+0x1aa>
        putc(fd, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
 702:	85d2                	mv	a1,s4
 704:	8556                	mv	a0,s5
 706:	00000097          	auipc	ra,0x0
 70a:	e92080e7          	jalr	-366(ra) # 598 <putc>
        putc(fd, c);
 70e:	85ca                	mv	a1,s2
 710:	8556                	mv	a0,s5
 712:	00000097          	auipc	ra,0x0
 716:	e86080e7          	jalr	-378(ra) # 598 <putc>
      }
      state = 0;
 71a:	4981                	li	s3,0
 71c:	b765                	j	6c4 <vprintf+0x60>
        printint(fd, va_arg(ap, int), 10, 1);
 71e:	008b0913          	addi	s2,s6,8
 722:	4685                	li	a3,1
 724:	4629                	li	a2,10
 726:	000b2583          	lw	a1,0(s6)
 72a:	8556                	mv	a0,s5
 72c:	00000097          	auipc	ra,0x0
 730:	e8e080e7          	jalr	-370(ra) # 5ba <printint>
 734:	8b4a                	mv	s6,s2
      state = 0;
 736:	4981                	li	s3,0
 738:	b771                	j	6c4 <vprintf+0x60>
        printint(fd, va_arg(ap, uint64), 10, 0);
 73a:	008b0913          	addi	s2,s6,8
 73e:	4681                	li	a3,0
 740:	4629                	li	a2,10
 742:	000b2583          	lw	a1,0(s6)
 746:	8556                	mv	a0,s5
 748:	00000097          	auipc	ra,0x0
 74c:	e72080e7          	jalr	-398(ra) # 5ba <printint>
 750:	8b4a                	mv	s6,s2
      state = 0;
 752:	4981                	li	s3,0
 754:	bf85                	j	6c4 <vprintf+0x60>
        printint(fd, va_arg(ap, int), 16, 0);
 756:	008b0913          	addi	s2,s6,8
 75a:	4681                	li	a3,0
 75c:	4641                	li	a2,16
 75e:	000b2583          	lw	a1,0(s6)
 762:	8556                	mv	a0,s5
 764:	00000097          	auipc	ra,0x0
 768:	e56080e7          	jalr	-426(ra) # 5ba <printint>
 76c:	8b4a                	mv	s6,s2
      state = 0;
 76e:	4981                	li	s3,0
 770:	bf91                	j	6c4 <vprintf+0x60>
        printptr(fd, va_arg(ap, uint64));
 772:	008b0793          	addi	a5,s6,8
 776:	f8f43423          	sd	a5,-120(s0)
 77a:	000b3983          	ld	s3,0(s6)
  putc(fd, '0');
 77e:	03000593          	li	a1,48
 782:	8556                	mv	a0,s5
 784:	00000097          	auipc	ra,0x0
 788:	e14080e7          	jalr	-492(ra) # 598 <putc>
  putc(fd, 'x');
 78c:	85ea                	mv	a1,s10
 78e:	8556                	mv	a0,s5
 790:	00000097          	auipc	ra,0x0
 794:	e08080e7          	jalr	-504(ra) # 598 <putc>
 798:	4941                	li	s2,16
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
 79a:	03c9d793          	srli	a5,s3,0x3c
 79e:	97de                	add	a5,a5,s7
 7a0:	0007c583          	lbu	a1,0(a5)
 7a4:	8556                	mv	a0,s5
 7a6:	00000097          	auipc	ra,0x0
 7aa:	df2080e7          	jalr	-526(ra) # 598 <putc>
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
 7ae:	0992                	slli	s3,s3,0x4
 7b0:	397d                	addiw	s2,s2,-1
 7b2:	fe0914e3          	bnez	s2,79a <vprintf+0x136>
        printptr(fd, va_arg(ap, uint64));
 7b6:	f8843b03          	ld	s6,-120(s0)
      state = 0;
 7ba:	4981                	li	s3,0
 7bc:	b721                	j	6c4 <vprintf+0x60>
        s = va_arg(ap, char*);
 7be:	008b0993          	addi	s3,s6,8
 7c2:	000b3903          	ld	s2,0(s6)
        if(s == 0)
 7c6:	02090163          	beqz	s2,7e8 <vprintf+0x184>
        while(*s != 0){
 7ca:	00094583          	lbu	a1,0(s2)
 7ce:	c9a1                	beqz	a1,81e <vprintf+0x1ba>
          putc(fd, *s);
 7d0:	8556                	mv	a0,s5
 7d2:	00000097          	auipc	ra,0x0
 7d6:	dc6080e7          	jalr	-570(ra) # 598 <putc>
          s++;
 7da:	0905                	addi	s2,s2,1
        while(*s != 0){
 7dc:	00094583          	lbu	a1,0(s2)
 7e0:	f9e5                	bnez	a1,7d0 <vprintf+0x16c>
        s = va_arg(ap, char*);
 7e2:	8b4e                	mv	s6,s3
      state = 0;
 7e4:	4981                	li	s3,0
 7e6:	bdf9                	j	6c4 <vprintf+0x60>
          s = "(null)";
 7e8:	00000917          	auipc	s2,0x0
 7ec:	3b890913          	addi	s2,s2,952 # ba0 <thread_start_threading+0x78>
        while(*s != 0){
 7f0:	02800593          	li	a1,40
 7f4:	bff1                	j	7d0 <vprintf+0x16c>
        putc(fd, va_arg(ap, uint));
 7f6:	008b0913          	addi	s2,s6,8
 7fa:	000b4583          	lbu	a1,0(s6)
 7fe:	8556                	mv	a0,s5
 800:	00000097          	auipc	ra,0x0
 804:	d98080e7          	jalr	-616(ra) # 598 <putc>
 808:	8b4a                	mv	s6,s2
      state = 0;
 80a:	4981                	li	s3,0
 80c:	bd65                	j	6c4 <vprintf+0x60>
        putc(fd, c);
 80e:	85d2                	mv	a1,s4
 810:	8556                	mv	a0,s5
 812:	00000097          	auipc	ra,0x0
 816:	d86080e7          	jalr	-634(ra) # 598 <putc>
      state = 0;
 81a:	4981                	li	s3,0
 81c:	b565                	j	6c4 <vprintf+0x60>
        s = va_arg(ap, char*);
 81e:	8b4e                	mv	s6,s3
      state = 0;
 820:	4981                	li	s3,0
 822:	b54d                	j	6c4 <vprintf+0x60>
    }
  }
}
 824:	70e6                	ld	ra,120(sp)
 826:	7446                	ld	s0,112(sp)
 828:	74a6                	ld	s1,104(sp)
 82a:	7906                	ld	s2,96(sp)
 82c:	69e6                	ld	s3,88(sp)
 82e:	6a46                	ld	s4,80(sp)
 830:	6aa6                	ld	s5,72(sp)
 832:	6b06                	ld	s6,64(sp)
 834:	7be2                	ld	s7,56(sp)
 836:	7c42                	ld	s8,48(sp)
 838:	7ca2                	ld	s9,40(sp)
 83a:	7d02                	ld	s10,32(sp)
 83c:	6de2                	ld	s11,24(sp)
 83e:	6109                	addi	sp,sp,128
 840:	8082                	ret

0000000000000842 <fprintf>:

void
fprintf(int fd, const char *fmt, ...)
{
 842:	715d                	addi	sp,sp,-80
 844:	ec06                	sd	ra,24(sp)
 846:	e822                	sd	s0,16(sp)
 848:	1000                	addi	s0,sp,32
 84a:	e010                	sd	a2,0(s0)
 84c:	e414                	sd	a3,8(s0)
 84e:	e818                	sd	a4,16(s0)
 850:	ec1c                	sd	a5,24(s0)
 852:	03043023          	sd	a6,32(s0)
 856:	03143423          	sd	a7,40(s0)
  va_list ap;

  va_start(ap, fmt);
 85a:	fe843423          	sd	s0,-24(s0)
  vprintf(fd, fmt, ap);
 85e:	8622                	mv	a2,s0
 860:	00000097          	auipc	ra,0x0
 864:	e04080e7          	jalr	-508(ra) # 664 <vprintf>
}
 868:	60e2                	ld	ra,24(sp)
 86a:	6442                	ld	s0,16(sp)
 86c:	6161                	addi	sp,sp,80
 86e:	8082                	ret

0000000000000870 <printf>:

void
printf(const char *fmt, ...)
{
 870:	711d                	addi	sp,sp,-96
 872:	ec06                	sd	ra,24(sp)
 874:	e822                	sd	s0,16(sp)
 876:	1000                	addi	s0,sp,32
 878:	e40c                	sd	a1,8(s0)
 87a:	e810                	sd	a2,16(s0)
 87c:	ec14                	sd	a3,24(s0)
 87e:	f018                	sd	a4,32(s0)
 880:	f41c                	sd	a5,40(s0)
 882:	03043823          	sd	a6,48(s0)
 886:	03143c23          	sd	a7,56(s0)
  va_list ap;

  va_start(ap, fmt);
 88a:	00840613          	addi	a2,s0,8
 88e:	fec43423          	sd	a2,-24(s0)
  vprintf(1, fmt, ap);
 892:	85aa                	mv	a1,a0
 894:	4505                	li	a0,1
 896:	00000097          	auipc	ra,0x0
 89a:	dce080e7          	jalr	-562(ra) # 664 <vprintf>
}
 89e:	60e2                	ld	ra,24(sp)
 8a0:	6442                	ld	s0,16(sp)
 8a2:	6125                	addi	sp,sp,96
 8a4:	8082                	ret

00000000000008a6 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 8a6:	1141                	addi	sp,sp,-16
 8a8:	e422                	sd	s0,8(sp)
 8aa:	0800                	addi	s0,sp,16
  Header *bp, *p;

  bp = (Header*)ap - 1;
 8ac:	ff050693          	addi	a3,a0,-16
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 8b0:	00000797          	auipc	a5,0x0
 8b4:	3107b783          	ld	a5,784(a5) # bc0 <freep>
 8b8:	a805                	j	8e8 <free+0x42>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
 8ba:	4618                	lw	a4,8(a2)
 8bc:	9db9                	addw	a1,a1,a4
 8be:	feb52c23          	sw	a1,-8(a0)
    bp->s.ptr = p->s.ptr->s.ptr;
 8c2:	6398                	ld	a4,0(a5)
 8c4:	6318                	ld	a4,0(a4)
 8c6:	fee53823          	sd	a4,-16(a0)
 8ca:	a091                	j	90e <free+0x68>
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
 8cc:	ff852703          	lw	a4,-8(a0)
 8d0:	9e39                	addw	a2,a2,a4
 8d2:	c790                	sw	a2,8(a5)
    p->s.ptr = bp->s.ptr;
 8d4:	ff053703          	ld	a4,-16(a0)
 8d8:	e398                	sd	a4,0(a5)
 8da:	a099                	j	920 <free+0x7a>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 8dc:	6398                	ld	a4,0(a5)
 8de:	00e7e463          	bltu	a5,a4,8e6 <free+0x40>
 8e2:	00e6ea63          	bltu	a3,a4,8f6 <free+0x50>
{
 8e6:	87ba                	mv	a5,a4
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 8e8:	fed7fae3          	bgeu	a5,a3,8dc <free+0x36>
 8ec:	6398                	ld	a4,0(a5)
 8ee:	00e6e463          	bltu	a3,a4,8f6 <free+0x50>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 8f2:	fee7eae3          	bltu	a5,a4,8e6 <free+0x40>
  if(bp + bp->s.size == p->s.ptr){
 8f6:	ff852583          	lw	a1,-8(a0)
 8fa:	6390                	ld	a2,0(a5)
 8fc:	02059713          	slli	a4,a1,0x20
 900:	9301                	srli	a4,a4,0x20
 902:	0712                	slli	a4,a4,0x4
 904:	9736                	add	a4,a4,a3
 906:	fae60ae3          	beq	a2,a4,8ba <free+0x14>
    bp->s.ptr = p->s.ptr;
 90a:	fec53823          	sd	a2,-16(a0)
  if(p + p->s.size == bp){
 90e:	4790                	lw	a2,8(a5)
 910:	02061713          	slli	a4,a2,0x20
 914:	9301                	srli	a4,a4,0x20
 916:	0712                	slli	a4,a4,0x4
 918:	973e                	add	a4,a4,a5
 91a:	fae689e3          	beq	a3,a4,8cc <free+0x26>
  } else
    p->s.ptr = bp;
 91e:	e394                	sd	a3,0(a5)
  freep = p;
 920:	00000717          	auipc	a4,0x0
 924:	2af73023          	sd	a5,672(a4) # bc0 <freep>
}
 928:	6422                	ld	s0,8(sp)
 92a:	0141                	addi	sp,sp,16
 92c:	8082                	ret

000000000000092e <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
 92e:	7139                	addi	sp,sp,-64
 930:	fc06                	sd	ra,56(sp)
 932:	f822                	sd	s0,48(sp)
 934:	f426                	sd	s1,40(sp)
 936:	f04a                	sd	s2,32(sp)
 938:	ec4e                	sd	s3,24(sp)
 93a:	e852                	sd	s4,16(sp)
 93c:	e456                	sd	s5,8(sp)
 93e:	e05a                	sd	s6,0(sp)
 940:	0080                	addi	s0,sp,64
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 942:	02051493          	slli	s1,a0,0x20
 946:	9081                	srli	s1,s1,0x20
 948:	04bd                	addi	s1,s1,15
 94a:	8091                	srli	s1,s1,0x4
 94c:	0014899b          	addiw	s3,s1,1
 950:	0485                	addi	s1,s1,1
  if((prevp = freep) == 0){
 952:	00000517          	auipc	a0,0x0
 956:	26e53503          	ld	a0,622(a0) # bc0 <freep>
 95a:	c515                	beqz	a0,986 <malloc+0x58>
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 95c:	611c                	ld	a5,0(a0)
    if(p->s.size >= nunits){
 95e:	4798                	lw	a4,8(a5)
 960:	02977f63          	bgeu	a4,s1,99e <malloc+0x70>
 964:	8a4e                	mv	s4,s3
 966:	0009871b          	sext.w	a4,s3
 96a:	6685                	lui	a3,0x1
 96c:	00d77363          	bgeu	a4,a3,972 <malloc+0x44>
 970:	6a05                	lui	s4,0x1
 972:	000a0b1b          	sext.w	s6,s4
  p = sbrk(nu * sizeof(Header));
 976:	004a1a1b          	slliw	s4,s4,0x4
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
 97a:	00000917          	auipc	s2,0x0
 97e:	24690913          	addi	s2,s2,582 # bc0 <freep>
  if(p == (char*)-1)
 982:	5afd                	li	s5,-1
 984:	a88d                	j	9f6 <malloc+0xc8>
    base.s.ptr = freep = prevp = &base;
 986:	00000797          	auipc	a5,0x0
 98a:	24278793          	addi	a5,a5,578 # bc8 <base>
 98e:	00000717          	auipc	a4,0x0
 992:	22f73923          	sd	a5,562(a4) # bc0 <freep>
 996:	e39c                	sd	a5,0(a5)
    base.s.size = 0;
 998:	0007a423          	sw	zero,8(a5)
    if(p->s.size >= nunits){
 99c:	b7e1                	j	964 <malloc+0x36>
      if(p->s.size == nunits)
 99e:	02e48b63          	beq	s1,a4,9d4 <malloc+0xa6>
        p->s.size -= nunits;
 9a2:	4137073b          	subw	a4,a4,s3
 9a6:	c798                	sw	a4,8(a5)
        p += p->s.size;
 9a8:	1702                	slli	a4,a4,0x20
 9aa:	9301                	srli	a4,a4,0x20
 9ac:	0712                	slli	a4,a4,0x4
 9ae:	97ba                	add	a5,a5,a4
        p->s.size = nunits;
 9b0:	0137a423          	sw	s3,8(a5)
      freep = prevp;
 9b4:	00000717          	auipc	a4,0x0
 9b8:	20a73623          	sd	a0,524(a4) # bc0 <freep>
      return (void*)(p + 1);
 9bc:	01078513          	addi	a0,a5,16
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}
 9c0:	70e2                	ld	ra,56(sp)
 9c2:	7442                	ld	s0,48(sp)
 9c4:	74a2                	ld	s1,40(sp)
 9c6:	7902                	ld	s2,32(sp)
 9c8:	69e2                	ld	s3,24(sp)
 9ca:	6a42                	ld	s4,16(sp)
 9cc:	6aa2                	ld	s5,8(sp)
 9ce:	6b02                	ld	s6,0(sp)
 9d0:	6121                	addi	sp,sp,64
 9d2:	8082                	ret
        prevp->s.ptr = p->s.ptr;
 9d4:	6398                	ld	a4,0(a5)
 9d6:	e118                	sd	a4,0(a0)
 9d8:	bff1                	j	9b4 <malloc+0x86>
  hp->s.size = nu;
 9da:	01652423          	sw	s6,8(a0)
  free((void*)(hp + 1));
 9de:	0541                	addi	a0,a0,16
 9e0:	00000097          	auipc	ra,0x0
 9e4:	ec6080e7          	jalr	-314(ra) # 8a6 <free>
  return freep;
 9e8:	00093503          	ld	a0,0(s2)
      if((p = morecore(nunits)) == 0)
 9ec:	d971                	beqz	a0,9c0 <malloc+0x92>
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 9ee:	611c                	ld	a5,0(a0)
    if(p->s.size >= nunits){
 9f0:	4798                	lw	a4,8(a5)
 9f2:	fa9776e3          	bgeu	a4,s1,99e <malloc+0x70>
    if(p == freep)
 9f6:	00093703          	ld	a4,0(s2)
 9fa:	853e                	mv	a0,a5
 9fc:	fef719e3          	bne	a4,a5,9ee <malloc+0xc0>
  p = sbrk(nu * sizeof(Header));
 a00:	8552                	mv	a0,s4
 a02:	00000097          	auipc	ra,0x0
 a06:	b7e080e7          	jalr	-1154(ra) # 580 <sbrk>
  if(p == (char*)-1)
 a0a:	fd5518e3          	bne	a0,s5,9da <malloc+0xac>
        return 0;
 a0e:	4501                	li	a0,0
 a10:	bf45                	j	9c0 <malloc+0x92>

0000000000000a12 <setjmp>:
 a12:	e100                	sd	s0,0(a0)
 a14:	e504                	sd	s1,8(a0)
 a16:	01253823          	sd	s2,16(a0)
 a1a:	01353c23          	sd	s3,24(a0)
 a1e:	03453023          	sd	s4,32(a0)
 a22:	03553423          	sd	s5,40(a0)
 a26:	03653823          	sd	s6,48(a0)
 a2a:	03753c23          	sd	s7,56(a0)
 a2e:	05853023          	sd	s8,64(a0)
 a32:	05953423          	sd	s9,72(a0)
 a36:	05a53823          	sd	s10,80(a0)
 a3a:	05b53c23          	sd	s11,88(a0)
 a3e:	06153023          	sd	ra,96(a0)
 a42:	06253423          	sd	sp,104(a0)
 a46:	4501                	li	a0,0
 a48:	8082                	ret

0000000000000a4a <longjmp>:
 a4a:	6100                	ld	s0,0(a0)
 a4c:	6504                	ld	s1,8(a0)
 a4e:	01053903          	ld	s2,16(a0)
 a52:	01853983          	ld	s3,24(a0)
 a56:	02053a03          	ld	s4,32(a0)
 a5a:	02853a83          	ld	s5,40(a0)
 a5e:	03053b03          	ld	s6,48(a0)
 a62:	03853b83          	ld	s7,56(a0)
 a66:	04053c03          	ld	s8,64(a0)
 a6a:	04853c83          	ld	s9,72(a0)
 a6e:	05053d03          	ld	s10,80(a0)
 a72:	05853d83          	ld	s11,88(a0)
 a76:	06053083          	ld	ra,96(a0)
 a7a:	06853103          	ld	sp,104(a0)
 a7e:	c199                	beqz	a1,a84 <longjmp_1>
 a80:	852e                	mv	a0,a1
 a82:	8082                	ret

0000000000000a84 <longjmp_1>:
 a84:	4505                	li	a0,1
 a86:	8082                	ret

0000000000000a88 <thread_create>:
static struct thread* root_thread = NULL;
static int id = 1;
// static jmp_buf env_st;
// static jmp_buf env_tmp;

struct thread *thread_create(void (*f)(void *), void *arg){
 a88:	7179                	addi	sp,sp,-48
 a8a:	f406                	sd	ra,40(sp)
 a8c:	f022                	sd	s0,32(sp)
 a8e:	ec26                	sd	s1,24(sp)
 a90:	e84a                	sd	s2,16(sp)
 a92:	e44e                	sd	s3,8(sp)
 a94:	1800                	addi	s0,sp,48
 a96:	89aa                	mv	s3,a0
 a98:	892e                	mv	s2,a1
    struct thread *t = (struct thread*) malloc(sizeof(struct thread));
 a9a:	0b000513          	li	a0,176
 a9e:	00000097          	auipc	ra,0x0
 aa2:	e90080e7          	jalr	-368(ra) # 92e <malloc>
 aa6:	84aa                	mv	s1,a0
    //unsigned long stack_p = 0;
    unsigned long new_stack_p;
    unsigned long new_stack;
    new_stack = (unsigned long) malloc(sizeof(unsigned long)*0x100);
 aa8:	6505                	lui	a0,0x1
 aaa:	80050513          	addi	a0,a0,-2048 # 800 <vprintf+0x19c>
 aae:	00000097          	auipc	ra,0x0
 ab2:	e80080e7          	jalr	-384(ra) # 92e <malloc>
    new_stack_p = new_stack +0x100*8-0x2*8;
    t->fp = f;
 ab6:	0134b023          	sd	s3,0(s1)
    t->arg = arg;
 aba:	0124b423          	sd	s2,8(s1)
    t->ID  = id;
 abe:	00000717          	auipc	a4,0x0
 ac2:	0fe70713          	addi	a4,a4,254 # bbc <id>
 ac6:	431c                	lw	a5,0(a4)
 ac8:	08f4a823          	sw	a5,144(s1)
    t->buf_set = 0;
 acc:	0804aa23          	sw	zero,148(s1)
    t->stack = (void*) new_stack;
 ad0:	e888                	sd	a0,16(s1)
    new_stack_p = new_stack +0x100*8-0x2*8;
 ad2:	7f050513          	addi	a0,a0,2032
    t->stack_p = (void*) new_stack_p;
 ad6:	ec88                	sd	a0,24(s1)
    id++;
 ad8:	2785                	addiw	a5,a5,1
 ada:	c31c                	sw	a5,0(a4)
    return t;
}
 adc:	8526                	mv	a0,s1
 ade:	70a2                	ld	ra,40(sp)
 ae0:	7402                	ld	s0,32(sp)
 ae2:	64e2                	ld	s1,24(sp)
 ae4:	6942                	ld	s2,16(sp)
 ae6:	69a2                	ld	s3,8(sp)
 ae8:	6145                	addi	sp,sp,48
 aea:	8082                	ret

0000000000000aec <thread_add_runqueue>:
void thread_add_runqueue(struct thread *t){
 aec:	1141                	addi	sp,sp,-16
 aee:	e422                	sd	s0,8(sp)
 af0:	0800                	addi	s0,sp,16
        // TODO
    }
    else{
        // TODO
    }
}
 af2:	6422                	ld	s0,8(sp)
 af4:	0141                	addi	sp,sp,16
 af6:	8082                	ret

0000000000000af8 <thread_yield>:
void thread_yield(void){
 af8:	1141                	addi	sp,sp,-16
 afa:	e422                	sd	s0,8(sp)
 afc:	0800                	addi	s0,sp,16
    // TODO
}
 afe:	6422                	ld	s0,8(sp)
 b00:	0141                	addi	sp,sp,16
 b02:	8082                	ret

0000000000000b04 <dispatch>:
void dispatch(void){
 b04:	1141                	addi	sp,sp,-16
 b06:	e422                	sd	s0,8(sp)
 b08:	0800                	addi	s0,sp,16
    // TODO
}
 b0a:	6422                	ld	s0,8(sp)
 b0c:	0141                	addi	sp,sp,16
 b0e:	8082                	ret

0000000000000b10 <schedule>:
void schedule(void){
 b10:	1141                	addi	sp,sp,-16
 b12:	e422                	sd	s0,8(sp)
 b14:	0800                	addi	s0,sp,16
    // TODO
}
 b16:	6422                	ld	s0,8(sp)
 b18:	0141                	addi	sp,sp,16
 b1a:	8082                	ret

0000000000000b1c <thread_exit>:
void thread_exit(void){
 b1c:	1141                	addi	sp,sp,-16
 b1e:	e422                	sd	s0,8(sp)
 b20:	0800                	addi	s0,sp,16
        // Hint: No more thread to execute
    }
    else{
        // TODO
    }
}
 b22:	6422                	ld	s0,8(sp)
 b24:	0141                	addi	sp,sp,16
 b26:	8082                	ret

0000000000000b28 <thread_start_threading>:
void thread_start_threading(void){
 b28:	1141                	addi	sp,sp,-16
 b2a:	e422                	sd	s0,8(sp)
 b2c:	0800                	addi	s0,sp,16
    // TODO
}
 b2e:	6422                	ld	s0,8(sp)
 b30:	0141                	addi	sp,sp,16
 b32:	8082                	ret
