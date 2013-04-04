/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
 \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_


#include "BaseApplication.h"
#include "CWorldMap.h"


class TutorialApplication : public BaseApplication
{
public:
    TutorialApplication(void);
    virtual ~TutorialApplication(void);

protected:
	
    virtual void createScene(void);
    virtual bool frameStarted (const Ogre::FrameEvent &evt);
	virtual void createFrameListener(void);

	// OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

private:

	CEGUI::OgreRenderer* mRenderer;
    CWorldMap m_WorldMap;
	UIManager *uiMgr;
	LevelManager *mLvMgr;

	//Camera Control
	bool mCamTurnLeft;
	bool mCamTurnRight;
	bool mCamTurnUp;
	bool mCamTurnDown;
	double mCameraSpeed;
	Ogre::SceneNode* mTarget;
	bool mCamToggle;
	
	//Mouse State
	bool mLMouseDown,mRMouseDown;
	double mRotateSpeed;
	Ogre::RaySceneQuery* mRayScnQuery;	//pointer to our ray scene query
	Ogre::SceneNode* Selected;

	OgreUtils::DirectShowMovieTexture* mDshowMovieTextureSystem;
	bool mCreatedCEGUI;
};

#endif // #ifndef __TutorialApplication_h_
