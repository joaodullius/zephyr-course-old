K_FIFO_DEFINE(my_fifo);

static int value = 0;

static struct data_item_t {
    void *fifo_reserved;   /* 1st word reserved for use by FIFO */
    int  value ;
};

void produce_data(void) {
    struct data_item_t tx_buf;
    tx_buf.timestamp=timestamp++;
    k_fifo_put(&temp_fifo, &tx_buf);
}

void consume_data(void){

    struct data_item_t *rx_buf;
    rx_buf = k_fifo_get(&temp_fifo, K_FOREVER);
    printf("Received %d\n", rx_buf->timestamp);
}
