//#include "noise.h"

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
HWND g_hWnd = NULL;

struct Keeper
{
    IDirect3D9Ptr             pD3D;
    IDirect3DDevice9Ptr       pd3dDevice;
    IDirect3DVertexBuffer9Ptr pVertexBuffer;
    IDirect3DTexture9Ptr pTexture;

    IDirect3DVertexShader9Ptr      pVertexShader;
    IDirect3DVertexDeclaration9Ptr pVertexDeclaration;
    ID3DXConstantTablePtr          pConstantTableVS;

    IDirect3DPixelShader9Ptr pPixelShader;
    ID3DXConstantTablePtr    pConstantTablePS;
} g_Keeper;

bool g_bUseShaders = true;

bool b_rebuildShaders = false;

D3DXMATRIX g_matWorld;
D3DXMATRIX g_matView;
D3DXMATRIX g_matProj;
float      g_fSpinX = 0.0f;
float      g_fSpinY = 0.0f;

struct Vertex
{
    float x, y, z;
    DWORD color;
    float tu, tv;

    enum FVF
    {
        FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1
    };
};

Vertex g_quadVertices[] =
{
    {-1.0f, 1.0f, 0.0f,  0xffffff00,  0.0f,0.0f },
    { 1.0f, 1.0f, 0.0f,  0xff00ff00,  1.0f,0.0f },
    {-1.0f,-1.0f, 0.0f,  0xffff0000,  0.0f,1.0f },
    { 1.0f,-1.0f, 0.0f,  0xff0000ff,  1.0f,1.0f }
};

//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void init(void);
void render(void);
void shutDown(void);
void initShader(void);

template<class Interface>
void SafeRelease(Interface **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR     lpCmdLine,
					int       nCmdShow )
{
	WNDCLASSEX winClass; 
	MSG        uMsg;

    memset(&uMsg,0,sizeof(uMsg));

	winClass.lpszClassName = L"MY_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = nullptr;//LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
    winClass.hIconSm	   = nullptr;//LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if( !RegisterClassEx(&winClass) )
		return E_FAIL;

	g_hWnd = CreateWindowEx( NULL, L"MY_WINDOWS_CLASS", 
                             L"Direct3D (DX9) - Simple Vertex & Pixel Shader Using HLSL",
						     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					         0, 0, 1024, 768, NULL, NULL, hInstance, NULL );

	if( g_hWnd == NULL )
		return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );
    UpdateWindow( g_hWnd );

	init();
	initShader();

    b_rebuildShaders = false;

	while( uMsg.message != WM_QUIT )
	{
		if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &uMsg );
			DispatchMessage( &uMsg );
		}
		else
		    render();
	}

	g_Keeper = Keeper();

    UnregisterClass( L"MY_WINDOWS_CLASS", winClass.hInstance );

	return uMsg.wParam;
}

//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND   hWnd, 
							 UINT   msg, 
							 WPARAM wParam, 
							 LPARAM lParam )
{
	static POINT ptLastMousePosit;
	static POINT ptCurrentMousePosit;
	static bool bMousing;
	
    switch( msg )
	{
        case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;

                case VK_F1:
                    g_bUseShaders = !g_bUseShaders;
                    break;

                case 'P':
                    b_rebuildShaders = true;
                    break;
			}
		}
        break;

		case WM_LBUTTONDOWN:
		{
			ptLastMousePosit.x = ptCurrentMousePosit.x = LOWORD (lParam);
            ptLastMousePosit.y = ptCurrentMousePosit.y = HIWORD (lParam);
			bMousing = true;
		}
		break;

		case WM_LBUTTONUP:
		{
			bMousing = false;
		}
		break;

		case WM_MOUSEMOVE:
		{
			ptCurrentMousePosit.x = LOWORD (lParam);
			ptCurrentMousePosit.y = HIWORD (lParam);

			if( bMousing )
			{
				g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
				g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
			}
			
			ptLastMousePosit.x = ptCurrentMousePosit.x;
            ptLastMousePosit.y = ptCurrentMousePosit.y;
		}
		break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);	
		}
		
        case WM_DESTROY:
		{
            PostQuitMessage(0);
		}
        break;

		default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: init()
