/*=====================================================================
	Camera.cpp :    Implementation of the CCamera class.
    Author :        Christopher Baptiste
    Version:        v1.0a
	Licence :	    GNU GENERAL PUBLIC LICENSE
	    Copywrite (C) 2006, 2007
	                Christopher Baptiste
                    Full Sail, Inc.
                    3300 University Blvd. Ste 160
                    Winter Park, Florida 32792
	    
	    Everyone is permitted to copy and distribute verbatim copies
        of this license document, but changing it is not allowed.
//===================================================================*/
#include ".\Camera.h"

CCamera::CCamera(void)
{
    D3DXMatrixIdentity(&this->m_mProjectionMatrix);
    D3DXMatrixIdentity(&this->m_mViewMatrix);
}
//================================================================================
CCamera::~CCamera(void)
{
}
//================================================================================
D3DXMATRIX CCamera::GetProjectionMatrix(void)
{
    return this->m_mProjectionMatrix;
}
//================================================================================
void CCamera::SetProjectionMatrix(D3DXMATRIX *_mMatrix)
{
    memcpy(&this->m_mProjectionMatrix, _mMatrix, sizeof(D3DXMATRIX));
}
//================================================================================
void CCamera::BuildPerspective(float _fFieldOfView,
                               float _fAspectRatio,
                               float _fZNear,
                               float _fZFar)
{
    D3DXMatrixPerspectiveFovLH(&this->m_mProjectionMatrix,
                               _fFieldOfView,
                               _fAspectRatio,
                               _fZNear,
                               _fZFar);
}
//================================================================================
void CCamera::BuildOrthoPerspective( float _fPerspectiveHeight,
							float _fAspect,
							float _fZNear,
							float _fZFar )
{
	//D3DXMatrixOrthoOffCenterLH( &this->m_mProjectionMatrix, 0.0f,1280,720,0.0f,0.0f,100.0f);
	D3DXMatrixOrthoLH( &this->m_mProjectionMatrix,  _fPerspectiveHeight * _fAspect, _fPerspectiveHeight, _fZNear, _fZFar );
}
//================================================================================
D3DXMATRIX CCamera::GetViewMatrix(bool _bTranslate)
{
    D3DXMATRIX _mRotation(this->m_mViewMatrix), _mTransform;
    _mRotation._41 = _mRotation._42 = _mRotation._43 = 0;
    D3DXMatrixTranspose(&_mRotation, &_mRotation);
    
    D3DXMatrixIdentity(&_mTransform);
    _mTransform._41 = -(this->m_mViewMatrix._41);
    _mTransform._42 = -(this->m_mViewMatrix._42);
    _mTransform._43 = -(this->m_mViewMatrix._43);
    
    D3DXMatrixMultiply(&_mTransform, &_mTransform, &_mRotation);
    return ( _bTranslate ? _mTransform : _mRotation );
}
//================================================================================
void CCamera::SetViewMatrix(D3DXMATRIX *_mMatrix)
{
    memcpy(&this->m_mViewMatrix, _mMatrix, sizeof(D3DXMATRIX));
}
//================================================================================
void CCamera::NormalizeViewMatrix(void)
{
    /*=================================================================
    D3DXVECTOR3 xAxis,
                yAxis(0.0f, 1.0f, 0.0f),
                zAxis(this->m_mViewMatrix._31, this->m_mViewMatrix._32, this->m_mViewMatrix._33);

    D3DXVec3Normalize(&zAxis, &zAxis);
    D3DXVec3Cross(&xAxis, &zAxis, &yAxis); 
    D3DXVec3Normalize(&xAxis, &xAxis);
    D3DXVec3Cross(&yAxis, &xAxis, &zAxis);
    D3DXVec3Normalize(&yAxis, &yAxis);
    
    this->m_mViewMatrix._11 = xAxis.x; this->m_mViewMatrix._12 = xAxis.y; this->m_mViewMatrix._13 = xAxis.z; this->m_mViewMatrix._14 = 0;
    this->m_mViewMatrix._21 = yAxis.x; this->m_mViewMatrix._22 = yAxis.y; this->m_mViewMatrix._23 = yAxis.z; this->m_mViewMatrix._24 = 0;
    this->m_mViewMatrix._31 = zAxis.x; this->m_mViewMatrix._32 = zAxis.y; this->m_mViewMatrix._33 = zAxis.z; this->m_mViewMatrix._34 = 0;
    //===============================================================*/
}
//================================================================================
D3DXVECTOR3 CCamera::GetViewXAxis(void)
{
    return D3DXVECTOR3(m_mViewMatrix._11,
                       m_mViewMatrix._12,
                       m_mViewMatrix._13);
}
//================================================================================
void CCamera::SetViewXAxis(D3DXVECTOR3 _vPosition)
{
    this->m_mViewMatrix._11 = _vPosition.x;
    this->m_mViewMatrix._12 = _vPosition.y;
    this->m_mViewMatrix._13 = _vPosition.z;
}
//================================================================================
void CCamera::SetViewXAxis(float _fX,
                           float _fY,
                           float _fZ)
{
    this->m_mViewMatrix._11 = _fX;
    this->m_mViewMatrix._12 = _fY;
    this->m_mViewMatrix._13 = _fZ;
}
//================================================================================
D3DXVECTOR3 CCamera::GetViewYAxis(void)
{
    return D3DXVECTOR3(m_mViewMatrix._21,
                       m_mViewMatrix._22,
                       m_mViewMatrix._23);
}
//================================================================================
void CCamera::SetViewYAxis(D3DXVECTOR3 _vPosition)
{
    this->m_mViewMatrix._21 = _vPosition.x;
    this->m_mViewMatrix._22 = _vPosition.y;
    this->m_mViewMatrix._23 = _vPosition.z;
}
//================================================================================
void CCamera::SetViewYAxis(float _fX,
                           float _fY,
                           float _fZ)
{
    this->m_mViewMatrix._21 = _fX;
    this->m_mViewMatrix._22 = _fY;
    this->m_mViewMatrix._23 = _fZ;
}
//================================================================================
D3DXVECTOR3 CCamera::GetViewZAxis(void)
{
    return D3DXVECTOR3(m_mViewMatrix._31,
                       m_mViewMatrix._32,
                       m_mViewMatrix._33);
}
//================================================================================
void CCamera::SetViewZAxis(D3DXVECTOR3 _vPosition)
{
    this->m_mViewMatrix._31 = _vPosition.x;
    this->m_mViewMatrix._32 = _vPosition.y;
    this->m_mViewMatrix._33 = _vPosition.z;
}
//================================================================================
void CCamera::SetViewZAxis(float _fX,
                           float _fY,
                           float _fZ)
{
    this->m_mViewMatrix._31 = _fX;
    this->m_mViewMatrix._32 = _fY;
    this->m_mViewMatrix._33 = _fZ;
}
//================================================================================
D3DXVECTOR3 CCamera::GetViewPosition(void)
{
    return D3DXVECTOR3(m_mViewMatrix._41,
                       m_mViewMatrix._42,
                       m_mViewMatrix._43);
}
//================================================================================
void CCamera::SetViewPosition(D3DXVECTOR3 _vPosition)
{
    this->m_mViewMatrix._41 = _vPosition.x;
    this->m_mViewMatrix._42 = _vPosition.y;
    this->m_mViewMatrix._43 = _vPosition.z;
}
//================================================================================
void CCamera::SetViewPosition(float _fX,
                              float _fY,
                              float _fZ)
{
    this->m_mViewMatrix._41 = _fX;
    this->m_mViewMatrix._42 = _fY;
    this->m_mViewMatrix._43 = _fZ;
}

