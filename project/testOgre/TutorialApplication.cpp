/*																					 
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
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
#include "stdafx.h"
//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
:mCamTurnLeft(false)
,mCamTurnRight(false)
,mCamTurnUp(false)
,mCamTurnDown(false)
,mCameraSpeed(70)
,mRotateSpeed(0.1)
,mLMouseDown(false)
,mRMouseDown(false)
,uiMgr(NULL)
,mRenderer(NULL)
,mCamToggle(false)
,mDshowMovieTextureSystem(NULL)
,mCreatedCEGUI(false)
,mLvMgr(NULL)
{
}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
	if (mLvMgr!=NULL)
	{
		delete mLvMgr;
		mLvMgr=NULL;
	}
	if (uiMgr!=NULL){
		delete uiMgr;
		uiMgr=NULL;

	} //if
	if (mRenderer!=NULL){
		CEGUI::OgreRenderer::destroySystem();
	} //if

	if (mDshowMovieTextureSystem!=NULL) {
		delete mDshowMovieTextureSystem;
		mDshowMovieTextureSystem=NULL;
	} //if
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
	using namespace Ogre;


	int x = mWindow->getWidth();
	int y = mWindow->getHeight();
	mDshowMovieTextureSystem = new OgreUtils::DirectShowMovieTexture(mWindow->getWidth(), mWindow->getHeight());
	Ogre::String movieName="teamvideo.avi";
	//mDshowMovieTextureSystem->loadMovie(movieName);
	//mDshowMovieTextureSystem->playMovie();

	Plane plane;
	plane.normal = Vector3::UNIT_Z;
	plane.d = 320;
	MeshPtr pGround = MeshManager::getSingleton().createPlane(
		"GroundPlane", // 模型名称
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane, // 平面定义
		mWindow->getWidth(), 
		mWindow->getHeight(), 
		1, // X 方向分割
		1, // Z 方向分割
		true, // 是否创建法线
		1, // 纹理坐标数量
		1, // U 方向纹理铺嵌的行数
		1, // V 方向纹理铺嵌的行数
		Vector3::UNIT_Y // 正面朝向
		);
	// 创建实体(地面)
	Entity* entGround = mSceneMgr->createEntity( "ground", "GroundPlane" );
	// 为地面设置材质
	entGround->setMaterialName( "Examples/Rockwall" );
	// 在场景根节点下创建一个子节点用于绑定这个地面实体
	SceneNode* groundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	// 把地面实体绑定到这个子节点
	groundNode->attachObject( entGround );
	groundNode->translate(x/2-320,-y/2+240,0);
	Ogre::String materialName="Examples/Rockwall";
	Ogre::MaterialPtr mat;
	Ogre::TextureUnitState* tex;
	if (!Ogre::MaterialManager::getSingleton().resourceExists(materialName)) {
		throw("Error, material doesn't exist!");
		return;
	} 

	mat=Ogre::MaterialManager::getSingleton().getByName(materialName);
	tex=mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	tex->setTextureName(mDshowMovieTextureSystem->getMovieTexture()->getName());


}
bool TutorialApplication::frameStarted (const Ogre::FrameEvent &evt)
{

	if (this->mDshowMovieTextureSystem->isPlayingMovie()) {
		mDshowMovieTextureSystem->updateMovieTexture();
	} else {
		if (mCreatedCEGUI == false) {
			//Set up CEGUI
			mSceneMgr->clearScene();
			mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
			//set up level manager
			mLvMgr=new LevelManager;
			mLvMgr->LoadLevelFile();

			//set up ui manager
			uiMgr=new UIManager;
			uiMgr->SetupUI(mSceneMgr,mLvMgr,&m_WorldMap);
			uiMgr->createMainMenuUI();

			mTarget=mSceneMgr->getRootSceneNode();
			mCreatedCEGUI = true;
		} else {
			if (!uiMgr->isCancel()) {
				this->m_WorldMap.Update(evt.timeSinceLastFrame,mLMouseDown);
				this->uiMgr->Update();
			}
		}
	}
	return true;
}

CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{

	switch (buttonID)
	{
	case OIS::MB_Left:
		return CEGUI::LeftButton;
		break;

	case OIS::MB_Right:
		return CEGUI::RightButton;
		break;

	case OIS::MB_Middle:
		return CEGUI::MiddleButton;
		break;

	default:
		return CEGUI::LeftButton;
		break;
	}
}

bool TutorialApplication::mouseMoved( const OIS::MouseEvent &arg )
{
	if (!mCreatedCEGUI){//正在播放动画 啥都不做
		return true;
	}

	Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();
	CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	//mCameraMan->injectMouseMove(arg);
	mCamera->setFixedYawAxis(false);
	if (mRMouseDown  && !uiMgr->isCancel())
	{
		mCamera->setPosition(mTarget->_getDerivedPosition());

		/*
		if(mCamera->getRealUp().dotProduct(Ogre::Vector3::UNIT_Y)>0)
		{
			mCamera->yaw(Ogre::Degree(-arg.state.X.rel * 0.25f));
		}
		else
		{
			mCamera->yaw(Ogre::Degree(arg.state.X.rel * 0.25f));
		}
		*/

		mCamera->yaw(Ogre::Degree(-arg.state.X.rel * 0.25f));
		mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * 0.25f));

		if (mCamToggle==false)
		{
			mCamera->moveRelative(Ogre::Vector3(0, 0, dist));
		}
		else
		{
			mCamera->moveRelative(Ogre::Vector3(0, 0, dist));
			mCamera->setPosition(mCamera->getPosition()*0.5);
		}

	}

	else if(!uiMgr->isCancel() && mLMouseDown==true)
	{
		CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(arg.state.width), mousePos.d_y/float(arg.state.height));
		mRayScnQuery->setRay(mouseRay);
		mRayScnQuery->setSortByDistance(true);
		mRayScnQuery->setQueryMask(ISLAND_MASK);
		// Execute query
		Ogre::RaySceneQueryResult &result = mRayScnQuery->execute();
		Ogre::RaySceneQueryResult::iterator iter= result.begin();

		// Get results, create a node/entity on the position
		bool selectflag=false;
		//for ( iter; iter != result.end(); iter++ )
		//{
			if (iter!=result.end() && iter->movable && iter->movable->getName().substr(0,2)=="is"
					&& iter->movable->getParentSceneNode()->getPosition().distance(mSceneMgr->getCamera("PlayerCam")->getPosition())<480)
			{
				m_WorldMap.OnMouseMoved(mLMouseDown,dynamic_cast<Ogre::Entity*>(iter->movable));
				selectflag=true;
				//break;
			} // if
		//} // for
		if(selectflag==false)
		{
			m_WorldMap.OnMouseMoved(mLMouseDown,NULL);
		}
	}
	return true;
}

