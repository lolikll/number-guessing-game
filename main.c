#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <wctype.h>

#define MAX_NAME_LENGTH 10 // Максимальная длина имени игрока
#define MAX_PLAYERS 101    // Максимальное количество игроков в таблице лидеров
#define MAX_NUMBER 100     // Максимальное значение случайного числа для игры

#define LEADERBOARD_FILE "leaderboard.txt" // Имя файла таблицы лидеров

// Структура для хранения данных об игроках
struct Players {
  char name[MAX_NAME_LENGTH];
  uint8_t record;
};

// Глобальные переменные для хранения данных о лидерах
struct Players player[MAX_PLAYERS]; // Массив структур игроков
int count = 0;                      // Количество лидеров

// Функция для сравнения записей игроков (используется при сортировке)
int compare_records(const void *a, const void *b)
{
  const struct Players *player1 = (const struct Players *)a;
  const struct Players *player2 = (const struct Players *)b;
  
  return (player1->record - player2->record); // Возвращает разницу рекордов для сортировки
}


// Функция для обработки данных из файла таблицы лидеров
void table_processing(void)
{
  FILE *file = fopen(LEADERBOARD_FILE, "r"); // Открытие файла для чтения
  char line[MAX_PLAYERS];                    // Буфер для считывания строки из файла

  // Чтение данных из файла и сохранение их в массив структур игроков
  while (fgets(line, sizeof(line), file) != NULL && count < MAX_PLAYERS) {
    char name[MAX_PLAYERS];
    uint8_t record;
    if (sscanf(line, "%s %hhu", name, &record) == 2) {
      if (count < MAX_PLAYERS) {
	strcpy(player[count].name, name); // Копирование имени игрока в структуру
	player[count].record = record;    // Сохранение рекорда игрока в структуру
	++count;
      } else {
	printf("Превышен максимальный размер таблицы лидеров\n");
	break;
      }
    }
  }

  fclose(file); // Закрытие файла
  qsort(player, count, sizeof(struct Players), compare_records); // Сортировка массива лидеров
}


// Функция для вывода таблицы лидеров
void print_leaderboard(void)
{
  puts("\nТаблица лидеров:\n");
  puts("┌────────────────────────┬────────┐");
  printf("│ %-25s │ %s │ \n", "Имя", "Рекорд");
  puts("├────────────────────────┼────────┤");

  // Вывод информации о каждом лидере в таблице
  for (int i = 0; i < count; ++i) {
    if (i == 0){
      // Подсветка топ-3 лидеров
      printf("│\033[48;2;255;215;0m %-22s │ %-7hhu\033[0m│\n"
	     , player[i].name
	     , player[i].record);
      continue;
    }
    if (i == 1){
      printf("│\033[48;2;128;128;128m %-22s │ %-7hhu\033[0m│\n"
	     , player[i].name
	     , player[i].record);
      continue;
    }
    if (i == 2){
      printf("│\033[48;2;205;127;50m %-22s │ %-7hhu\033[0m│\n"
	     , player[i].name
	     , player[i].record);
      continue;
    }
    // Вывод информации о лидерах не врошедших в топ-3
    printf("│ %-22s │ %-7hhu│\n", player[i].name, player[i].record);
  }
  puts("└────────────────────────┴────────┘\n");
}


// Функция для вывода логотипа игры
void print_game_logo()
{
  puts("\033[1;95m");
  puts("   ██████╗ ███████╗███╗   ███╗███████╗    ██████╗ ██╗   ██╗███████╗");
  puts("  ██╔════╝ ██╔════╝████╗ ████║██╔════╝    ██╔══██╗██║   ██║██╔════╝");
  puts("  ██║  ███╗█████╗  ██╔████╔██║███████╗    ██║  ██║██║   ██║███████╗");
  puts("  ██║   ██║██╔══╝  ██║╚██╔╝██║╚════██║    ██║  ██║██║   ██║╚════██║");
  puts("  ╚██████╔╝███████╗██║ ╚═╝ ██║███████║    ██████╔╝╚██████╔╝███████║");
  puts("   ╚═════╝ ╚══════╝╚═╝     ╚═╝╚══════╝    ╚═════╝  ╚═════╝ ╚══════╝");
  puts("\033[0m");
}


// Функция для генерации случайного числа
uint8_t generate_random_number(void)
{
  uint8_t random_number;
  
  srand(time(NULL));
  random_number = rand() % MAX_NUMBER + 1;

  return random_number;
}


