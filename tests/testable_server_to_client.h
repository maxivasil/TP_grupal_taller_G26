#pragma once

#pragma push_macro("private")
#undef private
#define private public

#include "../client/cmd/server_to_client_accumulatedResults.h"
#include "../client/cmd/server_to_client_assign_id.h"
#include "../client/cmd/server_to_client_lobbyResponse.h"
#include "../client/cmd/server_to_client_raceResults.h"
#include "../client/cmd/server_to_client_snapshot.h"
#include "../client/cmd/server_to_client_startingRace.h"

#pragma pop_macro("private")
