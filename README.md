# Грузовики

В данном проекте полная реализация задачи о перевозке железной руды на Урале.
Репозиторий будет постоянно обновляться.

## Общая информация

### Разделение задания на подзадачи

1. Реализация SystemFile;
2. Реализация программы, исполняющей роль одного самосвала (далее - Самосвал);
3. Реализация программы, исполняющей роль регулировщика с одной стороны моста (далее - Регулироввщик);
4. Реализация программы, отвечающей за запуск потоков регулировщиков и самосвалов (далее - Инициализатор).

### Используемые инструменты

1. Очереди сообщений для сообщения регулировщикам о прибытии самосвала;
2. Семафоры для передачи самосвалам разрешения на проезд и сигнала о завершении работы.

### Возможные события

1. У моста нет самосвалов;
2. С одной стороны моста нет самосвалов;
3. Самосвалы есть по обе стороны моста;
4. Прибыл самосвал недопустимого веса.

### Упрощенная задача

1. Все самосвалы одного веса;
2. Регулировщик допускает на мосту наличие только одного самосвала.

### Распределение между участниками группы

1. Шоислом;
2. Вальтер;
3. Глеб;
4. Аян.

## Межпроцессное взаимодействие

### 1. Введение в методы взаимодействия в данной задаче
1. #### Очереди сообщений
    1. Самосвал - Регулировщик:
        * Самосвал посылает сообщение Регулировщику, расположенному с ним по одну сторону моста, после подъезда к мосту, а Регулировщик принимает его.
2. #### Семафоры
    1. Самосвал - Регулировщик:
        1. Семафор очереди:
            * Самосвал посылает уведомление Регулировщику о проезде по мосту;
    2. Регулировщик - Регулировщик:
        1. Операционный семафор (далее - семафор регулировщиков):
            * Применяется для определения того, какой Регулировщик руководит движением на мосту в данный момент.
    3. Общие:
        1. Завершающий семафор:
            * Процессы завершают работу, если установлено значение 0.
        2. Семафор количества участников:
            * значение семафора №0 соответствует количеству запущенных процессов;
            * значение семафора №1 соответствует количеству активных регулировщиков.

### 2. Порядок взаимодействия с "соседними" процессами

_Далее под сообщением подразумевается `std::pair<int, int>`, так как именно такой тип использует класс `MessageQueue`,
объявленный в `sem_and_queue.hpp`, для приёма и передачи сообщений_

