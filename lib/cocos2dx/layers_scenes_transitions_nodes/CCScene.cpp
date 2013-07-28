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

#include "CCScene.h"
#include "support/CCPointExtension.h"
#include "CCDirector.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "keypad_dispatcher/CCKeypadDispatcher.h"
#include "CCAccelerometer.h"
#include "support/CCPointExtension.h"
#include "script_support/CCLuaStack.h"
#include "shaders/CCShaderCache.h"
#include "shaders/CCGLProgram.h"
#include "shaders/ccGLStateCache.h"
#include "support/TransformUtils.h"
// extern
#include "kazmath/GL/matrix.h"

NS_CC_BEGIN

CCScene::CCScene()
: m_bTouchEnabled(false)
, m_bAccelerometerEnabled(false)
, m_bKeypadEnabled(false)
, m_pScriptTouchHandlerEntry(NULL)
, m_nTouchPriority(0)
, m_eTouchMode(kCCTouchesAllAtOnce)
{
    m_bIgnoreAnchorPointForPosition = true;
    setAnchorPoint(ccp(0.5f, 0.5f));
}

CCScene::~CCScene()
{
    unregisterScriptTouchHandler();
    unregisterScriptKeypadHandler();
    unregisterScriptAccelerateHandler();
}

bool CCScene::init()
{
    bool bRet = false;
     do 
     {
         CCDirector * pDirector;
         CC_BREAK_IF( ! (pDirector = CCDirector::sharedDirector()) );
         this->setContentSize(pDirector->getWinSize());
         // success
         bRet = true;
     } while (0);
     return bRet;
}

CCScene *CCScene::create()
{
    CCScene *pRet = new CCScene();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}


/// Touch and Accelerometer related

void CCScene::registerWithTouchDispatcher()
{
    CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();

    // Using LuaBindings
    if (m_pScriptTouchHandlerEntry)
    {
        //	    if (m_pScriptTouchHandlerEntry->isMultiTouches())
        //	    {
        //	       pDispatcher->addStandardDelegate(this, 0);
        //	       LUALOG("[LUA] Add multi-touches event handler: %d", m_pScriptTouchHandlerEntry->getHandler());
        //	    }
        //	    else
        //	    {
        //	       pDispatcher->addTargetedDelegate(this,
        //						m_pScriptTouchHandlerEntry->getPriority(),
        //						m_pScriptTouchHandlerEntry->getSwallowsTouches());
        //	       LUALOG("[LUA] Add touch event handler: %d", m_pScriptTouchHandlerEntry->getHandler());
        //	    }
    }
    else
    {
        if( m_eTouchMode == kCCTouchesAllAtOnce ) {
            pDispatcher->addStandardDelegate(this, 0);
        } else {
            pDispatcher->addTargetedDelegate(this, m_nTouchPriority, true);
        }
    }
}

void CCScene::registerScriptTouchHandler(int nHandler, bool bIsMultiTouches, int nPriority, bool bSwallowsTouches)
{
    unregisterScriptTouchHandler();
    //    m_pScriptTouchHandlerEntry = CCTouchScriptHandlerEntry::create(nHandler, bIsMultiTouches, nPriority, bSwallowsTouches);
    //    m_pScriptTouchHandlerEntry->retain();
}

void CCScene::unregisterScriptTouchHandler(void)
{
//    CC_SAFE_RELEASE_NULL(m_pScriptTouchHandlerEntry);
}

int CCScene::excuteScriptTouchHandler(int nEventType, CCTouch *pTouch)
{
    //    return CCScriptEngineManager::sharedManager()->getScriptEngine()->executeLayerTouchEvent(this, nEventType, pTouch);
}

int CCScene::excuteScriptTouchHandler(int nEventType, CCSet *pTouches)
{
    //    return CCScriptEngineManager::sharedManager()->getScriptEngine()->executeLayerTouchesEvent(this, nEventType, pTouches);
}

/// isTouchEnabled getter
bool CCScene::isTouchEnabled()
{
    return m_bTouchEnabled;
}
/// isTouchEnabled setter
void CCScene::setTouchEnabled(bool enabled)
{
    if (m_bTouchEnabled != enabled)
    {
        m_bTouchEnabled = enabled;
        if (m_bRunning)
        {
            if (enabled)
            {
                this->registerWithTouchDispatcher();
            }
            else
            {
                // have problems?
                CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
            }
        }
    }
}

void CCScene::setTouchMode(ccTouchesMode mode)
{
    if(m_eTouchMode != mode)
    {
        m_eTouchMode = mode;

		if( m_bTouchEnabled)
        {
			setTouchEnabled(false);
			setTouchEnabled(true);
		}
    }
}

void CCScene::setTouchPriority(int priority)
{
    if (m_nTouchPriority != priority)
    {
        m_nTouchPriority = priority;

		if( m_bTouchEnabled)
        {
			setTouchEnabled(false);
			setTouchEnabled(true);
		}
    }
}

int CCScene::getTouchPriority()
{
    return m_nTouchPriority;
}

int CCScene::getTouchMode()
{
    return m_eTouchMode;
}

/// isAccelerometerEnabled getter
bool CCScene::isAccelerometerEnabled()
{
    return m_bAccelerometerEnabled;
}
/// isAccelerometerEnabled setter
void CCScene::setAccelerometerEnabled(bool enabled)
{
    if (enabled != m_bAccelerometerEnabled)
    {
        m_bAccelerometerEnabled = enabled;

        if (m_bRunning)
        {
            CCDirector* pDirector = CCDirector::sharedDirector();
            if (enabled)
            {
                pDirector->getAccelerometer()->setDelegate(this);
            }
            else
            {
                pDirector->getAccelerometer()->setDelegate(NULL);
            }
        }
    }
}


