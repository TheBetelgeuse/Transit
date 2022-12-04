#include "SystemFile.hpp"

template <typename T>
T* renew(T* array, size_t old_size, size_t new_size) {
  
  T* new_array = new T[new_size];

  for (size_t i = 0; i < old_size; ++i) {
    new_array = array;
  }

  delete[] array;
  return new_array;
}

// Конструктор. Инициализируем атрибуты.
SystemFile::SystemFile() {
  result = 0; //указатель на структуру 0
  id = -1; // если файл не открыт, дескриптор -1
  
};

// Деструктор. При удаленни объкта типа SystemFile
// файл должен быть закрыт.
SystemFile::~SystemFile() {
  delete [] filenames;
  if (id > -1){
    close(id);
    id = -1;
  }
};

//функция класса для открытия файла
unsigned char SystemFile::openf(const char* name) {
  unsigned char er;
// Связывание дескриптора id с именем файла
  id = open(name, O_RDWR| O_CREAT |O_EXCL,0775);
  er = errno;
//Проверка открылся ли файл
  if (errno){
    if (errno == EEXIST){
        id = open(name, O_RDWR );
        er = 0;
    } else {
        return er;
    }
  }
// заполнение структуры stat информацией о файле
  fstat(id, &sb);
// получение имени пользователя по uid
  result = getpwuid(sb.st_uid);

// получаем inode файла
  inode =  sb.st_ino;

// в массиве жестких ссылок пока только одна известная
  filenames = new string[sb.st_nlink];
  filenames[0] = name;
  hlinks = 1;

// возвращаем код ошибки
  return er;
};

// Печатаем все известные жесткие ссылки
void SystemFile::showAllHardLinks(){
  fstat(id, &sb);
  for (int i = 0; i < hlinks; i++) {
    std::cout << filenames[i] << std:: endl;
  }
};

// Проверка является ли этот файл жесткой ссылкой,
// и, если является, добавляем его в массив
int SystemFile::addHardLink(string  pathname){
  int hlink = sb.st_nlink;
  fstat(id, &sb);

  if ( sb.st_nlink < 2)
    return 0;

  if ( hlink < sb.st_nlink ) {
    string *newFname = new string [sb.st_nlink];
    size_t size = sizeof (string ) * hlink;
    memmove( newFname, filenames, size);
    delete[] filenames;
    filenames = newFname;
  }

  struct stat sbC;
  fstat(id, &sbC);

  if (inode == sbC.st_ino){
    filenames[hlinks] = pathname;
    hlinks++;
  }
  
  return 1;
};

// запись в файл всего что есть в буфере (побайтно)
bool SystemFile::writef(const void* buf, long size) {
  long skolko = 0;
// системный вызов
  skolko = write(id, buf, size);
  if (skolko == size)
    return 1;
  else
    return 0;
};

bool SystemFile::readf(void* buf, long size) {
  long skolko = 0;
  skolko = read(id, buf, size);
  if (skolko == size) {
    return 1;
  }
  return 0;
}

// закрыть файл
bool SystemFile::closef(){
  if (id >- 1){
    close(id);
    id = -1;
    return 1;
  }
  return 0;
};

// печать информации о файле
void SystemFile::about(){
  printf("user: %s\n",result -> pw_name);
  switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:  printf("block device\n");
      break;
    case S_IFCHR:  printf("character device\n");
      break;
    case S_IFDIR:  printf("directory\n");
      break;
    case S_IFIFO:  printf("FIFO/pipe\n");
      break;
    case S_IFLNK:  printf("symlink\n");
      break;
    case S_IFREG:  printf("regular file\n");
      break;
    case S_IFSOCK: printf("socket\n");
      break;
    default:       printf("unknown?\n");
   }

   printf("I-node number:  %ld\n", (long) sb.st_ino);
   printf("Mode:           %lo (octal)\n",(unsigned long) sb.st_mode);
   printf("Link count:    %ld\n", (long) sb.st_nlink);
   printf("Ownership:  UID=%ld   GID=%ld\n",(long) sb.st_uid, (long) sb.st_gid);
   printf("File size:  %lld bytes\n",(long long) sb.st_size);
   printf("Last status change:  %s", ctime(&sb.st_ctime));
   printf("Last file access: %s", ctime(&sb.st_atime));
   printf("Last file modification:   %s",ctime(&sb.st_mtime));
}

long SystemFile::getSize() {
  return sb.st_size;
}

long SystemFile::numbersOfRecord(long size_of_record) {
  return getSize() / size_of_record;
}

int SystemFile::replace(const void* f1, const void* f2, size_t sizeRec) {
  size_t index = 0;
  void* readed = malloc(sizeRec);
  for (; index < getSize() - sizeRec + 1; ++index) {
    lseek(id, index, SEEK_SET);

    if (!readf(readed, sizeRec))  {
      free(readed);
      return 0;
    }

    if (strcmp((char*) readed, (char*) f1) == 0) {
      break;
    }

  }

  if (index == getSize() - sizeRec + 1) {
    free(readed);
    return 0;
  }

  lseek(id, index, SEEK_SET);
  free(readed);
  return writef(f2, sizeRec);
}

int SystemFile::deletef(const void* f1,  size_t sizeRec) {
  size_t index = 0;
  void* readed = malloc(sizeRec);
  for (; index < getSize() - sizeRec + 1; ++index) {
    lseek(id, index, SEEK_SET);

    if (!readf(readed, sizeRec))  {
      free(readed);
      return 0;
    }

    if (strcmp((char*) readed, (char*) f1) == 0) {
      break;
    }
  }

  if (index == getSize() - sizeRec + 1) {
    free(readed);
    return 0;
  }

  if (!readf(readed, getSize() - index - sizeRec)) {
    free(readed);
    return 0;
  }

  ftruncate(id, index);
  lseek(id, 0, SEEK_END);

  if (!writef(readed, getSize() - index - sizeRec)) {
    free(readed);
    return 0;
  }

  free(readed);
  fstat(id, &sb);
  return 1;
};

Catalog::Catalog() {
  dirrec = nullptr;
  dir = NULL;
  names = nullptr;
  sdir = 0;
  numbersf = 0;
  sized = 0;
}

Catalog::~Catalog() {
  dclose();
  if (names != nullptr) {
    delete[] names;
  }
}

unsigned char Catalog::dopen(const char* dirname) {
  this->dirname = dirname;
  dir = opendir(dirname);
  sdir = errno;
  if(dir == NULL) {
    return sdir;
  }

  numbersf = 0;
  while ((dirrec = readdir(dir)) != NULL) {

    if (sized <= numbersf) {
      sized += 10;
      names = renew(names, numbersf, sized);
    }

    names[numbersf] = dirrec->d_name;
    if (names[numbersf] == "." || names[numbersf] == "..") {
      continue;
    }

    numbersf += 1;
  }

  return sdir;
}

string Catalog::getFile(int file_index) {
  if (file_index >= numbersf) {
    exit(2);
  }
  return names[file_index];
}

SystemFile* Catalog::openFile(string file_name) {
  string real_file_path = dirname;
  if (real_file_path[real_file_path.length() - 1] != '/') {
    real_file_path += '/';
  }
  real_file_path += file_name;
  SystemFile* file = new SystemFile;
  file->openf(real_file_path.c_str());
  return file;
}

bool Catalog::dclose() {
  if (dir == NULL) {
    return 0;
  }
  closedir(dir);
  numbersf = 0;
  return 1;
}

int Catalog::getNumbers() {
  return numbersf;
}

void Catalog::list() {
  for (int i = 0; i < numbersf; ++i) {
    std::cout << names[i] << std::endl;
  }
}
