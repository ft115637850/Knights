/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
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

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace spine;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    skeletonNode = SkeletonAnimation::createWithJsonFile("spineboy-pro.json", "spineboy.atlas", 0.6f);
    skeletonNode->setPosition(Vec2(_contentSize.width / 2, 20));
    addChild(skeletonNode);

    // Queue the "walk" animation on the first track.
    skeletonNode->setAnimation(0, "walk", true);

    // Queue the "aim" animation on a higher track.
    // It consists of a single frame that positions
    // the back arm and gun such that they point at
    // the "crosshair" bone. By setting this
    // animation on a higher track, it overrides
    // any changes to the back arm and gun made
    // by the walk animation, allowing us to
    // mix the two. The mouse position following
    // is performed in the lambda below.
    skeletonNode->setAnimation(1, "aim", true);

    // Next we setup a listener that receives and stores
    // the current mouse location. The location is converted
    // to the skeleton's coordinate system.
    EventListenerMouse* mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = [this](cocos2d::Event* event) -> void {
        // convert the mosue location to the skeleton's coordinate space
        // and store it.
        EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
        position = skeletonNode->convertToNodeSpace(mouseEvent->getLocationInView());
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    // Position the "crosshair" bone at the mouse
    // location.
    //
    // When setting the crosshair bone position
    // to the mouse position, we need to translate
    // from "skeleton space" to "local bone space".
    // Note that the local bone space is calculated
    // using the bone's parent worldToLocal() function!
    //
    // After updating the bone position based on the
    // converted mouse location, we call updateWorldTransforms()
    // again so the change of the IK target position is
    // applied to the rest of the skeleton.
    skeletonNode->setPostUpdateWorldTransformsListener([this](SkeletonAnimation* node) -> void {
        Bone* crosshair = node->findBone("crosshair"); // The bone should be cached
        float localX = 0, localY = 0;
        crosshair->getParent()->worldToLocal(position.x, position.y, localX, localY);
        crosshair->setX(localX);
        crosshair->setY(localY);
        crosshair->setAppliedValid(false);

        node->getSkeleton()->updateWorldTransform();
    });
 
    return true;
}

void HelloWorld::update(float deltaTime)
{
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
