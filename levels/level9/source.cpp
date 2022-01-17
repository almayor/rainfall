#include <string.h>
#include <unistd.h>

class N {
public:
	typedef  int (N::*MemFun)(const N&) const;

	MemFun	func; 				// from 0-4 bytes
	char	annotation[100];	// from 4-104 bytes
	int 	var;				// from 104-108 bytes

public:
	N(int var) : var(var) {
		func = &N::operator+;
	}

	void *setAnnotation(const char* annotation) {
		size_t len = strlen(annotation);
		return memcpy(this->annotation, annotation, len);
	}

	virtual int operator+(const N& other) const {
		return var + other.var;
	}

	virtual int operator-(const N& other) const {
		return var - other.var;
	}
};

int main(int argc, char** argv, char** envp) {
	if (argc > 1) {
		auto n1 = new N(5);
		auto n2 = new N(6);

    	n1->setAnnotation(argv[1]);
    	return (n2->*(n2->func))(*n1);
	}
	_exit(1);
}
