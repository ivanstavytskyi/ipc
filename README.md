# IPC Project.

> [!NOTE]
> This project is open-source. It represents most commonly applicapable process-communications concepts, including: 
> - semaphores
> - virtual memory
> - socket usage
> - tcp connection
> - udp connection
> - pipes
> - signals.

<br>

<img width="743" height="258" alt="image" src="https://github.com/user-attachments/assets/5435ada9-bc2c-4f78-88fe-4459d16b8e53" />


This project obtains different process related files (some binary, cpp files). The purpose of each file, is to perform it's own specific task in this chain of processes communication.

## Documentation

> At first we have two files p1.txt and p2.txt. That is the main content which will be transfered across the programm, throughout execution. The main question is how it will be transfered ?

For the transfering of content and synchronization between transfering, is responsible the file ```zadanie.cpp```.

Inside of it, is written tha main logic, related to the communication between processes.

## The transfer chain

At first point, the main process ```zadanie``` executed, as mentioned earlier its responsible for execution and synsynchronization between processes.

The first it performs, is executes p1 and p2 process, as well as Pr (precompiled process).

Processes p1, and p2, executed with the same argument of pipe, where they plan to further write. The both of them as well needs a little to prepare before it can roughly recieve the signal from Pr process.

Each process, p1 and p2, opens its related file ```p1.txt``` or ```p2.txt```. As well sets the private process handler of the signal, to handle signal ```SIGUSR1```, when it will come (further from Pr process). Then when the process finish its preparation, it send the signal to the parent process ```zadanie``` by that designating it state as prepared.

Process Pr gives the commands to the programs p1 and p2, with signals, when to read one word only, from the file and write it to the pipe.

For those who are interested in more detailed communication between processes, I recommend to read the ```documentation.docx``` which includes the detailed scheme of communication.

TL;DR

Then process T executes by ```zadanie```, and going to read words from the pipe betwenn Pr and T process, Pr transfers word by word to this pipe to process T.

Process T using semaphores, to synchronize with next ongoing process execution, process S ( precompiled binary).

So when process T write something to the shared memory which is between T and process S, it changes his value to 0, and makes value of semaphore S to the 1.

So process S can read from shared memroy this word (after process S read, this word, it changes semaphore conversely,
and then process T clears the shared memory, and writes new word and so on).

Process S (precompiled process) connected with process D, using shared memory to transfer data between, and semaphores to synchronize.
So process S reads word from his shared memory between previous process, and transfers this word to another shared memory of process D, using semaphore.

So when process S write the word to shared memory,
it makes his semaphore to 0, and semaphore of process D comes to 1. Process D reads the word form shared memory and transfers this word to the process ```Serv1``` via TCP connection.

Process Serv1 then when recieve this
word from tcp connection, from process D, transfers this word connecting to udp port, to Serv2 process, and then when process Serv2 recieves this word by udp connection, it writes this word
to the file Serv2.txt.

And so on, the words are keep transfering between the chain of communcation, until each words are transfered to the file serv2.txt.