// Desc: 
//-----------------------------------------------------------------------------
void init( void )
{
    g_Keeper.pD3D = Direct3DCreate9( D3D_SDK_VERSION );

    D3DDISPLAYMODE d3ddm;

    g_Keeper.pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    d3dpp.Windowed               = TRUE;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat       = d3ddm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

    g_Keeper.pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &d3dpp, &g_Keeper.pd3dDevice );

	g_Keeper.pd3dDevice->CreateVertexBuffer( 4*sizeof(Vertex), D3DUSAGE_WRITEONLY, 
                                      Vertex::FVF_Flags, D3DPOOL_DEFAULT, 
                                      &g_Keeper.pVertexBuffer, NULL );
    void *pVertices = NULL;

    g_Keeper.pVertexBuffer->Lock( 0, sizeof(g_quadVertices), (void**)&pVertices, 0 );
    memcpy( pVertices, g_quadVertices, sizeof(g_quadVertices) );
    g_Keeper.pVertexBuffer->Unlock();

	g_Keeper.pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_Keeper.pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	D3DXMatrixPerspectiveFovLH( &g_matProj, D3DXToRadian( 30.0f ), 
                                640.0f / 480.0f, 0.1f, 100.0f );
    g_Keeper.pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );

	D3DXMatrixIdentity( &g_matView ); // This sample is not really making use of a view matrix

    D3DVERTEXELEMENT9 declaration[] =
    {
        { 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
        { 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    g_Keeper.pd3dDevice->CreateVertexDeclaration( declaration, &g_Keeper.pVertexDeclaration );
}

//-----------------------------------------------------------------------------
// Name: initShader()
// Desc: Initialize a HLSL shader.
//-----------------------------------------------------------------------------



void initShader( void )
{
	//
	// Create a test texture for our effect to use...
	//

	D3DXCreateTextureFromFile( g_Keeper.pd3dDevice, L"test.bmp", &g_Keeper.pTexture );
    OutputDebugStringA("Building shaders...\n");
    
    g_Keeper.pVertexShader = nullptr;
    g_Keeper.pPixelShader = nullptr;
    HRESULT hr;
    ID3DXBufferPtr pCode;
    DWORD dwShaderFlags = 0;
    ID3DXBufferPtr pBufferErrors;

    // Assemble the vertex shader from the file
    hr = D3DXCompileShaderFromFileA("vertex_shader.hlsl", NULL, NULL, "main", "vs_3_0", dwShaderFlags, &pCode, &pBufferErrors, &g_Keeper.pConstantTableVS);
    if (FAILED(hr))
    {
        char const *str = reinterpret_cast<char const*>(pBufferErrors->GetBufferPointer());
        OutputDebugStringA(str);
    }
    else
    {
        g_Keeper.pd3dDevice->CreateVertexShader(reinterpret_cast<DWORD const *>(pCode->GetBufferPointer()), &g_Keeper.pVertexShader);
    }

    hr = D3DXCompileShaderFromFileA("pixel_shader.hlsl", NULL, NULL, "main", "ps_3_0", dwShaderFlags, &pCode, &pBufferErrors, &g_Keeper.pConstantTablePS);
    if (FAILED(hr))
    {
        char const *str = reinterpret_cast<char const*>(pBufferErrors->GetBufferPointer());
        OutputDebugStringA(str);
    }
    else
    {
        g_Keeper.pd3dDevice->CreatePixelShader(reinterpret_cast<DWORD const *>(pCode->GetBufferPointer()), &g_Keeper.pPixelShader);
    }
}

//-----------------------------------------------------------------------------
// Name: shutDown()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
void render( void )
{
    if (b_rebuildShaders)
    {
        initShader();
        b_rebuildShaders = false;
    }
    
    
    g_Keeper.pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f), 1.0f, 0 );

	g_Keeper.pd3dDevice->BeginScene();

    //
	// Set up a world matrix for spinning the quad about...
	//

	D3DXMATRIX matTrans;
	D3DXMATRIX matRot;

    D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, 4.0f );
	D3DXMatrixRotationYawPitchRoll( &matRot,
		                            D3DXToRadian(g_fSpinX),
		                            D3DXToRadian(g_fSpinY),
		                            0.0f );
    g_matWorld = matRot * matTrans;

    D3DXMatrixIdentity( &g_matView ); // This sample is not really making use of a view matrix
    
    if( g_bUseShaders == true )
    {
        //
        // Use vertex and pixel shaders...
        //

        D3DXMATRIX worldViewProjection = g_matWorld * g_matView * g_matProj;
	    if (g_Keeper.pConstantTableVS)
            g_Keeper.pConstantTableVS->SetMatrix( g_Keeper.pd3dDevice, "worldViewProj", &worldViewProjection );

        if (g_Keeper.pConstantTablePS)
        {
            float t = float(timeGetTime()) * 0.001f;
            g_Keeper.pConstantTablePS->SetFloat(g_Keeper.pd3dDevice, "time", t);
            g_Keeper.pConstantTablePS->SetFloat(g_Keeper.pd3dDevice, "time", t);
        }

        g_Keeper.pd3dDevice->SetVertexDeclaration( g_Keeper.pVertexDeclaration );
        g_Keeper.pd3dDevice->SetVertexShader( g_Keeper.pVertexShader );
        g_Keeper.pd3dDevice->SetTexture( 0, g_Keeper.pTexture );
        g_Keeper.pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);


        g_Keeper.pd3dDevice->SetPixelShader( g_Keeper.pPixelShader );
	    g_Keeper.pd3dDevice->SetFVF( Vertex::FVF_Flags );
        g_Keeper.pd3dDevice->SetStreamSource( 0, g_Keeper.pVertexBuffer, 0,sizeof(Vertex) );
        g_Keeper.pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

        g_Keeper.pd3dDevice->SetVertexShader( NULL );
        g_Keeper.pd3dDevice->SetPixelShader( NULL );
    }
    else
    {
        //
        // Render the normal way...
        //
    
        g_Keeper.pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );

        g_Keeper.pd3dDevice->SetTexture( 0, g_Keeper.pTexture );
	    g_Keeper.pd3dDevice->SetFVF( Vertex::FVF_Flags );
        g_Keeper.pd3dDevice->SetStreamSource( 0, g_Keeper.pVertexBuffer, 0,sizeof(Vertex) );
        g_Keeper.pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }

	g_Keeper.pd3dDevice->EndScene();
    g_Keeper.pd3dDevice->Present( NULL, NULL, NULL, NULL );
}
