#pragma once

// Класс для работы с логами
class LogHandler {
public:
    // Записывает в файл лога какое-то сообщение
    static void writeLog(const char* msg);
    // Очищает весь лог
    static void clearLog();
    // Задает включения логов
    static void setEnable(bool _enable);


private:
    LogHandler() = default;

    static bool enable;
    static const char* file_log;
};
