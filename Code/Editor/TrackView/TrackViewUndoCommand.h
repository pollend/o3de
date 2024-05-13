#pragma once

#include "TrackViewTrack.h"
#include <AzToolsFramework/Undo/UndoSystem.h>

class TrackViewUndoCommand : public AzToolsFramework::UndoSystem::URSequencePoint
{
public:
    AZ_CLASS_ALLOCATOR_DECL

    AZ_RTTI(TrackViewUndoCommand, "{978E24A2-17A7-48B5-8319-78BDE987E6DC}", AzToolsFramework::UndoSystem::URSequencePoint)

    explicit TrackViewUndoCommand();
    TrackViewUndoCommand(const TrackViewUndoCommand& other) = delete;
    TrackViewUndoCommand& operator=(const TrackViewUndoCommand& other) = delete;
    ~TrackViewUndoCommand() override = default;

    void SetTrack(CTrackViewTrack* track);
    void Undo() override;
    void Redo() override;
    bool Changed() const override;

private:
    CTrackViewTrack* FindTrack(CTrackViewSequence* sequence);

    // Internal state are id's used to uniquely identify
    // a track. This does not store pointers to tracks because
    // those can change when an AZ::Undo event happens and the entity
    // is reloaded.
    AZ::EntityId m_sequenceId;
    AZ::EntityId m_entityId;
    AZStd::string m_trackName;
    AZ::ComponentId m_trackComponentId;

    rapidjson::Document m_undoTrack;
};
