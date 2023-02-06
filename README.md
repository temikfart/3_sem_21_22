## Task 1
---
Write a `shell-wrapper` program which allow you to:
- run another programs via command line cyclically getting commands from `STDIN` and running it somewhere, e.g. in child process.
- get exit codes of terminated programs
- use pipelining of program sequences, for ex.: `env | grep HOSTNAME | wc`

**TIPS:**
1. Use `2_fork_wait_exit.c` and `4_exec_dir.c` from examples. Combine them.
2. Parse input string according to the type of `exec*` (for ex, see `man execlp`).
	1. if `execvp` is used, string splitting into `path` and `args` is all you need.
	2. the program should be environment(`$PATH` variable)-sensitive: for ex., `/bin/ls` should be executed with string `ls`. 
3. Collect exit codes via `waitpid`/`WEXITSTATUS`.
4. Note that a size of command can reach a lot of kbytes: type `getconf ARG_MAX` cmd to check it.
5. Let the cmd can represent a sequence of programs which can be executed sequentially, if `|` symbol is between programs’ names, in this case output of (i-1)’th should be redirected from stdout to stdin of i'th. See the example above. Implement this feature. 

## Task 2
---
* Implement duplex pipe in **C** using some **OOP** principles (see example 1).
* Maximize the performance of echo test (from parent to child and back) on huge files *(> 4 Gb)*. The main metric is time spent on data transmission.
* "Play" with buffer sizes and limits on pipe size.

**Notes:**
1. Create big file if needed (4 Gb file):
```Shell
time dd if=/dev/urandom of=file.txt bs=1048576 count=4096
```
2. If you want to check the data integrity, compare the checksums of input and received file:
```Shell
md5sum <file_name>
```

## Task 3
---
Compare in practice 3 ways of interaction between two processes.
For this:
1. Develop 3 programs that transfer a file of arbitrary (rather large) size between two processes using the following tools:
	1. Shared memory SYS V.
	2. SYS V message queues.
	3. Named channels (FIFO).
