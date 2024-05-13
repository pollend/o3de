#include "TrackViewUndoCommand.h"

// CryCommon
#include <CryCommon/Maestro/Types/AnimNodeType.h>

// Editor
#include "TrackView/TrackViewAnimNode.h"
#include "TrackView/TrackViewSequence.h"

AZ_CLASS_ALLOCATOR_IMPL(TrackViewUndoCommand, AZ::SystemAllocator);

TrackViewUndoCommand::TrackViewUndoCommand()
    : AzToolsFramework::UndoSystem::URSequencePoint("TrackViewUndoCommand")
    , m_undoTrack(rapidjson::kArrayType)
{
}

void TrackViewUndoCommand::SetTrack(CTrackViewTrack* track)
{
    AZ_Assert(track, "Expected a valid track");
    if (track)
    {
        m_trackName = track->GetName();
        AZ_Assert(!m_trackName.empty(), "Expected a valid track name");

        CTrackViewAnimNode* animNode = track->GetAnimNode();
        AZ_Assert(animNode, "Expected a valid anim node");
        if (animNode)
        {
            m_trackComponentId = animNode->GetComponentId();
            AZ_Assert(m_trackComponentId != AZ::InvalidComponentId, "Expected a valid track component id");

            CTrackViewSequence* sequence = track->GetSequence();
            AZ_Assert(sequence, "Expected to find the sequence");
            if (sequence)
            {
                m_sequenceId = sequence->GetSequenceComponentEntityId();
                AZ_Assert(m_sequenceId.IsValid(), "Expected a valid sequence id");

                AnimNodeType nodeType = animNode->GetType();
                AZ_Assert(nodeType == AnimNodeType::Component, "Expected a this node to be a AnimNodeType::Component type");
                if (nodeType == AnimNodeType::Component)
                {
                    CTrackViewAnimNode* parentAnimNode = static_cast<CTrackViewAnimNode*>(animNode->GetParentNode());
                    AZ_Assert(parentAnimNode, "Expected a valid parent node");
                    if (parentAnimNode)
                    {
                        m_entityId = parentAnimNode->GetAzEntityId();
                        AZ_Assert(m_entityId.IsValid(), "Expected a valid sequence id");

                        // AZ::JsonSerializerSettings settings;
                        // AZ::JsonSerialization::Store(m_undoTrack, m_undoTrack.GetAllocator(), *track, settings);

                        // Store undo info.
                        //                     m_undo = track->GetMemento();
                    }
                }
            }
        }
    }
}

CTrackViewTrack* TrackViewUndoCommand::FindTrack(CTrackViewSequence* sequence)
{
    AZ_Assert(sequence, "Expected to find the sequence");

    CTrackViewTrack* track = nullptr;
    CTrackViewTrackBundle allTracks = sequence->GetAllTracks();
    for (unsigned int trackIndex = 0; trackIndex < allTracks.GetCount(); trackIndex++)
    {
        CTrackViewTrack* curTrack = allTracks.GetTrack(trackIndex);
        if (curTrack->GetAnimNode() && curTrack->GetAnimNode()->GetComponentId() == m_trackComponentId)
        {
            if (curTrack->GetName() == m_trackName)
            {
                CTrackViewAnimNode* parentAnimNode = static_cast<CTrackViewAnimNode*>(curTrack->GetAnimNode()->GetParentNode());
                if (parentAnimNode && parentAnimNode->GetAzEntityId() == m_entityId)
                {
                    track = curTrack;
                    break;
                }
            }
        }
    }
    return track;
}

void TrackViewUndoCommand::Undo()
{
    CTrackViewSequence* sequence = CTrackViewSequence::LookUpSequenceByEntityId(m_sequenceId);
    AZ_Assert(sequence, "Expected to find the sequence");
    if (sequence)
    {
        CTrackViewTrack* track = FindTrack(sequence);
        AZ_Assert(track, "Expected to find track");
        {
            CTrackViewSequenceNoNotificationContext context(sequence);
            // Undo track state.
            // track->RestoreFromMemento(m_undo);
        }

        sequence->ForceAnimation();
    }
}

void TrackViewUndoCommand::Redo()
{
    CTrackViewSequence* sequence = CTrackViewSequence::LookUpSequenceByEntityId(m_sequenceId);
    AZ_Assert(sequence, "Expected to find the sequence");
    if (sequence)
    {
        CTrackViewTrack* track = FindTrack(sequence);
        AZ_Assert(track, "Expected to find track");

        // Redo track state.
        // track->RestoreFromMemento(m_redo);

        sequence->OnKeysChanged();
    }
}

bool TrackViewUndoCommand::Changed() const
{
    return true;
    // return m_redo == m_undo;
}
