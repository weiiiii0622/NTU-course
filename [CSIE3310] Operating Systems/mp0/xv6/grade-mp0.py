# from gradelib import *

# r = Runner()


# @test(5, "mp0 command with public testcase 0")
# def test_mp0_0():
#     r.run_qemu(shell_script([
#         'testgen',
#         'mp0 os2202 a',
#     ]))
#     r.match(
#         'os2202 [error opening dir]',
#         '',
#         '0 directories, 0 files',
#     )


# @test(5, "mp0 command with public testcase 1")
# def test_mp0_1():
#     r.run_qemu(shell_script([
#         # 'testgen',
#         'mp0 os2202/ o',
#     ]))
#     r.match(
#         'os2202/ [error opening dir]',
#         '',
#         '0 directories, 0 files',
#     )


# @test(5, "mp0 command with public testcase 2")
# def test_mp0_2():
#     r.run_qemu(shell_script([
#         # 'testgen',
#         'mp0 os2023 o',
#     ]))
#     r.match(
#         'os2023 1',
#         'os2023/d1 1',
#         'os2023/d2 1',
#         'os2023/d2/a 1',
#         'os2023/d2/b 1',
#         'os2023/d2/c 1',
#         'os2023/d3 1',
#         'os2023/d3/a 1',
#         'os2023/d3/b 1',
#         '',
#         '6 directories, 2 files',
#     )


# @test(5, "mp0 command with public testcase 3")
# def test_mp0_3():
#     r.run_qemu(shell_script([
#         # 'testgen',
#         'mp0 os2023/ o',
#     ]))
#     r.match(
#         'os2023/ 1',
#         'os2023//d1 1',
#         'os2023//d2 1',
#         'os2023//d2/a 1',
#         'os2023//d2/b 1',
#         'os2023//d2/c 1',
#         'os2023//d3 1',
#         'os2023//d3/a 1',
#         'os2023//d3/b 1',
#         '',
#         '6 directories, 2 files',
#     )


# @test(10, "mp0 command with public testcase 4")
# def test_mp0_4():
#     r.run_qemu(shell_script([
#         # 'testgen',
#         'mp0 a a',
#     ]))
#     r.match(
#         'a 1',
#         'a/0 1',
#         'a/1 1',
#         'a/2 1',
#         'a/3 1',
#         'a/4 1',
#         'a/5 1',
#         'a/6 1',
#         'a/7 1',
#         'a/8 1',
#         'a/9 1',
#         'a/10 1',
#         '',
#         '11 directories, 0 files',
#     )

# @test(10, "mp0 command with public testcase 5")
# def test_mp0_5():
#     r.run_qemu(shell_script([
#         # 'testgen',
#         'mp0 a/ a',
#     ]))
#     r.match(
#         'a/ 1',
#         'a//0 1',
#         'a//1 1',
#         'a//2 1',
#         'a//3 1',
#         'a//4 1',
#         'a//5 1',
#         'a//6 1',
#         'a//7 1',
#         'a//8 1',
#         'a//9 1',
#         'a//10 1',
#         '',
#         '11 directories, 0 files',
#     )

# run_tests()
from gradelib import *

r = Runner()


@test(5, "mp0 command with public testcase 0")
def test_mp0_0():
    r.run_qemu(shell_script([
        'testgen',
        'mp0 os2202 a',
    ]))
    r.match(
        'os2202 [error opening dir]',
        '',
        '0 directories, 0 files',
    )


@test(5, "mp0 command with public testcase 1")
def test_mp0_1():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 os2202/ o',
    ]))
    r.match(
        'os2202/ [error opening dir]',
        '',
        '0 directories, 0 files',
    )


@test(5, "mp0 command with public testcase 2")
def test_mp0_2():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 os2023 o',
    ]))
    r.match(
        'os2023 1',
        'os2023/d1 1',
        'os2023/d2 1',
        'os2023/d2/a 1',
        'os2023/d2/b 1',
        'os2023/d2/c 1',
        'os2023/d3 1',
        'os2023/d3/a 1',
        'os2023/d3/b 1',
        '',
        '6 directories, 2 files',
    )


