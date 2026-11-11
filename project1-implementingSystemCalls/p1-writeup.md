# Explain the pros and cons of using a FIFO queue (as compared to a hash table) for the global semaphore list.
### Pros
    FIFO Queues are very good for a variety of purposes. It's big pull is it's simplicity and fairness. 
    FIFO Queues require very little overhead, are incredibly simple to implement, and reliably manage processes in a fair manner.
### Cons
    The main drawback of FIFO Queues are their speed. They take O(n) lookup time, 
    compared to other optins like hashing which performs in O(1). This can lead to a bottleneck if the demand gets to high.
    This makes FIFO queues far from ideal for large scaling applications. Their simplicity is thus both a strength and a weakness.