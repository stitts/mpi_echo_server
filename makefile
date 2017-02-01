target=pserver

.PHONY: all clean

all: $(target)

$(target): parallel_server.cpp parallel_server.hpp
	mpicxx parallel_server.cpp -o $@

clean:
	rm $(target)
