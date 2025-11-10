#include "field.h"
#include <stdlib.h>
#include <string.h>

// --- Снимки ---

StateSnapshot* createSnapshot(Field* f, Dino* d) {
    if (!f || !d) return NULL;

    StateSnapshot* snap = malloc(sizeof(StateSnapshot));
    if (!snap) return NULL;

    snap->width = f->width;
    snap->height = f->height;

    // Копируем сетки
    memcpy(snap->grid, f->grid, sizeof(f->grid));
    memcpy(snap->paintGrid, f->paintGrid, sizeof(f->paintGrid));

    // Копируем структуру динозавра
    snap->dino = *d; // Предполагается, что Dino не содержит указателей

    return snap;
}

void restoreSnapshot(Field* f, Dino* d, StateSnapshot* snapshot) {
    if (!f || !d || !snapshot) return;

    // Проверяем, совпадают ли размеры? (Опционально, но желательно)
    if (f->width != snapshot->width || f->height != snapshot->height) {
        // Ошибка: невозможно восстановить состояние для поля другого размера
        return; // Или выводим сообщение об ошибке
    }

    // Восстанавливаем сетки
    memcpy(f->grid, snapshot->grid, sizeof(f->grid));
    memcpy(f->paintGrid, snapshot->paintGrid, sizeof(f->paintGrid));

    // Восстанавливаем структуру динозавра
    *d = snapshot->dino; // Присваиваем копию
}

void freeSnapshot(StateSnapshot* snapshot) {
    if (snapshot) {
        free(snapshot);
    }
}

// --- История ---

StateHistory* createStateHistory() {
    StateHistory* history = malloc(sizeof(StateHistory));
    if (!history) return NULL;

    history->top = -1; // Стек пуст

    for (int i = 0; i < MAX_HISTORY_SIZE; i++) {
        history->snapshots[i] = NULL;
    }

    return history;
}

void pushState(StateHistory* history, StateSnapshot* snapshot) {
    if (!history || !snapshot) return;

    if (history->top >= MAX_HISTORY_SIZE - 1) {
        return;
    }

    history->top++;
    history->snapshots[history->top] = snapshot; // Сохраняем указатель на снимок
}

StateSnapshot* popState(StateHistory* history) {
    if (!history || history->top < 0) {
        return NULL; // Стек пуст
    }

    StateSnapshot* snapshot = history->snapshots[history->top];
    history->snapshots[history->top] = NULL; // Убираем указатель из стека
    history->top--;
    return snapshot; // Возвращаем указатель, владелец должен вызвать freeSnapshot
}

void freeStateHistory(StateHistory* history) {
    if (!history) return;

    // Освобождаем все снимки в стеке
    for (int i = 0; i <= history->top; i++) {
        freeSnapshot(history->snapshots[i]);
    }

    free(history);
}