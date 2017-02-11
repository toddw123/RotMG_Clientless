#pragma once

#ifndef CLIENTLESS_H
#define CLIENTLESS_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS // stupid visual studios throwing errors over fopen and shit

// Include windows/winsock headers
#include <winsock2.h>

// Include standard c/c++ headers
#include <sstream>
#include <thread>
#include <math.h>
// Urlmon is for URLDownloadToFile
#include <Urlmon.h>

//Include curl headers
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/curlbuild.h>

// Misc
#include "packets\PacketIOHelper.h"
#include "packets\PacketType.h"
#include "xmlParser\IXMLParser.h"
#include "Client.h"

// Outgoing packets
#include "packets\outgoing\Load.h"
#include "packets\outgoing\Create.h"
#include "packets\outgoing\UpdateAck.h"
#include "packets\outgoing\Pong.h"
#include "packets\outgoing\Move.h"
#include "packets\outgoing\GotoAck.h"
#include "packets\outgoing\AoeAck.h"
#include "packets\outgoing\PlayerText.h"
#include "packets\outgoing\ShootAck.h"
#include "packets\outgoing\PlayerShoot.h"
#include "packets\outgoing\InvSwap.h"
#include "packets\outgoing\UsePortal.h"
#include "packets\outgoing\Teleport.h"
#include "packets\outgoing\Escape.h"

// Incoming packets
#include "packets\incoming\MapInfo.h"
#include "packets\incoming\Update.h"
#include "packets\incoming\Failure.h"
#include "packets\incoming\CreateSuccess.h"
#include "packets\incoming\GlobalNotification.h"
#include "packets\incoming\AccountList.h"
#include "packets\incoming\ShowEffect.h"
#include "packets\incoming\Ping.h"
#include "packets\incoming\NewTick.h"
#include "packets\incoming\Goto.h"
#include "packets\incoming\Aoe.h"
#include "packets\incoming\Text.h"
#include "packets\incoming\AllyShoot.h"
#include "packets\incoming\ServerPlayerShoot.h"
#include "packets\incoming\EnemyShoot.h"
#include "packets\incoming\Damage.h"
#include "packets\incoming\Reconnect.h"
#include "packets\incoming\BuyResult.h"
#include "packets\incoming\ClientStat.h"
#include "packets\incoming\Death.h"
#include "packets\incoming\EvolvedPetMessage.h"
#include "packets\incoming\FilePacket.h"
#include "packets\incoming\GuildResult.h"
#include "packets\incoming\InvitedToGuild.h"
#include "packets\incoming\InvResult.h"
#include "packets\incoming\KeyInfoResponse.h"
#include "packets\incoming\NameResult.h"
#include "packets\incoming\NewAbilityMessage.h"
#include "packets\incoming\Notification.h"
#include "packets\incoming\PasswordPrompt.h"
#include "packets\incoming\PlaySoundPacket.h"
#include "packets\incoming\QuestFetchResponse.h"
#include "packets\incoming\QuestObjId.h"
#include "packets\incoming\QuestRedeemResponse.h"


#endif // !CLIENTLESS_H
