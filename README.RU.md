[![EN](https://user-images.githubusercontent.com/9499881/33184537-7be87e86-d096-11e7-89bb-f3286f752bc6.png)](https://github.com/r57zone/SNES-USB-Controller-XInput/) 
[![RU](https://user-images.githubusercontent.com/9499881/27683795-5b0fbac6-5cd8-11e7-929c-057833e01fb1.png)](https://github.com/r57zone/SNES-USB-Controller-XInput/blob/master/README.RU.md)

# SNES USB Controller XInput
Xbox эмулятор для [SNES USB контроллера](http://alii.pub/6hw4yd). Работает на базе драйвера [ViGEm](https://github.com/ViGEm).

![](https://user-images.githubusercontent.com/9499881/197362614-fee74a6d-0f46-4421-95ff-56c5b3ab57e4.png)

# Режимы работы
Переключение эмуляции со стика на DPAD и обратно происходит по нажатию на `Select + DPAD ←` или `ALT + A`.


Переключение эмуляции с триггеров на бамперы и обратно происходит по нажатию на `Select + DPAD ↑` или `ALT + Q`.


Режимы по умолчанию можно настроить в конфигурационном файле `Config.ini`.

## Настройка (приложение)
1. Установить [ViGEmBus](https://github.com/ViGEm/ViGEmBus/releases).
2. Установить Microsoft Visual C++ Redistributable 2017 или новее.
3. Подключить контроллер.
4. Запустить программу.



Вместо приложения можно также использовать библиотеки для XInputInjector-а:

1. [Загрузить XInput Injector](https://github.com/r57zone/X360Advance/releases/) и распаковать.
2. Распаковать в папку `Libraries` содержимое архива `SNES.USB.Controller.XInput.Libraries.zip`.
3. Запустить XInput Injector и выбрать нужное приложение.

## Загрузка
>Версия для Windows 10.

**[Загрузить](https://github.com/r57zone/SNES-USB-Controller-XInput/releases)**

## Благодарности
* [ViGEm](https://github.com/ViGEm) за возможность эмуляции разных геймпадов.
* [Библиотеке MinHook](https://github.com/TsudaKageyu/minhook) за готовое решение для подмены функций.

## Обратная связь
`r57zone[собака]gmail.com`