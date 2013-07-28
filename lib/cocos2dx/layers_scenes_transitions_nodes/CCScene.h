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

#ifndef __CCSCENE_H__
#define __CCSCENE_H__

#include "base_nodes/CCNode.h"
#include "CCProtocols.h"
#include "touch_dispatcher/CCTouchDelegateProtocol.h"
#include "platform/CCAccelerometerDelegate.h"
#include "keypad_dispatcher/CCKeypadDelegate.h"
#include "cocoa/CCArray.h"

NS_CC_BEGIN

/**
 * @addtogroup scene
 * @{
 */

typedef enum {
	kCCTouchesAllAtOnce,
	kCCTouchesOneByOne,
} ccTouchesMode;

class CCTouchScriptHandlerEntry;

/** @brief CCScene is a subclass of CCNode that is used only as an abstract concept.

CCScene an CCNode are almost identical with the difference that CCScene has it's
anchor point (by default) at the center of the screen.

For the moment CCScene has no other logic than that, but in future releases it might have
additional logic.

It is a good practice to use and CCScene as the parent of all your nodes.
*/
class CC_DLL CCScene : public CCNode, public CCTouchDelegate, public CCAccelerometerDelegate, public CCKeypadDelegate
{
public:
    CCScene();
    virtual ~CCScene();
    bool init();

    static CCScene *create(void);


    virtual void onEnter();
    virtual void onExit();
    virtual void onEnterTransitionDidFinish();

    // default implements are used to call script callback if exist
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);

    // default implements are used to call script callback if exist
    virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);

    virtual void didAccelerate(CCAcceleration* pAccelerationValue);
    void registerScriptAccelerateHandler(int nHandler);
    void unregisterScriptAccelerateHandler(void);

    /** If isTouchEnabled, this method is called onEnter. Override it to change the
     way CCLayer receives touch events.
     ( Default: CCTouchDispatcher::sharedDispatcher()->addStandardDelegate(this,0); )
     Example:
     void CCLayer::registerWithTouchDispatcher()
     {
     CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this,INT_MIN+1,true);
     }
     @since v0.8.0
     */
    virtual void registerWithTouchDispatcher(void);

    /** Register script touch events handler */
    virtual void registerScriptTouchHandler(int nHandler, bool bIsMultiTouches = false, int nPriority = INT_MIN, bool bSwallowsTouches = false);
    /** Unregister script touch events handler */
    virtual void unregisterScriptTouchHandler(void);

    /** whether or not it will receive Touch events.
     You can enable / disable touch events with this property.
     Only the touches of this node will be affected. This "method" is not propagated to it's children.
     @since v0.8.1
     */
    virtual bool isTouchEnabled();
    virtual void setTouchEnabled(bool value);

    virtual void setTouchMode(ccTouchesMode mode);
    virtual int getTouchMode();

    /** priority of the touch events. Default is 0 */
    virtual void setTouchPriority(int priority);
    virtual int getTouchPriority();

    /** whether or not it will receive Accelerometer events
     You can enable / disable accelerometer events with this property.
     @since v0.8.1
     */
    virtual bool isAccelerometerEnabled();
    virtual void setAccelerometerEnabled(bool value);
    virtual void setAccelerometerInterval(double interval);

    /** whether or not it will receive keypad events
     You can enable / disable accelerometer events with this property.
     it's new in cocos2d-x
     */
    virtual bool isKeypadEnabled();
    virtual void setKeypadEnabled(bool value);

    /** Register keypad events handler */
    void registerScriptKeypadHandler(int nHandler);
    /** Unregister keypad events handler */
    void unregisterScriptKeypadHandler(void);

    virtual void keyBackClicked(void);
    virtual void keyMenuClicked(void);

private:
    bool m_bTouchEnabled;
    bool m_bAccelerometerEnabled;
    bool m_bKeypadEnabled;

    // Script touch events handler
    CCTouchScriptHandlerEntry* m_pScriptTouchHandlerEntry;

    int m_nTouchPriority;
    ccTouchesMode m_eTouchMode;

    int  excuteScriptTouchHandler(int nEventType, CCTouch *pTouch);
    int  excuteScriptTouchHandler(int nEventType, CCSet *pTouches);

};

// end of scene group
/// @}

NS_CC_END

#endif // __CCSCENE_H__
