#include "precompiled.h"
#include "onyxias_lair.h"

void instance_onyxias_lair::Initialize()
{
    m_uiOnyxiaState = NOT_STARTED;
}

uint32 instance_onyxias_lair::GetData(uint32 uiType)
{
    switch(uiType)
    {
        case TYPE_ONYXIA:
            return m_uiOnyxiaState;
        default:
            return 0;
    }
}

void instance_onyxias_lair::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_ONYXIA:
            m_uiOnyxiaState = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_uiOnyxiaState;

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_onyxias_lair::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);

    loadStream >> m_uiOnyxiaState;

    if (m_uiOnyxiaState == IN_PROGRESS)
        m_uiOnyxiaState = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_onyxias_lair(Map* pMap)
{
    return new instance_onyxias_lair(pMap);
}

void AddSC_instance_onyxias_lair()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "instance_onyxias_lair";
    newscript->GetInstanceData = &GetInstanceData_instance_onyxias_lair;
    newscript->RegisterSelf();
}
