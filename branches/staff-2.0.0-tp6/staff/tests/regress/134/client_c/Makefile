all:
	$(CC) echo_blocking.c -I /usr/local/axis2c/include/axis2-* -L/usr/local/axis2c/lib -laxutil -laxis2_engine -otest_client


clean:
	rm -f test_client

test: all
	./test_client