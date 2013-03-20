all:
	$(CC) echo_blocking.c -I $(AXIS2C_HOME)/include/axis2-* -L$(AXIS2C_HOME)/lib -laxutil -laxis2_engine -otest_client


clean:
	rm -f test_client

test: all
	./test_client