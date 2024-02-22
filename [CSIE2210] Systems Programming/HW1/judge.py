import subprocess
import traceback
import telnetlib
import functools
import argparse
import socket
import typing
import copy
import time
import enum
import sys
import os

TIMEOUT = 0.5
# TIMEOUT = socket._GLOBAL_DEFAULT_TIMEOUT

class TelnetClient(telnetlib.Telnet):
    def __init__(self, verbose: bool, cid: int = 0, sid: tuple[int, int] = (0, 0), *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.verbose = verbose
        self.client_idx = cid
        self.server_type = "Read server" if sid[0] == 0 else "Write server"
        self.server_idx = sid[1]
        if self.verbose:
            print(f"Client {self.client_idx} connects to {self.server_type.lower()} {self.server_idx}")

    def check_output(self, expected: str) -> bool:
        msg = self.read_until(b"\n", timeout=TIMEOUT).decode()
        if len(msg) == 0 or msg[-1] != "\n":
            if self.verbose:
                print(f"\33[31m{self.server_type} {self.server_idx} respond to client {self.client_idx}: \"{msg}\", timeout\33[0m")
            return False
        msg = msg.strip("\n")
        if self.verbose:
            if msg == expected:
                print(f"{self.server_type} {self.server_idx} send to client {self.client_idx}: " + msg)
            elif expected is not None:
                print(f"\33[31mClient {self.client_idx} expects \"{expected}\", but received \"{msg}\" from {self.server_type.lower()} {self.server_idx}\33[0m")
        return msg == expected if expected is not None else True
    
    def check_EOF(self) -> bool:
        msg = ""
        try:
            msg = self.read_until(b"\n", timeout=TIMEOUT).decode()
        except EOFError:
            return True
        else:
            if self.verbose:
                print(f"\33[31m{self.server_type} {self.server_idx} close connection of client {self.client_idx} timeout, msg: \"{msg}\"\33[0m")
            return False

    def write(self, buffer: bytes) -> None:
        if self.verbose:
            print(f"Client {self.client_idx} send: " + buffer.decode().strip("\r\n"))
        super().write(buffer)


class Scorer:
    def __init__(self, dir_, student_id=None, port=10123, verbose=False):
        self.dir_ = dir_
        self.student_id = student_id
        self.default_port = port
        self.punishment = 0
        self.verbose = verbose
        self.score = [0.5, 0.5, 0.25, 1.5, 1.5, 1.75, 1]
        self.statistics = {}
        self.record = {
            902001: [2, 0, 1], 902002: [3, 4, 4], 902003: [3, 3, 2], 902004: [0, 1, 1], 902005: [3, 2, 0], 
            902006: [2, 1, 3], 902007: [1, 0, 1], 902008: [1, 0, 4], 902009: [2, 4, 1], 902010: [1, 3, 4],
            902011: [0, 1, 4], 902012: [1, 4, 3], 902013: [2, 4, 3], 902014: [1, 1, 0], 902015: [2, 4, 4],
            902016: [3, 3, 0], 902017: [1, 2, 0], 902018: [4, 3, 0], 902019: [3, 2, 4], 902020: [1, 4, 3]
        }
        self.record_backup = copy.deepcopy(self.record)
        self.FOOD = 0
        self.CONCERT = 1
        self.ELECS = 2
        self.ID = 3
        self.COMMAND = 4
        self.EXIT = 5
        self.LOCKED = 6
        self.UPDATE = 7
        self.FAILED = 8
        self.EXCEED = 9
        self.NEGATIVE = 10
        self.strs = [
            "Food: {num} booked",
            "Concert: {num} booked",
            "Electronics: {num} booked",
            "Please enter your id (to check your booking state):",
            "Please input your booking command. (Food, Concert, Electronics. Positive/negative value increases/decreases the booking amount.):",
            "(Type Exit to leave...)",
            "Locked.",
            "Bookings for user {id} are updated, the new booking state is:",
            "[Error] Operation failed. Please try again.",
            "[Error] Sorry, but you cannot book more than 15 items in total.",
            "[Error] Sorry, but you cannot book less than 0 items."
        ]
        self.read_server = [None, None]
        self.write_server = [None, None]

    def bold(self, str_: str) -> str:
        return "\33[1m" + str_ + "\33[0m"

    def grey(self, str_: str) -> str:
        return "\33[2m" + str_ + "\33[0m"

    def underscore(self, str_: str) -> str:
        return "\33[4m" + str_ + "\33[0m"

    def red(self, str_: str) -> str:
        return "\33[31m" + str_ + "\33[0m"

    def bright_red(self, str_: str) -> str:
        return "\33[1;31m" + str_ + "\33[0m"

    def green(self, str_: str) -> str:
        return "\33[32m" + str_ + "\33[0m"
    
    def cyan(self, str_: str) -> str:
        return "\33[36m" + str_ + "\33[0m"
    
    def white(self, str_: str) -> str:
        return "\33[37m" + str_ + "\33[0m"

    def find_empty_port(self, start: int) -> int:
        for i in range(start, 65536):
            if not self.check_port_in_use(i):
                return i

    def check_port_in_use(self, port: int) -> bool:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            return s.connect_ex(("localhost", port)) == 0;

    def start_server(self, server_type: str, port: int) -> subprocess.Popen:
        # server_type: either "read_server" or "write_server"
        server = subprocess.Popen([os.path.join(self.dir_, server_type), str(port)], stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL)
        # server = subprocess.Popen([os.path.join(self.dir_, server_type), str(port)])
        time.sleep(TIMEOUT)
        return server
    
    def execute_and_check_ret(self, cmd: list[str], exit_on_fail=True) -> tuple[str, str]:
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        cmd_str = ' '.join(cmd)
        stdout, stderr = proc.communicate()
        if proc.returncode:
            print(self.red(f"Error when executing {' '.join(cmd)}"))
            print(self.red(f"stdout output: {stdout}"))
            print(self.red(f"stderr output: {stderr}"))
            if exit_on_fail:
                exit(1)
        return stdout, stderr

    def execute(self, cmd: list[str]) -> tuple[int, str, str]:
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        stdout, stderr = proc.communicate()
        return proc.returncode, stdout, stderr
    
    def read_record(self) -> None:
        with open("./bookingRecord", "rb") as f:
            content = f.read()
            order = sys.byteorder
            for i in range(0, len(content), 16):
                id_ = int.from_bytes(content[i:i+4], order)
                food = int.from_bytes(content[i+4:i+8], order)
                concert = int.from_bytes(content[i+8:i+12], order)
                elecs = int.from_bytes(content[i+12:i+16], order)
                self.record[id_] = [food, concert, elecs]

    def print_record(self) -> None:
        for id_ in self.record:
            print(f"id: {id_}, Food: {self.record[id_][self.FOOD]}, Concert: {self.record[id_][self.CONCERT]}, Electronics: {self.record[id_][self.ELECS]}")

    def update_record(self, id_: int, food: int, concert: int, elecs: int) -> None:
        self.record[id_][self.FOOD] = food
        self.record[id_][self.CONCERT] = concert
        self.record[id_][self.ELECS] = elecs

    def restore_record(self) -> None:
        self.execute_and_check_ret(["cp", "bookingRecord.backup", "bookingRecord"])
        self.record = copy.deepcopy(self.record_backup)

    def file_exists(self, path: str) -> bool:
        try:
            with open(path, "rb") as f:
                pass
        except OSError:
            return False
        else:
            return True

    def init_test(self) -> None:
        print(self.bold("Initializing for tests..."))

        print("Checking existence of read_server and write_server...")
        if self.file_exists("./read_server") or self.file_exists("./write_server"):
            self.punishment += 1
            print(self.red("Find read_server/write_server, lose 1 point.\nRemoving read_server and write_server..."))
            self.execute_and_check_ret(["rm", "-f", "read_server", "write_server"])

        print("Copying bookingState...")
        self.execute_and_check_ret(["cp", "bookingRecord", "bookingRecord.backup"])

        print("Testing command \"make clean\"...")
        self.execute_and_check_ret(["touch", "read_server", "write_server"])
        ret, stdout, stderr = self.execute(["make", "clean"])
        if ret:
            self.punishment += 0.25
            print(self.red(f"Command \"make clean\" failed, lose 0.25 point.\nstdout: {stdout}\nstderr: {stderr}"))
            self.execute_and_check_ret(["rm", "-f", "read_server", "write_server"])
        elif self.file_exists("./read_server") or self.file_exists("./write_server"):
            self.punishment += 0.25
            print(self.red("Command \"make clean\" did not remove files correctly, lose 0.25 point."))
            self.execute_and_check_ret(["rm", "-f", "read_server", "write_server"])

        print("Compiling source code...")
        ret, stdout, stderr = self.execute(["make"])
        if ret:
            print(self.bright_red(f"Make failed, 0 points.\nstdout: {stdout}\nstderr: {stderr}"))
            exit(1)
        elif not self.file_exists("./read_server") or not self.file_exists("./write_server"):
            print(self.bright_red("Make did not generate read_server and write_server, 0 points."))
            exit(1)
        # print("Reading bookingRecord...")
        # self.read_record()
        # self.print_record()
        print(self.green("Init test done!"))
    
    def check_state(self, client, id_: int, read_newline: bool = True, state: list[int] = None) -> bool:
        passed = True
        for idx in [self.FOOD, self.CONCERT, self.ELECS]:
            if state is None:
                passed = passed and client.check_output(expected=self.strs[idx].format(num=self.record[id_][idx]))
            else:
                passed = passed and client.check_output(expected=self.strs[idx].format(num=state[idx]))
        if passed and read_newline:
            # read empty line
            client.check_output(expected=None)
        return passed
    
    def single_valid_read(self, port: int, id_: int) -> bool:
        passed = True
        with TelnetClient(host="localhost", port=port, verbose=self.verbose, timeout=TIMEOUT) as client:
            passed = client.check_output(expected=self.strs[self.ID])
            if passed:
                payload = str(id_) + "\r\n"
                client.write(payload.encode())
                passed = self.check_state(client, id_) and client.check_output(expected=self.strs[self.EXIT])
            if passed:
                client.write(b"Exit\r\n")
                passed = client.check_EOF()
        return passed

    def single_valid_write(self, port: int, id_: int, cmd: list[int], case: str = "normal") -> bool:
        passed = True
        food, concert, elecs = 0, 0, 0
        with TelnetClient(host="localhost", port=port, verbose=self.verbose, sid=(1, 0), timeout=TIMEOUT) as client:
            passed = client.check_output(expected=self.strs[self.ID])
            if passed:
                payload = str(id_) + "\r\n"
                client.write(payload.encode())
                food, concert, elecs = self.record[id_][self.FOOD], self.record[id_][self.CONCERT], self.record[id_][self.ELECS]
                passed = self.check_state(client, id_) and client.check_output(expected=self.strs[self.COMMAND])
            if passed:
                payload = " ".join([str(i) for i in cmd]) + "\r\n"
                client.write(payload.encode())
                if case == "upper":
                    passed = client.check_output(expected=self.strs[self.EXCEED])
                elif case == "lower":
                    passed = client.check_output(expected=self.strs[self.NEGATIVE])
                else:
                    if type(cmd[self.FOOD]) == str or type(cmd[self.CONCERT]) == str or type(cmd[self.ELECS]) == str:
                        food, concert, elecs = food + int(cmd[self.FOOD]), concert + int(cmd[self.CONCERT]), elecs + int(cmd[self.ELECS])
                    else:
                        food, concert, elecs = food + cmd[self.FOOD], concert + cmd[self.CONCERT], elecs + cmd[self.ELECS]
                    passed = client.check_output(expected=self.strs[self.UPDATE].format(id=id_)) and \
                             client.check_output(expected=self.strs[self.FOOD].format(num=food)) and \
                             client.check_output(expected=self.strs[self.CONCERT].format(num=concert)) and \
                             client.check_output(expected=self.strs[self.ELECS].format(num=elecs))
                    if passed:
                        self.update_record(id_, food, concert, elecs)
            if passed:
                passed = client.check_EOF()
        return passed

    def run_and_handle_exception(self, idx: int, task: int, f, *args, **kwargs) -> int:
        try:
            if self.verbose:
                print(self.cyan(f"Messages between client and server in testdata {idx}:"))
            passed = f(*args, **kwargs)
        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            print(self.bright_red(f"Testdata {idx}: Exception encountered"))
            print(self.bright_red(f"{traceback.format_exc()}"))
            self.clean()
            self.score[task] = 0
            raise e
        else:
            if passed:
                print(self.green(f"Testdata {idx}: Passed"))
                return 0
            else:
                print(self.red(f"Testdata {idx}: Failed"))
                self.score[task] = 0
                return 1

    def task_1_1(self) -> None:
        print(self.bold("=====Task 1-1: Read server====="))
        task = 0
        read_port = self.find_empty_port(start=self.default_port)
        self.read_server[0] = self.start_server("read_server", read_port)
        
        test_read_server = functools.partial(self.single_valid_read, read_port)
        
        try:
            self.run_and_handle_exception(1, task, test_read_server, id_=902001)
            self.run_and_handle_exception(2, task, test_read_server, id_=902013)
        except Exception as e:
            return
        else:
            self.clean()

    def task_1_2(self) -> None:
        print(self.bold("=====Task 1-2: Write server====="))
        task = 1
        write_port = self.find_empty_port(start=self.default_port)
        self.write_server[0] = self.start_server("write_server", write_port)
        
        test_write_server = functools.partial(self.single_valid_write, write_port, id_=902002)

        try:
            self.run_and_handle_exception(1, task, test_write_server, cmd=[2, 1, -2])
            self.run_and_handle_exception(2, task, test_write_server, cmd=[1, 1, 2], case="upper")
            self.run_and_handle_exception(3, task, test_write_server, cmd=[-6, 1, 2], case="lower")
            self.run_and_handle_exception(4, task, test_write_server, cmd=[1, -6, 3], case="lower")
            self.run_and_handle_exception(5, task, test_write_server, cmd=[3, 2, -4], case="lower")
        except Exception as e:
            return
        else:
            self.clean()
    
    def task_2(self) -> None:
        print(self.bold("=====Task 2: Handle valid and invalid requests on 1 server, 1 simultaneous connection====="))
        task = 2
        read_port = self.find_empty_port(start=self.default_port)
        self.read_server[0] = self.start_server("read_server", read_port)

        def invalid_id(port: int, id_, sid: tuple[int, int]) -> bool:
            passed = True
            with TelnetClient(host="localhost", port=port, verbose=self.verbose, sid=sid, timeout=TIMEOUT) as client:
                passed = client.check_output(expected=self.strs[self.ID])
                if passed:
                    payload = str(id_) + "\r\n"
                    client.write(payload.encode())
                    passed = client.check_output(expected=self.strs[self.FAILED]) and client.check_EOF()
            return passed

        def invalid_cmd(port: int, id_: int, cmd: str, sid: tuple[int, int]) -> bool:
            passed = True
            with TelnetClient(host="localhost", port=port, verbose=self.verbose, sid=sid, timeout=TIMEOUT) as client:
                client.check_output(expected=None)
                payload = str(id_) + "\r\n"
                client.write(payload.encode())
                for _ in range(5):
                    client.check_output(expected=None)
                payload = cmd + "\r\n"
                client.write(payload.encode())
                passed = client.check_output(expected=self.strs[self.FAILED]) and client.check_EOF()
            return passed

        test_invalid_id = functools.partial(invalid_id, read_port, sid=(0, 0))
        
        try:
            self.run_and_handle_exception(1, task, test_invalid_id, id_=902000)
            self.run_and_handle_exception(2, task, test_invalid_id, id_=902)
            self.run_and_handle_exception(3, task, test_invalid_id, id_=1000000)
            self.run_and_handle_exception(4, task, test_invalid_id, id_=902021)
            self.run_and_handle_exception(5, task, test_invalid_id, id_="902001a")
            self.run_and_handle_exception(6, task, test_invalid_id, id_=-902002)
        except Exception as e:
            return
        else:
            self.clean()

        write_port = self.find_empty_port(start=self.default_port)
        self.write_server[0] = self.start_server("write_server", write_port)
        
        test_invalid_id = functools.partial(invalid_id, write_port, sid=(1, 0))
        test_invalid_cmd = functools.partial(invalid_cmd, write_port, sid=(1, 0))
        test_valid_cmd = functools.partial(self.single_valid_write, write_port)

        try:
            self.run_and_handle_exception(7, task, test_invalid_id, id_=902000)
            self.run_and_handle_exception(8, task, test_invalid_id, id_=902)
            self.run_and_handle_exception(9, task, test_invalid_id, id_=1000000)
            self.run_and_handle_exception(10, task, test_invalid_id, id_=902021)
            self.run_and_handle_exception(11, task, test_invalid_id, id_="902001a")
            self.run_and_handle_exception(12, task, test_invalid_id, id_=-902002)
            self.run_and_handle_exception(13, task, test_invalid_cmd, id_=902014, cmd="1 0 2jkl")
            self.run_and_handle_exception(14, task, test_invalid_cmd, id_=902014, cmd="3 -1 2-")
            self.run_and_handle_exception(15, task, test_invalid_cmd, id_=902014, cmd="2 AZBNT -1")
            self.run_and_handle_exception(16, task, test_invalid_cmd, id_=902014, cmd="- 0 1")
            self.run_and_handle_exception(17, task, test_invalid_cmd, id_=902014, cmd="--2 0 1")
            self.run_and_handle_exception(18, task, test_valid_cmd, id_=902014, cmd=["-0", "-0", "-0"])
        except Exception as e:
            return
        else:
            self.clean()

    def task_3(self) -> None:
        print(self.bold("=====Task 3, Handle valid requests on 1 server, multiple simultaneous connections====="))
        task = 3
        write_port = self.find_empty_port(start=self.default_port)
        self.write_server[0] = self.start_server("write_server", write_port)
        read_port = self.find_empty_port(start=write_port+1)
        self.read_server[0] = self.start_server("read_server", read_port)

        def test_3_1():
            # 3 simultaneous connections to write server, 2 accessing same ID
            passed = True
            with TelnetClient(host="localhost", port=write_port, verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0)) as client1, \
                 TelnetClient(host="localhost", port=write_port, verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=1) as client2, \
                 TelnetClient(host="localhost", port=write_port, verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=2) as client3:
                passed = client1.check_output(expected=self.strs[self.ID]) and \
                         client2.check_output(expected=self.strs[self.ID]) and \
                         client3.check_output(expected=self.strs[self.ID])
                if passed:
                    client2.write(b"902006\r\n")
                    passed = self.check_state(client2, 902006) and client2.check_output(expected=self.strs[self.COMMAND])
                if passed:
                    client1.write(b"902005\r\n")
                    passed = self.check_state(client1, 902005) and client1.check_output(expected=self.strs[self.COMMAND])
                if passed:
                    # simultaneous write, lock
                    client3.write(b"902005\r\n")
                    passed = client3.check_output(expected=self.strs[self.LOCKED]) and client3.check_EOF()
                if passed:
                    client1.write(b"-1 2 2\r\n")
                    passed = client1.check_output(expected=self.strs[self.UPDATE].format(id=902005)) and \
                             self.check_state(client1, 902005, read_newline=False, state=[2, 4, 2]) and \
                             client1.check_EOF()
                if passed:
                    client2.write(b"-2 -1 -3\r\n")
                    passed = client2.check_output(expected=self.strs[self.UPDATE].format(id=902006)) and \
                             self.check_state(client2, 902006, read_newline=False, state=[0, 0, 0]) and \
                             client2.check_EOF()
            if not passed:
                return passed
            self.write_server[0].kill()
            self.write_server[0].wait()
            self.write_server[0] = None
            with TelnetClient(host="localhost", port=read_port, verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=3) as client1, \
                 TelnetClient(host="localhost", port=read_port, verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=4) as client2, \
                 TelnetClient(host="localhost", port=read_port, verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=5) as client3:
                passed = client1.check_output(expected=self.strs[self.ID]) and \
                         client2.check_output(expected=self.strs[self.ID]) and \
                         client3.check_output(expected=self.strs[self.ID])
                if passed:
                    client2.write(b"902006\r\n")
                    passed = self.check_state(client2, 902006, state=[0, 0, 0]) and client2.check_output(expected=self.strs[self.EXIT])
                if passed:
                    client1.write(b"902005\r\n")
                    passed = self.check_state(client1, 902005, state=[2, 4, 2]) and client1.check_output(expected=self.strs[self.EXIT])
                if passed:
                    # simultaneous read, ok 
                    client3.write(b"902005\r\n")
                    passed = self.check_state(client3, 902005, state=[2, 4, 2]) and client3.check_output(expected=self.strs[self.EXIT])
                if passed:
                    client1.write(b"Exit\r\n")
                    passed = client1.check_EOF()
                if passed:
                    client3.write(b"Exit\r\n")
                    passed = client3.check_EOF()
                if passed:
                    client2.write(b"Exit\r\n")
                    passed = client2.check_EOF()
            return passed
       
        test_1 = functools.partial(test_3_1)
        try:
            self.run_and_handle_exception(1, task, test_1)
        except Exception as e:
            return
        else:
            self.clean()
                    
    def task_4(self) -> None:
        print(self.bold("=====Task 4, Handle valid requests on multiple servers, 1 simultaneous connection====="))
        task = 4
        read_port, write_port = [], []
        read_port.append(self.find_empty_port(start=self.default_port))
        self.read_server[0] = self.start_server("read_server", port=read_port[0])
        read_port.append(self.find_empty_port(start=read_port[0]+1))
        self.read_server[1] = self.start_server("read_server", port=read_port[1])
        write_port.append(self.find_empty_port(start=read_port[1]+1))
        self.write_server[0] = self.start_server("write_server", port=write_port[0])
        write_port.append(self.find_empty_port(start=write_port[0]+1))
        self.write_server[1] = self.start_server("write_server", port=write_port[1])

        def test_4_1():
            passed = True
            with TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=0) as rd_client1, \
                 TelnetClient(host="localhost", port=read_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 1), cid=1) as rd_client2, \
                 TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=2) as wr_client1, \
                 TelnetClient(host="localhost", port=write_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 1), cid=3) as wr_client2:
                passed = rd_client1.check_output(expected=self.strs[self.ID]) and rd_client2.check_output(expected=self.strs[self.ID]) and \
                         wr_client1.check_output(expected=self.strs[self.ID]) and wr_client2.check_output(expected=self.strs[self.ID])
                # simultaneous read, ok
                if passed:
                    rd_client2.write(b"902007\r\n")
                    passed = self.check_state(rd_client2, 902007) and rd_client2.check_output(expected=self.strs[self.EXIT])
                if passed:
                    rd_client1.write(b"902007\r\n")
                    passed = self.check_state(rd_client1, 902007) and rd_client1.check_output(expected=self.strs[self.EXIT])
                # write should be blocked by read
                if passed:
                    wr_client1.write(b"902007\r\n")
                    passed = wr_client1.check_output(expected=self.strs[self.LOCKED]) and wr_client1.check_EOF()
                # this write should not be blocked
                if passed:
                    wr_client2.write(b"902006\r\n")
                    passed = self.check_state(wr_client2, 902006) and wr_client2.check_output(expected=self.strs[self.COMMAND])
                # finish operations
                if passed:
                    rd_client1.write(b"Exit\r\n")
                    wr_client2.write(b"2 -1 -1\r\n")
                    rd_client2.write(b"Exit\r\n")
                    passed = rd_client1.check_EOF() and rd_client2.check_EOF() and \
                             wr_client2.check_output(expected=self.strs[self.UPDATE].format(id=902006)) and \
                             self.check_state(wr_client2, 902006, read_newline=False, state=[4, 0, 2]) and wr_client2.check_EOF()
                return passed
        
        def test_4_2():
            passed = True
            with TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=0) as rd_client1, \
                 TelnetClient(host="localhost", port=read_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 1), cid=1) as rd_client2, \
                 TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=2) as wr_client1, \
                 TelnetClient(host="localhost", port=write_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 1), cid=3) as wr_client2:
                passed = rd_client1.check_output(expected=self.strs[self.ID]) and rd_client2.check_output(expected=self.strs[self.ID]) and \
                         wr_client1.check_output(expected=self.strs[self.ID]) and wr_client2.check_output(expected=self.strs[self.ID])
                # simultaneous write should be blocked
                if passed:
                    wr_client2.write(b"902009\r\n")
                    time.sleep(TIMEOUT) # To ensure wr_client2 acquire write lock
                    wr_client1.write(b"902009\r\n")
                    passed = self.check_state(wr_client2, 902009) and wr_client2.check_output(expected=self.strs[self.COMMAND]) and \
                             wr_client1.check_output(expected=self.strs[self.LOCKED]) and wr_client1.check_EOF()
                # read should be blocked by write
                if passed:
                    rd_client2.write(b"902009\r\n")
                    passed = rd_client2.check_output(expected=self.strs[self.LOCKED]) and rd_client2.check_EOF()
                # this read should not be blocked
                if passed:
                    rd_client1.write(b"902010\r\n")
                    passed = self.check_state(rd_client1, 902010) and rd_client1.check_output(expected=self.strs[self.EXIT])
                # finish the operations
                if passed:
                    rd_client1.write(b"Exit\r\n")
                    wr_client2.write(b"1 -2 -1\r\n")
                    passed = rd_client1.check_EOF() and wr_client2.check_output(expected=self.strs[self.UPDATE].format(id=902009)) and \
                             self.check_state(wr_client2, 902009, read_newline=False, state=[3, 2, 0]) and wr_client2.check_EOF()
                return passed
        
        test_1 = functools.partial(test_4_1)
        test_2 = functools.partial(test_4_2)

        try:
            self.run_and_handle_exception(1, task, test_1)
            self.run_and_handle_exception(2, task, test_2)
        except Exception as e:
            return
        else:
            self.clean()

    def task_5_1(self) -> None:
        print(self.bold("=====Task 5-1, Client will not disconnect arbitrarily====="))
        task = 5
        read_port, write_port = [], []
        read_port.append(self.find_empty_port(start=self.default_port))
        self.read_server[0] = self.start_server("read_server", port=read_port[0])
        read_port.append(self.find_empty_port(start=read_port[0]+1))
        self.read_server[1] = self.start_server("read_server", port=read_port[1])
        write_port.append(self.find_empty_port(start=read_port[1]+1))
        self.write_server[0] = self.start_server("write_server", port=write_port[0])
        write_port.append(self.find_empty_port(start=write_port[0]+1))
        self.write_server[1] = self.start_server("write_server", port=write_port[1])

        def boss():
            passed = True
            rd_client, wr_client = [None for i in range(8)], [None for i in range(8)]
            with TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=0) as rd_client[0], \
                 TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=1) as rd_client[1], \
                 TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=2) as rd_client[2], \
                 TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=3) as rd_client[3], \
                 TelnetClient(host="localhost", port=read_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 1), cid=4) as rd_client[4], \
                 TelnetClient(host="localhost", port=read_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 1), cid=5) as rd_client[5], \
                 TelnetClient(host="localhost", port=read_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 1), cid=6) as rd_client[6], \
                 TelnetClient(host="localhost", port=read_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 1), cid=7) as rd_client[7], \
                 TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=8) as wr_client[0], \
                 TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=9) as wr_client[1], \
                 TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=10) as wr_client[2], \
                 TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=11) as wr_client[3], \
                 TelnetClient(host="localhost", port=write_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 1), cid=12) as wr_client[4], \
                 TelnetClient(host="localhost", port=write_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 1), cid=13) as wr_client[5], \
                 TelnetClient(host="localhost", port=write_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 1), cid=14) as wr_client[6], \
                 TelnetClient(host="localhost", port=write_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 1), cid=15) as wr_client[7]:
                # rd_client[:4]: read_server[0], rd_client[4:]: read_server[1]
                # wr_client[:4]: write_server[0], wr_client[4:]: write_server[1]
                for i in range(8):
                    passed = passed and rd_client[i].check_output(expected=self.strs[self.ID])
                for i in range(8):
                    passed = passed and wr_client[i].check_output(expected=self.strs[self.ID])
                # In-process simultaneous read
                if passed:
                    rd_client[1].write(b"902010\r\n")
                    passed = self.check_state(rd_client[1], 902010) and rd_client[1].check_output(expected=self.strs[self.EXIT])
                if passed:
                    rd_client[0].write(b"902010\r\n")
                    passed = self.check_state(rd_client[0], 902010) and rd_client[0].check_output(expected=self.strs[self.EXIT])
                # One read client exit, but read lock should not be released
                # Inter-process write blocked by read
                if passed:
                    rd_client[1].write(b"Exit\r\n")
                    passed = rd_client[1].check_EOF()
                if passed:
                    wr_client[0].write(b"902010\r\n")
                    passed = wr_client[0].check_output(expected=self.strs[self.LOCKED]) and wr_client[0].check_EOF()
                # Second read client exit, now write should not be blocked
                if passed:
                    rd_client[0].write(b"Exit\r\n")
                    passed = rd_client[0].check_EOF()
                if passed:
                    wr_client[1].write(b"902010\r\n")
                    passed = self.check_state(wr_client[1], 902010) and wr_client[1].check_output(expected=self.strs[self.COMMAND])
                # Inter-process read blocked by write
                if passed:
                    rd_client[4].write(b"902010\r\n")
                    passed = rd_client[4].check_output(expected=self.strs[self.LOCKED]) and rd_client[4].check_EOF()
                # Inter-process write blocked by write
                if passed:
                    wr_client[4].write(b"902010\r\n")
                    passed = wr_client[4].check_output(expected=self.strs[self.LOCKED]) and wr_client[4].check_EOF()
                # In-process write blocked by write
                if passed:
                    wr_client[2].write(b"902010\r\n")
                    passed = wr_client[2].check_output(expected=self.strs[self.LOCKED]) and wr_client[2].check_EOF()
                # Inter-process write lock should not be released here
                # Buggy program may release write lock in other connection
                if passed:
                    rd_client[2].write(b"902010\r\n")
                    passed = rd_client[2].check_output(expected=self.strs[self.LOCKED]) and rd_client[2].check_EOF()
                # Finish write to 902010
                if passed:
                    wr_client[1].write(b"3 -1 2\r\n")
                    passed = wr_client[1].check_output(expected=self.strs[self.UPDATE].format(id=902010)) and \
                             self.check_state(wr_client[1], 902010, read_newline=False, state=[4, 2, 6]) and \
                             wr_client[1].check_EOF()
                # Test if write lock is released (inter-process)
                if passed:
                    rd_client[5].write(b"902010\r\n")
                    passed = self.check_state(rd_client[5], 902010, state=[4, 2, 6]) and rd_client[5].check_output(expected=self.strs[self.EXIT])
                # Test if write lock is released (in-process)
                if passed:
                    rd_client[5].write(b"Exit\r\n")
                    passed = rd_client[5].check_EOF()
                if passed:
                    wr_client[3].write(b"902010\r\n")
                    passed = self.check_state(wr_client[3], 902010, state=[4, 2, 6]) and wr_client[3].check_output(expected=self.strs[self.COMMAND])
                # Test if multiple inter-process read lock are released
                if passed:
                    rd_client[3].write(b"902011\r\n")
                    passed = self.check_state(rd_client[3], 902011) and rd_client[3].check_output(expected=self.strs[self.EXIT])
                if passed:
                    rd_client[6].write(b"902011\r\n")
                    passed = self.check_state(rd_client[6], 902011) and rd_client[6].check_output(expected=self.strs[self.EXIT])
                if passed:
                    rd_client[6].write(b"Exit\r\n")
                    passed = rd_client[6].check_EOF()
                if passed:
                    wr_client[6].write(b"902011\r\n")
                    passed = wr_client[6].check_output(expected=self.strs[self.LOCKED]) and wr_client[6].check_EOF()
                if passed:
                    rd_client[3].write(b"Exit\r\n")
                    passed = rd_client[3].check_EOF()
                if passed:
                    wr_client[5].write(b"902011\r\n")
                    passed = self.check_state(wr_client[5], 902011) and wr_client[5].check_output(expected=self.strs[self.COMMAND])
                if passed:
                    wr_client[3].write(b"-2 -2 -2\r\n")
                    passed = wr_client[3].check_output(expected=self.strs[self.UPDATE].format(id=902010)) and \
                             self.check_state(wr_client[3], 902010, read_newline=False, state=[2, 0, 4]) and \
                             wr_client[3].check_EOF()
                # Test if server modifies the file
                if passed:
                    rd_client[7].write(b"902010\r\n")
                    passed = self.check_state(rd_client[7], 902010, state=[2, 0, 4]) and rd_client[7].check_output(expected=self.strs[self.EXIT])
                if passed:
                    rd_client[7].write(b"Exit\r\n")
                    passed = rd_client[7].check_EOF()
                if passed:
                    wr_client[7].write(b"902010\r\n")
                    passed = self.check_state(wr_client[7], 902010, state=[2, 0, 4]) and wr_client[7].check_output(expected=self.strs[self.COMMAND])
                if passed:
                    wr_client[7].write(b"1 0 0\r\n")
                    passed = wr_client[7].check_output(expected=self.strs[self.UPDATE].format(id=902010)) and \
                             self.check_state(wr_client[7], 902010, read_newline=False, state=[3, 0, 4]) and \
                             wr_client[7].check_EOF()
                # Finish the operation
                if passed:
                    wr_client[5].write(b"1 -1 1\r\n")
                    passed = wr_client[5].check_output(expected=self.strs[self.UPDATE].format(id=902011)) and \
                             self.check_state(wr_client[5], 902011, read_newline=False, state=[1, 0, 5]) and \
                             wr_client[5].check_EOF()
                # print([cl.check_EOF() for cl in rd_client + wr_client])
            return passed

        test_1 = functools.partial(boss)

        try:
            self.run_and_handle_exception(1, task, test_1)
        except Exception as e:
            return
        else:
            self.clean()

    def task_5_2(self) -> None:
        print(self.bold("=====Task 5-2, Client may disconnect arbitrarily====="))
        task = 6
        read_port, write_port = [], []
        read_port.append(self.find_empty_port(start=self.default_port))
        self.read_server[0] = self.start_server("read_server", port=read_port[0])
        read_port.append(self.find_empty_port(start=read_port[0]+1))
        self.read_server[1] = self.start_server("read_server", port=read_port[1])
        write_port.append(self.find_empty_port(start=read_port[1]+1))
        self.write_server[0] = self.start_server("write_server", port=write_port[0])
        write_port.append(self.find_empty_port(start=write_port[0]+1))
        self.write_server[1] = self.start_server("write_server", port=write_port[1])

        def test_5_2_1():
            # Test if disconnection is handled correctly in first stage
            # fd should be closed, use more than 1024 connections
            passed = True
            # Read server
            for i in range(1024):
                if not passed:
                    break
                with TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=i) as client:
                    passed = client.check_output(expected=self.strs[self.ID])
            # Write server
            for i in range(1024):
                if not passed:
                    break
                with TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=i+1024) as client:
                    passed = client.check_output(expected=self.strs[self.ID])
            return passed

        def test_5_2_2():
            # Test if disconnection is handled correctly in second stage
            passed = True
            # Read server
            for i in range(1024):
                if not passed:
                    break
                with TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=i) as client:
                    passed = client.check_output(expected=self.strs[self.ID])
                    if passed:
                        client.write(b"902013\r\n")
                        passed = self.check_state(client, 902013) and client.check_output(expected=self.strs[self.EXIT])
            # Write server
            for i in range(1024):
                if not passed:
                    break
                with TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=i) as client:
                    passed = client.check_output(expected=self.strs[self.ID])
                    if passed:
                        client.write(b"902013\r\n")
                        passed = self.check_state(client, 902013) and client.check_output(expected=self.strs[self.COMMAND])
            return passed
        
        def test_5_2_3():
            # Test if lock is released after disconnection
            passed = True
            rd_client, wr_client = [None for i in range(8)], [None for i in range(8)]
            with TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=0) as rd_client[0], \
                 TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=1) as rd_client[1], \
                 TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=2) as rd_client[2], \
                 TelnetClient(host="localhost", port=read_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 0), cid=3) as rd_client[3], \
                 TelnetClient(host="localhost", port=read_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 1), cid=4) as rd_client[4], \
                 TelnetClient(host="localhost", port=read_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 1), cid=5) as rd_client[5], \
                 TelnetClient(host="localhost", port=read_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 1), cid=6) as rd_client[6], \
                 TelnetClient(host="localhost", port=read_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(0, 1), cid=7) as rd_client[7], \
                 TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=8) as wr_client[0], \
                 TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=9) as wr_client[1], \
                 TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=10) as wr_client[2], \
                 TelnetClient(host="localhost", port=write_port[0], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 0), cid=11) as wr_client[3], \
                 TelnetClient(host="localhost", port=write_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 1), cid=12) as wr_client[4], \
                 TelnetClient(host="localhost", port=write_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 1), cid=13) as wr_client[5], \
                 TelnetClient(host="localhost", port=write_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 1), cid=14) as wr_client[6], \
                 TelnetClient(host="localhost", port=write_port[1], verbose=self.verbose, timeout=TIMEOUT, sid=(1, 1), cid=15) as wr_client[7]:
                # rd_client[:4]: read_server[0], rd_client[4:]: read_server[1]
                # wr_client[:4]: write_server[0], wr_client[4:]: write_server[1]
                for i in range(8):
                    passed = passed and rd_client[i].check_output(expected=self.strs[self.ID])
                for i in range(8):
                    passed = passed and wr_client[i].check_output(expected=self.strs[self.ID])
                if passed:
                    rd_client[0].write(b"902020\r\n")
                    passed = self.check_state(rd_client[0], 902020) and rd_client[0].check_output(expected=self.strs[self.EXIT])
                if passed:
                    rd_client[1].write(b"902020\r\n")
                    passed = self.check_state(rd_client[1], 902020) and rd_client[1].check_output(expected=self.strs[self.EXIT])
                # Inter-process read lock
                # One read client disconnect, should be locked
                if passed:
                    rd_client[0].close()
                    time.sleep(TIMEOUT)
                    wr_client[0].write(b"902020\r\n")
                    passed = wr_client[0].check_output(expected=self.strs[self.LOCKED]) and wr_client[0].check_EOF()
                # Second read client disconnect, should not be locked
                if passed:
                    rd_client[1].close()
                    time.sleep(TIMEOUT)
                    wr_client[4].write(b"902020\r\n")
                    passed = self.check_state(wr_client[4], 902020) and wr_client[4].check_output(expected=self.strs[self.COMMAND])
                # In-process write lock
                if passed:
                    wr_client[5].write(b"902020\r\n")
                    passed = wr_client[5].check_output(expected=self.strs[self.LOCKED]) and wr_client[5].check_EOF()
                if passed:
                    wr_client[4].close()
                    time.sleep(TIMEOUT)
                    wr_client[6].write(b"902020\r\n")
                    passed = self.check_state(wr_client[6], 902020) and wr_client[6].check_output(expected=self.strs[self.COMMAND])
                # Inter-process write lock
                if passed:
                    wr_client[1].write(b"902020\r\n")
                    passed = wr_client[1].check_output(expected=self.strs[self.LOCKED]) and wr_client[1].check_EOF()
                if passed:
                    rd_client[4].write(b"902020\r\n")
                    passed = rd_client[4].check_output(expected=self.strs[self.LOCKED]) and rd_client[4].check_EOF()
                if passed:
                    wr_client[6].close()
                    time.sleep(TIMEOUT)
                    rd_client[5].write(b"902020\r\n")
                    passed = self.check_state(rd_client[5], 902020) and rd_client[5].check_output(expected=self.strs[self.EXIT])
                if passed:
                    rd_client[5].close()
                    wr_client[2].write(b"902019\r\n")
                    passed = self.check_state(wr_client[2], 902019) and wr_client[2].check_output(expected=self.strs[self.COMMAND])
                if passed:
                    wr_client[3].write(b"902018\r\n")
                    passed = self.check_state(wr_client[3], 902018) and wr_client[3].check_output(expected=self.strs[self.COMMAND])
                if passed:
                    wr_client[2].write(b"-1 -1 0\r\n")
                    passed = wr_client[2].check_output(expected=self.strs[self.UPDATE].format(id=902019)) and \
                             self.check_state(wr_client[2], 902019, read_newline=False, state=[2, 1, 4]) and \
                             wr_client[2].check_EOF()
                if passed:
                    wr_client[3].write(b"2 -3 1\r\n")
                    passed = wr_client[3].check_output(expected=self.strs[self.UPDATE].format(id=902018)) and \
                             self.check_state(wr_client[3], 902018, read_newline=False, state=[6, 0, 1]) and \
                             wr_client[3].check_EOF()
                if passed:
                    time.sleep(TIMEOUT)
                    rd_client[2].write(b"902018\r\n")
                    passed = self.check_state(rd_client[2], 902018, state=[6, 0, 1]) and rd_client[2].check_output(expected=self.strs[self.EXIT])
                if passed:
                    rd_client[3].write(b"902019\r\n")
                    passed = self.check_state(rd_client[3], 902019, state=[2, 1, 4]) and rd_client[3].check_output(expected=self.strs[self.EXIT])
                if passed:
                    rd_client[2].write(b"Exit\r\n")
                    passed = rd_client[2].check_EOF()
                if passed:
                    rd_client[3].write(b"Exit\r\n")
                    passed = rd_client[3].check_EOF()
                return passed

        test_1 = functools.partial(test_5_2_1)
        test_2 = functools.partial(test_5_2_2)
        test_3 = functools.partial(test_5_2_3)

        try:
            self.run_and_handle_exception(1, task, test_1)
            self.run_and_handle_exception(2, task, test_2)
            self.run_and_handle_exception(3, task, test_3)
        except Exception as e:
            return
        else:
            self.clean()

    def clean(self) -> None:
        for i in range(2):
            if self.read_server[i] is not None:
                self.read_server[i].kill()
                self.read_server[i].wait()
                self.read_server[i] = None
            if self.write_server[i] is not None:
                self.write_server[i].kill()
                self.write_server[i].wait()
                self.write_server[i] = None
        self.restore_record()

    def print_score(self) -> None:
        print(self.bold(f"Raw score: {sum(self.score)}"))
        total = sum(self.score) - self.punishment
        if total < 0:
            total = 0
        print(self.bold(f"Final score: {total}"))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--verbose", action="store_true", required=False, help="If set, message received from server will be printed.")
    parser.add_argument(
        "-t", "--task", nargs="+", required=False, 
        help="Specify which tasks to run. E.g, python judge.py -t 1_1 3 5_1 will only run task 1-1, task 3 and task 5-1."
    )
    args = parser.parse_args()
    scorer = Scorer(dir_=os.path.dirname(os.path.abspath(__file__)), student_id=None, verbose=args.verbose)
    scorer.init_test()
    print(scorer.bold("Start testing!"))
    # print(args.task)
    if args.task is None:
        print(scorer.bold("=====Task 1: Handle requests on 1 server, 1 simultaneous connection====="))
        scorer.task_1_1()
        scorer.task_1_2()
        scorer.task_2()
        scorer.task_3()
        scorer.task_4()
        print(scorer.bold("=====Task 5, Handle valid requests on multiple servers, multiple simultaneous connection====="))
        scorer.task_5_1()
        scorer.task_5_2()
    else:
        tasks = {
            "1_1": scorer.task_1_1, "1_2": scorer.task_1_2, "2": scorer.task_2, "3": scorer.task_3,
            "4": scorer.task_4, "5_1": scorer.task_5_1, "5_2": scorer.task_5_2
        }
        index = {"1_1": 0, "1_2": 1, "2": 2, "3": 3, "4": 4, "5_1": 5, "5_2": 6}
        if not all([t in tasks for t in args.task]):
            print(f"Error: task should be 1_1, 1_2, 2, 3, 4, 5_1 or 5_2.")
            exit(0)
        for key in tasks:
            if key in args.task:
                tasks[key]()
            else:
                scorer.score[index[key]] = 0
    scorer.print_score()

if __name__ == '__main__':
    main()