2. For each of the programs, measure the execution time during file transfer using the `time` utility or the utilities from the [time.h] header (https://ru.wikipedia.org/wiki/Time.h ), making measurements for 2-3 characteristic buffer sizes (memory, data in the message):
	1. "small" size - less than a couple hundred bytes;
	2. 4 KB
	3. large size - significantly more than 4 KB.
3. Based on the measurement results, build 3 histograms of the transmission time from the buffer size. On each histogram there will be 3 bands, each for the instrument 1.1, 1.2, 1.3, respectively. Try to catch situations in which the results will be the most representative. After that, conclusions and conclusions should be drawn.

## Task 4
---
Calculate a certain integral of some simple, analytically integrable function f(x) over a certain interval by the Monte Carlo method:
- The calculation is carried out in parallel in n-threads of some program **A**, generating N/n points in each of them uniformly both over the interval and over the range of values.
- Collect and process the results in program B, which reads the results from shared memory. (In the case of using independent shared memory cells, the results can be saved without synchronization, in the case of a shared cell in shared memory, synchronization is necessary. The implementation of the storage method affects the results of experiment 2 a,b\*) (see below).

Tasks assigned to you as a researcher:
1. Estimate the performance gain due to the use of n' threads instead of 1, where n' is the number of physical cores of your machine. Find out the number of cores: `cat /cpu/procinfo`
2. Conduct a series of measurements in which, increasing the number of n':
	1. Observe the stagnation of performance growth with an increase in the number of threads
	2. (\*) Determine at which n' (how large) the overhead from using multithreading (and synchronization, if it took place) exceeds the advantages of using them.

## Task 5
---
1. Transfer a file between 2 processes by means of signals.
2. Check the correctness of the transmission by any automatic means (checksum, `diff` in the case of text, etc.). 
**Note:** You can use both `RT` signals and `RT` + `sigval` fields of the corresponding `SIGINFO` structure. IPCs other than signals are prohibited. There are no other restrictions.

## Task 6
---
Develop a daemon program that:
1. Finds the process by its **PID**, reads its working directory and writes to `procfs/proc/<pid name>`.
2. Performs the following:
	1. **Option 1:** recursively scans the working directory, finds changes in it only in text files (see the example on diff + think about how to programmatically determine that the file is text using the file utility), then copies the modified files to another directory, that is, makes a backup.
	2. **Option 2:** monitors virtual memory displays, recording changes, for example, the display of new shared libraries, after which it reports to the log or some buffer.
	    
	    **Note:** A potential plus in karma for incremental backups (we save only the sequences of diffs and the time stamp of creation, then we roll them sequentially to the initial states of the files).
	    
	    In any of the options, the daemon must save data in the form of periodic samples, that is, make one record in T ms, where T is the sampling period set to the daemon through some IPC channel of interaction with it (for example, FIFO).
	3. Option 3. Selected fields-counters /proc/vmstat. In this case, the results should be output as the difference between the readings of samples i-1 and i at the i-th step.

The daemon must be disconnected from the control terminal and protected from unwanted signals and input/output channels.

Resource leakage monitoring will also be performed at the reception.

## Task 7
---
The 7th homework is devoted to I/O multiplexing.

It is proposed to develop a software solution that implements a client-server architecture. This type of architecture is often used in the organization of network interaction of programs. In view of the fact that we have not yet passed the network, in our example, interaction is organized within one computer, and the components communicate with each other not by means of the Linux TCP/IP stack, but via FIFO.

The program is divided into 2 parts: 
1. Client (Cn) - sends a request to the server to register itself on it, after which it has the ability to download files through the server
2. Server (S) - registers the client, then gives the corresponding file on request

Consider the scheme:
1. The client is registered by sending a message via a special command IPC (it can be a FIFO or a message queue).
	
	The registration command contains 3 tokens: `REGISTER <fifo/tx/path> <fifo/rx/path>` - the command and paths to fifo records on the FS for transmitting commands to download files, respectively. Note that there may be more than one client (but not many, let's up to 64).
2. The server responds with an "ASK" to the client via the reverse communication channel of the selected IPC, saves data and opens 2 FIFOs to interact with it.
**Note:** FIFO на сервере следует открывать в `O_RDWR`-режиме (подробнее [здесь](https://stackoverflow.com/questions/14594508/fifo-pipe-is-always-readable-in-select))
```shell
CMD INTERFACE:
    REGISTER <file/tx/path> <fifo/rx/path>
(Cx) --------------------------------------> (S): open(fromclient_tx_fifo_fd, O_RDWR), open(fromclient_rx_fifo_fd, O_RDWR), then:
   <------------------ASK-------------------
   
# 2. Next, any connected client can request a file from the server with the GET <filename> command on its TX, the server is obliged to determine which client did it and send it via the client's RX channel. The list of files is known to the client and server, so you can score statically.

REQUEST-RESPONSE WORKFLOW:
          GET <filname>                       (S):
(Cx) --------------------------------------> |fromclient_tx_fifo_fd|
   <------------------file------------------ |fromclient_rx_fifo_fd|

# A note on the server device: the server monitors the status of a set of incoming descriptors through the select(...) call, determines the descriptor to which the request came, and then starts the file upload procedure in a SEPARATE THREAD. Thus, with a large (within our limitations :)) number of simultaneous requests, the server is non-blocking. For an example of using select(...), see "examples/".

REQUEST-RESPONSE in more detail...:
                                                                    (S):
(C1)                                         |fromclient_tx_fifo_fd1| - \
                                             |fromclient_rx_fifo_fd1|.   \
...               request is here!                                        \
(Cx) ---------------**************---------> |fromclient_tx_fifo_fdx| -  select() --> FD_ISSET(x, &read_descr_set)==TRUE --> then-->| 
   <------------------file------------------ |fromclient_rx_fifo_fdx|. <------------------------create_thread,get_file,transmit file|
                                                .....................    /
(Cn)                                         |fromclient_tx_fifo_fdn|   /
```

(\*) Task with an asterisk (potential "plus point"): try to "reuse" threads for new requests/use a "pool" of pre-prepared threads.
**Note:** _According to this point, I will ask hard. In addition, I can refer to [code](https://github.com/Pithikos/C-Thread-Pool ) (of course, when handing over, I can be interested in the internal device. And I can't guarantee that it's working everywhere...)_
