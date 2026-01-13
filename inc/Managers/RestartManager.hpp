#pragma once

class RestartManager { 
public:

    static void requestRestart();
    
private:
    
    static void performRestart();
    static bool m_restartRequested;
};