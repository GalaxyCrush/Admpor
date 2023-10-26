## Work within the scope of the Operating Systems subject

## Work carried out by:

- João Pereira @fc58189
- André Reis @fc58192
- Daniel Nunes @fc58257

### The following technologies were used: 

- C
- WSL
- Valgrind
- GDB
- POSIX

This work was done for the theoretical-practical part of Operating Systems with the aim of simulating a port that receives transactions from customers and processes them for companies, passing through intermediaries.

To compile the programme, follow these instructions:
Inside the ADMPOR folder run the following commands in order:

1. make clean(will clean the .o, executable and log and statistics files)
2. make (which will generate the .o and executable files)
3. cd bin
4. ./admpor input.txt (replace the arguments with the desired numbers, which in this case is the file with the inputs).

The project was tested and realised using the WSL system with Ubunto, in the VSCode IDE.

The final grade was 18.3/20.
