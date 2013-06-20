/*=====================================================================
	Camera.h :  Declaration of the CCamera class.
    Author :    Christopher Baptiste
    Version:    v1.0a
	Licence :	GNU GENERAL PUBLIC LICENSE
	    Copywrite (C) 2006, 2007
	                Christopher Baptiste
                    Full Sail, Inc.
                    3300 University Blvd. Ste 160
                    Winter Park, Florida 32792
	    
	    Everyone is permitted to copy and distribute verbatim copies
        of this license document, but changing it is not allowed.
//===================================================================*/
#ifndef _CAMERA_H_
#define _CAMERA_H_

/*=====================================================================
	Header Includes:
//===================================================================*/
#include <d3d9.h>
#include <d3dx9.h>
#include "../ATHUtil/hDataTypes.h"

/*=====================================================================
	Class Declaration:
//===================================================================*/
class CCamera
{
private:
    D3DXMATRIX      m_mProjectionMatrix;
    D3DXMATRIX      m_mViewMatrix;    
public:
    CCamera(void);
    ~CCamera(void);

/*=====================================================================
    Projection Matrix Functions
//===================================================================*/
    D3DXMATRIX GetProjectionMatrix(void);
    void SetProjectionMatrix(D3DXMATRIX *_mMatrix);
    void BuildPerspective(float _fFieldOfView,
                          float _fAspect,
                          float _fZNear,
                          float _fZFar);
    
/*=====================================================================
    View Matrix Functions
//===================================================================*/
    D3DXMATRIX GetViewMatrix(bool _bTranslate = true);
    void SetViewMatrix(D3DXMATRIX *_mMatrix);
    void NormalizeViewMatrix(void);

    D3DXVECTOR3 GetViewXAxis(void);
    void SetViewXAxis(D3DXVECTOR3 _vPosition);
    void SetViewXAxis(float _fX,
                      float _fY,
                      float _fZ);

    D3DXVECTOR3 GetViewYAxis(void);
    void SetViewYAxis(D3DXVECTOR3 _vPosition);
    void SetViewYAxis(float _fX,
                      float _fY,
                      float _fZ);

    D3DXVECTOR3 GetViewZAxis(void);
    void SetViewZAxis(D3DXVECTOR3 _vPosition);
    void SetViewZAxis(float _fX,
                      float _fY,
                      float _fZ);

    D3DXVECTOR3 GetViewPosition(void);
    void SetViewPosition(D3DXVECTOR3 _vPosition);
    void SetViewPosition(float _fX,
                         float _fY,
                         float _fZ);
    
    /*=================================================================
        Local Transform Functions
    //===============================================================*/
    void ViewRotateLocalX(float _fAngle);
    void ViewRotateLocalY(float _fAngle);
    void ViewRotateLocalZ(float _fAngle);
    
    void ViewTranslateLocal(D3DXVECTOR3 _vAxis, bool _bFPS = false);
    void ViewTranslateLocalX(float _fScale, bool _bFPS = false);
    void ViewTranslateLocalY(float _fScale, bool _bFPS = false);
    void ViewTranslateLocalZ(float _fScale, bool _bFPS = false);

    /*=================================================================
        Global Transform Functions
    //===============================================================*/
    void ViewRotateGlobalX(float _fAngle);
    void ViewRotateGlobalY(float _fAngle);
    void ViewRotateGlobalZ(float _fAngle);

    void ViewTranslateGlobal(D3DXVECTOR3 _vAxis);
    void ViewTranslateGlobalX(float _fScale);
    void ViewTranslateGlobalY(float _fScale);
    void ViewTranslateGlobalZ(float _fScale);

};
#endif //_CAMERA_H_