/*====================================================================/
    Local Transform Functions
//===================================================================*/
void CCamera::ViewRotateLocalX(float _fAngle)
{
    D3DXMATRIX  _mRotation;
    D3DXVECTOR3 _vLocalX(m_mViewMatrix._11, m_mViewMatrix._12, m_mViewMatrix._13);
    D3DXMatrixRotationAxis(&_mRotation, &_vLocalX, _fAngle);

    D3DXVECTOR4 Position(this->m_mViewMatrix._41, this->m_mViewMatrix._42, 
                         this->m_mViewMatrix._43, this->m_mViewMatrix._44);
    this->m_mViewMatrix._41 = this->m_mViewMatrix._42 = this->m_mViewMatrix._43 = 0.0f; 
    
    D3DXMatrixMultiply(&m_mViewMatrix, &m_mViewMatrix, &_mRotation);
    this->m_mViewMatrix._41 = Position.x; this->m_mViewMatrix._42 = Position.y;
    this->m_mViewMatrix._43 = Position.z; this->m_mViewMatrix._44 = Position.w;
}
//================================================================================
void CCamera::ViewRotateLocalY(float _fAngle)
{
    D3DXMATRIX  _mRotation;
    D3DXVECTOR3 _vLocalY(m_mViewMatrix._21, m_mViewMatrix._22, m_mViewMatrix._23);
    D3DXMatrixRotationAxis(&_mRotation, &_vLocalY, _fAngle);

    D3DXVECTOR4 Position(this->m_mViewMatrix._41, this->m_mViewMatrix._42, 
                         this->m_mViewMatrix._43, this->m_mViewMatrix._44);
    this->m_mViewMatrix._41 = this->m_mViewMatrix._42 = this->m_mViewMatrix._43 = 0.0f; 
    
    D3DXMatrixMultiply(&m_mViewMatrix, &m_mViewMatrix, &_mRotation);
    this->m_mViewMatrix._41 = Position.x; this->m_mViewMatrix._42 = Position.y;
    this->m_mViewMatrix._43 = Position.z; this->m_mViewMatrix._44 = Position.w;
}
//================================================================================
void CCamera::ViewRotateLocalZ(float _fAngle)
{
    D3DXMATRIX  _mRotation;
    D3DXVECTOR3 _vLocalZ(m_mViewMatrix._31, m_mViewMatrix._32, m_mViewMatrix._33);
    D3DXMatrixRotationAxis(&_mRotation, &_vLocalZ, _fAngle);
    
    D3DXVECTOR4 Position(this->m_mViewMatrix._41, this->m_mViewMatrix._42, 
                         this->m_mViewMatrix._43, this->m_mViewMatrix._44);
    this->m_mViewMatrix._41 = this->m_mViewMatrix._42 = this->m_mViewMatrix._43 = 0.0f; 
    
    D3DXMatrixMultiply(&m_mViewMatrix, &m_mViewMatrix, &_mRotation);
    this->m_mViewMatrix._41 = Position.x; this->m_mViewMatrix._42 = Position.y;
    this->m_mViewMatrix._43 = Position.z; this->m_mViewMatrix._44 = Position.w;
}
//================================================================================
void CCamera::ViewTranslateLocal(D3DXVECTOR3 _vAxis, bool _bFPS)
{
    if (_bFPS)
    {
        ViewTranslateLocalX(_vAxis.x, _bFPS);
        ViewTranslateLocalY(_vAxis.y, _bFPS);
        ViewTranslateLocalZ(_vAxis.z, _bFPS);
        return;
    }
    
    ViewTranslateLocalX(_vAxis.x);
    ViewTranslateLocalY(_vAxis.y);
    ViewTranslateLocalZ(_vAxis.z);
}
//================================================================================
void CCamera::ViewTranslateLocalX(float _fScale, bool _bFPS)
{
    if (_bFPS)
    {
        D3DXVECTOR3 _vTranslate((this->m_mViewMatrix._11),
                                0.0f,
                                (this->m_mViewMatrix._13));
        D3DXVec3Normalize(&_vTranslate,
                          &_vTranslate);
                                        
        this->m_mViewMatrix._41 += _vTranslate.x * _fScale;
        this->m_mViewMatrix._42 += _vTranslate.y * _fScale;
        this->m_mViewMatrix._43 += _vTranslate.z * _fScale;
        
        return;
    }
    this->m_mViewMatrix._41 += (this->m_mViewMatrix._11 * _fScale);
    this->m_mViewMatrix._42 += (this->m_mViewMatrix._12 * _fScale);
    this->m_mViewMatrix._43 += (this->m_mViewMatrix._13 * _fScale);
}
//================================================================================
void CCamera::ViewTranslateLocalY(float _fScale, bool _bFPS)
{
    if (_bFPS)
    {
        D3DXVECTOR3 _vTranslate((this->m_mViewMatrix._21),
                                0.0f,
                                (this->m_mViewMatrix._23));
        D3DXVec3Normalize(&_vTranslate,
                          &_vTranslate);
                                        
        this->m_mViewMatrix._41 += _vTranslate.x * _fScale;
        this->m_mViewMatrix._42 += _vTranslate.y * _fScale;
        this->m_mViewMatrix._43 += _vTranslate.z * _fScale;
        
        return;
    }
    this->m_mViewMatrix._41 += (this->m_mViewMatrix._21 * _fScale);
    this->m_mViewMatrix._42 += (this->m_mViewMatrix._22 * _fScale);
    this->m_mViewMatrix._43 += (this->m_mViewMatrix._23 * _fScale);
}
//================================================================================
void CCamera::ViewTranslateLocalZ(float _fScale, bool _bFPS)
{
    if (_bFPS)
    {
        D3DXVECTOR3 _vTranslate((this->m_mViewMatrix._31),
                                0.0f,
                                (this->m_mViewMatrix._33));
        D3DXVec3Normalize(&_vTranslate,
                          &_vTranslate);
                                        
        this->m_mViewMatrix._41 += _vTranslate.x * _fScale;
        this->m_mViewMatrix._42 += _vTranslate.y * _fScale;
        this->m_mViewMatrix._43 += _vTranslate.z * _fScale;
        
        return;
    }
    this->m_mViewMatrix._41 += (this->m_mViewMatrix._31 * _fScale);
    this->m_mViewMatrix._42 += (this->m_mViewMatrix._32 * _fScale);
    this->m_mViewMatrix._43 += (this->m_mViewMatrix._33 * _fScale);
}

