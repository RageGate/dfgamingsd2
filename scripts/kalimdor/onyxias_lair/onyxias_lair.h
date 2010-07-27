#ifndef ONYXIAS_LAIR_H
#define ONYXIAS_LAIR_H

enum
{
    MAX_ENCOUNTER       = 1,

    TYPE_ONYXIA         = 0,
};

class instance_onyxias_lair : public ScriptedInstance
{
    uint32 m_uiOnyxiaState;
    std::string m_strInstData;
public:
    instance_onyxias_lair(Map* pMap) : ScriptedInstance(pMap) { Initialize(); }

    void Initialize();
    bool IsEncounterInProgress() const { return m_uiOnyxiaState == IN_PROGRESS; }

    const char* Save() { return m_strInstData.c_str(); }
    void Load(const char* chrIn);

    uint32 GetData(uint32 uiType);
    void SetData(uint32 uiType, uint32 uiData);
};

#endif
