// #include "kernel/types.h"

// #include "kernel/fcntl.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// void mkfile(char *filename) {
//     int fd = open(filename, O_CREATE | O_RDWR);
//     write(fd, "hi", 3);
//     close(fd);
// }

// void mkd(char *dirname) {
//     if (mkdir(dirname) < 0) {
//         fprintf(2, "mkdir %s failed.", dirname);
//         exit(1);
//     }
// }
// void test0() {
//     mkd("os2023");

//     mkd("os2023/d1");
//     mkd("os2023/d2");
//     mkd("os2023/d3");

//     mkd("os2023/d2/a");
//     mkd("os2023/d2/b");
//     mkfile("os2023/d2/c");

//     mkd("os2023/d3/a");
//     mkfile("os2023/d3/b");
// }

// void test1() {
//     mkd("a");
//     mkd("a/0");
//     mkd("a/1");
//     mkd("a/2");
//     mkd("a/3");
//     mkd("a/4");
//     mkd("a/5");
//     mkd("a/6");
//     mkd("a/7");
//     mkd("a/8");
//     mkd("a/9");
//     mkd("a/10");
// }

// int main(int argc, char *argv[]) {
//     test0();
//     test1();
//     exit(0);
// }

#include "kernel/types.h"

#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "user/user.h"
void mkfile(char *filename) {
    int fd = open(filename, O_CREATE | O_RDWR);
    write(fd, "hi", 3);
    close(fd);
}

void mkd(char *dirname) {
    if (mkdir(dirname) < 0) {
        fprintf(2, "mkdir %s failed.", dirname);
        exit(1);
    }
}
void test0() {
    mkd("os2023");

    mkd("os2023/d1");
    mkd("os2023/d2");
    mkd("os2023/d3");

    mkd("os2023/d2/a");
    mkd("os2023/d2/b");
    mkfile("os2023/d2/c");

    mkd("os2023/d3/a");
    mkfile("os2023/d3/b");
}

void test1() {
    mkd("a");
    mkd("a/0");
    mkd("a/1");
    mkd("a/2");
    mkd("a/3");
    mkd("a/4");
    mkd("a/5");
    mkd("a/6");
    mkd("a/7");
    mkd("a/8");
    mkd("a/9");
    mkd("a/10");
}





//add by me

void test2() {
    mkd("b");
    mkd("b/a");
    mkd("b/a/a");
    mkd("b/a/a/aa");
    mkd("b/a/a/aa/a");
    mkfile("b/a/a/aa/p");
    mkd("b/a/a/aa/b");
    mkfile("b/a/a/aa/c");
    mkd("b/a/a/aa/d");
    mkfile("b/a/a/aa/e");
    mkd("b/a/a/aa/f");
    mkfile("b/a/a/aa/g");
    mkd("b/a/a/aa/h");
    mkfile("b/a/a/aa/i");
    mkd("b/a/a/aa/j");
    mkfile("b/a/a/aa/k");
    mkd("b/a/a/aa/l");
    mkfile("b/a/a/aa/m");
    mkd("b/a/a/aa/n");
    mkfile("b/a/a/aa/o");
    mkd("b/a/a/aa/q");
}
void test3() {
    mkd("c");
    mkd("c/a");
    mkd("c/a/a");
    mkd("c/a/a/a");
    mkd("c/a/a/a/a");
    mkfile("c/a/a/a/p");
    mkd("c/a/a/a/b");
    mkfile("c/a/a/a/c");
    mkd("c/a/a/a/d");
    mkfile("c/a/a/a/e");
    mkd("c/a/a/a/f");
    mkfile("c/a/a/a/g");
    mkd("c/a/a/a/h");
    mkfile("c/a/a/a/i");
    mkd("c/a/a/a/j");
    mkfile("c/a/a/a/k");
    mkd("c/a/a/a/l");
    mkfile("c/a/a/a/m");
    mkd("c/a/a/a/n");
    mkfile("c/a/a/a/o");
    mkd("c/a/a/a/q");

}
void test4() {
    mkd("d");
    mkfile("d/a");
    mkfile("d/b");
    mkfile("d/c");
    mkd("d/d");
    mkfile("d/e");
    mkfile("d/f");
    mkfile("d/g");
    mkfile("d/h");
    mkfile("d/i");
    mkfile("d/j");
    mkfile("d/k");
    mkfile("d/l");
    mkfile("d/m");
    mkfile("d/n");
    mkfile("d/o");
    mkd("d/d/d");
    mkd("d/d/a");
    mkd("d/d/c");
    mkfile("d/ddd");
    mkd("d/azs");
}