void CCScene::setAccelerometerInterval(double interval) {
    if (m_bAccelerometerEnabled)
    {
        if (m_bRunning)
        {
            CCDirector* pDirector = CCDirector::sharedDirector();
            pDirector->getAccelerometer()->setAccelerometerInterval(interval);
        }
    }
}


void CCScene::didAccelerate(CCAcceleration* pAccelerationValue)
{
    CC_UNUSED_PARAM(pAccelerationValue);
//    CCScriptEngineManager::sharedManager()->getScriptEngine()->executeAccelerometerEvent(this, pAccelerationValue);
}

void CCScene::registerScriptAccelerateHandler(int nHandler)
{
    unregisterScriptAccelerateHandler();
//    m_pScriptAccelerateHandlerEntry = CCScriptHandlerEntry::create(nHandler);
//    m_pScriptAccelerateHandlerEntry->retain();
}

void CCScene::unregisterScriptAccelerateHandler(void)
{
//    CC_SAFE_RELEASE_NULL(m_pScriptAccelerateHandlerEntry);
}

/// isKeypadEnabled getter
bool CCScene::isKeypadEnabled()
{
    return m_bKeypadEnabled;
}
/// isKeypadEnabled setter
void CCScene::setKeypadEnabled(bool enabled)
{
    if (enabled != m_bKeypadEnabled)
    {
        m_bKeypadEnabled = enabled;

        if (m_bRunning)
        {
            CCDirector* pDirector = CCDirector::sharedDirector();
            if (enabled)
            {
                pDirector->getKeypadDispatcher()->addDelegate(this);
            }
            else
            {
                pDirector->getKeypadDispatcher()->removeDelegate(this);
            }
        }
    }
}

void CCScene::registerScriptKeypadHandler(int nHandler)
{
    unregisterScriptKeypadHandler();
//    m_pScriptKeypadHandlerEntry = CCScriptHandlerEntry::create(nHandler);
//    m_pScriptKeypadHandlerEntry->retain();
}

void CCScene::unregisterScriptKeypadHandler(void)
{
//    CC_SAFE_RELEASE_NULL(m_pScriptKeypadHandlerEntry);
}

void CCScene::keyBackClicked(void)
{
//    if (m_pScriptKeypadHandlerEntry)
//    {
//        CCScriptEngineManager::sharedManager()->getScriptEngine()->executeLayerKeypadEvent(this, kTypeBackClicked);
//    }
}

void CCScene::keyMenuClicked(void)
{
//    if (m_pScriptKeypadHandlerEntry)
//    {
//        CCScriptEngineManager::sharedManager()->getScriptEngine()->executeLayerKeypadEvent(this, kTypeMenuClicked);
//    }
}

/// Callbacks
void CCScene::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    // register 'parent' nodes first
    // since events are propagated in reverse order
    if (m_bTouchEnabled)
    {
        this->registerWithTouchDispatcher();
    }

    // then iterate over all the children
    CCNode::onEnter();

    // add this layer to concern the Accelerometer Sensor
    if (m_bAccelerometerEnabled)
    {
        pDirector->getAccelerometer()->setDelegate(this);
    }

    // add this layer to concern the keypad msg
    if (m_bKeypadEnabled)
    {
        pDirector->getKeypadDispatcher()->addDelegate(this);
    }
}

void CCScene::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    if( m_bTouchEnabled )
    {
        pDirector->getTouchDispatcher()->removeDelegate(this);
        // [lua]:don't unregister script touch handler, or the handler will be destroyed
        // unregisterScriptTouchHandler();
    }

    // remove this layer from the delegates who concern Accelerometer Sensor
    if (m_bAccelerometerEnabled)
    {
        pDirector->getAccelerometer()->setDelegate(NULL);
    }

    // remove this layer from the delegates who concern the keypad msg
    if (m_bKeypadEnabled)
    {
        pDirector->getKeypadDispatcher()->removeDelegate(this);
    }

    CCNode::onExit();
}

void CCScene::onEnterTransitionDidFinish()
{
    if (m_bAccelerometerEnabled)
    {
        CCDirector* pDirector = CCDirector::sharedDirector();
        pDirector->getAccelerometer()->setDelegate(this);
    }

    CCNode::onEnterTransitionDidFinish();
}

bool CCScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    //    return excuteScriptTouchHandler(CCTOUCHBEGAN, pTouch) == 0 ? false : true;

    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
    CCAssert(false, "Layer#ccTouchBegan override me");
    return true;
}

void CCScene::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    //        excuteScriptTouchHandler(CCTOUCHMOVED, pTouch);
    //        return;

    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
}

void CCScene::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    //    excuteScriptTouchHandler(CCTOUCHENDED, pTouch);
    //        return;

    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
}

void CCScene::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    //        excuteScriptTouchHandler(CCTOUCHCANCELLED, pTouch);
    //        return;

    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
}

void CCScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    //        excuteScriptTouchHandler(CCTOUCHBEGAN, pTouches);
    //        return;

    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

void CCScene::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
    //        excuteScriptTouchHandler(CCTOUCHMOVED, pTouches);
    //        return;

    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

void CCScene::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
    //        excuteScriptTouchHandler(CCTOUCHENDED, pTouches);
    //        return;

    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

void CCScene::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
    //        excuteScriptTouchHandler(CCTOUCHCANCELLED, pTouches);
    //        return;
    
    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}



NS_CC_END
