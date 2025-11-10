#include "utilities.h"
#include "field.h" // Подключаем, чтобы знать структуру Field
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Windows ---
#ifdef _WIN32
    #include <windows.h> // Для функции Sleep
// --- Unix/Linux/macOS ---
#else
    #include <unistd.h>  // Для функции sleep
#endif

// Очистка консоли
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Задержка в секундах
void sleepSeconds(int seconds) {
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}

// Вывод поля в консоль
void printFieldToConsole(Field* f) {
    if (!f) {
        // Если поле не создано, нечего выводить
        return;
    }

    for (int i = 0; i < f->height; i++) {
        for (int j = 0; j < f->width; j++) {
            // putchar выводит один символ в консоль
            putchar(f->grid[i][j]);
        }
        putchar('\n');
    }
}

// Вывод поля в файл
void printFieldToFile(Field* f, FILE* file) {
    // Проверяем, что указатели на структуру поля и файл действительны
    if (!f || !file) {
        // Если один из указателей неверен, нечего записывать
        return;
    }

    for (int i = 0; i < f->height; i++) {
        for (int j = 0; j < f->width; j++) {
            // fputc записывает один символ в указанный файл
            fputc(f->grid[i][j], file);
        }
        // После записи строки символов в файл, добавляем символ новой строки
        fputc('\n', file);
    }
}