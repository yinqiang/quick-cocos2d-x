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

#include "CCScheduler.h"
#include "ccMacros.h"
#include "CCDirector.h"
#include "support/data_support/utlist.h"
#include "support/data_support/ccCArray.h"
#include "cocoa/CCArray.h"
#include "script_support/CCLuaStack.h"

using namespace std;

NS_CC_BEGIN

// implementation CCTimer

CCTimer::CCTimer(void)
: m_paused(false)
, m_removed(false)
, m_elapsed(-1)
, m_timesExecuted(0)
, m_repeat(kCCRepeatForever)
, m_delay(0)
, m_interval(0)
, m_target(NULL)
, m_selector(NULL)
, m_scriptHandler(0)
{
}

CCTimer::~CCTimer(void)
{
    CC_SAFE_RELEASE(m_target);
}

CCTimer *CCTimer::timerWithSelector(SEL_SCHEDULE selector, CCObject *target, float interval, int repeat, float delay, bool paused)
{
    CCTimer *timer = new CCTimer();
    timer->initWithSelector(selector, target, interval, repeat, delay, paused);
    timer->autorelease();
    return timer;
}

CCTimer *CCTimer::timerWithUpdateForTarget(CCObject *target, bool paused)
{
    CCTimer *timer = new CCTimer();
    timer->initWithUpdateForTarget(target, paused);
    timer->autorelease();
    return timer;
}

CCTimer *CCTimer::timerWithScriptHandler(int handler, float interval, int repeat, float delay, bool paused)
{
    CCTimer *timer = new CCTimer();
    timer->initWithScriptHandler(handler, interval, repeat, delay, paused);
    timer->autorelease();
    return timer;
}

bool CCTimer::initWithSelector(SEL_SCHEDULE selector, CCObject *target, float interval, int repeat, float delay, bool paused)
{
    m_target = target;
    m_target->retain();
    m_selector = selector;
    m_interval = interval;
    m_repeat = repeat;
    m_delay = delay;
    m_paused = paused;
    return true;
}

bool CCTimer::initWithUpdateForTarget(CCObject *target, bool paused)
{
    m_target = target;
    m_target->retain();
    m_paused = paused;
    return true;
}

bool CCTimer::initWithScriptHandler(int handler, float interval, int repeat, float delay, bool paused)
{
    m_scriptHandler = handler;
    m_interval = interval;
    m_repeat = repeat;
    m_delay = delay;
    m_paused = paused;
    return true;
}

CCObject *CCTimer::getTarget(void)
{
    return m_target;
}

SEL_SCHEDULE CCTimer::getSelector(void)
{
    return m_selector;
}

int CCTimer::getScriptHandler(void)
{
    return m_scriptHandler;
}

bool CCTimer::isPaused(void) const
{
    return m_paused;
}

void CCTimer::setPaused(bool paused)
{
    m_paused = paused;
}

void CCTimer::setRemoved(bool removed)
{
    m_removed = removed;
}

bool CCTimer::isRemoved(void) const
{
    return m_removed;
}

void CCTimer::update(float dt)
{
    if (m_paused || m_removed) return;

    if (m_elapsed == -1)
    {
        m_elapsed = 0;
        return;
    }

    if (m_delay > 0)
    {
        m_delay -= dt;
        if (m_delay <= 0)
        {
            m_elapsed = fabsf(m_delay);
        }
        return;
    }

    m_elapsed += dt;
    if (m_elapsed < m_interval) return;

    if (m_target)
    {
        if (m_selector)
        {
            (m_target->*m_selector)(m_elapsed);
        }
        else
        {
            m_target->update(m_elapsed);
        }
    }
    else if (m_scriptHandler)
    {
        CCLuaStack::defaultStack()->pushFloat(m_elapsed);
        CCLuaStack::defaultStack()->executeFunctionByHandler(m_scriptHandler, 1);
    }

    m_elapsed -= m_interval;

    if (m_repeat >= 0)
    {
        m_timesExecuted++;
        m_removed = m_timesExecuted >= m_repeat;
    }
}


// implementation of CCScheduler

CCScheduler *CCScheduler::create(void)
{
    CCScheduler *scheduler = new CCScheduler();
    scheduler->autorelease();
    return scheduler;
}

CCScheduler::CCScheduler(void)
{
    m_timers = CCArray::createWithCapacity(10);
    m_timers->retain();
}

CCScheduler::~CCScheduler(void)
{
    unscheduleAll();
    m_timers->release();
}

