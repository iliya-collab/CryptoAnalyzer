#include "Parser/Scanner.hpp"

Scanner::Scanner(WebSocketParser* _wsParser, const ParamsScannerConfig& _ScannerConfig, const ParamsMyWalletConfig& _MyWalletConfig) {
    ScannerConfig = _ScannerConfig;
    MyWalletConfig = _MyWalletConfig;
}

void Scanner::initTimers() {

}