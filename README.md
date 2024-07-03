# Part 1: Advanced Synchronization of File Access with Naive Methods

## Objective
This program demonstrates naive file access synchronization between a parent process and two child processes using methods like wait or sleep. It aims to prevent interleaved writes to a shared file.

## Instructions
The program forks two child processes and the parent process, each writing specified messages to a file (`output.txt`) a specified number of times. Naive synchronization ensures that writes do not overlap using simple timing mechanisms.

## Requirements
- Create `output.txt`.
- Ensure processes write their respective messages sequentially using naive synchronization (e.g., sleep).
- Use command-line arguments to specify messages and the count of writes.

## Example Usage
```bash
./part1 "Parent message\n" "Child1 message\n" "Child2 message\n" 3
```

# Part 2: Assignment: Implementing a Synchronization Lock for File Access

## Objective
Similar to Part 1, this program synchronizes file access between parent and child processes using naive methods. It emphasizes proper synchronization without using locks or complex synchronization mechanisms.

## Instructions
The program forks two child processes and the parent process, each writing specified messages to `output.txt` while preventing interleaved writes using naive synchronization methods.

## Requirements
- Create `output.txt`.
- Implement naive synchronization to prevent write interleaving.
- Accept command-line arguments for messages and write counts.

## Example Usage
```bash
./part2 "Parent message\n" "Child1 message\n" "Child2 message\n" 3
```

Part 3: Buffered File I/O with O_PREAPPEND Flag
Objective:
This program extends the previous parts by synchronizing file access between parent and child processes using naive methods such as wait or sleep. It explores varied writing patterns and ensures proper file synchronization without complex locking mechanisms.

Instructions:
Fork two child processes and the parent process, with each writing specific messages to output.txt. Naive synchronization methods should prevent write interleaving, maintaining order based on process execution.

Requirements:

Create output.txt.
Use naive synchronization methods to prevent file write interleaving.
Accept command-line arguments for message content and write counts.
Example Usage:

```bash
./part3 "Parent message\n" "Child1 message\n" "Child2 message\n" 3
```
Part 4: Assignment: Implementing a Directory Copy Library in C
Objective:
This program continues exploring file access synchronization between parent and child processes using naive methods like wait or sleep. It focuses on handling varied writing patterns and maintaining file integrity without advanced synchronization tools.

Instructions:
Fork two child processes and the parent process, each writing designated messages to output.txt. Naive synchronization ensures that file writes do not interleave, achieving orderly write sequences.

Requirements:

Create output.txt.
Implement naive synchronization to avoid file write overlap.
Use command-line arguments to define message content and write counts.
Example Usage:

```bash
./part4 "Parent message\n" "Child1 message\n" "Child2 message\n" 3
```
