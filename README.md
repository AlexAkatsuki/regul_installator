# Regul Installator

Графический установщик пакетов для Linux с встроенными пакетами. Разработан на Qt5.

## О проекте

**Regul Installator** - это демонстрационное приложение для установки программ/пакетов на Linux системах. Проект создан в рамках тестового задания и демонстрирует:

- Разработку графического интерфейса на Qt5 Widgets
- Встраивание пакетов непосредственно в исполняемый файл
- Автоматическое обнаружение и установку .deb пакетов
- Расширяемую архитектуру для добавления новых пакетов

## Особенности

- **Встроенные пакеты** - все зависимости включены в исполняемый файл
- **Автоматическое обнаружение** - новые пакеты добавляются без изменения кода
- **Реальная установка** - использует системный dpkg для установки

## Встроенные пакеты

- **cURL**
- **Git**
- **Htop**
- **Vim**
- **Wine**

### Требования

- **CMake** 3.16 или выше
- **Qt5** (Widgets, Core)
- **C++17**
- **GCC11** или выше
- **Linux** система Ubuntu22.04 или аналогичная

# Инструкция по сборке

## Клонирование репозитория
git clone https://github.com/yourusername/regul_installator.git
cd regul_installator

## Создание build директории
mkdir build && cd build

## Конфигурация проекта
cmake ..

## Сборка (Debug версия)
make

## Или сборка Release версии
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Запуск
cd ./regul_installator

# Добавление новых пакетов

## Создание структуры пакета
mkdir packages/new_package

cd packages/new_package

## Создание .list файла
packages/new_package/new_package.list:


## Добавление .deb файлов
cp /path/to/package_file.deb packages/new_package/

# Формат .list файлов
### Первая строка - отображаемое имя в интерфейсе
My Awesome Package

### Последующие строки - .deb файлы (относительные пути)
package.deb

dependency.deb

additional-package.deb
