
This demonstrates a minimal implementation of heap memory management using the First-Fit strategy. In this simulation, a total heap memory of 1KB is considered. The implementation has the following capabilities:



1. It finds the free block that can accommodate the required size.

2. It splits the free space if the requested size is smaller than the available free space.

3. It merges contiguous free space to reduce fragmentation.

