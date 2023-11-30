//#define PLAY_IMPLEMENTATION
//
//#include "..\PlayBuffer\Play.h"
//#include "GameObject.h"
//#include "State.h"
//
//#include "ObjectManager.h"
//#include "Agent8.h"
//
//int DISPLAY_WIDTH = 1280;
//int DISPLAY_HEIGHT = 720;
//int DISPLAY_SCALE = 1;
//
//// The entry point for a PlayBuffer program
//void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
//{
//	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
//	Play::CentreAllSpriteOrigins();
//	Play::LoadBackground( "Data\\Backgrounds\\background.png" );
//	Play::StartAudioLoop( "music" );
//	GetObjectManager()->CreateObject( TYPE_AGENT8, { 115, DISPLAY_HEIGHT }, { 0, 0 } );
//	GetObjectManager()->CreateObject( TYPE_FAN, { 1130, 250 }, { 0, 3 } );
//}
//
//// Called by PlayBuffer every frame (60 times a second!)
//bool MainGameUpdate( float )
//{
//	Play::DrawBackground();
//	GetObjectManager()->UpdateAll();
//	GetObjectManager()->DrawAll();
//	GetObjectManager()->CollideAll();
//	GetObjectManager()->CleanUpAll();
//
//	Play::DrawFontText( "64px", "ARROW KEYS TO MOVE UP AND DOWN AND SPACE TO FIRE",
//		{ DISPLAY_WIDTH / 2, 30 }, Play::CENTRE );
//
//	Play::DrawFontText( "132px", "SCORE: " + std::to_string( ((Agent8*)GetObjectManager()->GetPlayer())->GetScore() ),
//		{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 50.0f }, Play::CENTRE );
//
//	Play::PresentDrawingBuffer();
//	return Play::KeyDown( VK_ESCAPE );
//}
//
//// Gets called once when the player quits the game 
//int MainGameExit( void )
//{
//	DestroyObjectManager();
//	Play::DestroyManager();
//	return PLAY_OK;
//}
//
