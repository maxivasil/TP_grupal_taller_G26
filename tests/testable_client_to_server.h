#pragma once

#pragma push_macro("private")
#undef private
#define private public

#include "../server/cmd/client_to_server_applyUpgrades.h"
#include "../server/cmd/client_to_server_cheat.h"
#include "../server/cmd/client_to_server_joinLobby.h"
#include "../server/cmd/client_to_server_move.h"
#include "../server/cmd/client_to_server_readyToStart.h"
#include "../server/cmd/client_to_server_tour.h"

#pragma pop_macro("private")
