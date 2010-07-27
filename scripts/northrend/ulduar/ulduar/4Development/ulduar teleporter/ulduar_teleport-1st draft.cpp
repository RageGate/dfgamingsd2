/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Ulduar_teleport
SD%Complete: 100%
SDComment: by /dev/FallenAngelX
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "def_ulduar.h"

enum 
{
PORTALS_COUNT = 8
};

struct t_Locations
{
    char const* name[2];
    float x, y, z;
    uint32 spellID;
    bool state;
    bool active;
    uint32 encounter;
};

static t_Locations PortalLoc[]=
{
{"-?-English","??????-russian",-706.122f, -92.6024f, 429.876f, 0,true,true,TYPE_LEVIATHAN_TP}, //
{"-?-","???????",131.248f, -35.3802f, 409.804f, 0,true,true,TYPE_FLAME_LEVIATHAN},  //
{"-?-","???????",553.233f, -12.3247f, 409.679f, 0,true,true,TYPE_XT002_TP}, //
{"-?-","??????",926.292f, -11.4635f, 418.595f, 0,true,true,TYPE_XT002}, //
{"-?-","????????",1498.09f, -24.246f, 420.967f, 0,true,true,TYPE_KOLOGARN}, //
{"-?-","?????????",1859.45f, -24.1f, 448.9f, 0,true,true,TYPE_TYPE_KOLOGARN}, //
{"-?-","?????????",2536.87f, 2569.15f, 412.304f, 0,true,true,TYPE_TYPE_VEZAX}, //
{"-?-","??????????",1854.297f, -11.0173f, 334.4f, 0,true,true,TYPE_KOLOGARN}, //
};
//above is a crude list and needs revised but then again  this is still a work in progess

bool GOGossipSelect_go_ulduar_teleporter(Player *player, GameObject* pGo, uint32 sender, uint32 action)
{
    int32 damage = 0;
    if(sender != GOSSIP_SENDER_MAIN) return true;

    if(!player->getAttackers().empty()) return true;

    if(action >= 0 && action <= PORTALS_COUNT)
    player->TeleportTo(MAP_NUM, PortalLoc[action].x, PortalLoc[action].y, PortalLoc[action].z, 0);
    if (PortalLoc[action].spellID !=0 ) 
           if (SpellEntry const* spell = (SpellEntry *)GetSpellStore()->LookupEntry(PortalLoc[action].spellID))
                  player->AddAura(new BossAura(spell, EFFECT_INDEX_2, &damage,(Unit*)player, (Unit*)player));

    player->CLOSE_GOSSIP_MENU();
    return true;
}

bool GOGossipHello_go_uldar_teleporter(Player *player, GameObject* pGo)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pGo->GetInstanceData();
    if(!pInstance) return true;

    uint8 _locale;

    switch (LocaleConstant currentlocale = player->GetSession()->GetSessionDbcLocale())
    {
     case LOCALE_enUS:
     case LOCALE_koKR:
     case LOCALE_frFR:
     case LOCALE_deDE:
     case LOCALE_zhCN:
     case LOCALE_zhTW:
     case LOCALE_esES:
     case LOCALE_esMX:
                      _locale = 0;
                      break;
     case LOCALE_ruRU:
                      _locale = 1;
                      break;
     default:
                      _locale = 0;
                      break;
    };

    for(uint8 i = 0; i < PORTALS_COUNT; i++) {
    if (PortalLoc[i].active == true && (PortalLoc[i].state == true || pInstance->GetData(TYPE_TELEPORT) >= PortalLoc[i].encounter) || player->isGameMaster())
             player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, PortalLoc[i].name[_locale], GOSSIP_SENDER_MAIN, i);
    };
    player->SEND_GOSSIP_MENU(TELEPORT_GOSSIP_MESSAGE, pGo->GetGUID());
    return true;
}

bool GOGossipHello_go_(Player *player, GameObject* pGo)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pGo->GetInstanceData();
    if(!pInstance) return false;

    if (pInstance->GetData(TYPE_) == DONE)
           pInstance->SetData(TYPE_, DONE);
    if (pInstance->GetData(TYPE_) == DONE)
           pInstance->SetData(TYPE_, DONE);

    return true;
}

bool GOGossipHello_go_(Player *player, GameObject* pGo)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pGo->GetInstanceData();
    if(!pInstance) return false;

    if (pInstance->GetData(TYPE_) == DONE)
           pInstance->SetData(TYPE_, DONE);

    return true;
}

bool GOGossipHello_go_(Player *player, GameObject* pGo)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pGo->GetInstanceData();
    if(!pInstance) return false;

    if (pInstance->GetData(TYPE_) == DONE)
           pInstance->SetData(TYPE_, DONE);

    return true;
}


void AddSC_go_ulduar_teleporter()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "go_ulduar_teleporter";
    newscript->pGOGossipHello  = &GOGossipHello_go_ulduar_teleporter;
    newscript->pGOGossipSelect = &GOGossipSelect_go_ulduar_teleporter;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_";
    newscript->pGOGossipHello  = &GOGossipHello_go_;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_";
    newscript->pGOGossipHello  = &GOGossipHello_go_;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_";
    newscript->pGOGossipHello  = &GOGossipHello_go_;
    newscript->RegisterSelf();
}
