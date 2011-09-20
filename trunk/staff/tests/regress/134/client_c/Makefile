all:
	$(CC) echo_blocking.c -I/usr/local/axis2c/include/axis2-1.6.0 -L/usr/local/axis2c/lib -laxutil -laxis2_engine -otest_client


clean:
	rm test_client

test: all
	./test_client