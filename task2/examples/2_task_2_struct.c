/* You need something like this */

typedef struct pPipe Pipe;

typedef struct op_table Ops;

typedef struct op_table  {
    size_t (*rcv)(Pipe *self); 
    size_t (*snd)(Pipe *self); 
} Ops;

typedef struct pPipe {
        char* data; // intermediate buffer
        int fd_rx[2]; // array of RX descriptors for "pipe()" call
        int fd_tx[2]; // array of TX descriptors for "pipe()" call
        size_t len; // data length in intermediate buffer
        Ops actions;
} Pipe;
