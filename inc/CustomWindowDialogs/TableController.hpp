#pragma once

#include "Parser/Scanner.hpp"

#include "CustomWindowDialogs/DialogTable.hpp"
#include "CustomWindowDialogs/BaseDialogController.hpp"

class TableController : public BaseDialogController<DialogTable> {
private:
    Scanner* scanner;

    void updateTable(const WebSocketParser::stTicker& _ticker);

    void onDialogCreated() override;

public:
    explicit TableController(Scanner* _scanner, QWidget* parent = nullptr) : scanner(_scanner), BaseDialogController(parent) {}
};