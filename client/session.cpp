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

int ClientSession::run() {
    try {
        receiver.start();
        sender.start();
        while (true) {
            ParsedCommand parsed = parser.parse_and_filter_line();
            if (parsed.type == EXIT) {
                stop();
                break;
            } else if (parsed.type == READ) {
                int i = 0;
                while (i < parsed.lines_to_read) {
                    ServerToClientCmd_Client* cmd;
                    if (receive_queue.try_pop(cmd)) {
                        if (cmd) {
                            cmd->execute(*this);  // ejecutar el comando polimórfico
                        }
                        i++;
                    }
                }
            } else {
                auto it = registered_commands.get_send_registry().find(parsed.type);
                if (it != registered_commands.get_send_registry().end()) {
                    auto cmd_ptr = it->second(parsed);
                    send_queue.try_push(std::move(cmd_ptr));
                }
            }
        }
        protocol.close_connection();
        receiver.join();
        sender.join();
        return 0;
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
            return 0;
        }
        std::cerr << "Unexpected exception in ClientHandler" << e.what() << std::endl;
        return 1;
    }
}

void ClientSession::stop() {
    receive_queue.close();
    receiver.stop();
    send_queue.close();
    sender.stop();
}