@test(5, "mp0 command with public testcase 3")
def test_mp0_3():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 os2023/ o',
    ]))
    r.match(
        'os2023/ 1',
        'os2023//d1 1',
        'os2023//d2 1',
        'os2023//d2/a 1',
        'os2023//d2/b 1',
        'os2023//d2/c 1',
        'os2023//d3 1',
        'os2023//d3/a 1',
        'os2023//d3/b 1',
        '',
        '6 directories, 2 files',
    )


@test(10, "mp0 command with public testcase 4")
def test_mp0_4():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 a a',
    ]))
    r.match(
        'a 1',
        'a/0 1',
        'a/1 1',
        'a/2 1',
        'a/3 1',
        'a/4 1',
        'a/5 1',
        'a/6 1',
        'a/7 1',
        'a/8 1',
        'a/9 1',
        'a/10 1',
        '',
        '11 directories, 0 files',
    )

@test(10, "mp0 command with public testcase 5")
def test_mp0_5():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 a/ a',
    ]))
    r.match(
        'a/ 1',
        'a//0 1',
        'a//1 1',
        'a//2 1',
        'a//3 1',
        'a//4 1',
        'a//5 1',
        'a//6 1',
        'a//7 1',
        'a//8 1',
        'a//9 1',
        'a//10 1',
        '',
        '11 directories, 0 files',
    )

### add by me

'''
@test(3, "mp0 command with my testcase 1")
def test_mp0_6():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 b a',
    ]))
    r.match(
        'b 0',
        'b/a 1',
        'b/a/a 2',
        'b/a/a/aa 4',
        'b/a/a/aa/a 5',
        'b/a/a/aa/p 4',
        'b/a/a/aa/b 4',
        'b/a/a/aa/c 4',
        'b/a/a/aa/d 4',
        'b/a/a/aa/e 4',
        'b/a/a/aa/f 4',
        'b/a/a/aa/g 4',
        'b/a/a/aa/h 4',
        'b/a/a/aa/i 4',
        'b/a/a/aa/j 4',
        'b/a/a/aa/k 4',
        'b/a/a/aa/l 4',
        'b/a/a/aa/m 4',
        'b/a/a/aa/n 4',
        'b/a/a/aa/o 4',
        'b/a/a/aa/q 4',
        '',
        '12 directories, 8 files',
    )


@test(3, "mp0 command with my testcase 2")
def test_mp0_7():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 c a',
    ]))
    r.match(
        'c 0',
        'c/a 1',
        'c/a/a 2',
        'c/a/a/a 3',
        'c/a/a/a/a 4',
        'c/a/a/a/p 3',
        'c/a/a/a/b 3',
        'c/a/a/a/c 3',
        'c/a/a/a/d 3',
        'c/a/a/a/e 3',
        'c/a/a/a/f 3',
        'c/a/a/a/g 3',
        'c/a/a/a/h 3',
        'c/a/a/a/i 3',
        'c/a/a/a/j 3',
        'c/a/a/a/k 3',
        'c/a/a/a/l 3',
        'c/a/a/a/m 3',
        'c/a/a/a/n 3',
        'c/a/a/a/o 3',
        'c/a/a/a/q 3',
        '',
        '12 directories, 8 files',
    )


@test(3, "mp0 command with my testcase 3")
def test_mp0_8():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 c/ a',
    ]))
    r.match(
        'c/ 0',
        'c//a 1',
        'c//a/a 2',
        'c//a/a/a 3',
        'c//a/a/a/a 4',
        'c//a/a/a/p 3',
        'c//a/a/a/b 3',
        'c//a/a/a/c 3',
        'c//a/a/a/d 3',
        'c//a/a/a/e 3',
        'c//a/a/a/f 3',
        'c//a/a/a/g 3',
        'c//a/a/a/h 3',
        'c//a/a/a/i 3',
        'c//a/a/a/j 3',
        'c//a/a/a/k 3',
        'c//a/a/a/l 3',
        'c//a/a/a/m 3',
        'c//a/a/a/n 3',
        'c//a/a/a/o 3',
        'c//a/a/a/q 3',
        '',
        '12 directories, 8 files',
    )
### add by me

@test(3, "mp0 command with my testcase 4")
def test_mp0_9():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 d// a',
    ]))
    r.match(
        'd// 0',
        'd///a 1',
        'd///b 0',
        'd///c 0',
        'd///d 0',
        'd///d/d 0',
        'd///d/a 1',
        'd///d/c 0',
        'd///e 0',
        'd///f 0',
        'd///g 0',
        'd///h 0',
        'd///i 0',
        'd///j 0',
        'd///k 0',
        'd///l 0',
        'd///m 0',
        'd///n 0',
        'd///o 0',
        'd///ddd 0',
        'd///azs 1',
        '',
        '5 directories, 15 files',
    )






@test(3, "mp0 command with my testcase 5")
def test_mp0_10():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 d///// d',
    ]))
    r.match(
        'd///// 1',
        'd//////a 1',
        'd//////b 1',
        'd//////c 1',
        'd//////d 2',
        'd//////d/d 3',
        'd//////d/a 2',
        'd//////d/c 2',
        'd//////e 1',
        'd//////f 1',
        'd//////g 1',
        'd//////h 1',
        'd//////i 1',
        'd//////j 1',
        'd//////k 1',
        'd//////l 1',
        'd//////m 1',
        'd//////n 1',
        'd//////o 1',
        'd//////ddd 4',
        'd//////azs 1',
        '',
        '5 directories, 15 files',
    )

'''
@test(3, "mp0 command with my testcase 6")
def test_mp0_11():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 e z ',
    ]))
    r.match(
        'e 0',
        'e/aaddssa 0',
        'e/aaddssb 0',
        'e/aaddssc 0',
        'e/aaddssd 0',
        'e/aaddsse 0',
        'e/aaddssf 0',
        'e/aaddssg 0',
        'e/aaddssh 0',
        'e/aaddssi 0',
        'e/aaddssj 0',
        'e/aaddssk 0',
        'e/aaddssl 0',
        'e/aaddssm 0',
        'e/aaddssn 0',
        'e/aaddsso 0',
        'e/aaddssp 0',
        'e/aaddssq 0',
        'e/aaddssr 0',
        'e/aaddsss 0',
        'e/aaddsst 0',
        '',
        '0 directories, 20 files',
    )