1. #### Самосвал - Регулировщик
    1. После подъезда к мосту Самосвал посылает соответствующему Регулировщику сообщение типа `1` по форме `{"Номер
       Самосвала", "Масса Самосвала"}`, а затем ждёт разрешение на проезд от Регулировщика;
    2. Если Регулировщик разрешает Самосвалу движение по мосту, то посылает сообщение типа `"Номер Самосвала" + 2` по
       форме `{0, 0};
    3. После выезда с моста Самосвал уменьшает значение Семафора очереди на 1 (гарантируется, что данная операция может
       быть выполнена);

2. #### Регулировщик - Регулировщик
    1. Для передачи управления движением Регулировщик меняет значение семафора на номер другого регулировщика, а затем ждёт, пока семафор не примет значение, соответствующее его номеру.

### 3. Правила создания ключей семафоров и очередей сообщений
1. Для получения ключа используем ```ftok(const char *pathname, int proj_id)```;
2. ```pathname = "transit"```
3. `proj_id`
    1. `proj_id = 1, 2` для очередей сообщений регулировщиков `mine` и `factory` соответсвенно;
    2. `proj_id = 3, 4` для семафоров регулировщиков `mine` и `factory` соответственно;
    3. `proj_id = 5` для семафора регулировщиков;
    4. `prog_id = 6` для семафора завершения;
    5. `proj_id = 7` для семафора количества участников (единственный семафор, в массиве которого содержутся 2 семафора).

# Требования к реализации программ (задач)
### 1. Общие требования
1. Использовать `enum Location` из `initialization.hpp` для определения положения объектов.
2. Логировать существенные действия самосвалов / регулировщиков (каждое изменение семафоров и т.п. логировать не нужно).

### 2. Аргументы конструкторов
1. Самосвал: `Truck(bool init_location, int number, int weight, int speed)`;
2. Регулировщик: `TrafficController(bool location, int max_weight, int max_number_of_trucks, int bridge_lenth)`.

### 3. Создание и удаление семафоров / очередей сообщений
1. Очереди сообщений регулировщиков и семафоры очереди создает первый обратившийся участник, а удаляют только
   соответствующие регулировщики;
2. Семафор регулировщиков создаёт первый прибывший регулировщик, а удаляет последний отключившийся;
3. Семафор завершения создаётся первым прибывшим участником, а удаляется последним отключившимся участником;
4. Семафор количества участников создаётся первым прибывшим регулировщиком, а удаляется последним отключившимся участником.

### 4. Названия файлов
1. Инициализатор: реализация в файле `initialization.cpp`, логирование в файл `LogInitializator.txt`;
2. Самосвал: объявление класса `Truck` в файле `truck.hpp`, реализация - в файле, `truck.cpp`, логирование в файл
   `LogTruck"номер самосвала".txt`;
3. Регулировщик: объявление класса `TrafficController` в файле `traffic_controller.hpp`, реализация - в файле
   `traffic_controller.cpp`, логирование в файл `LogTrafficController"расположение регулировщика".txt`;
4. SystemFile: объявление класса `SystemFile` в файле `SystemFile.hpp`, реализация - в файле `SystemFile.cpp`.

### 5. Запуск
1. Инициализатор:
    1. Создаёт два объекта класса ```TrafficController``` с различными параметрами (расположение относительно моста,
       максимальная допустимая нагрузка на мост, количество самосвалов, которых нужно пропустить),
       а затем вызывает метод `StartProcess` у каждого из объектов;
    2. Создаёт необходимое количество объектов класса `Truck` с различными параметрами (изначальное расположение самосвала относительно моста,
       номер самосвала (0-индексация)), а затем вызывает `StartProcess` у каждого из объектов.
2. Самосвал (на усмотрение Вальтера):
    1. Создаёт инструменты межпроцессорного взаимодействия;
    2. Добавляет единицу к значению семафора количества участников (0 в массиве).
3. Регулировщик:
    1. Создаёт инструменты межпроцессорного взаимодействия;
    2. Добавляет единицу к значению семафора количества участников (0 и 1 (если он не является владельцем семафора) в массиве).

### 6. Завершение
1. Инициализатор: ничего не делает;
2. Самосвал:
    1. Отнимает единицу от значения семафора количества участников (0 в массиве);
    2. (правила создания и удаления семафоров / очередей сообщений).
3. Регулировщик:
    1. Отнимает единицу от значения семафора количества участников (0 и 1 в массиве);
    2. Устанавливает значение семафора завершение равным 0;
    3. (правила создания и удаления семафоров / очередей сообщений).

# Критерий завершение работы
1. Самосвал завершает работу, если значение завершающего семафора == 0;
2. Регулировщик завершает работу, если по мосту проехало заданное количество самосвалов, либо если значение завершающего
   семафора == 0, либо прибыл самосвал недопустимого веса. При завершении работы устанавливает значение завершающего семафора = 0 (если оно уже не было
   установлено);
3. Инициализатор завершается сразу после запуска всех процессов.

# Ввод
1. Используем стандартный ввод;
2. В первой строчки параметры моста: максимальная нагрузка и длина;
3. Во второй строчке количество самосвалов, которые могут проехать через мост;
4. В третьей строчке единственное число `n` - количество запускаемых самосвалов;
5. Далее `n` строчек, содержащих параметры запускаемых самосвалов: изначальное расположение относительно моста, масса,
скорость и длина моста.