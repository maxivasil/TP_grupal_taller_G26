#ifndef CONSTANTS_H
#define CONSTANTS_H

// Constants for server communication protocol
#define SNAPSHOT_COMMAND 0x01
#define JOIN_RESPONSE_COMMAND 0x04
#define GAME_STARTING_COMMAND 0x07
#define RACE_RESULTS_COMMAND 0x08
#define ASSIGN_ID_COMMAND 0x0A
#define STARTING_RACE_COMMAND 0x0C

// Aliases for clarity
#define SERVER_TO_CLIENT_RACE_RESULTS RACE_RESULTS_COMMAND

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
#define CLIENT_TO_SERVER_FINISH_RACE 0x09
#define TOUR_COMMAND 0x0B
#define APPLY_UPGRADES_COMMAND 0x0D

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
