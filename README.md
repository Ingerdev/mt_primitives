# mt_primitives
Версия на прмитивах синхронизации (мутексы).

Архитектура: один поток читает построчно (getline()) из файла, записывает в очередь строки, из которой
рабочие потоки их разбирают. Потоки обрабатывают строки и пишут в контейнер, который разбирает поток вывода в файл.

Недостатки: 
- считает, что в одной строке может быть только одно вхождение паттерна.
- практически не отлажен, вследствие чего допускает реордеринг на выходе.
- сама система ущербна, т.к. приходится вручную подбирать соотношение рабочих потоков к потокам парсинга и записи файлов, иначе множество рабочих потоков просто не пустят
  файловые потоки к очередям.
- нет ограничения на размер входной строки, в результате нет контроля над потребляемой памятью.

- работа только с файлами, нет вывода в консоль.
- нет тестов.
- код совершенно не причесан, т.к. брошен на полдороге.

  
