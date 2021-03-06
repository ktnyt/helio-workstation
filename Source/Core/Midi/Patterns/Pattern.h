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

#include "Serializable.h"
#include "Clip.h"

class ProjectEventDispatcher;
class ProjectTreeItem;
class UndoStack;
class MidiTrack;

// A sorted array of clips
class Pattern : public Serializable
{
public:

    explicit Pattern(MidiTrack &track,
        ProjectEventDispatcher &eventDispatcher);

    ~Pattern() override;
    
    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    float getFirstBeat() const noexcept;
    float getLastBeat() const noexcept;
    MidiTrack *getTrack() const noexcept;

    //===------------------------------------------------------------------===//
    // Undoing
    //===------------------------------------------------------------------===//

    void checkpoint();
    void undo();
    void redo();
    void clearUndoHistory();

    //===------------------------------------------------------------------===//
    // Track editing
    //===------------------------------------------------------------------===//

    // This one is for import and checkout procedures.
    // Does not notify anybody to prevent notification hell.
    // Always call notifyLayerChanged() when you're done using it.
    void silentImport(const Clip &clipToImport);

    bool insert(Clip clip, const bool undoable);
    bool remove(Clip clip, const bool undoable);
    bool change(Clip clip, Clip newClip, const bool undoable);

    //===------------------------------------------------------------------===//
    // Array wrapper
    //===------------------------------------------------------------------===//

    void sort();

    inline int size() const
    { return this->clips.size(); }

    inline Clip** begin() const noexcept
    { return this->clips.begin(); }
    
    inline Clip** end() const noexcept
    { return this->clips.end(); }
    
    inline Clip *getUnchecked(const int index) const
    { return this->clips.getUnchecked(index); }
    
    inline OwnedArray<Clip> &getClips() noexcept;
    
    //===------------------------------------------------------------------===//
    // Events change listener
    //===------------------------------------------------------------------===//

    void notifyClipChanged(const Clip &oldClip, const Clip &newClip);
    void notifyClipAdded(const Clip &clip);
    void notifyClipRemoved(const Clip &clip);
    void notifyClipRemovedPostAction();
    void updateBeatRange(bool shouldNotifyIfChanged);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    String createUniqueClipId() const noexcept;
    String getTrackId() const noexcept;

    friend inline bool operator==(const Pattern &lhs, const Pattern &rhs)
    {
        return (&lhs == &rhs);
    }

    int hashCode() const noexcept;

protected:

    void clearQuick();

    float lastEndBeat;
    float lastStartBeat;

    ProjectTreeItem *getProject();
    UndoStack *getUndoStack();

    OwnedArray<Clip> clips;
    SparseHashSet<Clip::Id, StringHash> usedClipIds;

private:
    
    MidiTrack &track;
    ProjectEventDispatcher &eventDispatcher;

private:
    
    WeakReference<Pattern>::Master masterReference;
    friend class WeakReference<Pattern>;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pattern);
};

class PatternHashFunction
{
public:

    static int generateHash(const Pattern *pattern, const int upperLimit) noexcept
    {
        return static_cast<int>((static_cast<uint32>(pattern->hashCode())) % static_cast<uint32>(upperLimit));
    }
};
