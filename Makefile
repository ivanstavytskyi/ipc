BINFILES = zadanie proc_p1 proc_p2 proc_t proc_d proc_serv2
OBJFILES = *.out *.err serv2.txt

all: $(BINFILES)

zadanie: zadanie.cpp
	g++ zadanie.cpp -o zadanie

proc_p1: proc_p1.cpp
	g++ proc_p1.cpp -o proc_p1

proc_p2: proc_p2.cpp
	g++ proc_p2.cpp -o proc_p2

proc_t: proc_t.cpp
	g++ proc_t.cpp -o proc_t

proc_d: proc_d.cpp
	g++ proc_d.cpp -o proc_d

proc_serv2: proc_serv2.cpp
	g++ proc_serv2.cpp -o proc_serv2

clean:
	rm -f $(OBJFILES) $(BINFILES)
