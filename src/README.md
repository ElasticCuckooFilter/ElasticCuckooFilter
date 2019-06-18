## Source Codes

This directory contains necessary source codes for our paper. The source codes include the code of implementation of CF, ECF and DCF, and our test codes for experiments.  
CF: Cuckoo Filter  
DCF: Dynamic Cuckoo Filter  
ECF: Elastic Cuckoo Filter  

### Experiments
- `testMaxInsertNumber`: estimate the maximum number of items that can be inserted into filters.
- `testInsertThp`: estimate insertion throughput.
- `testLookupThp`: estimate query throughput.
- `testLoadRatio`: estimate load ratio.
- `testResizeLookupThp`: estimate query throughput before and after shrinkage or extension.
- `testMemoryAccess`: estimate hop number(kicks) and memory access(memory write and memory read).
- `testExtensionFPRate`: estimate false positive rate before and after extension.
- `testShrinkageFPRate`: estimate false positive rate before and after shrinkage.

### Usage
- To run the test, `make` and `./test`.
- To choose different datasets, modify the parameter of function `readDataset`. Notice that you need to build and execute the `syn_dat_gen` program in the `../dat/syn/` directory.
- The outcome of the test will be outputted into the file `out.txt`.
