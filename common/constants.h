#ifndef CONSTANTS_H
#define CONSTANTS_H

// Constants for server communication protocol
#define SNAPSHOT_COMMAND 0x01
#define JOIN_RESPONSE_COMMAND 0x04

// Constants for ERROR_CODE de JOIN_RESPONSE_COMMAND
#define LOBBY_NOT_FOUND 0x01
#define LOBBY_ALREADY_STARTED 0x02
#define LOBBY_ALREADY_EXISTS 0x03

// Constants for STATUS de JOIN_RESPONSE_COMMAND
#define STATUS_OK 0x01
#define STATUS_ERROR 0x02

// Constants for client communication protocol
#define MOVE_COMMAND 0x02
#define JOIN_COMMAND 0x03
#define READY_TO_START_COMMAND 0x05
#define CHEAT_COMMAND 0x06

// Constants for JOIN_COMMAND type
#define TYPE_JOIN 0
#define TYPE_CREATE 1

// Constants for movement directions
#define MOVE_UP 0x01
#define MOVE_DOWN 0x02
#define MOVE_LEFT 0x03
#define MOVE_RIGHT 0x04

// Constants for cheat types
#define CHEAT_INFINITE_HEALTH 0x01
#define CHEAT_WIN 0x02
#define CHEAT_LOSE 0x03

#endif
