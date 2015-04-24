


#include "Camera.h"







/***********************************
CameraPath Constructor

Author: Jamie Gault
***********************************/
CameraPath::CameraPath(CAM_PATH_TYPE t, camera* c, void* foc, float speed, float val ): 
																m_cam(c), 
																m_type(t), 
																m_focus(NULL),
																m_player_foc(NULL),
																m_active(true),
																m_speed(speed),
																m_value(val)
{

	if( foc )
	{	
		if( m_type == CPT_FOLLOW )
			m_player_foc = reinterpret_cast<player*>(foc);
		else if( m_type == CPT_INTERP_POS )
			m_focus = reinterpret_cast<vec4*>(foc);		
	}


	if( m_speed == 0.0f )
		m_speed = 1.0f;

	m_velo[0] = 0.0f;
	m_velo[1] = 0.0f;
	m_velo[2] = 0.0f;

	switch( m_type )
	{
	case CPT_FOLLOW: 
		break;
	case CPT_INTERP_POS:

		if( m_cam && m_focus ) 
		{

			float x = m_focus->x - m_cam->pos.x;
			float y = m_focus->y - m_cam->pos.y;
			float z = m_focus->z - m_cam->pos.z;
				
			float mag = sqrt( x*x + y*y + z*z );

			if( mag != 0.0f )
			{
				//assign unit vector
				m_velo[0] = x / mag * m_speed; //include speed for optimization
				m_velo[1] = y / mag * m_speed; 
				m_velo[2] = z / mag * m_speed;
			}
			else
				m_type = CPT_NONE;
			
		}
	

		break;
	case CPT_ZOOM:
	case CPT_ROTATE_XZ:
	case CPT_ROTATE_YZ:
		//make speed a value that would add towards value;
		m_speed =( (m_speed > 0.0f && m_value > 0.0f ) || (m_speed < 0.0f && m_value < 0.0f )) ? m_speed: -m_speed;
		break;
	}
	
	//if the cameras don't exist
	if( !m_cam  ) 
	{
		m_type = CPT_NONE;
	}

	//assert( m_type != CPT_NONE );
}


/***********************************
CameraPath Constructor

Author: Jamie Gault
***********************************/
void CameraPath::Update( float t )
{
	if( !m_active )
		return;

	switch( m_type )
	{
	case CPT_FOLLOW: 

		//follow the focus point until it stops moving
		if( !(m_player_foc->bAtDest) ) 
		{
			m_cam->pos.x = m_player_foc->pos.x;
			m_cam->pos.y = m_player_foc->pos.y;
			m_cam->pos.z = m_player_foc->pos.z;

		}
		else
			m_active = false;

		break;
	case CPT_INTERP_POS:
		{
			m_cam->pos.x += m_velo[0]*t; //include speed for optimization
			m_cam->pos.y += m_velo[1]*t; 
			m_cam->pos.z += m_velo[2]*t;

			float rad = m_velo[0]*t * m_velo[0]*t + m_velo[1]*t * m_velo[1]*t + m_velo[2]*t * m_velo[2]*t;
			
			float x = m_focus->x - m_cam->pos.x;
			float y = m_focus->y - m_cam->pos.y;
			float z = m_focus->z - m_cam->pos.z;
			
			//if the camera has gotten as close as it's going to get
			if( x*x + y*y + z*z < rad )
			{
				m_cam->pos = *m_focus;
				m_active = false;
			}

		}
		break;
		
	case CPT_ZOOM:

		if( m_speed*t * m_speed*t < m_value * m_value )
		{
			m_cam->setCamDistance( m_cam->getCamDistance() + m_speed*t );
			m_value -= m_speed*t; 
		}
		else
		{
			m_cam->setCamDistance( m_cam->getCamDistance() + m_value );
			m_active = false;
		}

		break;
	case CPT_ROTATE_XZ:
		//rotate the camera around
		if( m_speed*t * m_speed*t < m_value )
		{
			m_cam->rot.y += m_speed*t ;
			m_value -= m_speed*t; 
		}
		else
		{
			m_cam->rot.y += m_value ;
			m_active = false;
		}
		break;
	case CPT_ROTATE_YZ:
		//rotate the camera
		if( m_speed*t * m_speed*t < m_value )
		{
			m_cam->rot.x +=  m_speed*t ;
			m_value -= m_speed*t; 
		}
		else
		{
			m_cam->rot.x +=  m_value ;
			m_active = false;
		}
		break;
	default:
		m_active = false;
		break;
	}
}


