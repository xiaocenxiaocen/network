include config.h

all: target

target: client server test thread_pool_test

client: client.o rio.o task.o util.o
	$(LINK) -o $@ $+ $(LDFLAGS) $(LIBRARIES)

server: server.o rio.o task.o util.o
	$(LINK) -o $@ $+ $(LDFLAGS) $(LIBRARIES)

test: test.o
	$(LINK) -o $@ $+ $(LDFLAGS) $(LIBRARIES)

thread_pool_test: thread_pool.o
	$(CXX) -o $@ $+ $(LDFLAGS) $(LIBRARIES)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

.c.o:
	$(CC) -c $(CXXFLAGS) $<

%.o: %.cu
	$(NVCC) -c $(INCLUDES) $(CUDA_FLAGS) -o $@ -c $<

.PHONY: clean
clean:
	-rm -f *.o
	-rm -f server
	-rm -f client
	-rm -f test
