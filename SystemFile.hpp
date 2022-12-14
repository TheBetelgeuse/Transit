#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#include <iostream>
#include <cstdlib>

using namespace std;


class SystemFile {
// системная структура для информации о пользователе
  struct passwd *result, *res_name;
// системная структура для информации о файле
  struct stat sb;
// файловый дескрипор
  int id;
// для прав доступа к файлу
  char mod;
// для списка всех жестких сылок на файл
  string *filenames;
  long inode;
  int hlinks;
  
public:
// Конструктор. Вызывается всегда при создании объекта
  SystemFile();

// Деструктор вызывается всегда, когда должен быть
// удален объект: при завершении функции, в которой он локальный
// или при вызове delete
  ~SystemFile();

// закрытие файла
  bool closef();

// открытие файла по его имени в каталоге
  unsigned char openf(const char*);
// получить имя владельца файла

  string getUser();
// получить имя владельца файла (в байтах)
  long getSize();

// Добавть имя жесткой ссылке, если это жесткая ссылка
  int addHardLink(string  pathname);

// напечатаь все жесткие ссылки
  void showAllHardLinks();

// запись в файл. Передается уаказатель
// на память для записи и размер (в байтах)
  bool writef(const void*, long);

// Чтение из файла. В память по указателю
// указаного размера. Память предварительно
// должна быть ВЫДЕЛЕНА
  bool readf(void*, long);

// печать общей информации о файле
  void about();

// получить количество записей в файле,
// если известен размер записи
  long numbersOfRecord(long);

// найти f1 и заменить запись на f2 (полное соответствие)
// только для бинарного файла
  int replace(const void* f1, const void* f2, size_t sizeRec);

// найти f1 и удалить (полное соответствие)
// только для бинарного файла
// для удаления использовать функцию truncate
// или ftruncate
  int deletef(const void* f1,  size_t sizeRec);
};

class Catalog{
// указатель на системную файловую
// структуру для записей в каталоге (список файлов)
  struct dirent* dirrec;
// дескриптор каталога
  DIR *dir;
  int sdir;
  string dirname;
  unsigned int numbersf;
// массив имен файлов
  string* names;
  long long sized;

public:

// конструктор
  Catalog();

// деструктор
  ~Catalog();

// открыть каталог
  unsigned char dopen(const char*);

// получить имя файла из списка (по номеру)
  string getFile(int);

  SystemFile* openFile(string);

// закрыть каталог
  bool dclose();

// получить количество файлов.
  int getNumbers();

// напечатать все имена файлов из этого каталога
  void list();
};