bool TutorialApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (!mCreatedCEGUI){//正在播放动画 啥都不做
		return true;
	}
	CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
	//mCameraMan->injectMouseDown(arg, id);
	if(id == OIS::MB_Left  && !uiMgr->isCancel())
	{
		mLMouseDown=true;
		CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(arg.state.width), mousePos.d_y/float(arg.state.height));
		mRayScnQuery->setRay(mouseRay);
		mRayScnQuery->setSortByDistance(true);
		mRayScnQuery->setQueryMask(ISLAND_MASK);
		// Execute query
		Ogre::RaySceneQueryResult &result = mRayScnQuery->execute();
		Ogre::RaySceneQueryResult::iterator iter= result.begin();

		// Get results, create a node/entity on the position
		bool selectflag=false;
	/*	for ( iter; iter != result.end(); iter++ )
		{
			*/if (iter!=result.end() && iter->movable && iter->movable->getName().substr(0,2)=="is"
					&& iter->movable->getParentSceneNode()->getPosition().distance(mSceneMgr->getCamera("PlayerCam")->getPosition())<480)
			{
				//Selected = iter->movable->getParentSceneNode();
				//Selected->showBoundingBox(true);
				m_WorldMap.OnMousePressed(dynamic_cast<Ogre::Entity*>(iter->movable));
				selectflag=true;
				//break;
			} // if
			/*else
			{
			if (Selected!=NULL)
			{
			Selected->showBoundingBox(false);
			Selected=NULL;
			}
			}*/
		//} // for
		if(selectflag==false)
		{
			m_WorldMap.OnMousePressed(NULL);
		}
	}
	else if(id == OIS::MB_Right)
		mRMouseDown=true;
	return true;
}

bool TutorialApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (!mCreatedCEGUI){//正在播放动画 啥都不做
		return true;
	}
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
	if(id == OIS::MB_Left  && !uiMgr->isCancel())
	{
		mLMouseDown=false;
		CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(arg.state.width), mousePos.d_y/float(arg.state.height));
		mRayScnQuery->setRay(mouseRay);
		mRayScnQuery->setSortByDistance(true);
		mRayScnQuery->setQueryMask(ISLAND_MASK);
		// Execute query
		Ogre::RaySceneQueryResult &result = mRayScnQuery->execute();
		Ogre::RaySceneQueryResult::iterator iter= result.begin();

		// Get results, create a node/entity on the position
		bool selectflag=false;
		//for ( iter; iter != result.end(); iter++ )
		//{
			if (iter!=result.end() && iter->movable && iter->movable->getName().substr(0,2)=="is"
				&& iter->movable->getParentSceneNode()->getPosition().distance(mSceneMgr->getCamera("PlayerCam")->getPosition())<480)
			{
				//Selected = iter->movable->getParentSceneNode();
				//Selected->showBoundingBox(true);
				m_WorldMap.OnMouseReleased(dynamic_cast<Ogre::Entity*>(iter->movable));
				selectflag=true;
				//break;
			} // if
		//} // for
		if(selectflag==false)
		{
			m_WorldMap.OnMouseReleased(NULL);
		}
	}
	else if(id == OIS::MB_Right)
		mRMouseDown=false;
	//mCameraMan->injectMouseUp(arg, id);
	return true;
}