/*====================================================================/
    Global Transform Functions
//===================================================================*/
void CCamera::ViewRotateGlobalX(float _fAngle)
{
    D3DXMATRIX _mRotation;
    D3DXVECTOR4 Position(this->m_mViewMatrix._41, this->m_mViewMatrix._42, 
                         this->m_mViewMatrix._43, this->m_mViewMatrix._44);
    D3DXMatrixRotationX(&_mRotation, _fAngle);
    
    this->m_mViewMatrix._41 = this->m_mViewMatrix._42 = this->m_mViewMatrix._43 = 0.0f; 
    
    D3DXMatrixMultiply(&this->m_mViewMatrix, &this->m_mViewMatrix, &_mRotation);
    this->m_mViewMatrix._41 = Position.x; this->m_mViewMatrix._42 = Position.y;
    this->m_mViewMatrix._43 = Position.z; this->m_mViewMatrix._44 = Position.w;
}
//================================================================================
void CCamera::ViewRotateGlobalY(float _fAngle)
{
    D3DXMATRIX _mRotation;
    D3DXVECTOR4 Position(this->m_mViewMatrix._41, this->m_mViewMatrix._42, 
                         this->m_mViewMatrix._43, this->m_mViewMatrix._44);
    D3DXMatrixRotationY(&_mRotation, _fAngle);
    
    this->m_mViewMatrix._41 = this->m_mViewMatrix._42 = this->m_mViewMatrix._43 = 0.0f; 
    
    D3DXMatrixMultiply(&this->m_mViewMatrix, &this->m_mViewMatrix, &_mRotation);
    this->m_mViewMatrix._41 = Position.x; this->m_mViewMatrix._42 = Position.y;
    this->m_mViewMatrix._43 = Position.z; this->m_mViewMatrix._44 = Position.w;
}
//================================================================================
void CCamera::ViewRotateGlobalZ(float _fAngle)
{
    D3DXMATRIX _mRotation;
    D3DXVECTOR4 Position(this->m_mViewMatrix._41, this->m_mViewMatrix._42, 
                         this->m_mViewMatrix._43, this->m_mViewMatrix._44);
    D3DXMatrixRotationZ(&_mRotation, _fAngle);
    
    this->m_mViewMatrix._41 = this->m_mViewMatrix._42 = this->m_mViewMatrix._43 = 0.0f; 
    
    D3DXMatrixMultiply(&this->m_mViewMatrix, &this->m_mViewMatrix, &_mRotation);
    this->m_mViewMatrix._41 = Position.x; this->m_mViewMatrix._42 = Position.y;
    this->m_mViewMatrix._43 = Position.z; this->m_mViewMatrix._44 = Position.w;
}
//================================================================================
void CCamera::ViewTranslateGlobal(D3DXVECTOR3 _vAxis)
{
    this->m_mViewMatrix._41 += _vAxis.x;
    this->m_mViewMatrix._42 += _vAxis.y;
    this->m_mViewMatrix._43 += _vAxis.z;
}
//================================================================================
void CCamera::ViewTranslateGlobalX(float _fScale)
{
    this->m_mViewMatrix._41 += _fScale;
}
//================================================================================
void CCamera::ViewTranslateGlobalY(float _fScale)
{
    this->m_mViewMatrix._42 += _fScale;
}
//================================================================================
void CCamera::ViewTranslateGlobalZ(float _fScale)
{
    this->m_mViewMatrix._43 += _fScale;
}
//================================================================================