@test(3, "mp0 command with my testcase 7")
def test_mp0_12():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 e/ e',
    ]))
    r.match(
        'e/ 1',
        'e//aaddssa 1',
        'e//aaddssb 1',
        'e//aaddssc 1',
        'e//aaddssd 1',
        'e//aaddsse 2',
        'e//aaddssf 1',
        'e//aaddssg 1',
        'e//aaddssh 1',
        'e//aaddssi 1',
        'e//aaddssj 1',
        'e//aaddssk 1',
        'e//aaddssl 1',
        'e//aaddssm 1',
        'e//aaddssn 1',
        'e//aaddsso 1',
        'e//aaddssp 1',
        'e//aaddssq 1',
        'e//aaddssr 1',
        'e//aaddsss 1',
        'e//aaddsst 1',
        '',
        '0 directories, 20 files',
    )

@test(3, "mp0 command with my testcase 8")
def test_mp0_13():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 e1 z ',
    ]))
    r.match(
        'e1 0',
        'e1/aaddssa 0',
        'e1/aaddssb 0',
        'e1/aaddssc 0',
        'e1/aaddssd 0',
        'e1/aaddsse 0',
        'e1/aaddssf 0',
        'e1/aaddssg 0',
        'e1/aaddssh 0',
        'e1/aaddssi 0',
        'e1/aaddssj 0',
        'e1/aaddssk 0',
        'e1/aaddssl 0',
        'e1/aaddssm 0',
        'e1/aaddssn 0',
        'e1/aaddsso 0',
        'e1/aaddssp 0',
        'e1/aaddssq 0',
        'e1/aaddssr 0',
        'e1/aaddsss 0',
        'e1/aaddsst 0',
        '',
        '0 directories, 20 files',
    )