/***********************************
Camera_Manager Constructor

Author: Jamie Gault
***********************************/
Camera_Manager::Camera_Manager( camera* cam ): m_cam(cam), 
											   m_active(false),
											   m_pause(false),
											   m_type(TP_FIXED_POS),
											   m_follow_type(CFT_TOKEN_FOLLOW),
											   near_z_lim(0.0f),
											   far_z_lim(20.0f),
											   zoom_range(60.0f),
											   speed( 1/150.0f)


{
	m_focal_pos[0] = 0.0f;
	m_focal_pos[1] = 0.0f;
	m_focal_pos[2] = 0.0f;

	m_goal_fpos[0] = 0.0f;
	m_goal_fpos[1] = 0.0f;
	m_goal_fpos[2] = 0.0f;

	m_goal_cpos[0] = 0.0f;
	m_goal_cpos[1] = 0.0f;
	m_goal_cpos[2] = 0.0f;

	m_goal_z_dis = 0.0f;

	m_trans_active = false;
}


/***********************************
Camera_Manager Destructor

Author: Jamie Gault
***********************************/
Camera_Manager::~Camera_Manager()
{
	ClearPaths();
}


/***********************************
Camera_Manager Update

Author: Jamie Gault
***********************************/
void Camera_Manager::Update( float t )
{
	//if active and not paused
	/*if( m_active && !m_pause )*/
	{
		switch( m_type )
		{
		case TP_STICK_END:
			UpdateStickCam( t);
			break;
		case TP_FIXED_POS:
			UpdateFollowCam(t);
			break;

		}
	}
}


/***********************************
Camera_Manager ClearPaths

Author: Jamie Gault
***********************************/
void Camera_Manager::ClearPaths()
{
	//while the queue isn't empty
	while( ! m_cam_path_que.empty() )
	{
		//delete the elements it has
		if(m_cam_path_que.front())
			delete m_cam_path_que.front();
		
		//and pop it off
		m_cam_path_que.pop();
	}

	m_active = false;

}


/***********************************
Camera_Manager AddPath

Author: Jamie Gault
***********************************/
void Camera_Manager::AddPath( CAM_PATH_TYPE cam_p, void* foc, float speed, float val )
{
	return; //////////////////////////////////////////////////////////////////////////////////don't use paths any more

	m_active = true;

	CameraPath *cp = new CameraPath( cam_p, m_cam, foc, speed, val );
	
	m_cam_path_que.push(cp);
}

/***********************************
Camera_Manager DumpPath

Author: Jamie Gault
***********************************/
bool Camera_Manager::DumpPath()
{
	//delete the elements it has
	if(m_cam_path_que.front())
		delete m_cam_path_que.front();
	
	//and pop it off
	m_cam_path_que.pop();

	//return if the path queue is not empty
	return ! m_cam_path_que.empty();

}

/***********************************
Camera_Manager LockedOn

Author: Jamie Gault
***********************************/
bool Camera_Manager::LockedOn() 
{
	if( m_cam_path_que.empty() )
		return false;
	else
		return m_cam_path_que.front()->GetType() == CPT_FOLLOW;
}


/***********************************
Camera_Manager LockedOn

Author: Jamie Gault
***********************************/
bool Camera_Manager::AtPos(vec4 pos)
{
	if( pos.x == m_cam->pos.x && pos.y == m_cam->pos.y && pos.z == m_cam->pos.z )
		return true;

	return false;
}



/***********************************
Camera_Manager LockedOn

Author: Jamie Gault
***********************************/
void Camera_Manager::UpdateStickCam( float t)
{
	//call update
	m_cam_path_que.front()->Update( t );

	//if the path has finished
	if( m_cam_path_que.front()->IsFinished() )
	{
		//if a path was not able to be dumped
		if( ! DumpPath() )
		{
			m_active = false;
		}
	}
}


