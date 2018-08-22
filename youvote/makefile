

build:
	eosiocpp -g youvote.abi youvote.cpp
	eosiocpp -o youvote.wast youvote.cpp

publish:
	cleos set contract vote ../youvote

clean:
	rm *.abi
	rm *.was*