@test(3, "mp0 command with my testcase 9")
def test_mp0_14():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 1 z ',
    ]))
    r.match(
        '1 0',
        '1/aaddssa 0',
        '1/aaddssb 0',
        '1/aaddssc 0',
        '1/aaddssd 0',
        '1/aaddsse 0',
        '1/aaddssf 0',
        '1/aaddssg 0',
        '1/aaddssh 0',
        '1/aaddssi 0',
        '1/aaddssj 0',
        '1/aaddssk 0',
        '1/aaddssl 0',
        '1/aaddssm 0',
        '1/aaddssn 0',
        '1/aaddsso 0',
        '1/aaddssp 0',
        '1/aaddssq 0',
        '1/aaddssr 0',
        '1/aaddsss 0',
        '1/aaddsst 0',
        '',
        '20 directories, 0 files',
    )
    

@test(3, "mp0 command with my testcase 10")
def test_mp0_15():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 1/ k',
    ]))
    r.match(
        '1/ 0',
        '1//aaddssa 0',
        '1//aaddssb 0',
        '1//aaddssc 0',
        '1//aaddssd 0',
        '1//aaddsse 0',
        '1//aaddssf 0',
        '1//aaddssg 0',
        '1//aaddssh 0',
        '1//aaddssi 0',
        '1//aaddssj 0',
        '1//aaddssk 1',
        '1//aaddssl 0',
        '1//aaddssm 0',
        '1//aaddssn 0',
        '1//aaddsso 0',
        '1//aaddssp 0',
        '1//aaddssq 0',
        '1//aaddssr 0',
        '1//aaddsss 0',
        '1//aaddsst 0',
        '',
        '20 directories, 0 files',
    )

@test(3, "mp0 command with my testcase 11")
def test_mp0_16():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 12 z ',
    ]))
    r.match(
        '12 0',
        '12/aaddssa 0',
        '12/aaddssb 0',
        '12/aaddssc 0',
        '12/aaddssd 0',
        '12/aaddsse 0',
        '12/aaddssf 0',
        '12/aaddssg 0',
        '12/aaddssh 0',
        '12/aaddssi 0',
        '12/aaddssj 0',
        '12/aaddssk 0',
        '12/aaddssl 0',
        '12/aaddssm 0',
        '12/aaddssn 0',
        '12/aaddsso 0',
        '12/aaddssp 0',
        '12/aaddssq 0',
        '12/aaddssr 0',
        '12/aaddsss 0',
        '12/aaddsst 0',
        '',
        '20 directories, 0 files',
    )

@test(3, "mp0 command with my testcase 12")

def test_mp0_17():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 12345678 a',
    ]))
    r.match(
        '12345678 0',
        '12345678/1 0',
        '12345678/2 0',
        '12345678/3 0',
        '12345678/4 0',
        '12345678/5 0',
        '12345678/6 0',
        '12345678/7 0',
        '12345678/8 0',
        '12345678/9 0',
        '12345678/a 1',
        '12345678/c 0',
        '12345678/b 0',
        '12345678/d 0',
        '12345678/e 0',
        '12345678/f 0',
        '12345678/h 0',
        '12345678/g 0',
        '12345678/i 0',
        '12345678/k 0',
        '12345678/j 0',
        '',
        '20 directories, 0 files',
    )
  
@test(3, "mp0 command with my testcase 13")


def test_mp0_18():
    r.run_qemu(shell_script([
        # 'testgen',
        'mp0 gggggggg g',
    ]))
    r.match(
        'gggggggg 8',
        'gggggggg/g 9',
        'gggggggg/a 8',
        'gggggggg/b 8',
        'gggggggg/c 8',
        'gggggggg/1 8',
        'gggggggg/3 8',
        'gggggggg/5 8',
        'gggggggg/7 8',
        'gggggggg/9 8',
        'gggggggg/2 8',
        'gggggggg/4 8',
        'gggggggg/6 8',
        'gggggggg/8 8',
        'gggggggg/k 8',
        'gggggggg/j 8',
        'gggggggg/h 8',
        'gggggggg/e 8',
        'gggggggg/f 8',
        'gggggggg/z 8',
        'gggggggg/w 8',
        '',
        '0 directories, 20 files',
    )
  


run_tests()


