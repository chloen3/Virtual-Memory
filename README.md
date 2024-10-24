# Virtual-Memory
Virtual memory simulator with page replacement algorithms.

Description:
This project is a simulation of a virtual memory system as part of the Computer Systems and Networks course. The simulator includes implementations of page replacement algorithms, such as Clock Sweep and Approximate LRU, and tracks memory statistics like Average Memory Access Time (AMAT). This project demonstrates key concepts in memory management, including page tables, frame allocation, and handling page faults in a simulated environment.

Project Context:
Modern operating systems use virtual memory to efficiently manage and isolate memory for processes. In this project, you'll simulate a virtual memory system using a 24-bit virtual address space with a 20-bit physical address space. The memory is divided into 16KB pages, and the project involves implementing critical parts of the memory management unit, such as:
  - Splitting a virtual address into its components (VPN and offset).
  - Handling page faults and memory access.
  - Implementing page replacement algorithms (Clock Sweep, Approximate LRU).
  - Calculating and analyzing statistics like AMAT.

Source Files:
  mmu.c - Handles memory operations and access.
  proc.c - Manages process-related operations.
  page_fault.c - Implements the page fault handler.
  page_replacement.c - Contains page replacement algorithms.
  stats.c - Calculates and tracks memory access statistics.
  Makefile - Facilitates compilation and testing of the project. Provides commands for building and running the simulator.

Future Improvements:
  Extend to support additional page replacement algorithms.
  Add visualization for page faults and memory operations.
  Experiment with different page sizes or address configurations.

Author:
Chloe Nicola Fall 2024.


