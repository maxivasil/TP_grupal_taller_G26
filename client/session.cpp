#include "session.h"

ClientSession::ClientSession(const char* hostname, const char* servname,
                             Queue<ServerToClientCmd_Client*>& recv_queue):
        protocol(hostname, servname),
        parser(),
        registered_commands(),
        send_queue(UINT32_MAX),
        receive_queue(recv_queue),
        sender(protocol, send_queue),
        receiver(protocol, receive_queue, registered_commands.get_recv_registry()) {}

void ClientSession::run() {
    try {
        receiver.start();
        sender.start();
        receiver.join();
        sender.join();
    } catch (const std::exception& e) {
        if (!receiver.is_alive() || !sender.is_alive() || protocol.is_connection_closed()) {
            if (receiver.is_alive()) {
                receiver.join();
            }
            if (sender.is_alive()) {
                sender.join();
            }
            if (!protocol.is_connection_closed()) {
                protocol.close_connection();
            }
        }
        std::cerr << "Unexpected exception in ClientHandler" << e.what() << std::endl;
    }
}

void ClientSession::stop() {
    ServerToClientCmd_Client* raw;
    while (receive_queue.try_pop(raw)) {
        if (raw) {
            std::unique_ptr<ServerToClientCmd_Client> cmd(raw);
        }
    }
    receive_queue.close();
    receiver.stop();
    protocol.close_connection();
    send_queue.close();
    sender.stop();
    Thread::stop();
}

void ClientSession::send_command(ClientToServerCmd_Client* cmd) { send_queue.push(cmd); }

Queue<ServerToClientCmd_Client*>& ClientSession::get_recv_queue() { return receive_queue; }
