/*============================================================================*/
/*
  Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
  See the file GNU_GPL_v2.txt for full licensing terms.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 51
  Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*============================================================================*/

/** Include this to get the @ref vf_concurrent module.

    @file vf_concurrent.h
    @ingroup vf_concurrent
*/

#ifndef VF_CONCURRENT_VFHEADER
#define VF_CONCURRENT_VFHEADER

/*============================================================================*/
/**
  Classes for building a concurrent system.

  This module offers a specially crafted set of classes that work together
  to solve a fundamental problem encountered when designing any concurrent
  system: synchronization and sharing of state information between multiple
  threads of execution. Desiging and maintaining these systems is difficult:

  @par The Trouble with Locks
  <em>Unfortunately, today's reality is that only thoughtful experts can write
  explicitly concurrent programs that are correct and efficient. This is
  because today's programming models for concurrency are subtle, intricate,
  and fraught with pitfalls that easily (and frequently) result in unforeseen
  races (i.e., program corruption) deadlocks (i.e., program lockup) and
  performance cliffs (e.g., priority inversion, convoying, and sometimes
  complete loss of parallelism and/or even worse performance than a
  single-threaded program). And even when a correct and efficient concurrent
  program is written, it takes great care to maintain - it's usually brittle
  and difficult to maintain correctly because current programming models
  set a very high bar of expertise required to reason reliably about the
  operation of concurrent programs, so that apparently innocuous changes to a
  working concurrent program can (and commonly do, in practice) render it
  entirely or intermittently nonworking in unintended and unexpected ways.
  Because getting it right and keeping it right is so difficult, at many
  major software companies there is a veritable priesthood of gurus who write
  and maintain the core concurrent code.
  </em> @n - [Herb Sutter][1]

  We offer an alternative approach to traditional synchronization methods, with
  these features:

  - Fast paths are lock free.

  - Deadlocks are avoided.

  - Object histories are [linearizable][2].

  - Invocations and responses run in bounded time.

  The resulting concurrent system is simpler to design and analyze, higher in
  theoretical performance, and easier to maintain over the traditional approach.

  ## Traditional Approach

  The traditional approach uses a [critical section][3] (implemented by the JUCE
  object of the same name, CriticalSection) to protect a shared state and
  enforce mutual exclusion of access. We will consider a simplified example of
  a shared state consisting of the playback speed, owned by the
  AudioDeviceIOCallback, and the user interface logic owned by the message
  thread. In our example, the user interface queries the audio thread to
  get the initial playback speed, and changes the shared state in response
  to user interface actions:

  @code

  class AudioThread : public AudioDeviceIOCallback
  {
  public:
    // This lock protects our shared state
    CriticalSection& getCriticalSection ();

    void setPlaybackSpeed ()
    ResamplingAudioSource& getResampler ();

    //...
    
    void audioDeviceIOCallback (...)
    {
      // Take the lock to access the shared state
      {
        CriticalSection::ScopedLockType lock (getCriticalSection ());
      }
    }
  };

  AudioThread audioThread;

  class Gui
  {
  public:
    // Called when the user changes the resampling ratio
    void userChangedResamplingRatio (double newRatio)
    {
      
    }
  };

  @endcode

  ## Deadlock

  Calling into unknown code can cause deadlock.

  [1]: http://drdobbs.com/cpp/184401930 "The Trouble with Locks"
  [2]: http://en.wikipedia.org/wiki/Linearizability "Linearizability"
  [3]: http://en.wikipedia.org/wiki/Critical_section "Critical section"

  @defgroup vf_concurrent vf_concurrent
*/

#include "../vf_core/vf_core.h"

#include "modules/juce_gui_basics/juce_gui_basics.h"

namespace vf
{

#include "memory/vf_AllocatedBy.h"
#include "memory/vf_FifoFreeStore.h"
#if VF_USE_BOOST
#include "memory/vf_FifoFreeStoreWithTLS.h"
#else
#include "memory/vf_FifoFreeStoreWithoutTLS.h"
#endif
#include "memory/vf_GlobalFifoFreeStore.h"
#include "memory/vf_GlobalPagedFreeStore.h"
#include "memory/vf_PagedFreeStore.h"

#include "threads/vf_ReadWriteMutex.h"
#include "threads/vf_ThreadGroup.h"

#include "threads/vf_CallQueue.h"
#include "threads/vf_ConcurrentObject.h"
#include "threads/vf_ConcurrentState.h"
#include "threads/vf_GlobalThreadGroup.h"
#include "threads/vf_Listeners.h"
#include "threads/vf_ManualCallQueue.h"
#include "threads/vf_ParallelFor.h"
#include "threads/vf_ThreadWithCallQueue.h"

#include "threads/vf_GuiCallQueue.h"

#include "threads/vf_MessageThread.h"

}

#endif