#define FOREACH_TIMERS_SELECTOR(fn) \
CCTimer *timer; \
for (int i = m_timers->count() - 1; i >= 0; --i) \
{ \
    timer = static_cast<CCTimer*>(m_timers->objectAtIndex(i)); \
    if (timer->getSelector() == selector && timer->getTarget() == target) \
    { \
        fn; \
    } \
}

void CCScheduler::scheduleSelector(SEL_SCHEDULE selector, CCObject *target, float interval, int repeat, float delay, bool paused)
{
    m_timers->addObject(CCTimer::timerWithSelector(selector, target, interval, repeat, delay, paused));
}

void CCScheduler::unscheduleSelector(SEL_SCHEDULE selector, CCObject *target)
{
    FOREACH_TIMERS_SELECTOR(timer->setRemoved(true));
}

void CCScheduler::pauseSelector(SEL_SCHEDULE selector, CCObject *target)
{
    FOREACH_TIMERS_SELECTOR(timer->setPaused(true));
}

void CCScheduler::resumeSelector(SEL_SCHEDULE selector, CCObject *target)
{
    FOREACH_TIMERS_SELECTOR(timer->setPaused(false));
}

bool CCScheduler::isSelectorPaused(SEL_SCHEDULE selector, CCObject *target)
{
    FOREACH_TIMERS_SELECTOR(return timer->isPaused());
    return false;
}

#define FOREACH_TIMERS_TARGET(fn) \
CCTimer *timer; \
for (int i = m_timers->count() - 1; i >= 0; --i) \
{ \
    timer = static_cast<CCTimer*>(m_timers->objectAtIndex(i)); \
    if (timer->getTarget() == target) \
    { \
        fn; \
    } \
}

void CCScheduler::scheduleUpdateForTarget(CCObject *target, bool paused)
{
    m_timers->addObject(CCTimer::timerWithUpdateForTarget(target, paused));
}

void CCScheduler::unscheduleUpdateForTarget(CCObject *target)
{
    FOREACH_TIMERS_TARGET(timer->setRemoved(true));
}

void CCScheduler::pauseUpdateForTarget(CCObject *target)
{
    FOREACH_TIMERS_TARGET(timer->setPaused(true));
}

void CCScheduler::resumeUpdateForTarget(CCObject *target)
{
    FOREACH_TIMERS_TARGET(timer->setPaused(false));
}

bool CCScheduler::isUpdateForTargetPaused(CCObject *target)
{
    FOREACH_TIMERS_TARGET(return timer->isPaused());
    return false;
}

#define FOREACH_TIMERS_SCRIPT_HANDLER(fn) \
CCTimer *timer; \
for (int i = m_timers->count() - 1; i >= 0; --i) \
{ \
    timer = static_cast<CCTimer*>(m_timers->objectAtIndex(i)); \
    if (timer->getScriptHandler() == handler) \
    { \
        fn; \
    } \
}

int CCScheduler::scheduleScriptHandler(int handler, float interval, unsigned int repeat, float delay, bool paused)
{
    m_timers->addObject(CCTimer::timerWithScriptHandler(handler, interval, repeat, delay, paused));
    return handler;
}

void CCScheduler::unscheduleScriptHandler(int handler)
{
    FOREACH_TIMERS_SCRIPT_HANDLER(timer->setRemoved(true));
}

void CCScheduler::pauseScriptHandler(int handler)
{
    FOREACH_TIMERS_SCRIPT_HANDLER(timer->setPaused(true));
}

void CCScheduler::resumeScriptHandler(int handler)
{
    FOREACH_TIMERS_SCRIPT_HANDLER(timer->setPaused(false));
}

bool CCScheduler::isScriptHandlerPaused(int handler)
{
    FOREACH_TIMERS_SCRIPT_HANDLER(return timer->isPaused());
    return false;
}


#define FOREACH_TIMERS(fn) \
CCTimer *timer; \
for (int i = m_timers->count() - 1; i >= 0; --i) \
{ \
    timer = static_cast<CCTimer*>(m_timers->objectAtIndex(i)); \
    fn; \
}

void CCScheduler::unscheduleAll(void)
{
    FOREACH_TIMERS(timer->setRemoved(true));
}

void CCScheduler::pauseAll()
{
    FOREACH_TIMERS(timer->setPaused(true));
}

void CCScheduler::resumeAll()
{
    FOREACH_TIMERS(timer->setPaused(false));
}

void CCScheduler::update(float dt)
{
    FOREACH_TIMERS(timer->update(dt));
    for (int i = m_timers->count() - 1; i >= 0; --i)
    {
        if (static_cast<CCTimer*>(m_timers->objectAtIndex(i))->isRemoved())
        {
            m_timers->removeObjectAtIndex(i);
        }
    }
}

NS_CC_END
