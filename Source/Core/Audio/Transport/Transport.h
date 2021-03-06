/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class Instrument;
class OrchestraPit;
class PlayerThread;
class PlayerThreadPool;
class RendererThread;

#include "TransportListener.h"
#include "ProjectSequencesWrapper.h"
#include "ProjectListener.h"
#include "OrchestraListener.h"

class Transport : public ProjectListener, private OrchestraListener
{
public:

    explicit Transport(OrchestraPit &orchestraPit);
    ~Transport() override;
    
    static String getTimeString(double timeMs, bool includeMilliseconds = false);
    static String getTimeString(const RelativeTime &relTime, bool includeMilliseconds = false);
    
    //===------------------------------------------------------------------===//
    // Transport
    //===------------------------------------------------------------------===//
    
    double getSeekPosition() const noexcept;
    double getTotalTime() const noexcept;
    void seekToPosition(double absPosition);
    
    void probeSoundAt(double absTrackPosition, 
        const MidiSequence *limitToLayer = nullptr);
    
    void probeSequence(const MidiMessageSequence &sequence);

    void startPlaybackLooped(double absLoopStart, double absLoopEnd);
    bool isLooped() const;
    double getLoopStart() const;
    double getLoopEnd() const;
    
    void startPlayback();
    bool isPlaying() const;
    void stopPlayback();
    void toggleStatStopPlayback();

    void startRender(const String &filename);
    bool isRendering() const;
    void stopRender();
    
    float getRenderingPercentsComplete() const;
    
    void calcTimeAndTempoAt(const double absPosition,
                            double &outTimeMs,
                            double &outTempo);

    MidiMessage findFirstTempoEvent();

    //===------------------------------------------------------------------===//
    // Sending messages at real-time
    //===------------------------------------------------------------------===//
    
    void sendMidiMessage(const String &layerId, const MidiMessage &message) const;
    void allNotesAndControllersOff() const;
    void allNotesControllersAndSoundOff() const;
    
    //===------------------------------------------------------------------===//
    // OrchestraListener
    //===------------------------------------------------------------------===//

    void instrumentAdded(Instrument *instrument) override;
    void instrumentRemoved(Instrument *instrument) override;
    void instrumentRemovedPostAction() override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//
    
    void onChangeMidiEvent(const MidiEvent &oldEvent,
        const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;
    void onPostRemoveMidiEvent(MidiSequence *const layer) override;

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override {}
    void onReloadProjectContent(const Array<MidiTrack *> &tracks) override;

    //===------------------------------------------------------------------===//
    // Listeners management
    //===------------------------------------------------------------------===//

    void addTransportListener(TransportListener *listener);
    void removeTransportListener(TransportListener *listener);

protected:

    void setTotalTime(const double val);
    void setSeekPosition(const double absPosition);

    void broadcastTempoChanged(const double newTempo);
    void broadcastTotalTimeChanged(const double timeMs);
    void broadcastPlay();
    void broadcastStop();
    void broadcastSeek(const double newPosition,
                       const double currentTimeMs,
                       const double totalTimeMs);

private:
    
    OrchestraPit &orchestra;

    ScopedPointer<PlayerThreadPool> player;
    ScopedPointer<RendererThread> renderer;

    friend class RendererThread;
    friend class PlayerThread;

private:

    ProjectSequences getSequences();
    void rebuildSequencesIfNeeded();
    
    SpinLock sequencesLock;
    ProjectSequences sequences;
    bool sequencesAreOutdated;
    
    Array<const MidiTrack *> tracksCache;
    HashMap<String, Instrument *> linksCache; // layer id : instrument
    
    void updateLinkForTrack(const MidiTrack *track);
    void removeLinkForTrack(const MidiTrack *track);
    
private:
    
    bool loopedMode;
    double loopStart;
    double loopEnd;
    
private:

    Atomic<double> seekPosition;
    Atomic<double> totalTime;
    
    Atomic<double> trackStartMs;
    Atomic<double> trackEndMs;
    
    Atomic<float> projectFirstBeat;
    Atomic<float> projectLastBeat;

    ListenerList<TransportListener> transportListeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Transport)
};
