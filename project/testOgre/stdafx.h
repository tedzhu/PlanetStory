#pragma once

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include <list>
#include <set>
#include <map>
#include <vector>
#include <CEGUI.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>
#include <cmath>
#include <cstdlib>
#include <OgreParticleSystem.h>
#include "UtilsOgreDshow.h"
#include <cassert>
#include <utility>
#include <fstream>
#include <ctime>
class CIsland;
class CForceController;
class CEnemyController;
class CPlayerController;
class CWorldMap;
class CGas;
class TiXmlNode;


#include "UIManager.h"
#include "BaseApplication.h"
#include "TutorialApplication.h"
#include "CForceController.h"
#include "CEnemyController.h"
#include "CPlayerController.h"
#include "CGas.h"
#include "CWorldMap.h"
#include "CIsland.h"
#include "MovableText.h"
#include "LevelManager.h"
#include <windows.h>
#include "Mmsystem.h"
//#include "TinyXMLResource.h"
//#include "TinyXMLResourceManager.h"
//#include "tinyxml.h"
//#include "tinystr.h"