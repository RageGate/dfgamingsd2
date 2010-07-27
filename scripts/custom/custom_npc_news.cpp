//* Copyright (C) 2009 - 2010 ESSENCEGAMING.ORG
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: news
SD%Complete: 100
SDComment: news NPC
SDCategory: NPC
SDAuthor: Flame
EndScriptData */

#include "precompiled.h"

#define MSG_N1		"Welcome to The Fallen World of WarCraft a 3.3.5 Hard Realm"
#define MSG_COMBAT	"You Are In Combat -.-"



bool GossipHello_news(Player* pPlayer, Creature* pCreature)
{

		// Top Menu News & Updates
		pPlayer->ADD_GOSSIP_ITEM( 3, "Realm News & Updates"			, GOSSIP_SENDER_MAIN, 1000);
		// Close Menu
		pPlayer->ADD_GOSSIP_ITEM( 0, "No Thanks"			        , GOSSIP_SENDER_MAIN, 10000);

    pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());

return true;
}

void SendDefaultMenu_news(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{

// Not allow in combat
if (pPlayer->isInCombat())
{
    pPlayer->CLOSE_GOSSIP_MENU();
   pCreature->MonsterSay(MSG_COMBAT, LANG_UNIVERSAL, NULL);
	return;
}

switch(uiAction)
{

case 1000: // News & Updates submenu
		// News & Updates v1
	 pPlayer->ADD_GOSSIP_ITEM( 3, "20th July 2010"			, GOSSIP_SENDER_MAIN, 1001);
	pPlayer->ADD_GOSSIP_ITEM( 3, "No Thanks"			, GOSSIP_SENDER_MAIN, 10000);

	pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());
break;

case 1001: // Actual News
	 pPlayer->CLOSE_GOSSIP_MENU();
	pCreature->MonsterSay(MSG_N1, LANG_UNIVERSAL, NULL);

break; 

case 10000: //Close Menu
	pPlayer->CLOSE_GOSSIP_MENU();
break;


} // end of switch
} //end of function

bool GossipSelect_news(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
	if (uiSender == GOSSIP_SENDER_MAIN)
	SendDefaultMenu_news(pPlayer, pCreature, uiAction);

return true;
}

void AddSC_news()
{
    Script *newscript;

	newscript                   = new Script;
    newscript->Name             = "news";
    newscript->pGossipHello     = &GossipHello_news;
    newscript->pGossipSelect    = &GossipSelect_news;
    newscript->RegisterSelf();
}