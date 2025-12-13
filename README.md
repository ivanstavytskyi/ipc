This task represents the usage os semaphores, virtual memory, socket usage, tcp connection, udp connection, pipes, signals.
It contains different .cpp files, each of them corresponding to the specified process, which is runs his own role in this chain of processes.
At first we have two files p1.txt and p2.txt, we are executing all the process in chain, with main process zadanie, it first raises p1 process and p2 process, also process
Pr (precompiled process) executes. Process Pr gives the commands to the programs p1 and p2, with signals, when to read one word only from the file and write it to the pipe, which is connected to the Pr process,
and then going to put this word into the pipe, between p1 and process Pr, and p2 and process Pr. Pr process is the pre compiled program, which is only contains the binary code. Then
Then process T executes, and going to read words from the pipe betwenn Pr and T process, Pr transfers word by word to this pipe ro process T. Process T using semaphores,
to synchronize with next process which is going to execute, process S (also precompiled binary file), so when process T write something to the shared memory which is between T and process S,
it changes his value to 0, and makes value of semaphore S to the 1, so S process can read from shared memroy this word (after process S read, this word, it changes semaphore conversely,
and then process T clears the shared memory, and writes new word and so on). Process S (precompiled process) connected with process D, using shared memory to transfer data between, and semaphores to synchronize,
so process S reads word from his shared memory between previous process, and transfers this word to another shared memory of other process, D, using semaphore, so when it write the word to shared memory,
it makes his semaphore to 0, and semaphore of process D to 1. Process D reads the word form shared memory and transfers this word to the process which have executed,
process Serv1 (precompiled process) using TCP connection with the process Serv1, and then sets his semaphore to 0 and semaphore of process S to 1. Process Serv1 then when recieve this
word from tcp connection, from process D, transfers this word connecting to udp port, to Serv2 process, and then when process Serv2 recieves this word by udp connection, it writes this word
to the file Serv2.txt.
