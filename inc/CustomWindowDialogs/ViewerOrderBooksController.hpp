#pragma once

#include "Parser/Scanner.hpp"

#include "CustomWindowDialogs/ViewerOrderBooks.hpp"
#include "CustomWindowDialogs/BaseDialogController.hpp"

class ViewerOrderBooksController : public BaseDialogController<ViewerOrderBooks> {
private:
    Scanner* scanner;

    void updateOrderBooks(const WebSocketParser::stOrderBooks& _orderBooks);

    void onDialogCreated() override;

public:
    explicit ViewerOrderBooksController(Scanner* _scanner, QWidget* parent = nullptr) : scanner(_scanner), BaseDialogController(parent) {}
};