/***********************************
Camera_Manager UpdateFollowCam

Author: Jamie Gault
***********************************/
void Camera_Manager::UpdateFollowCam( float t)
{
	
	vec3 rot = vec3(0.0f, 0.0f, 0.0f);

	float dx = m_focal_pos[0]-m_cam->getPosX();
	float dy = m_focal_pos[1]-m_cam->getPosY();
	float dz = m_focal_pos[2]-m_cam->getPosZ();

	/*if( GetCamFollowType() == CFT_Q_TRANS_IN )
	{
		glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45.0f + m_trans_cnt *80.0f,(GLfloat) DATA("SCREEN_WIDTH")/(GLfloat) DATA("SCREEN_HEIGHT"),10.0f,1100.0f);
		glMatrixMode (GL_MODELVIEW);
	}
	else if( GetCamFollowType() == CFT_Q_TRANS_OUT )
	{
		glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45.0f - m_trans_cnt *80.0f + 80.0f,(GLfloat) DATA("SCREEN_WIDTH")/(GLfloat) DATA("SCREEN_HEIGHT"),10.0f,1100.0f);
		glMatrixMode (GL_MODELVIEW);
	}*/


	if( m_trans_active )
	{
		m_trans_cnt += t*speed;

		if( m_trans_cnt > 1.0f )
		{
			m_trans_cnt = 1.0f;
			m_trans_active = false;
		}

		//don't update when the camera is zooming in
		/*if( GetCamFollowType() != CFT_Q_TRANS_IN && GetCamFollowType() !=CFT_Q_TRANS_OUT)*/
		{
			//deteremine the focal points position along the spline
			vec3 foc = m_foc_spl.GetPoint( m_trans_cnt );
			m_focal_pos[0] = foc.x;
			m_focal_pos[1] = foc.y;
			m_focal_pos[2] = foc.z;


			//determine the cameras position along it's spline
			vec3 c = m_cam_spl.GetPoint( m_trans_cnt );
			m_cam->pos.x = c.x;
			m_cam->pos.y = c.y;
			m_cam->pos.z = c.z;
		}

		vec3 d = m_cdis_spl.GetPoint( m_trans_cnt );
		m_cam->setCamDistance( d.x );
	}
	else
	{
		//actively update the camera distance when the camera is following a token
		if( m_follow_type == CFT_TOKEN_FOLLOW )
			m_cam->setCamDistance( CalcZDis(dx, dy, dz) );
	}
	

	if( dz != 0.0f )
	{
		m_cam->rot.y = - 180.0f/3.14159f*atan( dx/dz );
		m_cam->rot.x = -180.0f/3.14159f*atan( dy/sqrt(dz*dz+dx*dx) );
	}
	else
	{
		m_cam->rot.y = 0.0f;
		m_cam->rot.x = 0.0f;
	}	
}



/***********************************
Camera_Manager SetupTokenFollowCam

Author: Jamie Gault
***********************************/
void Camera_Manager::SetupTokenFollowCam()
{
	near_z_lim = 40.0f;
	far_z_lim = 100.0f;
	zoom_range = -60.0f;

	m_goal_cpos[0] = 0.0f;
	m_goal_cpos[1] = 60.0f;
	m_goal_cpos[2] = 60.0f;
}


