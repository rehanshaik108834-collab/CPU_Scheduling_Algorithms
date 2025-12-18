CPU Scheduling Algorithms
=========================

This repository contains a unified C++ program implementing several CPU scheduling algorithms and visualizations:

- FCFS (First-Come, First-Served)
- Round Robin (RR) with configurable quantum
- SPN (Shortest Process Next)
- SRT (Shortest Remaining Time)

Features
- Gantt chart visualization with times aligned under the pipes
- Idle time detection and display
- Ready queue timeline (compact view) that shows ready-queue transitions and which process is executing (available for RR and SRT)
- Per-process statistics: waiting time and turnaround time

Build

Requires g++ (C++11). From the project folder run:

```powershell
cd C:\Users\rehan\OneDrive\Documents\CPU-SCHEDULING-ALGORITHMS\CPU_Scheduling_Algorithms
g++ -std=c++11 main.cpp -o scheduler.exe
```

Run

Interactive (recommended):

```powershell
.\scheduler.exe
```

Or feed a test file (PowerShell):

```powershell
Get-Content test.txt | .\scheduler.exe
```

On Linux/macOS:

```bash
./scheduler < test.txt
```

Input format
- First integer: number of processes `n`.
- Next `n` integers: burst times for processes 1..n (order matches process ids)
- Next `n` integers: arrival times for processes 1..n
- Final integer: time quantum (used by Round Robin)

Examples

Inline example (3 processes):
```
3
5 3 2
0 0 1
2
```
This indicates 3 processes with burst times 5,3,2 and arrival times 0,0,1 and RR quantum 2.

Files in this repo
- `main.cpp` — main program
- `scheduler.exe` — compiled binary (if present)
- Several `test_*.txt` files with example inputs

Notes
- The program prints a Gantt chart and a ready-queue timeline (for RR and SRT) showing the queue contents at each scheduling decision and which process is on the CPU.
- The `README.md` is intentionally concise — tell me if you want more details (API, diagrams, more examples).

Quick commands

```powershell
# compile
g++ -std=c++11 main.cpp -o scheduler.exe
# run with test file
Get-Content test_inline.txt | .\scheduler.exe
```

License & Contact
- Add a license as you prefer. For questions or changes, open an issue or contact the repo owner.
