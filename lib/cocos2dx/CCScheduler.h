/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2011      Zynga Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __CCSCHEDULER_H__
#define __CCSCHEDULER_H__

#include "cocoa/CCObject.h"

NS_CC_BEGIN

/**
 * @addtogroup global
 * @{
 */

//
// CCTimer
//
/** @brief Light-weight timer */
//
class CC_DLL CCTimer : public CCObject
{
public:
    /** Allocates a timer with a selector. */
    static CCTimer *timerWithSelector(SEL_SCHEDULE selector, CCObject *target, float interval, int repeat, float delay, bool paused);
    /** Allocates a timer with a update for target. */
    static CCTimer *timerWithUpdateForTarget(CCObject *target, bool paused);
    /** Allocates a timer with a script callback function. */
    static CCTimer *timerWithScriptHandler(int handler, float interval, int repeat, float delay, bool paused);

    virtual ~CCTimer(void);

    CCObject *getTarget(void);
    SEL_SCHEDULE getSelector(void);
    int getScriptHandler(void);
    bool isPaused(void) const;
    void setPaused(bool paused);

    /** used for internal */
    void setRemoved(bool removed);
    inline bool isRemoved(void) const;

    /** triggers the timer */
    void update(float dt);

private:
    CCTimer(void);
    bool initWithSelector(SEL_SCHEDULE selector, CCObject *target, float interval, int repeat, float delay, bool paused);
    bool initWithUpdateForTarget(CCObject *target, bool paused);
    bool initWithScriptHandler(int handler, float interval, int repeat, float delay, bool paused);

    bool m_paused;
    bool m_removed;
    float m_elapsed;
    int m_timesExecuted;
    int m_repeat; // kCCRepeatForever = repeat forever, 0 = once, 1 is 2 x executed
    float m_delay;
    float m_interval;

    CCObject *m_target;
    SEL_SCHEDULE m_selector;
    int m_scriptHandler;
};

//
// CCScheduler
//

class CCArray;

/** @brief Scheduler is responsible for triggering the scheduled callbacks.
 You should not use NSTimer. Instead use this class.

 There are 2 different types of callbacks (selectors):

 - update selector: the 'update' selector will be called every frame.
 - custom selector: A custom selector will be called every frame, or with a custom interval of time

 The 'custom selectors' should be avoided when possible. It is faster, and consumes less memory to use the 'update selector'.

 */
class CC_DLL CCScheduler : public CCObject
{
public:
    static CCScheduler *create(void);
    virtual ~CCScheduler(void);

    /** The scheduled method will be called every 'interval' seconds.
     If paused is YES, then it won't be called until it is resumed.
     If 'interval' is 0, it will be called every frame, but if so, it's recommended to use 'scheduleUpdateForTarget:' instead.
     repeat let the action be repeated repeat + 1 times, use kCCRepeatForever to let the action run continuously
     delay is the amount of time the action will wait before it'll start

     @since v0.99.3, repeat and delay added in v1.1
     */
    void scheduleSelector(SEL_SCHEDULE selector, CCObject *target, float interval, int repeat, float delay, bool paused);
    /** Unschedule a selector for a given target.
     If you want to unschedule the "update", use unscheudleUpdateForTarget.
     @since v0.99.3
     */
    void unscheduleSelector(SEL_SCHEDULE selector, CCObject *target);
    /** Pauses a selector for a given target.
     @since v0.99.3
     */
    void pauseSelector(SEL_SCHEDULE selector, CCObject *target);
    /** Resumes a selector for a given target.
     @since v0.99.3
     */
    void resumeSelector(SEL_SCHEDULE selector, CCObject *target);
    /** Returns whether or not the target is paused
     @since v1.0.0
     */
    bool isSelectorPaused(SEL_SCHEDULE selector, CCObject *target);

    /** Schedules the 'update' selector for a given target.
     The 'update' selector will be called every frame.
     @since v0.99.3
     */
    void scheduleUpdateForTarget(CCObject *target, bool paused);
    /** Unschedules the update selector for a given target
     @since v0.99.3
     */
    void unscheduleUpdateForTarget(CCObject *target);
    /** Pauses the target.
     All scheduled selectors/update for a given target won't be 'ticked' until the target is resumed.
     If the target is not present, nothing happens.
     @since v0.99.3
     */
    void pauseUpdateForTarget(CCObject *target);
    /** Resumes the target.
     The 'target' will be unpaused, so all schedule selectors/update will be 'ticked' again.
     If the target is not present, nothing happens.
     @since v0.99.3
     */
    void resumeUpdateForTarget(CCObject *target);
    /** Returns whether or not the target is paused
     @since v1.0.0
     */
    bool isUpdateForTargetPaused(CCObject *target);

    /** The scheduled script handler will be called every 'interval' seconds.
     If paused is YES, then it won't be called until it is resumed.
     If 'interval' is 0, it will be called every frame, but if so, it's recommended to use 'scheduleUpdateForTarget:' instead.
     repeat let the action be repeated repeat + 1 times, use kCCRepeatForever to let the action run continuously
     delay is the amount of time the action will wait before it'll start
     return script handler, used for unscheduleScriptHandler(), pauseScriptHandler(), resumeScriptHandler(), isScriptHandlerPaused().
     */
    int scheduleScriptHandler(int handler, float interval, unsigned int repeat, float delay, bool paused);
    /** Unschedule the script handler. */
    void unscheduleScriptHandler(int handler);
    /** Pauses the script handler.
     @since v0.99.3
     */
    void pauseScriptHandler(int handler);
    /** Resumes the script handler.
     @since v0.99.3
     */
    void resumeScriptHandler(int handler);
    /** Returns whether or not the script handler is paused
     @since v1.0.0
     */
    bool isScriptHandlerPaused(int handler);

    /** Unschedules all selectors from all targets.
     You should NEVER call this method, unless you know what you are doing.
     @since v0.99.3
     */
    void unscheduleAll(void);

    /** Pause all schedule.
     @since v2.0.0
     */
    void pauseAll();
    /** Resume all paused schedule.
     */
    void resumeAll();

    /** 'update' the scheduler.
     You should NEVER call this method, unless you know what you are doing.
     */
    virtual void update(float dt);

private:
    CCScheduler(void);
    
    CCArray *m_timers;
};

// end of global group
/// @}

NS_CC_END

#endif // __CCSCHEDULER_H__
