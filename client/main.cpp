#include <QApplication>
#include <exception>
#include <iostream>

#include "../common/queue.h"
#include "UI/hostwindow.h"
#include "UI/initialwindow.h"
#include "UI/mainwindow.h"
#include "UI/readywindow.h"
#include "UI/trackwindow.h"

#include "client_game.h"
#include "session.h"

int main() {
    try {
        int argc = 0;
        Queue<ServerToClientCmd_Client*> recv_queue(UINT32_MAX);
        QApplication menu_display(argc, nullptr);
        ReadyWindow ready_window;
        MainWindow selector_lobby(ready_window);
        TrackWindow track_window(selector_lobby);
        InitialWindow lobby_window(selector_lobby, track_window);
        HostWindow host_window(recv_queue, lobby_window, ready_window);
        host_window.show();
        menu_display.exec();
        ClientSession& session = host_window.getSession();
        Game game(session);
        game.start();
        session.stop();
        session.join();
        return 0;
    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Something went wrong and an unknown exception was caught.\n";
        return 1;
    }
}
