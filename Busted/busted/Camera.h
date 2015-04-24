
#pragma once

#include "player/player.h"
#include "busted.all.h"

/******************************************************************************
The following are all the camera path actions that can be preformed.

CPT_NONE - no camera path, set to this when given bad data

CPT_FOLLOW - locks the camera to the focal point and moves with it

CPT_INTERP_POS - transitions from it's current position to a newly specified one

CPT_ZOOM - Zooms the camera in or out based on the number given in val

CPT_ROTATE_XZ - rotates the camera around the x-z planes based on the number given in val.

CPT_ROTATE_YZ - rotates the camera around the y-z planes based on the number given in val.

******************************************************************************/
enum CAM_PATH_TYPE{ CPT_NONE, CPT_FOLLOW, CPT_INTERP_POS, CPT_ZOOM, CPT_ROTATE_XZ, CPT_ROTATE_YZ};



//!class for moving and orienting camera with interpolation
class CameraPath
{
private:
	CAM_PATH_TYPE m_type; //!< type of the path 
	camera* m_cam; //!<camera that is set to the path
	vec4* m_focus; //!< point that the camera is moving to
	player* m_player_foc; //!<pointer for player types
	bool m_active; //!< if the path is still active
	float m_velo[3]; //!<velocity
	float m_speed; //!< velocity scalar.
	float m_value; //!< general variable used depending on the type of the camera path
	
public:
	
	CameraPath( CAM_PATH_TYPE t, camera* c, void* foc = NULL, float speed = 1.0f, float val = 1.0f );

	void Update( float t );
	CAM_PATH_TYPE GetType(){return m_type;}
	bool IsFinished(){ return m_active == false; }

};





/******************************************************************************
The following are all the camera path actions that can be preformed.

TP_FIXED_POS - camera locks to a position and follows a point

TP_STICK_END - locks camera to the end of a stick based around a point

******************************************************************************/
enum CAMERA_TYPE{ TP_FIXED_POS, TP_STICK_END }; 



/******************************************************************************
The following are all the camera path actions that can be preformed.

CFT_OVERVIEW - load in the values to set the camera to overlook the board

CFT_TOKEN_FOLLOW - have the camera transition to the player and follow them

CFT_Q_TRANS_IN - accellerated zoom into a player and fade to white

CFT_Q_TRANS_OUT - accellerated zoom out of a player and fade to white

******************************************************************************/
enum CAM_FOLLOW_TYPE{ CFT_OVERVIEW, CFT_TOKEN_FOLLOW, CFT_Q_TRANS_IN, CFT_Q_TRANS_OUT};



//!class for managing all the camera paths and the camera itself
class Camera_Manager
{
private:
	camera* m_cam;
	
	std::queue<CameraPath*> m_cam_path_que; //!<queue of all the paths set up to be preformed by the camera
	
	bool m_active; //!< this will update the camera along paths when this is true when this is true
	bool m_pause; //!<flag for when the camera is paused


	bool DumpPath();//!< dumps the first path on the queue and moves to the next, returns false if the queue is empty

	CAMERA_TYPE m_type; //!<type of the camera

	float	m_focal_pos[3]; //!<position of the focal point

	float   m_goal_fpos[3]; //!<goal position that the camera focal point is trying to end up at
	float   m_goal_cpos[3]; //!<goal position that the camera position is trying to end up at
	float	m_goal_z_dis; //!<goal zoom distance that the camera is trying to end up at.


	Spline	m_foc_spl;//!<spline for the focal point
	Spline	m_cam_spl;//!<spline for the camera
	Spline	m_cdis_spl; //!<spline for the distance from the computer
	float	m_trans_cnt; //!<holds the interpolant value along the splines

	void UpdateStickCam( float t);
	void UpdateFollowCam( float t);


	CAM_FOLLOW_TYPE m_follow_type; //!<type of following camera

	//!<loaders for the different types of follow cameras
	void SetupTokenFollowCam();
	void SetupOverviewCam();
	void SetupQZoomInCam();
	void SetupQZoomOutCam();


	bool m_trans_active; //!<whether or not the camera is transitioning

	float CalcZDis( float x, float y, float z );


public:

	

	Camera_Manager( camera* cam );
	~Camera_Manager();

	void Update( float t ); 
	void ClearPaths();

	void AddPath( CAM_PATH_TYPE cam_p, void* foc = NULL, float speed = 1.0f, float val = 1.0f ); 
		//adds a new path for the camera to follow

	bool LockedOn();

	bool IsActive(){ return m_active; }
	bool IsTransActive(){ return m_trans_active; }

	bool AtPos(vec4 pos);

	CAMERA_TYPE GetType(){return m_type;}
	void SetType( CAMERA_TYPE type );



	void SetFocalPoint( float x, float y, float z ){m_focal_pos[0] = x; m_focal_pos[1] = y; m_focal_pos[2] = z; }
	void SetCamPoint( float x, float y, float z){m_cam->pos.x = x; m_cam->pos.y = y; m_cam->pos.z = z; }

	void SetCamFollowType( CAM_FOLLOW_TYPE t);
	CAM_FOLLOW_TYPE GetCamFollowType() { return m_follow_type; }

	//sets up the camera to transition from it's current positions to another
	void TransitionCamera( float cx, float cy, float cz, float fx, float fy, float fz, float z_dis );
	void TransitionCamera( float fx, float fy, float fz );
	void TransitionCamera( float zoom );
	float GetTransInterp() { return m_trans_cnt;}


	float near_z_lim; //limit for the minimum that the camera can zoom in
	float far_z_lim; //limit for the max that the camera can zoom in
	float zoom_range; //range of the zoom factor
	float speed;//overall speed the camera moves at
};