void test5(){
    mkdir("e");
    mkfile("e/aaddssa");
    mkfile("e/aaddssb");
    mkfile("e/aaddssc");
    mkfile("e/aaddssd");
    mkfile("e/aaddsse");
    mkfile("e/aaddssf");
    mkfile("e/aaddssg");
    mkfile("e/aaddssh");
    mkfile("e/aaddssi");
    mkfile("e/aaddssj");
    mkfile("e/aaddssk");
    mkfile("e/aaddssl");
    mkfile("e/aaddssm");
    mkfile("e/aaddssn");
    mkfile("e/aaddsso");
    mkfile("e/aaddssp");
    mkfile("e/aaddssq");
    mkfile("e/aaddssr");
    mkfile("e/aaddsss");
    mkfile("e/aaddsst");
}

void test6(){
    mkdir("e1");
    mkfile("e1/aaddssa");
    mkfile("e1/aaddssb");
    mkfile("e1/aaddssc");
    mkfile("e1/aaddssd");
    mkfile("e1/aaddsse");
    mkfile("e1/aaddssf");
    mkfile("e1/aaddssg");
    mkfile("e1/aaddssh");
    mkfile("e1/aaddssi");
    mkfile("e1/aaddssj");
    mkfile("e1/aaddssk");
    mkfile("e1/aaddssl");
    mkfile("e1/aaddssm");
    mkfile("e1/aaddssn");
    mkfile("e1/aaddsso");
    mkfile("e1/aaddssp");
    mkfile("e1/aaddssq");
    mkfile("e1/aaddssr");
    mkfile("e1/aaddsss");
    mkfile("e1/aaddsst");
}


void test7(){
    mkdir("1");
    mkdir("1/aaddssa");
    mkdir("1/aaddssb");
    mkdir("1/aaddssc");
    mkdir("1/aaddssd");
    mkdir("1/aaddsse");
    mkdir("1/aaddssf");
    mkdir("1/aaddssg");
    mkdir("1/aaddssh");
    mkdir("1/aaddssi");
    mkdir("1/aaddssj");
    mkdir("1/aaddssk");
    mkdir("1/aaddssl");
    mkdir("1/aaddssm");
    mkdir("1/aaddssn");
    mkdir("1/aaddsso");
    mkdir("1/aaddssp");
    mkdir("1/aaddssq");
    mkdir("1/aaddssr");
    mkdir("1/aaddsss");
    mkdir("1/aaddsst");
}

void test8(){
    mkdir("12");
    mkdir("12/aaddssa");
    mkdir("12/aaddssb");
    mkdir("12/aaddssc");
    mkdir("12/aaddssd");
    mkdir("12/aaddsse");
    mkdir("12/aaddssf");
    mkdir("12/aaddssg");
    mkdir("12/aaddssh");
    mkdir("12/aaddssi");
    mkdir("12/aaddssj");
    mkdir("12/aaddssk");
    mkdir("12/aaddssl");
    mkdir("12/aaddssm");
    mkdir("12/aaddssn");
    mkdir("12/aaddsso");
    mkdir("12/aaddssp");
    mkdir("12/aaddssq");
    mkdir("12/aaddssr");
    mkdir("12/aaddsss");
    mkdir("12/aaddsst");
}

void test9(){
    mkdir("12345678");
    mkdir("12345678/1");
    mkdir("12345678/2");
    mkdir("12345678/3");
    mkdir("12345678/4");
    mkdir("12345678/5");
    mkdir("12345678/6");
    mkdir("12345678/7");
    mkdir("12345678/8");
    mkdir("12345678/9");
    mkdir("12345678/a");
    mkdir("12345678/c");
    mkdir("12345678/b");
    mkdir("12345678/d");
    mkdir("12345678/e");
    mkdir("12345678/f");
    mkdir("12345678/h");
    mkdir("12345678/g");
    mkdir("12345678/i");
    mkdir("12345678/k");
    mkdir("12345678/j");
}

void test10(){
    mkdir("gggggggg");
    mkfile("gggggggg/g");
    mkfile("gggggggg/a");
    mkfile("gggggggg/b");
    mkfile("gggggggg/c");
    mkfile("gggggggg/1");
    mkfile("gggggggg/3");
    mkfile("gggggggg/5");
    mkfile("gggggggg/7");
    mkfile("gggggggg/9");
    mkfile("gggggggg/2");
    mkfile("gggggggg/4");
    mkfile("gggggggg/6");
    mkfile("gggggggg/8");
    mkfile("gggggggg/k");
    mkfile("gggggggg/j");
    mkfile("gggggggg/h");
    mkfile("gggggggg/e");
    mkfile("gggggggg/f");
    mkfile("gggggggg/z");
    mkfile("gggggggg/w");


}

int main(int argc, char *argv[]) {
    test0();
    test1();
    //add by me
    //test2();
    //test3();
    //test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    //add by me
    exit(0);
}