// Функция для получения числа, введенного пользователем
uint8_t get_user_input(void)
{
  uint8_t guessed_number;
  
 input:
  printf("Введите число: ");
  scanf("%hhu", &guessed_number);

  // Проверка на дурака
  if (guessed_number < 1 || guessed_number > MAX_NUMBER){
    puts("\033[1;91mОшибка: введен некорректный символ!\033[0m");
    puts("Введите число от 1 до 100!\n");
    while (getchar() != '\n'); // Очистка потока ввода
    goto input;
  }
  
  return guessed_number;
}


// Функция для обновления таблицы лидеров с учетом нового рекорда игрока
void update_leaderboard(const char *name, uint8_t record) {
  FILE *file = fopen(LEADERBOARD_FILE, "r+"); // Открытие файла для чтения и записи
  if (file == NULL) {
    printf("Ошибка открытия файла leaderboard.txt для чтения и записи\n");
    return;
  }
  
  char line[MAX_PLAYERS];    // Буфер для считывания строки из файла
  uint8_t stored_record = 0; // Переменная для хранения рекорда из файла
  
  int user_found = 0; // Флаг указывающий был ли найден игрок в таблице

  while (fgets(line, sizeof(line), file) != NULL) { // Поиск игрока в таблице лидеров
    char stored_name[MAX_NAME_LENGTH];              // Буфер для хранения имени из файла

    // Считывание имени и рекорда из строки файла
    if (sscanf(line, "%s %hhu", stored_name, &stored_record) == 2) {
      // Проверка, совпадает ли имя из файла с именем текущего игрока
      if (strcmp(name, stored_name) == 0) {
        user_found = 1; // Установка флага, что игрок найден
	// Проверка, побит ли текущий рекорд
        if (record >= stored_record) {
          printf("Ваш рекорд не побит. Лучший рекорд: %hhu\n", stored_record);
          fclose(file); // Закрытие файла
          return;
        }
	
        break;
      }
    }
  }

  puts("\033[48;2;0;255;0m\t\t\t  НОВЫЙ РЕКОРД!!!   \t\t\t\033[0m");

  // Установка указателя файла на конец если игрок новый, иначе перед его записью
  if (!user_found) {
    fseek(file, 0, SEEK_END);
  } else {
    fseek(file, -strlen(line), SEEK_CUR);
  }

  // Запись нового рекорда игрока в файл
  fprintf(file, "%s %hhu\n", name, record);
  
  fclose(file); // Закрытие файла

  // Обновление данных о лидерах
  memset(player, 0, sizeof(player)); // Обнуление массива структур игроков
  count = 0;                         // Обнуление счетчика лидеров
  
  table_processing();
}


// Функция для вывода меню игры и обработки выбора пользователя
void game_menu(void)
{
 menu:
  uint8_t user_choice; // Переменная для выбора пользователя
  
  puts("\t\tМеню");
  printf("\t1. Играть\n\t2. Таблица лидеров\n\t3. Выйти\nВыбирите действие: ");

  scanf("%hhu", &user_choice);

  // Проверка на корректность выбора
  if (user_choice < 1 || user_choice > 3){
    puts("\033[1;91mОшибка: введен некорректный символ!\033[0m");
    puts("Введите число от 1 до 3!\n");

    while (getchar() != '\n'); // Очистка потока ввода
    goto menu;
  }

  // Обработка выбора пользователя
  if (user_choice == 1) return; // Возврат к игре
  if (user_choice == 2){
    print_leaderboard();

    while (getchar() != '\n'); // Очистка потока ввода
    goto menu;
  }
  else exit(EXIT_SUCCESS); // Выход из программы
}


// Основная функция программы
int main(void)
{
  table_processing();
  print_game_logo();

  char name[MAX_NAME_LENGTH]; // Объявляем массив для хранения введенного имени
  printf("Введите ваше имя: ");
  scanf("%s", name);

 start:
  game_menu();
  
  puts("\033[48;2;255;105;180m\033[30m\t\t\t  !!!ИГРА НАЧАЛАСЬ!!!   \t\t\t\033[0m");
  puts("Угадай число от 1 до 100.\n");
  
  uint8_t random_number = generate_random_number();
  uint64_t torture = 0; // Переменная для подсчета попыток
  
  //printf("%hhu", random_number); // Проверка

  // Основной игровой цикл
  while (1){
    uint8_t guessed_number = get_user_input();
    ++torture;

    // Проверка введенного числа и выдача подсказки
    if (guessed_number < random_number) puts("Слишком мало, попробуйте еще раз!\n");
    else if (guessed_number > random_number) puts("Слишком много, попробуйте еще раз!\n");
    else{
      printf("\033[1;92mВы угадали число с %llu попытки, поздравляем!\033[0m\n\n", torture);
      
      update_leaderboard(name, torture);
      goto start;
    }
  }
}
