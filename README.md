# Part 1: Advanced Synchronization of File Access with Naive Methods

## Objective
This program demonstrates naive file access synchronization between a parent process and two child processes using methods like wait or sleep. It aims to prevent interleaved writes to a shared file.

## Instructions
The program forks two child processes and the parent process, each writing specified messages to a file (`output.txt`) a specified number of times. Naive synchronization ensures that writes do not overlap using simple timing mechanisms.

## Example Usage
```bash
./part1 "Parent message\n" "Child1 message\n" "Child2 message\n" 3
```

# Part 2: Assignment: Implementing a Synchronization Lock for File Access

## Objective
implement a synchronization lock for file access, ensuring that only one process writes to the file at a time while the others wait. This assignment will involve creating a dynamic number of child processes and using a separate file as a lock to control access.

## Example Usage
```bash
./part2 "Parent message\n" "Child1 message\n" "Child2 message\n" 3
```

# Part 3: Buffered File I/O with O_PREAPPEND Flag

## Objective:
This part extends the project by implementing buffered file I/O with special support for the O_PREAPPEND flag. It involves creating wrappers for standard file operations to achieve buffered reading and writing, ensuring efficient file access and supporting prepending data to files without overriding existing content.

# Part 4: Assignment: Implementing a Directory Copy Library in C

## Objective:
This assignment involves implementing a C library that mimics Python's shutil.copytree. The library allows copying entire directory trees from a source to a destination directory, including handling symbolic links and file permissions based on specified flags.

## Example Usage
```bash
./main_program -l -p source_directory destination_directory
```