//-------------------------------------------------------------------------------------
bool TutorialApplication::keyPressed( const OIS::KeyEvent &arg )
{
	if (!mCreatedCEGUI){//正在播放动画 啥都不做
		return true;
	}
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(arg.key);
	sys.injectChar(arg.text);

	Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();
	if (arg.key == OIS::KC_ESCAPE)
	{
		mShutDown = true;
	}
	else if (arg.key == OIS::KC_LEFT||arg.key == OIS::KC_A)
	{
		mCamTurnRight=true;
	}
	else if (arg.key == OIS::KC_RIGHT||arg.key == OIS::KC_D)
	{
		mCamTurnLeft=true;
	}
	else if (arg.key == OIS::KC_UP ||arg.key == OIS::KC_W)
	{
		mCamTurnDown=true;
	}
	else if (arg.key == OIS::KC_DOWN||arg.key == OIS::KC_S)
	{
		mCamTurnUp=true;
	}
	else if (arg.key == OIS::KC_SPACE || arg.key == OIS::KC_RETURN)
	{
		if(mLvMgr->GetGameState()==0)
			m_WorldMap.DisableAnim();
	}
	else if (arg.key == OIS::KC_TAB)
	{
		if(mLvMgr->GetGameState()==0)
			m_WorldMap.DisableAnim();
		m_WorldMap.s_LvMgr->SetGameState(1);
	}
	//else if (arg.key == OIS::KC_SPACE)
	//{
	//	mCamToggle=!mCamToggle;
	//}

	//mCameraMan->injectKeyDown(arg);
	return true;
}

bool TutorialApplication::keyReleased( const OIS::KeyEvent &arg )
{
	if (!mCreatedCEGUI){//正在播放动画 啥都不做
		return true;
	}
	CEGUI::System::getSingleton().injectKeyUp(arg.key);
	//mCameraMan->injectKeyUp(arg);
	if (arg.key == OIS::KC_LEFT||arg.key == OIS::KC_A)
	{
		mCamTurnRight=false;
	}
	else if (arg.key == OIS::KC_RIGHT||arg.key == OIS::KC_D)
	{
		mCamTurnLeft=false;
	}
	else if (arg.key == OIS::KC_UP||arg.key == OIS::KC_W)
	{
		mCamTurnDown=false;
	}
	else if (arg.key == OIS::KC_DOWN||arg.key == OIS::KC_S)
	{
		mCamTurnUp=false;
	}	
	return true;
}

void TutorialApplication::createFrameListener(void)
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem( pl );

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);


	mLMouseDown=false;
	mRMouseDown=false;
	mRotateSpeed=0.1;
	mRayScnQuery = mSceneMgr->createRayQuery(Ogre::Ray());

	mRoot->addFrameListener(this);
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(mWindow->isClosed())
		return false;

	if(mShutDown)
		return false;


	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	if (!mCreatedCEGUI){//正在播放动画 啥都不做
		return true;
	} //if
	if (uiMgr->isQuit())
	{
		return false;
	}

	if (uiMgr->isCancel())
	{
		return true;
	}


	Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();
	mCamera->setPosition(mTarget->_getDerivedPosition());
	if (mCamTurnLeft)
	{
		if(mCamera->getRealUp().dotProduct(Ogre::Vector3::UNIT_Y)>0)
			mCamera->yaw(Ogre::Degree(-mCameraSpeed* evt.timeSinceLastFrame));
		else
			mCamera->yaw(Ogre::Degree(mCameraSpeed* evt.timeSinceLastFrame));

	}
	else if (mCamTurnRight)
	{
		if(mCamera->getRealUp().dotProduct(Ogre::Vector3::UNIT_Y)>0)
			mCamera->yaw(Ogre::Degree(mCameraSpeed* evt.timeSinceLastFrame));
		else
			mCamera->yaw(Ogre::Degree(-mCameraSpeed* evt.timeSinceLastFrame));
	}
	else if (mCamTurnUp)
	{
		mCamera->pitch(Ogre::Degree(-mCameraSpeed* evt.timeSinceLastFrame));
	}
	else if (mCamTurnDown)
	{
		mCamera->pitch(Ogre::Degree(mCameraSpeed* evt.timeSinceLastFrame));
	}
	mCamera->moveRelative(Ogre::Vector3(0, 0, 450));

	return true;
}


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
	int main(int argc, char *argv[])
#endif
	{
		// Create application object
		TutorialApplication app;

		try {
			app.go();
		} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occured: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif

