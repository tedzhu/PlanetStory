#include "ExampleApplication.h"

#include <deque>
using namespace std;

class MoveDemoListener : public ExampleFrameListener
{
public:

    MoveDemoListener(RenderWindow* win, Camera* cam, SceneNode *sn,
        Entity *ent, std::deque<Vector3> &walk)
        : ExampleFrameListener(win, cam, false, false), mNode(sn), mEntity(ent), mWalkList( walk )
    {
        mWalkSpeed = 35.0f;
        mDirection = Vector3::ZERO;


    } // MoveDemoListener

    /* This function is called to start the object moving to the next position
    in mWalkList.
    */
    bool nextLocation( )
    {
        if (mWalkList.empty())
            return false;
        mDestination = mWalkList.front();  // this gets the front of the deque
        mWalkList.pop_front();             // this removes the front of the deque
        mDirection = mDestination - mNode->getPosition();
        mDistance = mDirection.normalise();

        return true;
    } // nextLocation( )

    bool frameStarted(const FrameEvent &evt)
    {
        if (mDirection == Vector3::ZERO) {
            if (nextLocation()){
                // Set walking animation
                mAnimationState = mEntity->getAnimationState("Walk");
                mAnimationState->setLoop(true);
                mAnimationState->setEnabled(true);
            }
        } else{
            Real move = mWalkSpeed * evt.timeSinceLastFrame;
            mDistance -= move;
            if (mDistance <= 0.0f){
                mNode->setPosition(mDestination);
                mDirection = Vector3::ZERO;
                if (! nextLocation()){
                    // Set Idle animation                     
                    mAnimationState = mEntity->getAnimationState("Idle");
                    mAnimationState->setLoop(true);
                    mAnimationState->setEnabled(true);
                } else{
                    // Rotation Code will go here later
                    Vector3 src = mNode->getOrientation() * Vector3::UNIT_X;
                    Ogre::Quaternion quat = src.getRotationTo(mDirection);
                    if ((1.0f + src.dotProduct(mDirection)) < 0.0001f) {
                        mNode->yaw(Degree(180));
                    }else{
                        Ogre::Quaternion quat = src.getRotationTo(mDirection);
                        mNode->rotate(quat);
                    } // else
                }
            } else{
                mNode->translate(mDirection * move);
            } // else
        }
        mAnimationState->addTime(evt.timeSinceLastFrame);
        return ExampleFrameListener::frameStarted(evt);
    }
protected:
    Real mDistance;                  // The distance the object has left to travel
    Vector3 mDirection;              // The direction the object is moving
    Vector3 mDestination;            // The destination the object is moving towards

    AnimationState *mAnimationState; // The current animation state of the object

    Entity *mEntity;                 // The Entity we are animating
    SceneNode *mNode;                // The SceneNode that the Entity is attached to
    std::deque<Vector3> mWalkList;   // The list of points we are walking to

    Real mWalkSpeed;                 // The speed at which the object is moving
};


class MoveDemoApplication : public ExampleApplication
{
protected:
public:
    MoveDemoApplication()
    {
    }

    ~MoveDemoApplication() 
    {
    }
protected:
    Entity *mEntity;                // The entity of the object we are animating
    SceneNode *mNode;               // The SceneNode of the object we are moving
    std::deque<Vector3> mWalkList;  // A deque containing the waypoints

    void createScene(void)
    {
        mSceneMgr->setAmbientLight( ColourValue( 1.0f, 1.0f, 1.0f ) );
        mEntity = mSceneMgr->createEntity( "Robot", "robot.mesh" );

        // Create the scene node
        mNode = mSceneMgr->getRootSceneNode( )->
            createChildSceneNode( "RobotNode", Vector3( 0.0f, 0.0f, 25.0f ) );
        mNode->attachObject( mEntity );
        mWalkList.push_back( Vector3( 550.0f,  0.0f,  50.0f  ) );
        mWalkList.push_back( Vector3(-100.0f,  0.0f, -200.0f ) );
        Entity *ent;
        SceneNode *node;

        ent = mSceneMgr->createEntity( "Knot1", "knot.mesh" );
        node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot1Node",
            Vector3(  0.0f, -10.0f,  25.0f ) );
        node->attachObject( ent );
        node->setScale( 0.1f, 0.1f, 0.1f );

        ent = mSceneMgr->createEntity( "Knot2", "knot.mesh" );
        node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot2Node",
            Vector3( 550.0f, -10.0f,  50.0f ) );
        node->attachObject( ent );
        node->setScale( 0.1f, 0.1f, 0.1f );

        ent = mSceneMgr->createEntity( "Knot3", "knot.mesh" );
        node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot3Node",
            Vector3(-100.0f, -10.0f,-200.0f ) );
        node->attachObject( ent );
        node->setScale( 0.1f, 0.1f, 0.1f );
        // Set the camera to look at our handywork
        mCamera->setPosition( 90.0f, 280.0f, 535.0f );
        mCamera->pitch( Degree(-30.0f) );
        mCamera->yaw( Degree(-15.0f) );

    }

    void createFrameListener(void)
    {
        mFrameListener= new MoveDemoListener(mWindow, mCamera, mNode, mEntity, mWalkList);
        mFrameListener->showDebugOverlay(true);
        mRoot->addFrameListener(mFrameListener);
    }

};


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
    // Create application object
    MoveDemoApplication app;

    try {
        app.go();
    } catch( Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!",
            MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occured: %s\n",
            e.getFullDescription().c_str());
#endif
    }


    return 0;
}
