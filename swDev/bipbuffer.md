# bip buffer

## highlight
1. FIFO
2. Data is continuous in each regions
3. No need to allocate, move, copy, free buffer's data, so it has high performance.
4. Reserve Buffer always from the bigger region.
5. Need to read twice to check whether buffer is empty.

## usage
- write buffer
  1. firstly reserveBuffer
   get a subBuffer, which can write data.
  2. then commitBuffer
   make the size of written data
- read buffer
  1. firstly, getBuffer
   get data from a region
  2. then decommitBuffer
   mark the consumed data.
### note
1. need to loop getting data from the bip buffer until the buffer empty.

## ref
[bip buffer](https://www.codeproject.com/articles/3479/the-bip-buffer-the-circular-buffer-with-a-twist)