/***********************************
Camera_Manager SetupOverviewCam

Author: Jamie Gault
***********************************/
void Camera_Manager::SetupOverviewCam()
{

	TransitionCamera( 0.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}


/***********************************
Camera_Manager CalcZDis

Author: Jamie Gault
***********************************/
void Camera_Manager::SetupQZoomInCam()
{
	pMusic_man->play("ZOOM_IN");
	float mag = (m_cam->pos.x - m_focal_pos[0])*(m_cam->pos.x - m_focal_pos[0]) + 
				(m_cam->pos.y - m_focal_pos[1])*(m_cam->pos.y - m_focal_pos[1]) + 
				(m_cam->pos.z - m_focal_pos[2])*(m_cam->pos.z - m_focal_pos[2]);

	mag = sqrt(mag);

	TransitionCamera( -(mag - 20.0f) );
}

/***********************************
Camera_Manager CalcZDis

Author: Jamie Gault
***********************************/
void Camera_Manager::SetupQZoomOutCam()
{
	//TransitionCamera( 0.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f, -20.0f);

	pMusic_man->play("ZOOM_OUT");
	TransitionCamera( 0.0f);
}

/***********************************
Camera_Manager SetCamFollowType

Author: Jamie Gault
***********************************/
void Camera_Manager::SetCamFollowType( CAM_FOLLOW_TYPE t)
{
	//assign camera
	switch(t)
	{
	case CFT_OVERVIEW:
		SetupOverviewCam();
		break;
	case CFT_TOKEN_FOLLOW:
		SetupTokenFollowCam();
		//Update(0.0f);
		break;
	case CFT_Q_TRANS_IN:
		SetupQZoomInCam();
		//Update(0.0f);
		break;
	case CFT_Q_TRANS_OUT:
		SetupQZoomOutCam();
		//Update(0.0f);
		break;
	}

	m_follow_type = t;
}


/***********************************
Camera_Manager TransitionCamera

Author: Jamie Gault
***********************************/
void Camera_Manager::TransitionCamera( float cx, float cy, float cz, float fx, float fy, float fz, float z_dis )
{
	m_trans_cnt = 0.0f;

	float cdx = cx - m_cam->pos.x;
	float cdy = cy - m_cam->pos.y;
	float cdz = cz - m_cam->pos.z;

	vec3 c0 = vec3(m_cam->pos.x, m_cam->pos.y, m_cam->pos.z);
	vec3 c1 = vec3(cdx*0.001f + m_cam->pos.x, cdy*0.001f + m_cam->pos.y, cdz*0.001f + m_cam->pos.z);
	vec3 c2 = vec3(cdx*0.9f + m_cam->pos.x, cdy*0.9f + m_cam->pos.y, cdz*0.9f + m_cam->pos.z);
	vec3 c3 = vec3(cdx*1.0f + m_cam->pos.x, cdy*1.0f + m_cam->pos.y, cdz*1.0f + m_cam->pos.z);

	m_cam_spl = Spline(c0, c1, c2, c3);



	float fdx = fx - m_focal_pos[0];
	float fdy = fy - m_focal_pos[1];
	float fdz = fz - m_focal_pos[2];

	vec3 f0 = vec3(m_focal_pos[0], m_focal_pos[1], m_focal_pos[2]);
	vec3 f1 = vec3(fdx*0.001f + m_focal_pos[0], fdy*0.001f + m_focal_pos[1], fdz*0.001f + m_focal_pos[2]);
	vec3 f2 = vec3(fdx*0.9f + m_focal_pos[0], fdy*0.9f + m_focal_pos[1], fdz*0.9f + m_focal_pos[2]);
	vec3 f3 = vec3(fdx*1.000f + m_focal_pos[0], fdy*1.0f + m_focal_pos[1], fdz*1.0f + m_focal_pos[2]);

	m_foc_spl = Spline(f0, f1, f2, f3);


	float ddx = z_dis - m_cam->getCamDistance();

	vec3 d0 = vec3(m_cam->getCamDistance(), 0.0f, 0.0f);
	vec3 d1 = vec3(ddx*0.001f + m_cam->getCamDistance(), 0.0f, 0.0f);
	vec3 d2 = vec3(ddx*0.7f + m_cam->getCamDistance(), 0.0f, 0.0f);
	vec3 d3 = vec3(ddx*1.0f + m_cam->getCamDistance(), 0.0f, 0.0f);

	m_cdis_spl = Spline(d0, d1, d2, d3);
										


	m_goal_cpos[0] = cx;
	m_goal_cpos[1] = cy;
	m_goal_cpos[2] = cz;

	m_goal_fpos[0] = fx;
	m_goal_fpos[1] = fy;
	m_goal_fpos[2] = fz;

	m_goal_z_dis = z_dis;

	m_trans_active = true;
}


/***********************************
Camera_Manager TransitionCamera

Author: Jamie Gault
***********************************/
void Camera_Manager::TransitionCamera( float fx, float fy, float fz )
{
	TransitionCamera( m_goal_cpos[0],m_goal_cpos[1],m_goal_cpos[2], fx, fy, fz, 
						CalcZDis(fx-m_goal_cpos[0], fy-m_goal_cpos[1], fz-m_goal_cpos[2]) ); 
}


/***********************************
Camera_Manager TransitionCamera

Author: Jamie Gault
***********************************/
void Camera_Manager::TransitionCamera( float zoom )
{
	TransitionCamera( m_cam->pos.x,m_cam->pos.y,m_cam->pos.z, 
					  m_focal_pos[0],m_focal_pos[1],m_focal_pos[2], 
						zoom ); 
}


/***********************************
Camera_Manager CalcZDis

Author: Jamie Gault
***********************************/
float Camera_Manager::CalcZDis( float x, float y, float z )
{
	float mag = sqrt(x*x+y*y+z*z);

	//prevent division by zero
	if( near_z_lim != far_z_lim )
	{
		//zoom the camera in and out with lerps
		if( mag < near_z_lim ) 
		{
			return (0.0f);
		}
		else if( near_z_lim < mag && mag < far_z_lim )
		{
			return ( ( mag-near_z_lim)/(far_z_lim - near_z_lim)*zoom_range );
		}
		else 
		{
			return (zoom_range);
		}
	}
	else
		return 0.0f;

}

